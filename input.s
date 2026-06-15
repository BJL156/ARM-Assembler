.global _start
_start:
    mov x0, #10
    mov x1, #20
    add x0, x0, x1
    mov x1, #5
    sub x0, x0, x1
    mov x8, #93
    svc #0
