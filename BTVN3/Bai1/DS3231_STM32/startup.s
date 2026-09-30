.syntax unified
.cpu cortex-m3
.thumb

.global g_pfnVectors
.global Reset_Handler

.extern main

.section .isr_vector, "a", %progbits

g_pfnVectors:

    .word _estack
    .word Reset_Handler

    .word NMI_Handler
    .word HardFault_Handler
    .word MemManage_Handler
    .word BusFault_Handler
    .word UsageFault_Handler

    .word 0
    .word 0
    .word 0
    .word 0

    .word SVC_Handler
    .word DebugMon_Handler
    .word 0
    .word PendSV_Handler
    .word SysTick_Handler


.section .text.Reset_Handler
.thumb_func

Reset_Handler:

    /* Copy .data from Flash to RAM */

    ldr r0, =_sidata
    ldr r1, =_sdata
    ldr r2, =_edata

copy_data:

    cmp r1, r2
    bcc copy_loop

    b start_main

copy_loop:

    ldr r3, [r0]
    str r3, [r1]

    add r0, r0, #4
    add r1, r1, #4

    b copy_data


start_main:

    bl main


hang:

    b hang


NMI_Handler:

HardFault_Handler:

MemManage_Handler:

BusFault_Handler:

UsageFault_Handler:

SVC_Handler:

DebugMon_Handler:

PendSV_Handler:

SysTick_Handler:

    b .