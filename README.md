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
  let name:message = "What is your name\n";
  print(message);
  let name:string = input();
  print("Hello ");
  print(name);
  return 0;
}
```

```shell
$ om main.om -o main.exe
$ main.exe
```