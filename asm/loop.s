.global _start
_start:
  mov x0, #0
  mov x1, #5
loop:
  add x0, x0, #1
  sub x1, x1, #1
  cbnz x1, loop
done:
  mov x8, #93
  svc #0
