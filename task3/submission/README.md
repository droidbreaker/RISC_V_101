### Task 3 :  Peripheral Driver Library Development + ApplicationLibrary API 

#### Implemented UART library and application

- UART library with API which can be called by the application.
- more modular approcach:
    - uart_init(GPIO_Port port, GPIO_PIN tx_pin, GPIO_PIN rx_pin);
    - uart_SendBuffer(uint8_t *buffer, uint16_t length);
    - uart_receiveData(uint8_t *buffer, uint16_t length);
    - uart_receiveByte(); 

