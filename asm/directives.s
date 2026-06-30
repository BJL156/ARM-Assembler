.global _start
.data
  val: .quad 0
  buf: .space 16
  msg: .asciz "hello"
.text
_start:
  mov x0, #0
  mov x8, #93
  svc #0
