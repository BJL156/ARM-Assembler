.global _start
.text
  mov w0, #1
  mov w1, #2

  add w0, w0, w1
  mov x8, #93
  svc #0
