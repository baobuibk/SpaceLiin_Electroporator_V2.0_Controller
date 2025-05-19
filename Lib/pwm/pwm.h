#ifndef PWM_H_
#define PWM_H_

#include <stdint.h>

#define MY_CORE_H_
#define MY_RCC_H_
#define MY_TIM_H_
#include "stm32f4_header.h"

#define APB1_TIMER_CLK \
 __LL_RCC_CALC_PCLK1_FREQ(__LL_RCC_CALC_HCLK_FREQ(SystemCoreClock, 2), LL_RCC_GetAPB1Prescaler())

typedef struct
{
    TIM_TypeDef *TIMx;
    uint32_t    Channel;
    uint32_t    Mode;
    uint32_t    Polarity;
    uint16_t    Prescaler;
    uint32_t    Duty;
    uint32_t    Freq;
}PWM_TypeDef;


void PWM_Init(PWM_TypeDef *PWMx);
void PWM_Enable(PWM_TypeDef *PWMx);
void PWM_Disable(PWM_TypeDef *PWMx);
void PWM_Set_Duty(PWM_TypeDef *PWMx, uint32_t _Duty);
void PWM_Set_Freq(PWM_TypeDef *PWMx, uint32_t _Freq);

#endif /* PWM_H_ */
