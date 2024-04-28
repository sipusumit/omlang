global _start

extern ExitProcess
extern bigBang

section .text

  _start:

    call bigBang
    call ExitProcess