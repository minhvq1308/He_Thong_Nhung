.syntax unified
.cpu cortex-m3
.thumb

.global Reset_Handler

/* Vector Table */
.section .isr_vector, "a", %progbits

.word 0x20005000
.word Reset_Handler


/* Reset Handler */
.section .text.Reset_Handler
.type Reset_Handler, %function

Reset_Handler:
    bl main

loop:
    b loop