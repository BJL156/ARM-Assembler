.global _start
.text
_start:
  mov x0, #67
  neg x0, x0
  neg x0, x0
  mov x8, #93
  svc #0
