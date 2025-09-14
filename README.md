Key-Value Store Daemon
Design a Multi-threaded Linux Key-Value Store Daemon
●
●
●
●
Problem Description
Design and implement a simple Linux storage daemon in C or C++ that provides:
A custom block-based key-value store
A command-line client to read/write keys
A backend file or device acting as the persistent storage
Requirements
1.Daemon Functionality
Run as a Linux daemon (use fork, setsid, etc.)
Accept client requests over:
Option A: UNIX domain sockets
Option B: Named pipes (FIFO)
Support commands:
PUT <key> <value>
GET <key>
DELETE <key>
2. Storage Backend
Fixed-size block file (e.g., 64MB)
Each key-value pair is stored in blocks
Simple free block management (free list or bitmap)
On-disk metadata for indexing keys
3. Concurrency
Handle multiple client connections concurrently
Use POSIX threads or epoll + thread pool
Ensure thread-safe access to shared structures
4. System Concepts
Use low-level POSIX I/O (open, read, write, mmap)
Demonstrate understanding of:
File descriptors
Signals (e.g., for shutdown)
Memory management
Synchronization (mutex, condvar, or spinlock)
Submission Guidelines:
What to Submit
1. Source Code
○
○
All .c / .cpp / .h files for both daemon and client
Any helper scripts (e.g., build or test runners)
○
Clean and modular code structure (use folders if needed)
2. Build Instructions
○
Include a Makefile or build.sh
○
Must work on a standard Linux system with:
■ GCC or Clang
■ Standard Linux/POSIX headers
■ No external dependencies unless discussed
3. Run Instructions
○
How to start the daemon
○
How to connect the client
Sample commands for:
■ PUT <key> <value>
■ GET <key>
■ DELETE <key>
4. Readme (README.md)
Include the following:
○
Overview of the design and key components
○
Data layout for storage backend (blocks, index, etc.)
○
Concurrency model (threads, locks used, if any)
○
Design trade-offs or assumptions
○
Known limitations (e.g., max key size, crash recovery status)
5. Test Results
○
Output from sample test runs (can be in text file or included in README)
○
Optionally include a script like test.sh or test
_
cases.txt to validate basic functionality
6. Bonus (Optional but Valued)
○
Code comments explaining critical sections
○
Metrics/log output (puts, gets, time taken)
○
Async design or thread pool
○
Use of mmap, epoll, or signals
What Not to Submit
●
●
●
Large binaries or build artifacts (*
.o, a.out, etc.)
External packages or dependency managers unless explicitly allowed
GUI tools or non-POSIX/C-based client tools (no Python/Node/etc.)
Submission Format
Preferred: ZIP or TAR.GZ archive (e.g., linux
_
storage
daemon
_
_
<yourname>.tar.gz)
Include everything in a single top-level folder:
linux
_
storage
_
daemon/
├── src/
├── Makefile
├── README.md
├── test.sh
└── client
_
tool/
