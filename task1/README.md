# Task 1: Firmware Library Fundamentals

## Deadline
4 days from the task announcement date.

---

## Objective

The goal of Task 1 is to:

- Understand how firmware libraries are structured  
- Understand how application code uses APIs  
- Set up a basic development environment  
- Build and study a simple firmware-style library  

This task does **not** require the VSDSquadron Mini board.

---


## What to Do

### Step 1: Read the Code

- Open `gpio.h` and understand the exposed API  
- Open `gpio.c` and see how the API is implemented  
- Open `main.c` and observe how the API is used by the application  

---

### Step 2: Build the Code

From inside the `task1` directory, run:

```bash
gcc main.c gpio.c -o task1_demo
````

---

### Step 3: Run the Program

```bash
./task1_demo
```

---

## Expected Output

Console messages simulating GPIO behavior.

---

## What to Submit

1. A short explanation (Markdown or PDF) covering:

   * What is a firmware library?
   * Why are APIs important?
   * What you understood from this task

2. Screenshots of:

   * Successful compilation
   * Program output

3. GitHub repository link (forked or personal)

---

## Important

This task sets the foundation for all upcoming hardware and firmware work.
Incomplete understanding here **will cause difficulties in later tasks**.


