# Kernel-Process-Manager-
-under development 

## Description
This project simulates how a kernel manages processes
Each individual thread represents a single task/process(for now) :)

## Requirements 
-C version c99

## Installation 
1.Clone the repository : git clone https://github.com/ChrinovicMu/Kernel-Process-Manager-.git  
2.Navigate to the directory: 'cd Kernel-Process-Manager-'

## Usage 
commands:  
-'make build'- to compile  
-'make run'- to run

## Function Descriptions
Below is a breakdown of thw key Functions in the main code:


### creatProcess()
-creates a single process, taking in process id and size as arguments.  
-Note : using 'size' and 'process id' arguments are just for testing , will fix later

### push_P()
-pushes a process onto the kernel stack if conditions have been met

### run_P()
-executes a process if conditions have been met 

### run_process_thread()
-executes the process using run_P as a single thread

### kill_P()
-looks for a process id in the process data structure, free up the memory of both the process and it's assigned memory block

