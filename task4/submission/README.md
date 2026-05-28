# TASK 4 : Power On Self Test (POST) Firmware V1.2

A bare-metal POST (Power On Self Test) framework for the **CH32V003 RISC-V microcontroller**, written entirely from scratch without any HAL or vendor library. Tests GPIO, UART, and TIMER peripherals at startup to verify hardware integrity before entering the main application.

---

## Target Hardware

| Property | Value |
|----------|-------|
| MCU | CH32V003 (RISC-V) |
| Clock | 24 MHz  |
| Build system | PlatformIO |
| Debug interface | WCH-Link via SWIO (PD1) |

---

## Peripherals Tested

### 1. GPIO — All available ports and pins

**Method:** Output readback via `INDR` register (same pin drive and read).

Each pin is configured as push-pull output, driven HIGH via `BSHR`, then read back via `INDR`. A match confirms the output driver and pad are functional.

**Pin map respected:**

| Port | Pins tested | Pins skipped | Reason |
|------|-------------|--------------|--------|
| GPIOA | PA1, PA2 | PA0, PA3–PA7 | Do not exist on CH32V003 |
| GPIOC | PC0–PC7 | — | All available |
| GPIOD | PD2–PD4, PD7 | PD0 (NRST), PD1 (SWIO), PD5 (TX), PD6 (RX) | Reserved pins |

**Key lessons learned:**

- `gpio_reset()` resets the **entire port** via `RCC_APB2PRSTR`. Calling it inside `gpio_init()` on every pin initialization wiped the UART pin configuration on GPIOD mid-test.
- Fix: `gpio_reset()` is called **once at boot** before `uart_init()`, never again after.
- `gpio_config_output()` must use **read-modify-write** (`&= ~mask`, `|= value`) on `CFGLR` — never a full register overwrite, which destroys other pins' configurations.
- `BSHR` is **write-only**. Reading it back only confirms the bus write, not the actual pin state. `INDR` is the correct register to verify pad voltage.

---

### 2. UART — Full loopback test

**Method:** Hardware loopback — PD5 (TX) shorted to PD6 (RX) with a jumper wire. A known string is transmitted and received simultaneously, then compared byte-for-byte.

**Configuration:**

| Parameter | Value |
|-----------|-------|
| Peripheral | USART1 |
| TX pin | PD5 — AF push-pull output (CFGLR nibble `0xB`)|
| RX pin | PD6 — Floating input (CFGLR nibble `0x4`) |
| Baud rate | 9600 |
| BRR value | `(24000000 + 4800) / 9600 = 2500` (rounded) |
| Loopback wire | PD5 → PD6 (remove after test) |

**Key lessons learned:**

- ISSUE: CFGLR nibble layout is `[CNF1|MODE1|CNF0|MODE0]` at `pin × 4`. Writing CNF at `(pin × 4) + 2` shifts bits into the adjacent pin's field — corrupting neighbouring pin configuration.
- The correct full nibble values: TX = `0xB` (AF push-pull 50 MHz), RX = `0x4` (floating input).
- USART1 has a **single shared `DATAR` register** for both TX and RX. Any `uart_SendBuffer` call between send and receive writes to `DATAR`, clearing `RXNE` and destroying the received byte.

- ISSUE :  At 9600 baud, 16 bytes transmit. Waiting for `TC` before starting receive means all bytes have already overwritten each other in the 1-byte RX buffer — only the last byte survives.
- Fix: TX and RX run **simultaneously** in `uart_sendReceive()` — send next byte when `TXE=1`, read byte when `RXNE=1`, interleaved in a single loop.
- `uart_flushRx()` must drain **all** stale bytes (loop until `RXNE=0`), not just one. `\r\n` is two bytes — a single-read flush leaves `\n` behind, corrupting the next receive buffer.

**UART inline status functions** (defined in `uart.h` as `static inline`):

```c
uart_isTxEmpty()       // TXE — safe to write next byte
uart_isTxComplete()    // TC  — last bit shifted out
uart_isRxReady()       // RXNE — byte waiting in DATAR
uart_flushRx()         // drain all stale RX bytes
uart_waitTxComplete()  // block until TC=1
```

**UART API USED:**

```c
void uart_init(GPIO_Port port, GPIO_PIN tx_pin, GPIO_PIN rx_pin);
void uart_SendBuffer(uint8_t *buffer, uint16_t length);
uint8_t* uart_sendReceive(const uint8_t* tx_buf,uint8_t* rx_buf,uint16_t  length);
uint8_t* uart_receiveData_from_User_input(uint8_t *buffer, uint16_t maxLength);
uint8_t uart_receiveByte(); 
```
---

### 3. TIM2 — Hardware timer for timeout handling

**Method:** TIM2 configured as a free-running millisecond counter. Used as the timeout source for all UART receive operations, replacing unreliable loop-counter timeouts that vary with compiler optimisation level.

**Configuration:**

| Parameter | Value |
|-----------|-------|
| Peripheral | TIM2 (APB1, base `0x40000000`) |
| RCC enable | `RCC->APB1PCENR` bit 0 (TIM2EN) |
| Prescaler | `24000 - 1` → 1 kHz tick (1 ms per count) |
| Auto-reload | `0xFFFF` → 65535 ms range |
| Counter mode | Upcounting, free-running |

**Key lessons learned:**

- TIM2 is on **APB1**, not APB2. Enabling the wrong RCC bit means registers accept writes but the timer never counts.
- `PSC` and `ATRLR` are **shadow registers**. Writing them does not take effect until an update event. The `UG` bit in `SWEVGR` must be written after every `PSC`/`ATRLR` change to force an immediate load — without it, the timer runs at the wrong frequency until the first overflow.
- `uint16_t` subtraction for elapsed time (`now - start`) handles counter rollover correctly without any special case — a natural property of unsigned integer wrap around.

**Timer API:**

```c
void     tim2_init(void);                                  // configure and start
void     tim2_reset(void);                                 // reset CNT to 0
uint16_t tim2_get_ms(void);                                // current ms count
uint8_t  tim2_elapsed(uint16_t start_ms, uint16_t timeout_ms);  // non-blocking check
```

---

## Initialisation Order

Order matters. Deviating from this sequence causes peripheral conflicts.

```c
int main(void)
{
    gpio_reset_all();          // 1. Reset all GPIO ports ONCE — never again after this
    tim2_init();               // 2. Start hardware timer — needed by uart timeout
    uart_init(...);            // 3. Configure UART — sets PD5/PD6 AF config
                               //    gpio_reset_all() must already be done
    test_GPIO();               // 4. GPIO POST — skips PD5, PD6, PD1, PD0
    test_UART();               // 5. UART POST — requires loopback wire on PD5→PD6
    test_TIM2();               // 6. TIM2 POST — verify known delay accuracy
}
```

---

## POST Output

```
=======================================
      Board Name: CH32V003
     Welcome to RISC-V embedded systems
     Firmware Version: 1.2
=======================================
=================================================================
     POWER ON SELF TEST INITIALIZING FOR GPIO, USART, TIMER 
=================================================================

================= PERIPHERAL TESTING ================
================= GPIO TEST START =====================
Portx = GPIOA ,pin = 2 PASS 
Portx = GPIOC ,pin = 0 PASS 
Portx = GPIOC ,pin = 1 PASS 
Portx = GPIOC ,pin = 2 PASS 
Portx = GPIOC ,pin = 3 PASS 
Portx = GPIOC ,pin = 4 PASS 
Portx = GPIOC ,pin = 5 PASS 
Portx = GPIOC ,pin = 6 PASS 
Portx = GPIOC ,pin = 7 PASS 
Portx = GPIOD ,pin = 0 PASS 
Portx = GPIOD ,pin = 1 PASS 
Portx = GPIOD ,pin = 2 PASS 
Portx = GPIOD ,pin = 3 PASS 
Portx = GPIOD ,pin = 4 PASS 
GPIOD pin 5 USART Rx/Tx
GPIOD pin 6 USART Rx/Tx
Portx = GPIOD ,pin = 7 PASS 

================= GPIO TEST PASS ====================
================= GPIO TEST DONE ======================

================= UART TEST START =====================
UART_TEST_STRING
UART                        PASS

================= UART TEST PASS ====================
================= UART TEST DONE ======================

================= TIMER TEST START =====================
TIM2 t1=10001 t2=11001

TIMER                      PASS 

================= TIMER TEST PASS ====================
================= TIMER TEST DONE ======================


=================== ALL TEST PASS ================
Booting in  10 seconds
Booting in  9 seconds
Booting in  8 seconds
Booting in  7 seconds
Booting in  6 seconds
Booting in  5 seconds
Booting in  4 seconds
Booting in  3 seconds
Booting in  2 seconds
Booting in  1 seconds
Booting complete!
System is now running.
System booted in 22 seconds
```

---

## Common Bugs Found and Fixed

| Bug | Root cause | Fix |
|-----|-----------|-----|
| GPIOD pins not tested | `gpio_reset()` inside `gpio_init()` wiped UART config | Call `gpio_reset()` once at boot only |
| UART receive always timeout | `waitTxComplete()` then receive — all bytes already overwritten | Interleave TX and RX simultaneously |
| UART loopback stale byte | `uart_flushRx()` only read one byte, left `\n` behind | Loop flush until `RXNE=0` |
| Wrong pin config (CFGLR) | CNF written at `(pin×4)+2` offset — corrupts adjacent pin | Write full 4-bit nibble at `pin×4` |
| Loop counter timeout unreliable | Varies with `-O0` vs `-O2` optimisation | Replace with TIM2 hardware timer |
| `gpio_reset` linker error | Declared `static` — invisible outside its `.c` file | Move to `uart.h` as `static inline` |

---

## Register Quick Reference

### GPIO (GPIOD base: `0x40011400`)

| Register | Offset | Purpose |
|----------|--------|---------|
| `CFGLR` | `0x00` | Pin mode config (4 bits per pin, pins 0–7) |
| `INDR` | `0x08` | Input data — actual pad voltage, read-only |
| `OUTDR` | `0x0C` | Output data register |
| `BSHR` | `0x10` | Bit set (low 16) / reset (high 16), write-only |
| `BCR` | `0x14` | Bit clear, write-only |

### USART1 (base: `0x40013800`)

| Register | Offset | Key bits |
|----------|--------|---------|
| `STATREG` | `0x00` | `RXNE`[5], `TC`[6], `TXE`[7], `ORE`[3], `FE`[1] |
| `DATAR` | `0x04` | TX write / RX read — shared single register |
| `BRR` | `0x08` | Baud rate = HCLK / baud |
| `CTLR1` | `0x0C` | `UE`[13], `TE`[3], `RE`[2] |

### TIM2 (base: `0x40000000`)

| Register | Offset | Purpose |
|----------|--------|---------|
| `CTLR1` | `0x00` | `CEN`[0] start/stop, `ARPE`[7] buffer ARR |
| `INTFR` | `0x10` | `UIF`[0] overflow flag — must clear in ISR |
| `SWEVGR` | `0x14` | `UG`[0] force shadow register load |
| `CNT` | `0x24` | Current counter value |
| `PSC` | `0x28` | Prescaler (shadow register) |
| `ATRLR` | `0x2C` | Auto-reload value (shadow register) |

---

### How to BUILD and FLASH it: 
1. move `Main.c` file in `src` folder, if its not there then make it new as name: `src` , move `main.c` file into `src`
2. move `gpio.c` , `timer2.c` , `uart.c` files in `src`.
3. make sure in the `lib` folder only *.h should be there, as `gpio.h`,`timer2.h`, `uart.h` 
4. open PlatformIO on vscode, then open this directory where you have kept `src` , `lib` files.
5. also you will be requiring the startup files for the CH32v003 controller board.
6. you need to get it from importing the library from platformIO.`VSDsqaudron` as project.
7. then use `build` tool of PlatformIO , after build is successful, upload it onto your controller, by connecting USB to c type connector.

---
### HOW to use DEMO :
1. after uploading it onto controller, just open serial terminal, in VSCode, or on Putty.
2. select your USB : `dev/ttyACM0`
3. select baud Rate: `9800`
4. then press `reset` button on controller, also make sure terminal is monitoring, (start monitoring). 
5. you will be seeing the output on serial monitor and all the test cases which have passed or failed.

---

# Conclusion : successfully implemented Power On Self Test from scratch, on RISC-V CH32v003 controller.