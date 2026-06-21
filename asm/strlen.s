.global _start
.data
  msg: .asciz "This string is 34 characters long.\0"

.text
strlen:
  mov x1, #0
  loop:
    ldrb x2, [x0, #0]
    cbz x2, done
    add x1, x1, #1
    add x0, x0, #1
    b loop
  done:
    ret

_start:
  adr x0, msg
  bl strlen

  mov x0, x1
  mov x8, #93
  svc #0
