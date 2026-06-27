.global _start
.text
_start:
  stp x29, x30, [sp, #-16]!
  mov x29, sp
  mov x0, #42
  ldp x29, x30, [sp], #16
  mov x8, #93
  svc #0
