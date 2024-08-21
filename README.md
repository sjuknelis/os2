# os2

## Features

* **Pre-emptive multitasking**: Tasks are periodically interrupted to allow other tasks to run for fixed time slices.
* **Segmentation**: Each task has a distinct memory segment assigned by the kernel. Each task can access its RAM segment at the same virtual addresses regardless of their physical location.
* **Privilege levels**: Tasks cannot directly access hardware resources and must instead make syscalls to the kernel.
* **Interrupt I/O**: Keyboard input triggers an interrupt and is immediately received
* **Task blocking**: Tasks waiting on input are blocked and do not consume CPU time.
* **Memory allocator**: Simple dynamic memory allocator provided for task use.

## Project layout

`emu` contains the Fake6502 C emulator together with code implementing various hardware features. `proj` contains the actual operating system. `run.sh` compiles both and runs the emulator with the system as its ROM.

## Overview

The total RAM accessible to the system is $80000 bytes. Since the address space of the 6502 is $10000 bytes, of which the upper half is used to access ROM and I/O devices, the RAM is broken into 16 $8000-byte banks. Writing to the memory bank hardware register determines which bank is accessible to the CPU and effective "swaps out" one bank for another. This allows each bank to function as a segmented virtual memory space assigned by the system for each task. For example, if a task's code writes to address $0200, then this will function as a write to real address $08200 if the bank register was set to 1, or real address $10200 if it was set to 2.

To prevent a task from accessing data belonging to the kernel or another task, there is an I/O lockout flag which disables writing to the memory bank register or to any other I/O port. This flag can be set by writing to a certain I/O port, and once set, it is only cleared when a non-maskable interrupt (NMI) is triggered. The NMI causes execution to switch from task code to the interrupt handler in the kernel within one instruction. Therefore, as long as the kernel sets the lockout flag before passing control to a task, there is no way for the task to access I/O devices except through the kernel.

An NMI is triggered:
1. When a task writes to the I/O port (not subject to the lockout flag) that triggers one
2. When a hardware timer fires (so a task cannot spin and take up all CPU time)
3. When keyboard input is received

The I/O lockout flag being cleared inhibits an NMI from firing to prevent this from happening while kernel code is running.

When a task wants to access hardware resources, it must perform a syscall by writing a syscall code to $7e00. The code will be received and processed by the kernel at the next NMI (which can be immediately triggered by the task). The task can also provide arguments in the buffer at $7e01-$7eff, and any return data from the syscall will be stored in that buffer after the next NMI completes and CPU time is given back to the task.

## Implementation

### Basic Memory Layout

|Addresses|Usage|
|-|-|
|$0000-$7fff|Swappable RAM|
|$8000-$8fff|I/O Ports|
|$9000-$ffff|ROM|

### I/O Ports

|Address|Port|Description|
|-|-|-|
|$8000|Display Out|On write, prints the provided byte as an ASCII character|
|$8001|Keyboard In|On read, provides the last ASCII character entered on keyboard; on write, value cleared to 0|
|$8002|Memory Bank|On write, sets the RAM memory bank register|
|$80fe|NMI Trigger|On write, fires an NMI|
|$80ff|I/O Lockout|On write, sets the I/O lockout flag|

### Task Space Layout (Banks $1-$f)

All RAM except the following locations is available for task space use.

|Addresses|Data|Description|
|-|-|-|
|$7e00|Syscall code|Index of kernel function to be called; no function called when set to 0|
|$7e01-$7eff|Syscall data|Argument buffer to kernel function|
|$7f00|Syscall copy index|Used by kernel to copy data from kernel space|
|$7f01|Stored stack pointer|Used by kernel to save stack pointer on context switch|

### Syscall Codes

|Code|Name|Description|
|-|-|-|
|$01|`print`|Prints the string starting at $7e01|
|$02|`scan`|Blocks until newline entered; returns last line starting at $7e01|
|$03|`spawn`|Spawns a task with PC initialized to vector $7e01-$7e02|
|$04|`exit`|Terminates the running task|

### Kernel Space Layout (Bank $0)

|Location|Data|Description|
|-|-|-|
|$0000-$00fd|Usable zero page||
|$00fe-$00ff|Syscall copy vector|Used when copying syscall data from task|
|$6100-$6fff|Syscall data storage|Task 1 syscall data stored from $6100-$61ff, task 2 data from $6200-$62ff|
|$7d00-$7dff|Input data queue|NMI handler writes input bytes to circular queue|
|$7f02|Input data queue index||
|$7f03|Zero on NMI flag|NMI handler writes 0 on every interrupt|
|$7f10-$7f11|Task init vector|Used when initializing new task; first address for task to jump to|

### Syscall Protocol

1. Task writes syscall argument data in $7e01-$7eff
2. Task writes syscall code in $7e00
3. Task writes to $80fe, firing NMI

*I/O lockout cleared*

4. Interrupt handler copies from $7e00-$7eff in task's memory bank to appropriate location in kernel memory bank ($6000 + $100 * task MB index) ("kernel syscall page")
5. Interrupt handler switches context to kernel space
6. Syscall handler table indexed with provided syscall code and run (task possibly blocked)
7. Handler writes return data, if any, to appropriate kernel syscall page
8. Kernel copies from appropriate kernel syscall page to $7e00-$7eff in task's memory bank
9. Kernel switches context to task space and activates I/O lockout

*I/O lockout set*

10. Task sets $7e00 to 0, preventing double syscall
11. Task reads data returned by syscall from $7e01-$7eff