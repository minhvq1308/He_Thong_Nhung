.syntax unified
.cpu cortex-m3
.thumb

.section .isr_vector,"a",%progbits
.type g_pfnVectors, %object

g_pfnVectors:
    .word _estack
    .word Reset_Handler
    .word Default_Handler
    .word Default_Handler
    .word Default_Handler
    .word Default_Handler
    .word Default_Handler
    .word 0
    .word 0
    .word 0
    .word 0
    .word Default_Handler
    .word Default_Handler
    .word 0
    .word Default_Handler
    .word Default_Handler

.section .text.Reset_Handler
.weak Reset_Handler
.type Reset_Handler, %function

Reset_Handler:
    ldr r0, =_sdata
    ldr r1, =_edata
    ldr r2, =_sidata

CopyData:
    cmp r0, r1
    bcc CopyDataLoop
    b ClearBss

CopyDataLoop:
    ldr r3, [r2], #4
    str r3, [r0], #4
    b CopyData

ClearBss:
    ldr r0, =_sbss
    ldr r1, =_ebss
    movs r2, #0

ClearBssLoop:
    cmp r0, r1
    bcc ClearBssWrite
    b StartMain

ClearBssWrite:
    str r2, [r0], #4
    b ClearBssLoop

StartMain:
    bl main

LoopForever:
    b LoopForever

.section .text.Default_Handler
Default_Handler:
    b Default_Handler