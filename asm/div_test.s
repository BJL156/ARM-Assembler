.global _start
.text
_start:
  mov x0, #10
  mov x1, #2
  udiv x0, x0, x1
  mov x8, #93
  svc #0
