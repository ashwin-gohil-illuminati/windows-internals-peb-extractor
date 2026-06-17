![Language](https://img.shields.io/badge/Language-C++-blue.svg)
![Core](https://img.shields.io/badge/Core-Windows_Internals-red.svg)

---

## 📂 Repository 1: Process Environment Block (PEB) Extractor

### 📝 Description
When an executable launches, the Windows Kernel (`ntoskrnl.exe`) creates a massive, protected data structure in Kernel Space (Ring 0) called the `EPROCESS` block. Because user-mode (Ring 3) applications cannot access this without crashing, the Kernel maps a smaller, secondary structure directly into the application's memory. This is the **Process Environment Block (PEB)**. This program completely bypasses standard Win32 API calls, using raw compiler intrinsics and pointer arithmetic to locate and read the PEB directly from CPU registers.

### 🎯 Objective
To tangibly prove the existence of the PEB architecture by extracting the internal memory address of the process and manually reading the undocumented `BeingDebugged` byte flag without relying on standard safety-net APIs like `IsDebuggerPresent()`.

### 🧠 Major Underlying Concepts
* **Execution Containers & Privilege Rings:** Understanding the strict physical wall between Ring 0 (Kernel Mode, where `EPROCESS` lives) and Ring 3 (User Mode, where our application and the PEB live). 
* **CPU Registers & The TEB:** On 64-bit Windows, the `gs` segment register holds a pointer to the current thread's Thread Environment Block (TEB). The PEB is reliably located at an exact, predictable offset (`0x60`) inside that TEB.
* **Systems-Level Pointer Arithmetic:** * **Pointer Size vs. Payload:** Recognizing that all pointers on a 64-bit OS are strictly 64 bits wide. Casting a memory address to a `BYTE*` does not truncate the 64-bit address; it simply tells the compiler that the payload at that address is 1 byte wide.
  * **Absolute Byte Offsets:** In systems programming, an "offset" is always measured in absolute, single bytes. By casting the raw 64-bit memory coordinate to a `BYTE*`, we force the C++ compiler to step forward in exact 1-byte increments (e.g., `pPeb[2]`), landing flawlessly on the 3rd byte (Offset `0x02`) where the `BeingDebugged` flag is buried.
* **Reinterpret Casting Raw Memory:** Utilizing the `__readgsqword(0x60)` intrinsic to fetch a raw 64-bit memory coordinate (`unsigned __int64`), and aggressively casting that raw integer into a structure blueprint (`PPEB` from `winternl.h`) to map a C++ template over live physical RAM.

### 🛠️ Tool Usage & Physical Justification
* **Tool Used:** `x64dbg` (Instruction-level Debugger)
* **What to Look For:** 1. Executing the program normally via the desktop yields the output: `Is Debugger Present: No`.
  2. Dragging the executable directly into `x64dbg` and running it until the console populates.
* **Physical Justification:** When the program is executed inside `x64dbg`, the output changes to `Is Debugger Present: Yes`. This proves that we are actively reading OS-managed memory. The moment the Windows Kernel intercepted the process creation and hooked the debugger, it dynamically flipped the bit at offset `0x02` in our process's PEB from `0` to `1`. We observed the Kernel altering our process architecture in real-time.

---
