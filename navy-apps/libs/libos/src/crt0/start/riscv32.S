.globl  _start
_start:
  move s0, zero
  move sp, a0 # to support context switch
  # # DONE: move &argc to a0
  # &argc and heap top both need to be passed in a0, so we set &argc into a0 in nanos_lite
  jal call_main
