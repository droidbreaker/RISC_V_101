# Task 3 :  Peripheral Driver Library Development + Application Library API 

### Implemented UART library and application
#### API list with brief description

- UART library with API which can be called by the application.
- more modular approcach:
    
    #### uart_init(GPIO_Port port, GPIO_PIN tx_pin, GPIO_PIN rx_pin);
        * Uart inititialization which will use the `USART1` `PortD` `Rx pin : PD6` `Tx pin : PD5`
        * First we have enable the `USART1` clock by using `APB2PCENR`  to enable clock for `GPIO` port D as well as 
        `USART1`.
        * Configuration using `CFGLR` we can change the MODE, and CNF as it has total 4 bits. 2 for MODE and 2 for CNF.
        * MODE uses : `00` for INPUT MODE, `01` OUTPUT MODE with 10MHz, `10` OUTPUT MODE with 20MHz, `11` OUPUT MODE with 50MHZ.
        * CNF uses : `00` for analog INPUT, `01` for floating INPUT, `10` for pull up INPUT, `11` Alternate Function mdoe.
        * by using the modes and cnf registers we have implemented :
          FOR RX pin : MODE : 00 we have used, CNF : 10
          FOR TX pin : MODE : 11 we have used, CNF : 10
        * we have use Baud Rate : 9800. by using `BRR` register.
            `USART1->BRR = (uint16_t)(24000000UL / 9600)` by using this formula.
            at the moment its hard-coded but we can easily modify it, by providing one more argument to the function as `uint32_t baud`   
    #### uart_SendBuffer(uint8_t *buffer, uint16_t length);
        * Send buffer will send the data onto the serial terminal where we can see the `UART` logs.
        * here we have used `USART_STATREG` we have used USART status register and by checking the value of `TXE` register. It will wait until the transmit register is empty.
        * by using `USART1_DATAREG` we can tranfer the data from buffer to data register which can store it into data register.
        * by using `USART1_STATSREG->TC` we can transmitt it onto serial monitor from which we can see the output, till the last byte has been sent. It will wait until the last byte getting transmitted onto serial monitor.

    #### uart_receiveData(uint8_t *buffer, uint16_t length);
        * receive data from the serial monitor so that we can use this function for `ECHO` purpose.
        * we have used `USART_STATREG` with the `RXNE` where it will wait for next byte until we hit `Enter` it will receive byte by byte from the screen.
        * after that we are storing those bytes into the buffer.
        * then we can transmit that same buffer so that we can see the `ECHO` effect on serial monitor.
        whatever we have typed we can see it again via `uart_sendBuffer` function.
            
    #### uart_receiveByte(); 
        * it will receive only 1 byte, and then it will stop reciving it.
        * it is used to modify the flow of the application, where we can communicate via serial monitor.
        * when we pressed 1 on serial monitor as human input , it will recieve it and it will do the task which has been applied for `1` data, like wise it will keep on checking the input from the `USER` whenever we call this function.

### Implemented Library : UART library + GPIO (small scale)

- UART for `Tx` and `Rx`  from the serial monitor and vise-versa.
- GPIO for `set_pin` , `clear_pin`, `toggle_pin`, `read_pin`. Here we have used read_pin to check the status of the pin on the screen via using the UART functions.

### Application Demo: 
- application has been built in total 4 modes.
- which we can select it from the screen.
- by selecting `1`, we can turn LED ON/OFF , by givin  input from keyboard to UART serial terminal, as `1` to turn ON, `0` to turn OFF.
- by selecting `2`, we can check the status of the `PortA` `GPIO_PIN_02` if its sensing 3.3v or 5v then it will give us status as `HIGH` or else it will give `LOW` as status output on screen via UART serial terminal.
- by selecting `3`, we can check the message we have typed on screen , which is the Echo demo program, which will echo out typed message onto the screen.
- by selecting `4`, it will end the program, by exiting from the loop `while(1)`, and by giving the exiting message on the screen as `this demo has ended successfully`.

Those were the main application demo steps as modules.





### How to build and flash the firmware

- Use the platfomrIO build option as well as we need to do modification on the platformio.ini files like this: 
`[env:vsdsquadronMini]`
`platform = ch32v`
`board = vsdsquadronMini`
`framework = noneos-sdk`
`monitor_speed = 9600`

- In `src` folder we will have  `main.c`, `gpio.c` , `uart.c`. (main file + driver files).
- In `lib` folder we will have `gpio.h`, `uart.h` (header files). 
- also include the proper path , if you are using vscode , then you have to include the path for your source directory as well as library directory add all in  `c_cpp_properties.json` file.
- then start building it by using PlatformIO build tools, and then upload it onto the controller via wch-link USB cable.
- after these configuration we can see the output on the screen by using wch-link USB cable, and by selecting the `/dev/ttyACM0` port on serial terminal.

### NOTE: But here in this DEMO we have used `Putty` because , Serial terminal of the PlatformIO is having `UFT-8 encoder` , so what it does: when ever we are sending the data it will get encoded into some other data, from which we cant get out echo message properly, sometimes it omits the data, sometimes it manipulate/modify it internally.

### Hence to avoid this issue we have used `Putty` where we can get `Raw` proper data. 

## CONCLUSION : we have successfully implemented UART driver + application.



