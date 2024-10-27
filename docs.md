# Process Management System Documentation

## Overview
This system implements a basic process management and scheduling system in C, featuring memory management, process states, thread-based execution, and synchronization mechanisms. The system manages processes in a stack-like memory structure with thread-safe operations.

## Core Components

### Data Structures

#### 1. Memory Block (struct Mem_Block)
- Purpose: Represents a block of memory in the process memory space
- Components:
  - process: Pointer to the Process structure
  - next_b: Pointer to the next memory block
  - previous_b: Pointer to the previous memory block

#### 2. Kernel Information (struct Kernel_Info)
- Purpose: Tracks kernel memory usage
- Components:
  - kernel_stack_mem: Total available kernel stack memory
  - kernel_stack_used: Currently used kernel stack memory

#### 3. Process Context (struct Context)
- Purpose: Maintains process register information
- Registers tracked:
  - eip: Instruction pointer
  - esp: Stack pointer
  - General purpose registers: eax, ebx, ecx, edx
  - Index registers: esi, edi
  - Base pointer: ebp

#### 4. Process (struct Process)
- Purpose: Main process structure
- Components:
  - size: Process size
  - state: Current process state (READY, RUNNING, etc.)
  - pid: Process ID
  - context: Register context information

### Process States (enum P_State)
- READY: Process is ready for execution
- RUNNING: Process is currently executing
- SLEEPING: Process is in sleep state
- UNUSED: Process is not in use
- FINISHED: Process has completed execution

## Key Functions

### Process Management

#### createProcess(unsigned int id)
- Purpose: Creates a new process with specified ID
- Parameters:
  - id: Process identifier (must be non-zero)
- Returns: Pointer to new Process structure or NULL on failure
- Notes: Initializes process with READY state and default context values

#### push_P(struct Process * p, struct Kernel_Info * kernel_stack_info)
- Purpose: Pushes process onto memory stack
- Parameters:
  - p: Pointer to process
  - kernel_stack_info: Pointer to kernel information
- Returns: 0 on success, -1 on failure
- Thread-safe: Yes (uses process_mutex)

#### kill_P(struct Process *p, struct Kernel_Info *kernel_stack_info)
- Purpose: Terminates and removes process from memory
- Parameters:
  - p: Pointer to process to terminate
  - kernel_stack_info: Pointer to kernel information
- Thread-safe: Yes (uses process_mutex)

### Execution Management

#### run_P(struct Process *p)
- Purpose: Executes a single process
- Parameters:
  - p: Pointer to process to execute
- Behavior: Runs process for 10 seconds, updating state accordingly
- Thread-safe: Yes (uses process_mutex)

#### run_process_threads(struct Process *p_array[], size_t len)
- Purpose: Executes multiple processes concurrently
- Parameters:
  - p_array: Array of process pointers
  - len: Length of process array
- Creates and manages threads for concurrent process execution

### Memory Management

#### clean_memoryBlocks()
- Purpose: Cleans up all memory blocks
- Frees all allocated memory blocks in the stack

## System Constants
- MEMORY_LIMIT: 1024 bytes
- MEM_BLOCK_SIZE: Size of Memory Block structure

## Threading and Synchronization
- Uses pthread library for thread management
- process_mutex ensures thread-safe operations on shared resources
- Implements proper locking mechanisms in critical sections

## Error Handling
- Input validation on all function parameters
- Memory allocation checks
- Thread creation and joining error checks
- Appropriate error messages via stderr

## Usage Example
```c
// Initialize kernel info
struct Kernel_Info *kernel_stack_info = malloc(sizeof(struct Kernel_Info));
kernel_stack_info->kernel_stack_mem = MEMORY_LIMIT;
kernel_stack_info->kernel_stack_used = 0;

// Create processes
struct Process *p1 = creatProcess(1);

// Push to memory stack
push_P(p1, kernel_stack_info);

// Execute
struct Process *p_array[] = {p1};
run_process_threads(p_array, 1);

// Cleanup
kill_P(p1, kernel_stack_info);
clean_memoryBlocks();
free(kernel_stack_info);
```

## Future Improvements
- TODO: Implementation of Round Robin scheduling algorithm (RRschedule function)
- Potential improvements to memory management
- Enhanced error handling and recovery mechanisms

## Limitations
- Fixed memory limit of 1024 bytes
- Basic process execution simulation (10-second runtime)
- Limited process states and transitions
- No priority scheduling implemented

## Notes
- Thread safety is maintained throughout the codebase
- Memory management requires careful attention to prevent leaks
- Process states must be properly managed for successful execution
