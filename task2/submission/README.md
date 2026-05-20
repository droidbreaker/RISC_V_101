## TASK 2 : Board Bring-Up, GPIO & UART Validation
## USING Linux Environment for the project


### ========================================================================================================

### Troubleshooting permission issues for the WCH-Link USB 
#### NOTE: Because of the linux environment when you connect your USB WCH-Link for the first time to the system, it will generate the file as /dev/ttyACM0 But when you start to upload the firmware.elf file to it, it will show some error as 
`:Error: libusb_open() failed with LIBUSB_ERROR_ACCESS Error: open failed`
- How to fix these error: 
1.  First check if the `plugdev` is showing when you type `groups` on terminal.
2. If its not showing then you need to add it as `newgrp plugdev` 
4. After making new group add your USER to it: `sudo usermod -a -G plugdev $USER`
5. then check `ls -larth /dev/ttyACM0` it will show Crw-rw-r root plugdev ttyACM0
6. then restart your system.
7. After that upload your code via platformIO interface in VsCode.

#### After uploading when we try to view for serial monitor it wont get recognized and it will show error as no USB serial device detected or similar error on debug console.
- How to fix that : 
1.  First check if the `dialout` is showing when you type `groups` on terminal.
2. If its not showing then you need to add it as `newgrp dialout` 
4. After making new group add your USER to it: `sudo usermod -a -G dialout $USER`
5. then check `ls -larth /dev/ttyACM0` it will show Crw-rw-r root dialout ttyACM0
6. then restart your system.
7. After that upload your code via platformIO interface in VsCode.
8. and open serial monitor you will get connected and it will show you the UART message as output on screen.

### =========================================================================================================

### 1. What is implemented?
- This submission brings up the CH32V003 board peripherals by configuring GPIO and USART1. The code initializes a blinking LED on `GPIOD`, pin `PD6` (built-in Blue LED) pin ,and  `GPIO_Pin_5` as Tx pin and `GPIO_Pin_6` as Rx pin and configures UART communication on `USART1`.

- Hence validating the UART and GPIO peripherals on the board.

Key implementation pieces:
- `src/gpio.c` defines `GPIO_Config()` to initialize output pins.
- `src/usart.c` defines `USARTx_CFG()` to configure USART1 and `UartBufferSend()` to transmit data.
- `src/main.c` sends startup messages over UART, then sends booting sequence and blinks the LED.
- here we have used the library functions which has been implemented on `ch32v00x.h` header files.
- Where mapping of all the peripherals has been done as per the datasheet. and it has been verified by properly navigating throught the datasheet and reference manuals, where we can check for the register address mappings and types of registers.
- also used other respected header files : 
   - `ch32v00x_gpio.h`  : for the GPIO configuration and initialization.  
   - `ch32v00x_rcc.h`   : for clock configuration RCC .
   - `debug.h`          : for Delay initialization for the perfect 1 second delay configuration.
   - `ch32v00x_usart.h` : for the USART communication throught the serial terminal by providing proper GPIO pins as Tx and Rx, initialization and configuration of the USART peripherals.

### 2. GPIO pin chosen and why?
The GPIO output pin chosen for the LED blink is `GPIOD` pin `GPIO_Pin_6` (in-built LED).
- `GPIO_Config()` enables the `RCC_APB2Periph_GPIOD` clock and configures `GPIOD, GPIO_Pin_6` as `GPIO_Mode_Out_PP` at `GPIO_Speed_50MHz`.
- This pin is used because it is initialized explicitly in the GPIO bring-up code and is a standard digital output pin for board validation.

### 3. UART message description
USART1 is configured with:
- TX on `GPIOD` pin `GPIO_Pin_5`
- RX on `GPIOD` pin `GPIO_Pin_6`
- Baud rate: `115200`
- Data format: `8` data bits, `1` stop bit, no parity, no flow control

Messages sent by `main.c`:
- `Board Name: CH32V003`
- `Welcome to RISC-V embedded systems`
- `Firmware Version: 1.0`
- A line `Booting in : <n> seconds` once per second for `n = 10..1`


### 4. How to build and flash (short steps)
1. Add the source files from `/task2/submission/src` and headers from `/task2/submission/lib` to your CH32V003 your new platform.io project.
2. Build with a CH32V003-compatible platform.io IDE, ensuring the device headers (`<ch32v00x.h>`, `<ch32v00x_gpio.h>`, `<ch32v00x_usart.h>`, etc.) are available, by giving proper path to these files.
3. After building upload it to the respected `"dev/ttyACM0"` USB debugger/programmer WCH-Link which will upload the generated firmware.elf files to the device.
4. Open a serial terminal at `115200` baud rate and select the board which is connected to the machine `ttyACM0` , so when the board is reset it will show/print the welcome message including the board name , firmware version, count decreasing from `10-1` , then it gets booted into application as blinking LED.

## Implemented UART + GPIO validation firmware successfully.