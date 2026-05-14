# Task 1: Firmware Library Fundamentals

## Tasks Done

- How firmware libraries are structured
- How application code uses APIs
- Setting up a basic development environment
- Building and studying a simple firmware-style library

---

### Step 1: Files Observation

Observed the files `gpio.h`, `gpio.c`, `main.c`.

Here in the above files:
- `gpio.h` -> header file for gpio.c.
- `gpio.c` -> main gpio driver file.
- `main.c` -> where all the drivers and header files have been called.

Also, have explained it in details in the understanding of the task.

---

### Step 2: Build the Code

Building the code by running the following commands:

```bash
gcc main.c gpio.c -o task1_demo
```

---

### Step 3: Run the Program

```bash
./task1_demo
```
![Output of the given program simulating GPIO behavior](images/task_1_demo.png)
---

## Expected Output

As expected, it generates the proper simulating behavior of the GPIO using the `printf()` library functions.

---

## Explanation

### What is a firmware library?

A firmware library is where register-level programming is done. For example, each GPIO port has a dedicated register address, which we can get from the microcontroller datasheet.

![Address for all GPIO and other peripherals, e.g., STM32 board](images/gpio_dedicated_address_space.png)

### Why are APIs important?

APIs are important because each new microcontroller has different addresses for their peripherals, and it's more time-consuming than doing the actual real-world project. APIs for GPIO, which we have used here in the above example, are in the `gpio.c` file. So, rather than going deep into the register level, we simply call the API, which is more convenient and beneficial to complete the project that needs more attention.

### Understanding from the above task

- From the above task, I have understood that to run the program, you need proper header files that are at the system default location with the syntax: `#include <stdio.h>`, or in our local directory: `#include "gpio.h"`.
- Which we can include in our program so that it can call the APIs which we have implemented in the header files.
- Here are the APIs for the GPIO functions which are in the gpio.h file:
  - `void gpio_init(int pin, int direction);`
  - `void gpio_write(int pin, int value);`
  - `int gpio_read(int pin);`
- Which can act as input as well as output with 0 and 1, and it has been implemented as macros.
- We used macros because it's easy to change the GPIO pin which we needed for the specific task.
- In the above example, we have used PIN 5 and PIN 3, respectively, as OUTPUT and as INPUT.
- Also, this does not apply to the microcontroller board yet; the structure which we are going through will remain the same, where gpio.c acts as a driver for the GPIO peripheral as it has function definitions, and gpio.h has the function declarations.
- And we have linked the main.c and gpio.c files together to make an executable file because we have linked the gpio.c file via static linking, and gpio.h and stdio.h files get linked automatically as dynamically linked.

---



