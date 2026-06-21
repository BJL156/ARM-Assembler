.global _start
.data
  path: .asciz "output.txt"
  msg: .asciz "Hello, world!\n"

.text
  mov x0, #-100
  adr x1, path
  mov x2, #0x241
  mov x3, #0x1A4
  mov x8, #56
  svc #0

  mov x9, x0

  mov x0, x9
  adr x1, msg
  mov x2, #14
  mov x8, #64
  svc #0

  mov x0, x9
  mov x8, #57
  svc #0

  mov x0, #0
  mov x8, #93
  svc #0
