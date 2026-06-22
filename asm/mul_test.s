.global _start
.text
_start:
  mov x0, #3
  mov x1, #23
  mul x0, x0, x1

  mov, x8, #93
  svc #0
