#include <iostream>
#include <windows.h>
#include <intrin.h>
#include <winternl.h> //the use shown below

/*
First refer the coursework for this program

Compilation instructions :
x86_64-w64-mingw32-g++ peb_reader.cpp -o peb_reader.exe -O0 -g -static

The above is compiled under linux and the executable is tested in windows enterprise ltsc vm.

*/

int main(){
    std::cout << "[*] Bypassing Win32 APIs. Extracting PEB directly via CPU registers..." << std::endl;
    
    // In 64-bit Windows, the 'gs' register points to the TEB.
    // The PEB pointer is located exactly at offset 0x60 inside the TEB.

    PVOID pebAddress = (PVOID)__readgsqword(0x60);
    std::cout << "[+] PEB Base Address: " << pebAddress << std::endl;

    //pebAddress is basically a number and exactly a memory address
    //In basic , when you create int *p;, and print by cout << p will show the memory address

    // The PEB is a struct. At offset 0x02 of this struct is a 1-byte boolean 
    // called 'BeingDebugged'. The OS sets this to 1 if a debugger is attached.

    //BYTE* is a pointer to a byte. It acts as a memory address pointing to the
    //beginning of a sequence of 8-bit data chunks (an unsigned 8-bit integer). 

    BYTE* pPeb = (BYTE*)pebAddress; // this address is the memory address
    BYTE beingDebugged = 0;
    beingDebugged = *(pPeb+2); // also can be done as BYTE beingDebugged = pPeb[2];
    /*
    Because we want to move in the offsets of 1 byte as a rule in windows programming. pebAddress
    is already a pointer of type PVOID
    */

    // Another method.

    // 1. Create a local pointer variable named 'myPeb' using the PPEB blueprint, 
    // and initialize it with that address.
    PPEB myPeb = (PPEB)pebAddress;

    // 2. Now you can safely use the arrow operator!
    BYTE debuggerFlag = myPeb->BeingDebugged;

    std::cout << "[+] Is Debugger Present (PEB->BeingDebugged): " << (beingDebugged ? "Yes" : "No") << std::endl;
    std::cout << "[*] Press Enter to exit..." << std::endl;
    std::cin.get();
    
    return 0;
}
