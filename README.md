# Global Atom Manager
A simple GUI utility used to inspect and manage the system's [global Atom Table](https://learn.microsoft.com/en-us/windows/win32/dataxchg/about-atom-tables#global-atom-table). This utility allows you to view a list of global atoms, as well as to add new global atoms or delete existing ones.

> [!CAUTION]
> Take caution when **deleting a global atom**. Deleting an atom that belongs to a running application will prevent that application from using it, possibly **leading to an application crash or system instability**. Note that in the context of this utility, to delete an atom doesn't mean to decrement its reference count, but **to remove that atom entirely from the global atom table**.

The file `GlobalAtomManager.hta` is the main file that must be opened to start the application. It is accompanied by a DLL file named `GlobalAtomManager.dll`, which you obtain by building the C++ source code in this repository.

This application is based on the **Active Station** technology. Therefore, to compile the source code of `GlobalAtomManager.dll`, it is required that the static library files `libstation.x86.lib` and `libstation.x64.lib` be present in the project directory. To obtain these files, head to [the ActiveStation repository](https://github.com/javadbayat/ActiveStation), and compile the static library's source code from that repository.
