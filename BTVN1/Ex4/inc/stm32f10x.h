#ifndef STM32F10X_H
#define STM32F10X_H

#include <stdint.h>



#define RCC_BASE        0x40021000UL

#define RCC_APB2ENR     (*(volatile uint32_t *)(RCC_BASE + 0x18UL))




#define GPIOA_BASE      0x40010800UL

#define GPIOA_CRL       (*(volatile uint32_t *)(GPIOA_BASE + 0x00UL))
#define GPIOA_CRH       (*(volatile uint32_t *)(GPIOA_BASE + 0x04UL))
#define GPIOA_IDR       (*(volatile uint32_t *)(GPIOA_BASE + 0x08UL))
#define GPIOA_ODR       (*(volatile uint32_t *)(GPIOA_BASE + 0x0CUL))
#define GPIOA_BSRR      (*(volatile uint32_t *)(GPIOA_BASE + 0x10UL))
#define GPIOA_BRR       (*(volatile uint32_t *)(GPIOA_BASE + 0x14UL))




#define GPIOB_BASE      0x40010C00UL

#define GPIOB_CRL       (*(volatile uint32_t *)(GPIOB_BASE + 0x00UL))
#define GPIOB_CRH       (*(volatile uint32_t *)(GPIOB_BASE + 0x04UL))
#define GPIOB_IDR       (*(volatile uint32_t *)(GPIOB_BASE + 0x08UL))
#define GPIOB_ODR       (*(volatile uint32_t *)(GPIOB_BASE + 0x0CUL))
#define GPIOB_BSRR      (*(volatile uint32_t *)(GPIOB_BASE + 0x10UL))
#define GPIOB_BRR       (*(volatile uint32_t *)(GPIOB_BASE + 0x14UL))




#define GPIOC_BASE      0x40011000UL

#define GPIOC_CRL       (*(volatile uint32_t *)(GPIOC_BASE + 0x00UL))
#define GPIOC_CRH       (*(volatile uint32_t *)(GPIOC_BASE + 0x04UL))
#define GPIOC_IDR       (*(volatile uint32_t *)(GPIOC_BASE + 0x08UL))
#define GPIOC_ODR       (*(volatile uint32_t *)(GPIOC_BASE + 0x0CUL))
#define GPIOC_BSRR      (*(volatile uint32_t *)(GPIOC_BASE + 0x10UL))
#define GPIOC_BRR       (*(volatile uint32_t *)(GPIOC_BASE + 0x14UL))




#define GPIO_MODE_INPUT             0x0U
#define GPIO_MODE_OUTPUT_10MHz      0x1U
#define GPIO_MODE_OUTPUT_2MHz       0x2U
#define GPIO_MODE_OUTPUT_50MHz      0x3U




#define GPIO_CNF_INPUT_ANALOG       0x0U
#define GPIO_CNF_INPUT_FLOATING     0x1U
#define GPIO_CNF_INPUT_PULL         0x2U

#define GPIO_CNF_OUTPUT_PP          0x0U
#define GPIO_CNF_OUTPUT_OD          0x1U
#define GPIO_CNF_AF_PP              0x2U
#define GPIO_CNF_AF_OD              0x3U




#define LED_PIN         13U


#endif