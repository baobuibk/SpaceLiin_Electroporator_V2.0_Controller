/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Include~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include <stdint.h>
#include <stdbool.h>

#define MY_GPIO_H_
#include "stm32f4_header.h"

#include "pid.h"
#include "pwm.h"

#include "pid_task.h"

#include "app.h"

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Defines ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Enum ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Struct ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Class ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Private Types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/* :::::::::: PID Task State ::::::::::::: */
typedef enum
{
	PID_OFF_STATE,
    PID_CAP_CHARGE_STATE,
    PID_H_BRIDGE_STATE,
} PID_State_typedef;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
// bool is_300V_notified_enable = false;
// bool is_50V_notified_enable = false;

extern bool is_measure_impedance_enable;

extern uart_stdio_typedef  	GPP_UART;
//static PID_State_typedef 	PID_State = PID_CAP_CHARGE_STATE;
extern uint16_t 			Impedance_Period;
extern uint8_t  			Impedance_pos_pole, Impedance_neg_pole;

// PWM_TypeDef 	Flyback_300V_Switching_PWM =
// {
//     .TIMx       =   FLYBACK_SW1_HANDLE,
//     .Channel    =   FLYBACK_SW1_CHANNEL,
//     .Prescaler  =   0,
//     .Mode       =   LL_TIM_OCMODE_PWM1,
//     .Polarity   =   LL_TIM_OCPOLARITY_HIGH,
//     .Duty       =   0,
//     .Freq       =   0,
// };

// static PWM_TypeDef	Flyback_50V_Switching_PWM =
// {
//     .TIMx       =   FLYBACK_SW2_HANDLE,
//     .Channel    =   FLYBACK_SW2_CHANNEL,
//     .Prescaler  =   0,
//     .Mode       =   LL_TIM_OCMODE_PWM1,
//     .Polarity   =   LL_TIM_OCPOLARITY_HIGH,
//     .Duty       =   0,
//     .Freq       =   0,
// };

// 		bool		g_PID_is_300V_on = false;
// 		uint16_t 	g_PID_300V_set_voltage = 0;
// 		uint8_t 	PID_300V_PWM_duty;

// 	   	bool		g_PID_is_50V_on = false;
// 	   	uint16_t 	g_PID_50V_set_voltage	= 0;
// 		uint8_t 	PID_50V_PWM_duty;

// static PID_TypeDef Charge_300V_Cap_PID =
// {
// 	.PID_Mode 		= 	_PID_MODE_AUTOMATIC,
// 	.PON_Type 		= 	_PID_P_ON_E,
// 	.PID_Direction 	=	_PID_CD_DIRECT,
// 	.Kp				= 	(0.04 + 0.26),
// 	.Ki				= 	0.5,
// 	//.Ki				= 	0.001,
// 	.Kd 			=	0.0,
// 	.MyInput		=	&g_Feedback_Voltage[0],
// 	.MyOutput		= 	&PID_300V_PWM_duty,
// 	.MySetpoint		=	&g_PID_300V_set_voltage,
// 	.Output_Min		= 	0,
// 	.Output_Max		=	25,
// 	//.Output_Max		=	50,
// 	//.Output_Max		=	22,
// };

// static PID_TypeDef Charge_50V_Cap_PID =
// {
// 	.PID_Mode 		= 	_PID_MODE_AUTOMATIC,
// 	.PON_Type 		= 	_PID_P_ON_E,
// 	.PID_Direction 	=	_PID_CD_DIRECT,
// 	.Kp				= 	(0.04 + 0.26),
// 	.Ki				= 	0.5,
// 	.Kd 			=	0.0,
// 	.MyInput		=	&g_Feedback_Voltage[1],
// 	.MyOutput		= 	&PID_50V_PWM_duty,
// 	.MySetpoint		=	&g_PID_50V_set_voltage,
// 	.Output_Min		= 	0,
// 	//.Output_Max		=	8,
// 	.Output_Max		=	12,
// };

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
static inline void FlyBack_Set_Duty(PWM_TypeDef *PWMx, uint32_t _Duty);
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
extern uart_stdio_typedef RF_UART;
extern uart_stdio_typedef GPP_UART;

/* :::::::::: PID Init ::::::::::::: */
void PID_Task_Init(void)
{
	// // Init 300V PWM PID
	// PID_Init(&Charge_300V_Cap_PID);

	// PWM_Init(&Flyback_300V_Switching_PWM);
	// PWM_Set_Freq(&Flyback_300V_Switching_PWM, 60000);
	// LL_TIM_DisableIT_UPDATE(Flyback_300V_Switching_PWM.TIMx);
    // PWM_Enable(&Flyback_300V_Switching_PWM);

	// // Init 50V PWM PID
	// PID_Init(&Charge_50V_Cap_PID);

	// PWM_Init(&Flyback_50V_Switching_PWM);
	// PWM_Set_Freq(&Flyback_50V_Switching_PWM, 60000);
	// LL_TIM_DisableIT_UPDATE(Flyback_50V_Switching_PWM.TIMx);
    // PWM_Enable(&Flyback_50V_Switching_PWM);

	// LL_GPIO_SetOutputPin(FLYBACK_SD1_PORT, FLYBACK_SD1_PIN);
	// LL_GPIO_SetOutputPin(FLYBACK_SD2_PORT, FLYBACK_SD2_PIN);
}

/* :::::::::: PID Task ::::::::::::: */
//TODO: Adaptive PID, Ki, or sync uart command to change Ki.
void PID_Task(void*)
{
	// if (g_PID_is_300V_on == false)
	// {
	// 	FlyBack_Set_Duty(&Flyback_300V_Switching_PWM, 0);
	// }
	// else if (g_PID_is_300V_on == true)
	// {
	// 	PID_Compute(&Charge_300V_Cap_PID);
	// 	FlyBack_Set_Duty(&Flyback_300V_Switching_PWM, PID_300V_PWM_duty);
	// }

	// if (g_PID_is_50V_on == false)
	// {
	// 	FlyBack_Set_Duty(&Flyback_50V_Switching_PWM, 0);
	// }
	// else if (g_PID_is_50V_on == true)
	// {
	// 	PID_Compute(&Charge_50V_Cap_PID);
	// 	FlyBack_Set_Duty(&Flyback_50V_Switching_PWM, PID_50V_PWM_duty);
	// }
}

// void Notify_Charge_Cap_Task(void*)
// {
// 	if ((g_PID_is_300V_on == true) && (is_300V_notified_enable == true))
// 	{
// 		if (g_Feedback_Voltage[0] >= (g_PID_300V_set_voltage * 0.90))
// 		{
// 			uint16_t _300V_set_volt = (g_PID_300V_set_voltage / HV_calib_coefficient.average_value) + 1;
// 			UART_Printf(CMD_line_handle, "HV CAP FINISHED CHARGING TO %dV\n", _300V_set_volt);
// 			UART_Send_String(CMD_line_handle, "> ");
// 			is_300V_notified_enable = false;

// 			if (is_measure_impedance_enable == true)
// 			{
// 				ps_FSP_TX->CMD 									 	= FSP_CMD_MEASURE_IMPEDANCE;
// 				ps_FSP_TX->Payload.measure_impedance.Pos_pole_index = Impedance_pos_pole;
// 				ps_FSP_TX->Payload.measure_impedance.Neg_pole_index = Impedance_neg_pole;
// 				ps_FSP_TX->Payload.measure_impedance.Period_low  	= Impedance_Period;
// 				ps_FSP_TX->Payload.measure_impedance.Period_high 	= (Impedance_Period >> 8);
// 				s_FSP_TX_Packet.sod 		= FSP_PKT_SOD;
// 				s_FSP_TX_Packet.src_adr 	= fsp_my_adr;
// 				s_FSP_TX_Packet.dst_adr 	= FSP_ADR_GPP;
// 				s_FSP_TX_Packet.length 		= 5;
// 				s_FSP_TX_Packet.type 		= FSP_PKT_TYPE_CMD_W_DATA;
// 				s_FSP_TX_Packet.eof 		= FSP_PKT_EOF;
// 				s_FSP_TX_Packet.crc16 		= crc16_CCITT(FSP_CRC16_INITIAL_VALUE, &s_FSP_TX_Packet.src_adr, s_FSP_TX_Packet.length + 4);

// 				uint8_t encoded_frame[20] = { 0 };
// 				uint8_t frame_len;
// 				fsp_encode(&s_FSP_TX_Packet, encoded_frame, &frame_len);
// 				UART_FSP(&GPP_UART, (char*)encoded_frame, frame_len);

// 				is_measure_impedance_enable = false;
// 			}
// 		}
// 	}
// 	else if ((g_is_Discharge_300V_On == true) && (is_300V_notified_enable == true))
// 	{
// 		if (g_Feedback_Voltage[0] <= 10)
// 		{
// 			UART_Send_String(CMD_line_handle, "HV CAP FINISHED RELEASING TO 0V\n");
// 			UART_Send_String(CMD_line_handle, "> ");
// 			g_is_Discharge_300V_On  = false;
// 			is_300V_notified_enable = false;
// 		}
// 	}
	

// 	if ((g_PID_is_50V_on == true) && (is_50V_notified_enable == true))
// 	{
// 		if (g_Feedback_Voltage[1] >= (g_PID_50V_set_voltage * 0.99))
// 		{
// 			uint16_t _50V_set_volt = g_PID_50V_set_voltage / LV_calib_coefficient.average_value;
// 			UART_Printf(CMD_line_handle, "LV CAP FINISHED CHARGING TO %dV\n", _50V_set_volt);
// 			UART_Send_String(CMD_line_handle, "> ");
// 			is_50V_notified_enable = false;
// 		}
// 	}
// 	else if ((g_is_Discharge_50V_On == true) && (is_50V_notified_enable == true))
// 	{
// 		if (g_Feedback_Voltage[1] <= 10)
// 		{
// 			UART_Send_String(CMD_line_handle, "LV CAP FINISHED RELEASING TO 0V\n");
// 			UART_Send_String(CMD_line_handle, "> ");
// 			g_is_Discharge_50V_On  = false;
// 			is_50V_notified_enable = false;
// 		}
// 	}
// }

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
static inline void FlyBack_Set_Duty(PWM_TypeDef *PWMx, uint32_t _Duty)
{
	// Limit the duty to 100
    if (_Duty > 100)
      return;

    // Set PWM DUTY for channel 1
    PWMx->Duty = (PWMx->Freq * (_Duty / 100.0));
    switch (PWMx->Channel)
    {
    case LL_TIM_CHANNEL_CH1:
        LL_TIM_OC_SetCompareCH1(PWMx->TIMx, PWMx->Duty);
        break;
    case LL_TIM_CHANNEL_CH2:
        LL_TIM_OC_SetCompareCH2(PWMx->TIMx, PWMx->Duty);
        break;
    case LL_TIM_CHANNEL_CH3:
        LL_TIM_OC_SetCompareCH3(PWMx->TIMx, PWMx->Duty);
        break;
    case LL_TIM_CHANNEL_CH4:
        LL_TIM_OC_SetCompareCH4(PWMx->TIMx, PWMx->Duty);
        break;

    default:
        break;
    }
}
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ End of the program ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
