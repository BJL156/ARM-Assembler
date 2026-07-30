.global _start
_start:
  mov x0, #0x0042
  movk x0, #0x0001, lsl #16
  mov x8, #93
  svc #0
