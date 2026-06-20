.global _start
.data
  msg: .asciz "Hello, world!\n"

.text
_start:
  adr x1, msg
  mov x2, #14
  mov x0, #1
  mov x8, #64
  svc #0

  mov x0, #0
  mov x8, #93
  svc #0
