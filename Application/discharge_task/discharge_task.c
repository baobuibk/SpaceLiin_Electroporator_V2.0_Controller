/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Include~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include <stdint.h>
#include <stdbool.h>

#define MY_GPIO_H_
#include "stm32f4_header.h"

//#include "pwm.h"

#include "discharge_task.h"

#include "app.h"

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Defines ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Enum ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Struct ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Class ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Private Types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/* :::::::::: PID Task State ::::::::::::: */
// typedef enum
// {
// 	DISCHARGE_OFF_STATE,
//     DISCHARGE_START_STATE,
// } Discharge_State_typedef;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
// static Discharge_State_typedef Discharge_State = DISCHARGE_OFF_STATE;

//static 	PWM_TypeDef Discharge_300V_PWM;
//static 	PWM_TypeDef	Discharge_50V_PWM;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
// bool g_is_Discharge_300V_On = false;
// bool g_is_Discharge_50V_On  = false;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* :::::::::: Discharge Task Init ::::::::::::: */
void Discharge_Task_Init(void)
{
	//PWM_Init(&Discharge_300V_PWM, DISCHARGE_300V_HANDLE, DISCHARGE_300V_CHANNEL, LL_TIM_OCMODE_PWM1, LL_TIM_OCPOLARITY_HIGH);
    //PWM_Init(&Discharge_50V_PWM, DISCHARGE_50V_HANDEL, DISCHARGE_50V_CHANNEL, LL_TIM_OCMODE_PWM1, LL_TIM_OCPOLARITY_HIGH);

    // LL_GPIO_ResetOutputPin(DISCHARGE_300V_PORT, DISCHARGE_300V_PIN);
    // LL_GPIO_ResetOutputPin(DISCHARGE_50V_PORT, DISCHARGE_50V_PIN);
}

/* :::::::::: Discharge Task ::::::::::::: */
void Discharge_Task(void*)
{
    // switch (Discharge_State)
    // {
    // case DISCHARGE_OFF_STATE:
    //     LL_GPIO_ResetOutputPin(DISCHARGE_300V_PORT, DISCHARGE_300V_PIN);
    //     LL_GPIO_ResetOutputPin(DISCHARGE_50V_PORT, DISCHARGE_50V_PIN);

    //     if ((g_is_Discharge_300V_On == true) || (g_is_Discharge_50V_On == true))
    //     {
    //         Discharge_State = DISCHARGE_START_STATE;
    //     }

    //     break;
    // case DISCHARGE_START_STATE:
    //     if ((g_is_Discharge_300V_On == false) || (g_Feedback_Voltage[0] = 0))
    //     {
    //         LL_GPIO_ResetOutputPin(DISCHARGE_300V_PORT, DISCHARGE_300V_PIN);
    //         //Discharge_State = DISCHARGE_OFF_STATE;
    //     }
    //     else if (g_is_Discharge_300V_On == true)
    //     {
    //         LL_GPIO_SetOutputPin(DISCHARGE_300V_PORT, DISCHARGE_300V_PIN);
    //     }

    //     if ((g_is_Discharge_50V_On == false) || (g_Feedback_Voltage[1] = 0))
    //     {
    //         LL_GPIO_ResetOutputPin(DISCHARGE_50V_PORT, DISCHARGE_50V_PIN);
    //         //Discharge_State = DISCHARGE_OFF_STATE;
    //     }
    //     else if (g_is_Discharge_50V_On == true)
    //     {
    //         LL_GPIO_SetOutputPin(DISCHARGE_50V_PORT, DISCHARGE_50V_PIN);
    //     }

    //     if (((g_is_Discharge_300V_On == false) && (g_is_Discharge_50V_On == false)) || 
    //         ((g_Feedback_Voltage[0] = 0) && (g_Feedback_Voltage[1] = 0)))
    //     {
    //         LL_GPIO_ResetOutputPin(DISCHARGE_300V_PORT, DISCHARGE_300V_PIN);
    //         LL_GPIO_ResetOutputPin(DISCHARGE_50V_PORT, DISCHARGE_50V_PIN);
    //         Discharge_State = DISCHARGE_OFF_STATE;
    //     }

    //     break;
    // default:
    //     break;
    // }
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ End of the program ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
