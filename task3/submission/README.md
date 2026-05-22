### Task 3 :  Peripheral Driver Library Development + ApplicationLibrary API 

#### Implemented UART library and application

- UART library with API which can be called by the application.
- more modular approcach:
    - USART_INIT (); // with proper GPIO as RX and TX.
    - USART_send_buffer (); it will send message on serial monitor.
    - USART_send_Byte (); it will send one byte which have '0' and '1'.
    - USART_receive_data(); It will receive the data from the serial monitor which has been sent by the Tx pin. and it will store it inside buffer.
    

