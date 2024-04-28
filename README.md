# OmLang

A Toy Compiler Built using [LLVM](https://llvm.org/)
# Usage
``` console
$ om ./examples/main.om
$ ./a
```
## Installation

### Windows
Download the latest windows installer from [release page](https://github.com/sipusumit/omlang/releases/). Get the latest version, which is usually at the top.
    Once Downloaded run the installer and install it
    
## Usage/Examples

```javascript
#use "std";

fun main(): int{
  let message:string = "What is your name\n";
  cosmicSpit(message);
  let name:string = galacticReceive();
  cosmicSpit("Hello ");
  cosmicSpit(name);
  return 0;
}
```

```shell
$ om main.om -o main.exe
$ main.exe
```

## Build LLVM
```
 $ cmake -DCMAKE_BUILD_TYPE:STRING=Release -DCMAKE_C_COMPILER:FILEPATH=D:\ProgramFiles\mingw64\bin\gcc.exe -DCMAKE_CXX_COMPILER:FILEPATH=D:\ProgramFiles\mingw64\bin\g++.exe -S./llvm -B./build -G "Ninja" -DLLVM_ENABLE_PROJECTS="clang" -DLLVM_TARGETS_TO_BUILD="X86" -DLLVM_INCLUDE_TESTS=OFF
$ cd .\build\
$ cmake --build .
```