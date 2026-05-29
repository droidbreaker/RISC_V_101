# APPLICATION GUIDE : 

---

## PROJECT : BLINK LED: 
- TASK 2 : 
1. After building upload it to the respected `"dev/ttyACM0"` USB debugger/programmer WCH-Link which will upload the generated firmware.elf files to the device.
2. Open a serial terminal at `115200` baud rate and select the board which is connected to the machine `ttyACM0` , so when the board is reset it will show/print the welcome message including the board name , firmware version, count decreasing from `10-1` , then it gets booted into application as blinking LED.
---

## PROJECT : UART DRIVER FROM SCRATCH:
- TASK 3 :
1. Open a serial terminal at `115200` baud rate and select the board which is connected to the system `dev/ttyACM0`.
2. application has been built in total 4 modes.
3. which we can select it from the screen.
4. by selecting `1`, we can turn LED ON/OFF , by givin  input from keyboard to UART serial terminal, as `1` to turn ON, `0` to turn OFF.
5. by selecting `2`, we can check the status of the `PortA` `GPIO_PIN_02` if its sensing 3.3v or 5v then it will give us status as `HIGH` or else it will give `LOW` as status output on screen via UART serial terminal.
6. by selecting `3`, we can check the message we have typed on screen , which is the Echo demo program, which will echo out typed message onto the screen.
7. by selecting `4`, it will end the program, by exiting from the loop `while(1)`, and by giving the exiting message on the screen as `this demo has ended successfully`.
---

## PROJECT : POWER ON SELF TEST : 
- TASK 4 : 
1. after uploading it onto controller, just open serial terminal, in VSCode, or on Putty.
2. select your USB : `dev/ttyACM0`
3. select baud Rate: `9800`
4. then press `reset` button on controller, also make sure terminal is monitoring, (start monitoring). 
5. you will be seeing the output on serial monitor and all the test cases which have passed or failed.
---
