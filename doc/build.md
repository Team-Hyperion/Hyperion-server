# Building

Requirements:

* Compiler supporting c++17 [gcc >= 9, clang >= 9, msvc >= 142]
* CMake
* Bash interpreter

Remarks:

* Windows - Ensure CMake is added to the `Paths` environmental variable
* Windows - Use git bash to run the bash files
* MacOS - Version >= 10.15

---

**Build types:** `Debug, Release, RelWithDebInfo`

**Parameters:** `--notest` Do not build the tests

Executable will be placed in `out/<build_type>/src/`

---

In a bash shell, run `build.sh`

```bash
bash ./build.sh <build_type> <params>
```
