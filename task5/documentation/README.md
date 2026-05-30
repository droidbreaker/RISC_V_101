# CH32V003 Power On Self Test (POST)

## Summary

A bare-metal Power On Self Test (POST) framework for the **VSDSquadron Mini** development board, built on the **CH32V003 RISC-V microcontroller**. Written entirely from scratch without any HAL or vendor library, the framework systematically tests GPIO, UART, and TIM2 peripherals at startup — verifying hardware integrity before the main application runs. Each peripheral is tested through direct register access, with results reported over UART as individual PASS, FAIL, or SKIP messages followed by a final consolidated result. The project was developed iteratively, uncovering and fixing real hardware bugs including port-level resets wiping UART pin config, shared TX/RX register contention during loopback, shadow register timing on the timer, and loop-counter timeout drift under compiler optimisation.

---

## Target Hardware

| Property | Value |
|----------|-------|
| Board | VSDSquadron Mini |
| MCU | CH32V003F4P6 (RISC-V QingKe V2A) |
| Clock | 24 MHz (internal HSI) |
| Build system | PlatformIO |
| Debug interface | WCH-Link via SWIO (PD1) |

---

## Supported Drivers

| Driver | File | What is tested |
|--------|------|---------------|
| GPIO | `gpio.c / .h` | All available pins on GPIOA, GPIOC, GPIOD — output drive and INDR readback |
| UART | `uart.c / .h` | USART1 full loopback — TX/RX interleaved send+receive, byte-for-byte compare |
| Timer | `tim2.c / .h` | TIM2 free-running ms counter — known delay accuracy within +-50ms tolerance |
| PWM | planned | GPIO brightness from LOW to HIGH testing |

---

## Folder Structure

## Folder Structure

```
POST_project/
├── src/
│   ├── main.c              — entry point, initialisation order, POST sequence, result reporter
│   ├── gpio.c              — GPIO driver: clock enable, CFGLR config, BSHR/INDR access
│   ├── uart.c              — UART driver: init, SendBuffer, sendReceive, flushRx
│   └── tim2.c              — TIM2 driver: init, free-running ms counter
│
├── lib/
│   ├── gpio.h              — GPIO API + pin/port enums
│   ├── uart.h              — UART API + static inline status functions
│   └── tim2.h              — Timer API: tim2_get_ms, tim2_elapsed
│
├── platformio.ini          — build target, toolchain, upload config
├── ARCHITECTURE.md         — system layers, data flow, design decisions
└── README.md               
```

---

## Quick Start


1. move `Main.c` file in `src` folder, if its not there then make it new as name: `src` , move `main.c` file into `src`
2. move `gpio.c` , `timer2.c` , `uart.c` files in `src`.
3. make sure in the `lib` folder only *.h should be there, as `gpio.h`,`timer2.h`, `uart.h` 
4. open PlatformIO on vscode, then open this directory where you have kept `src` , `lib` files.
5. also you will be requiring the startup files for the CH32v003 controller board.
6. you need to get it from importing the library from platformIO.`VSDsqaudron` as project.
7. then use `build` tool of PlatformIO , after build is successful, upload it onto your controller, by connecting USB to c type connector.

> **Before running the UART test:** short PD5 (TX) to PD6 (RX) with a jumper wire on the board. Remove the jumper after the test completes.

---

## Initialisation Order

Order is mandatory. Deviating from this sequence causes peripheral conflicts.

```c
int main(void)
{
    gpio_reset_all();     // 1. Reset all GPIO ports ONCE — never again after this
    tim2_init();          // 2. Start hardware timer — uart timeout depends on it
    uart_init(...);       // 3. Configure UART — sets PD5/PD6 AF config
                          //    gpio_reset_all() must already be done before this
    test_GPIO();          // 4. GPIO POST — skips PD0 (NRST), PD1 (SWIO), PD5, PD6
    test_UART();          // 5. UART POST — requires loopback jumper on PD5 → PD6
    test_TIM2();          // 6. TIM2 POST — verify 100ms delay within ±5% tolerance
}
```

---

## Peripherals Tested

### 1. GPIO — All available ports and pins

**Method:** Output readback via `INDR` register. Each pin is configured as push-pull output, driven HIGH via `BSHR`, then read back via `INDR`. A match confirms the output driver and pad are functional.

**Pin map:**

| Port | Pins tested | Pins skipped | Reason |
|------|-------------|--------------|--------|
| GPIOA | PA1, PA2 | PA0, PA3–PA7 | Do not exist on CH32V003 |
| GPIOC | PC0–PC7 | — | All available |
| GPIOD | PD2–PD4, PD7 | PD0 (NRST), PD1 (SWIO), PD5 (TX), PD6 (RX) | Reserved pins |

**Key findings:**

- `gpio_reset()` resets the **entire port** via `RCC_APB2PRSTR`. Calling it inside `gpio_init()` per pin wiped UART config on GPIOD mid-test. Fix: called once at boot only.
- `CFGLR` must use **read-modify-write** — full register overwrite destroys other pins.
- `BSHR` is **write-only**. `INDR` is the only register that reflects actual pad voltage.

---

### 2. UART — Full loopback test

**Method:** PD5 (TX) shorted to PD6 (RX). A known string is transmitted and received simultaneously via `uart_sendReceive()`, then compared byte-for-byte with `strcmp`.

**Configuration:**

| Parameter | Value |
|-----------|-------|
| Peripheral | USART1 |
| TX pin | PD5 — AF push-pull output (CFGLR nibble `0xB`) |
| RX pin | PD6 — Floating input (CFGLR nibble `0x4`) |
| Baud rate | 9600 |
| BRR value | `(24000000 + 4800) / 9600 = 2500` (rounded) |
| Loopback wire | PD5 → PD6 (remove after test) |

**Key findings:**

- USART1 has a **single shared `DATAR` register**. Any `uart_SendBuffer` call between send and receive clears `RXNE` — destroying the received byte.
- At 9600 baud, Waiting for `TC` before receive means all bytes have overwritten each other — only the last survives.
- Fix: TX and RX interleaved in one loop — write when `TXE=1`, read when `RXNE=1`.
- `uart_flushRx()` loops until `RXNE=0` — a single read flush leaves `\n` behind.

**Inline status API** (`static inline` in `uart.h`):

```c
uart_isTxEmpty()       // TXE  — safe to write next byte
uart_isTxComplete()    // TC   — last bit shifted out
uart_isRxReady()       // RXNE — byte waiting in DATAR
uart_flushRx()         // drain all stale RX bytes
uart_waitTxComplete()  // block until TC=1
```

---

### 3. TIM2 — Hardware timer

**Method:** TIM2 as a free-running ms counter. Used as the timeout source for all UART receive operations.

**Configuration:**

| Parameter | Value |
|-----------|-------|
| Peripheral | TIM2 (APB1, base `0x40000000`) |
| RCC enable | `RCC->APB1PCENR` bit 0 |
| Prescaler | `24000 - 1` → 1 kHz (1 ms per tick) |
| Auto-reload | `0xFFFF` → 65535 ms range |
| Counter mode | Upcounting, free-running |

**Key findings:**

- TIM2 is **APB1**, not APB2. Wrong RCC bit = timer never counts.
- `PSC` and `ATRLR` are **shadow registers** — `UG` bit in `SWEVGR` must be written after every change to force immediate load.
- `uint16_t` subtraction for elapsed time handles rollover correctly — no special case needed.

**Timer API:**

```c
void     tim2_init(void);
void     tim2_reset(void);
uint16_t tim2_get_ms(void);
uint8_t  tim2_elapsed(uint16_t start_ms, uint16_t timeout_ms);
```

---

## Common Bugs Found and Fixed

| Bug | Root cause | Fix |
|-----|-----------|-----|
| GPIOD never tested | `gpio_reset()` inside `gpio_init()` wiped UART config | Call `gpio_reset()` once at boot only |
| UART receive timeout | `waitTxComplete()` then receive — bytes already overwritten | Interleave TX and RX simultaneously |
| Stale byte in RX buffer | `uart_flushRx()` read only one byte, left `\n` behind | Loop flush until `RXNE=0` |
| Timer wrong frequency | Forgot `UG` bit after writing `PSC`/`ARR` | Always write `SWEVGR=0x01` after PSC/ARR |
| Timeout unreliable | Loop counter varies with `-O0` vs `-O2` | Replace with TIM2 hardware timer |
| Linker error on `gpio_reset` | Declared `static` — invisible outside `.c` file | Expose via header or wrap in `gpio_reset_all()` |

---

## Register Quick Reference

### GPIO (GPIOD base: `0x40011400`)

| Register | Offset | Purpose |
|----------|--------|---------|
| `CFGLR` | `0x00` | Pin mode config — 4 bits per pin, pins 0–7 |
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
| `SWEVGR` | `0x14` | `UG`[0] force shadow register load |
| `CNT` | `0x24` | Current counter value |
| `PSC` | `0x28` | Prescaler (shadow register) |
| `ATRLR` | `0x2C` | Auto-reload value (shadow register) |
