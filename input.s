.global _start
_start:
  mov x0, #5
loop:
  sub x0, x0, #1
  cmp x0, #0
  b.ne loop
done:
  mov x8, #93
  svc #0
