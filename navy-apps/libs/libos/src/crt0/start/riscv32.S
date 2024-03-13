.globl  _start
_start:
  move s0, zero
  # move sp, a0 # to support context switch
  jal call_main
