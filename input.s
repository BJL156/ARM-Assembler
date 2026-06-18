.global _start
_start:
  mov x0, #99
  sub sp, sp, #16
  str x0, [sp, #0]
  mov x0, #0
  ldr x0, [sp, #0]
  mov x8, #93
  svc #0
