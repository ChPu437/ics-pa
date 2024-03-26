.globl  _start
_start:
  move s0, zero
  # to support context switch
  move sp, a0
  jal call_main
