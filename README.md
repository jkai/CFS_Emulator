# CFS_Emulator

Completely Fair Share Scheduler emulation using threads

# Introduction

This assignment emulates Linux Completely Fair Share Scheduler (FSS) emulation using threads, due to time limitation the functionalities are defined roughly.

### 	-Producer

Producer will generate required number of processes and put them into 4 cores' queues (RQ0, RQ1, and RQ2).

### 	-Consumer (Core)

Single consumer acts as a CPU core, and consumes processes in its own queues based on defined scheduling policy.

### 	-Balancer

Balancer wakes up every 3s and check if there's an unbalance. If so, it will move one process from the CPU has longest queue to the CPU has the least.

# Usage
# CFS_Emulator

Completely Fair Share Scheduler emulation using threads

# Introduction

This assignment emulates Linux Completely Fair Share Scheduler (FSS) emulation using threads, due to time limitation the functionalities are defined roughly.

### 	-Producer

Producer will generate required number of processes and put them into 4 cores' queues (RQ0, RQ1, and RQ2).

### 	-Consumer (Core)

Single consumer acts as a CPU core, and consumes processes in its own queues based on defined scheduling policy.

### 	-Balancer

Balancer wakes up every 3s and check if there's an unbalance. If so, it will move one process from the CPU has longest queue to the CPU has the least.

# Usage

A makefile is provided, use make all to compile.

The main program can be started as:
./CFS_Emulator

# For grader# CFS_Emulator

Completely Fair Share Scheduler emulation using threads

# Introduction

This assignment emulates Linux Completely Fair Share Scheduler (FSS) emulation using threads, due to time limitation the functionalities are defined roughly.

### 	-Producer

Producer will generate required number of processes and put them into 4 cores' queues (RQ0, RQ1, and RQ2).

### 	-Consumer (Core)

Single consumer acts as a CPU core, and consumes processes in its own queues based on defined scheduling policy.

### 	-Balancer

Balancer wakes up every 3s and check if there's an unbalance. If so, it will move one process from the CPU has longest queue to the CPU has the least.

# Usage

A makefile is provided, use make all to compile.

The main program can be started as:
./CFS_Emulator

# For grader

Due to the large number of outputs as prof required, verify is in a log file can be handy, it can be done by:

./CFS_Emulator > log.txt

Have good winter break!

Due to the large number of outputs as prof required, verify is in a log file can be handy, it can be done by:

./CFS_Emulator > log.txt


A makefile is provided, use make all to compile.

The main program can be started as:
./CFS_Emulator

# For grader

Due to the large number of outputs as prof required, verify is in a log file can be handy, it can be done by:

./CFS_Emulator > log.txt

Have good winter break!