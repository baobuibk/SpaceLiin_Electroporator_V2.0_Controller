/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Include~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include <stdint.h>
#include <stdbool.h>

#define MY_CORE_H_
#define MY_GPIO_H_
#define MY_ADC_H_
#include "stm32f4_header.h"

#include "pid.h"
#include "pwm.h"

#include "cap_controller_task.h"

#include "app.h"

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Defines ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Enum ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Struct ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Class ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Private Types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define CAP_300V_CHARNGE_RANGE_MAX 9
Charge_Range_t Cap_300V_charge_range[CAP_300V_CHARNGE_RANGE_MAX] =
{
    {
        .Volt_Value = 30,
        .Duty_Max = 10,
    },
    {
        .Volt_Value = 50,
        .Duty_Max = 15,
    },
    {
        .Volt_Value = 75,
        .Duty_Max = 20,
    },
    {
        .Volt_Value = 100,
        .Duty_Max = 30,
    },
    {
        .Volt_Value = 150,
        .Duty_Max = 35,
    },
    {
        .Volt_Value = 200,
        .Duty_Max = 40,
    },
    {
        .Volt_Value = 250,
        .Duty_Max = 40,
    },
    {
        .Volt_Value = 275,
        .Duty_Max = 40,
    },
    {
        .Volt_Value = 300,
        .Duty_Max = 40,
    }
};

#define CAP_50V_CHARNGE_RANGE_MAX 5
Charge_Range_t Cap_50V_charge_range[CAP_50V_CHARNGE_RANGE_MAX] =
{
    {
        .Volt_Value = 10,
        .Duty_Max = 10,
    },
    {
        .Volt_Value = 20,
        .Duty_Max = 15,
    },
    {
        .Volt_Value = 30,
        .Duty_Max = 20,
    },
    {
        .Volt_Value = 40,
        .Duty_Max = 25,
    },
    {
        .Volt_Value = 50,
        .Duty_Max = 30,
    }
};

typedef struct _Cap_typedef_
{
    uint16_t    ADC_Value;

    bool        is_charge_on;

    uint16_t    set_charge_voltage_ADC;
    uint8_t     charge_PWM_duty;
	
    PID_TypeDef charge_PID;
    PWM_TypeDef charge_PWM;

    bool        is_discharge_on;

    uint16_t    set_discharge_voltage_ADC;
    uint8_t     discharge_PWM_duty;
	
    PID_TypeDef discharge_PID;
    PWM_TypeDef discharge_PWM;

    bool        is_notified_enable;

} Cap_typedef;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
//static uint16_t Cap_ADC_Value[ADC_CHANNEL_COUNT] = {0};

static Cap_typedef s_Cap_300V =
{
    .ADC_Value = 0,

    .is_charge_on = false,

    .set_charge_voltage_ADC = 0,
    .charge_PWM_duty = 0,

    .is_discharge_on = false,

    .set_discharge_voltage_ADC = 0,
    .discharge_PWM_duty = 0,
};

static Cap_typedef s_Cap_50V =
{
    .ADC_Value = 0,

    .is_charge_on = false,

    .set_charge_voltage_ADC = 0,
    .charge_PWM_duty = 0,

    .is_discharge_on = false,

    .set_discharge_voltage_ADC = 0,
    .discharge_PWM_duty = 0,
};

volatile static Cap_typedef* Cap_array[2] =
{
    &s_Cap_300V,
    &s_Cap_50V,
};

volatile static uint8_t  Cap_array_index = 0;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
static void Cap_ADC_Init(uint32_t ADC_Sampling_Time);
static void Cap_PWM_Init(uint32_t freq_300V, uint32_t freq_50V);
static void Cap_Discharge_Init(void);

static inline void FlyBack_PID_Compute(volatile Cap_typedef* p_cap_x);
static inline void Flyback_Set_Freq(Cap_typedef* p_cap_x, uint32_t _Freq);
static inline void Flyback_Set_Duty(Cap_typedef* p_cap_x, uint32_t _Duty);
// static inline void Discharge_Set_Duty(Cap_typedef* p_cap_x);

static void Cap_Controller_Charge_Monitor_300V(void);
static void Cap_Controller_Discharge_Monitor_300V(void);
static void Cap_Controller_Charge_Monitor_50V(void);
static void Cap_Controller_Discharge_Monitor_50V(void);

static uint16_t Cap_Calib_Calculate(Cap_Controller_Task_typedef* p_cap_x, uint16_t raw_voltage);
static uint16_t Cap_ADC_to_Volt(Cap_Controller_Task_typedef* p_cap_x, uint16_t ADC_value);
static void Cap_Set_Volt_Internal(Cap_Controller_Task_typedef* p_cap_x, uint16_t set_voltage);

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
uint16_t    g_Feedback_Voltage[ADC_CHANNEL_COUNT] = {0};

bool		g_PID_is_300V_on = false;
uint16_t    g_PID_300V_set_voltage = 0;
bool        g_is_Discharge_300V_On = false;
bool        is_300V_notified_enable = false;

bool		g_PID_is_50V_on = false;
uint16_t 	g_PID_50V_set_voltage	= 0;
bool        g_is_Discharge_50V_On  = false;
bool        is_50V_notified_enable = false;

Cap_Controller_Task_typedef g_Cap_300V =
{
    .max_cap_volt = 300,

    .charge_state = CAP_SOFT_START_STATE,
    .p_range = Cap_300V_charge_range,
    .range_max = CAP_300V_CHARNGE_RANGE_MAX,
    .range_set = 0,
    .range_index = 0,
    .soft_start_count = 0,

    .is_charge_on = false,
    .set_charge_voltage_ADC = 0,

    .is_discharge_on = false,
    .set_discharge_voltage_ADC = 0,

    .is_notified_enable = false,
};

Cap_Controller_Task_typedef g_Cap_50V =
{
    .max_cap_volt = 50,

    .charge_state = CAP_SOFT_START_STATE,
    .p_range = Cap_50V_charge_range,
    .range_max = CAP_50V_CHARNGE_RANGE_MAX,
    .range_set = 0,
    .range_index = 0,
    .soft_start_count = 0,

    .is_charge_on = false,
    .set_charge_voltage_ADC = 0,

    .is_discharge_on = false,
    .set_discharge_voltage_ADC = 0,

    .is_notified_enable = false,
};

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* :::::::::: Cap Controller Init :::::::: */
void Cap_Controller_Init(void)
{
    s_Cap_300V.charge_PID = (PID_TypeDef)
    {
        .PID_Mode       = _PID_MODE_AUTOMATIC,
        .PON_Type       = _PID_P_ON_E,
        .PID_Direction  = _PID_CD_DIRECT,
        .Kp             = 0.04 + 0.26,
        .Ki             = 0.5,
        .Kd             = 0.0,
        .MyInput        = &s_Cap_300V.ADC_Value,
        .MyOutput       = &s_Cap_300V.charge_PWM_duty,
        .MySetpoint     = &s_Cap_300V.set_charge_voltage_ADC,
        .Output_Min     = 0,
        .Output_Max     = 25,
    };

    s_Cap_50V.charge_PID = (PID_TypeDef)
    {
        .PID_Mode 		= 	_PID_MODE_AUTOMATIC,
        .PON_Type 		= 	_PID_P_ON_E,
        .PID_Direction 	=	_PID_CD_DIRECT,
        .Kp				= 	0.04 + 0.26,
        .Ki				= 	0.5,
        .Kd 			=	0.0,
        .MyInput		=	&s_Cap_50V.ADC_Value,
        .MyOutput		= 	&s_Cap_50V.charge_PWM_duty,
        .MySetpoint		=	&s_Cap_50V.set_charge_voltage_ADC,
        .Output_Min		= 	0,
        //.Output_Max		=	12,
        .Output_Max		=	10,
    };

    s_Cap_300V.charge_PWM = (PWM_TypeDef)
    {
        .TIMx       =   FLYBACK_SW1_HANDLE,
        .Channel    =   FLYBACK_SW1_CHANNEL,
        .Prescaler  =   0,
        .Mode       =   LL_TIM_OCMODE_PWM1,
        .Polarity   =   LL_TIM_OCPOLARITY_HIGH,
        .Duty       =   0,
        .Freq       =   0,
    };

    s_Cap_50V.charge_PWM = (PWM_TypeDef)
    {
        .TIMx       =   FLYBACK_SW2_HANDLE,
        .Channel    =   FLYBACK_SW2_CHANNEL,
        .Prescaler  =   0,
        .Mode       =   LL_TIM_OCMODE_PWM1,
        .Polarity   =   LL_TIM_OCPOLARITY_HIGH,
        .Duty       =   0,
        .Freq       =   0,
    };

    uint32_t ADC_Sampling_Time = LL_ADC_SAMPLINGTIME_480CYCLES;
    Cap_ADC_Init(ADC_Sampling_Time);
    
    uint32_t PWM_300V_Freq = 100000, PWM_50V_Freq = 100000;
    Cap_PWM_Init(PWM_300V_Freq, PWM_50V_Freq);

    Cap_Discharge_Init();

    Calib_Task_Init();

    for (uint8_t Idx = 0; Idx < g_Cap_300V.range_max; Idx++)
    {
        g_Cap_300V.p_range[Idx].ADC_Value = Cap_Calib_Calculate(&g_Cap_300V, g_Cap_300V.p_range[Idx].Volt_Value);
    }
    
    for (uint8_t Idx = 0; Idx < g_Cap_50V.range_max; Idx++)
    {
        g_Cap_50V.p_range[Idx].ADC_Value = Cap_Calib_Calculate(&g_Cap_50V, g_Cap_50V.p_range[Idx].Volt_Value);
    }
}

/* :::::::::: Cap Controller Charge Task :::::::: */
void Cap_Controller_Charge_Task(void*)
{
    //s_Cap_300V.is_charge_on           = g_Cap_300V.is_charge_on;
    s_Cap_300V.set_charge_voltage_ADC = g_Cap_300V.set_charge_voltage_ADC;
    s_Cap_300V.is_discharge_on        = g_Cap_300V.is_discharge_on;

    //s_Cap_50V.is_charge_on            = g_Cap_50V.is_charge_on;
    s_Cap_50V.set_charge_voltage_ADC  = g_Cap_50V.set_charge_voltage_ADC;
    s_Cap_50V.is_discharge_on         = g_Cap_50V.is_discharge_on;

    s_Cap_300V.is_notified_enable     = g_Cap_300V.is_notified_enable;
    s_Cap_50V.is_notified_enable      = g_Cap_50V.is_notified_enable;

    if (g_Cap_300V.is_charge_on == true)
    {
        if (s_Cap_300V.ADC_Value >= (s_Cap_300V.set_charge_voltage_ADC * 0.99))
        {
            if (g_Cap_300V.charge_state == CAP_SET_FREE_CHARGE_STATE)
            {
                if (s_Cap_300V.is_notified_enable == true)
                {
                    UART_Printf(CMD_line_handle, "HV CAP FINISHED CHARGING TO %dV\n", g_Cap_300V.set_charge_voltage_USER);
                    UART_Send_String(CMD_line_handle, "> ");
                
                    g_Cap_300V.is_notified_enable = false;
                    s_Cap_300V.is_notified_enable = false;
                }
            }

            g_Cap_300V.cap_state = CAP_IS_FINISH_CHARGING;
        }
    }

    if (g_Cap_50V.is_charge_on == true)
    {
        if (s_Cap_50V.ADC_Value >= (s_Cap_50V.set_charge_voltage_ADC * 0.99))
        {
            if (g_Cap_50V.charge_state == CAP_SET_FREE_CHARGE_STATE)
            {
                if (s_Cap_50V.is_notified_enable == true)
                {
                    UART_Printf(CMD_line_handle, "LV CAP FINISHED CHARGING TO %dV\n", g_Cap_50V.set_charge_voltage_USER);
                    UART_Send_String(CMD_line_handle, "> ");

                    g_Cap_50V.is_notified_enable = false;
                    s_Cap_50V.is_notified_enable = false;
                }
            }

            g_Cap_50V.cap_state = CAP_IS_FINISH_CHARGING;
        }
    }

    if (s_Cap_300V.is_charge_on == false)
    {
        s_Cap_300V.charge_PWM_duty = 0;
        Flyback_Set_Duty(&s_Cap_300V, 0);
    }

    if (s_Cap_50V.is_charge_on == false)
    {
        s_Cap_50V.charge_PWM_duty = 0;
        Flyback_Set_Duty(&s_Cap_50V, 0);
    }

    if (s_Cap_300V.is_discharge_on == false)
    {
        LL_GPIO_ResetOutputPin(DISCHARGE_300V_PORT, DISCHARGE_300V_PIN);
    }
    else if (s_Cap_300V.is_discharge_on == true)
    {
        LL_GPIO_SetOutputPin(DISCHARGE_300V_PORT, DISCHARGE_300V_PIN);
    }

    if (s_Cap_50V.is_discharge_on == false)
    {
        LL_GPIO_ResetOutputPin(DISCHARGE_50V_PORT, DISCHARGE_50V_PIN);
    }
    else if (s_Cap_50V.is_discharge_on == true)
    {
        LL_GPIO_SetOutputPin(DISCHARGE_50V_PORT, DISCHARGE_50V_PIN);
    }
}

void Cap_Controller_Monitor_Task(void*)
{
    Cap_Controller_Charge_Monitor_300V();
    Cap_Controller_Discharge_Monitor_300V();

    Cap_Controller_Charge_Monitor_50V();
    Cap_Controller_Discharge_Monitor_50V();
}

/* :::::::::: Cap Controller Command :::::::: */
void Cap_Set_Volt(Cap_Controller_Task_typedef* p_cap_x, uint16_t set_voltage, bool is_notified)
{
    p_cap_x->set_charge_voltage_USER = set_voltage;
    //p_cap_x->set_charge_voltage_ADC  = Cap_Calib_Calculate(p_cap_x, set_voltage);

    p_cap_x->is_notified_enable = is_notified;
}

void Cap_Set_Volt_All(uint16_t HV_set_voltage, uint16_t LV_set_voltage, bool HV_is_notified, bool LV_is_notified)
{
    g_Cap_300V.set_charge_voltage_USER  = HV_set_voltage;
    g_Cap_50V.set_charge_voltage_USER   = LV_set_voltage;

    //g_Cap_300V.set_charge_voltage_ADC  = Cap_Calib_Calculate(&g_Cap_300V, HV_set_voltage);
    //g_Cap_50V.set_charge_voltage_ADC   = Cap_Calib_Calculate(&g_Cap_50V, LV_set_voltage);

    g_Cap_300V.is_notified_enable   = HV_is_notified;
    g_Cap_50V.is_notified_enable    = LV_is_notified;
}

void Cap_Set_Charge(Cap_Controller_Task_typedef* p_cap_x, bool charge_state, bool is_notified)
{
    if (charge_state == true)
    {
        p_cap_x->is_discharge_on = false;
        p_cap_x->cap_state = CAP_IS_CHARGING;
    }
    else
    {
        p_cap_x->cap_state = CAP_IS_IDLE;
    }

    p_cap_x->is_charge_on = charge_state;

    p_cap_x->soft_start_count = 0;

    p_cap_x->charge_state = CAP_SOFT_START_STATE;

    p_cap_x->is_notified_enable = is_notified;
}

void Cap_Set_Charge_All(bool HV_charge_state, bool LV_charge_state, bool HV_is_notified, bool LV_is_notified)
{
    if (HV_charge_state == true)
    {
        g_Cap_300V.is_discharge_on = false;
        g_Cap_300V.cap_state = CAP_IS_CHARGING;
    }
    else
    {
        g_Cap_300V.cap_state = CAP_IS_IDLE;
    }

    if (LV_charge_state == true)
    {
        g_Cap_50V.is_discharge_on = false;
        g_Cap_50V.cap_state = CAP_IS_CHARGING;
    }
    else
    {
        g_Cap_50V.cap_state = CAP_IS_IDLE;
    }
    
    g_Cap_300V.is_charge_on = HV_charge_state;
    g_Cap_50V.is_charge_on  = LV_charge_state;

    g_Cap_300V.soft_start_count = 0;
    g_Cap_50V.soft_start_count = 0;

    g_Cap_300V.charge_state = CAP_SOFT_START_STATE;
    g_Cap_50V.charge_state = CAP_SOFT_START_STATE;

    g_Cap_300V.is_notified_enable   = HV_is_notified;
    g_Cap_50V.is_notified_enable    = LV_is_notified;
}

void Cap_Set_Discharge(Cap_Controller_Task_typedef* p_cap_x, bool discharge_state, bool is_notified)
{
    if (discharge_state == true)
    {
        p_cap_x->is_charge_on = false;
        p_cap_x->cap_state = CAP_IS_DISCHARGING;
    }
    else
    {
        p_cap_x->cap_state = CAP_IS_IDLE;
    }
    
    p_cap_x->is_discharge_on = discharge_state;

    p_cap_x->is_notified_enable = is_notified;
}

void Cap_Set_Discharge_All(bool HV_discharge_state, bool LV_discharge_state, bool HV_is_notified, bool LV_is_notified)
{
    if (HV_discharge_state == true)
    {
        g_Cap_300V.is_charge_on = false;
        g_Cap_300V.cap_state = CAP_IS_DISCHARGING;
    }
    else
    {
        g_Cap_300V.cap_state = CAP_IS_IDLE;
    }

    if (LV_discharge_state == true)
    {
        g_Cap_50V.is_charge_on = false;
        g_Cap_50V.cap_state = CAP_IS_DISCHARGING;
    }
    else
    {
        g_Cap_50V.cap_state = CAP_IS_IDLE;
    }

    g_Cap_300V.is_discharge_on  = HV_discharge_state;
    g_Cap_50V.is_discharge_on   = LV_discharge_state;
    
    g_Cap_300V.is_notified_enable   = HV_is_notified;
    g_Cap_50V.is_notified_enable    = LV_is_notified;
}

uint16_t Cap_Get_Set_Volt(Cap_Controller_Task_typedef* p_cap_x)
{
    return p_cap_x->set_charge_voltage_USER;
}

bool Cap_Get_is_Charge(Cap_Controller_Task_typedef* p_cap_x)
{
    return p_cap_x->is_charge_on;
}

bool Cap_Get_is_Discharge(Cap_Controller_Task_typedef* p_cap_x)
{
    return p_cap_x->is_discharge_on;
}

bool Cap_Get_is_Notified(Cap_Controller_Task_typedef* p_cap_x)
{
    return p_cap_x->is_notified_enable;
}

uint16_t Cap_Measure_Volt(Cap_Controller_Task_typedef* p_cap_x)
{
    uint16_t ADC_value_temp = 0;

    if (p_cap_x == &g_Cap_300V)
    {
        ADC_value_temp = s_Cap_300V.ADC_Value;
    }
    else if (p_cap_x == &g_Cap_50V)
    {
        ADC_value_temp = s_Cap_50V.ADC_Value;
    }
    
    return Cap_ADC_to_Volt(p_cap_x, ADC_value_temp);
}

/* :::::::::: Cap Controller Interupt Handler ::::::::::::: */
void Cap_Controller_ADC_IRQHandler(void)
{
    // Overrun handle
    if(LL_ADC_IsActiveFlag_OVR(ADC_FEEDBACK_HANDLE) == true)
    {
        LL_ADC_ClearFlag_OVR(ADC_FEEDBACK_HANDLE);

        // Reset index để đồng bộ với rank 1
        Cap_array_index = 0;

        // Dừng để reset sequencer
        LL_ADC_Disable(ADC_FEEDBACK_HANDLE);
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        LL_ADC_Enable(ADC_FEEDBACK_HANDLE);

        LL_ADC_REG_StartConversionSWStart(ADC_FEEDBACK_HANDLE);
    }

    // End of Conversion handle
    if(LL_ADC_IsActiveFlag_EOCS(ADC_FEEDBACK_HANDLE) == true)
    {
        LL_ADC_ClearFlag_EOCS(ADC_FEEDBACK_HANDLE);

        Cap_array[Cap_array_index]->ADC_Value = LL_ADC_REG_ReadConversionData12(ADC_FEEDBACK_HANDLE);

        FlyBack_PID_Compute(Cap_array[Cap_array_index]);

        Cap_array_index ^= 1;
    }
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
static void Cap_ADC_Init(uint32_t ADC_Sampling_Time)
{
    // Init the value of the cap array index
    Cap_array_index = 0;

    // The total number of conversions in the regular group, which is 2
    LL_ADC_REG_SetSequencerLength(ADC_FEEDBACK_HANDLE, LL_ADC_REG_SEQ_SCAN_ENABLE_2RANKS);

    // Their order in the conversion sequence
    LL_ADC_REG_SetSequencerRanks(ADC_FEEDBACK_HANDLE, LL_ADC_REG_RANK_1, ADC_300V_CHANNEL);

    LL_ADC_REG_SetSequencerRanks(ADC_FEEDBACK_HANDLE, LL_ADC_REG_RANK_2, ADC_50V_CHANNEL);

    // Setting sampling time for each channel
    LL_ADC_SetChannelSamplingTime(ADC_FEEDBACK_HANDLE, ADC_300V_CHANNEL, ADC_Sampling_Time);

    LL_ADC_SetChannelSamplingTime(ADC_FEEDBACK_HANDLE, ADC_50V_CHANNEL, ADC_Sampling_Time);

    // Enable scan mode
    LL_ADC_SetSequencersScanMode(ADC_FEEDBACK_HANDLE, LL_ADC_SEQ_SCAN_ENABLE);

    // Enable continuos mode
    LL_ADC_REG_SetContinuousMode(ADC_FEEDBACK_HANDLE, LL_ADC_REG_CONV_CONTINUOUS);

    // Since we are using continuos mode so over run is
    // inevitable
    LL_ADC_EnableIT_OVR(ADC_FEEDBACK_HANDLE);

    // Enable end-of-conversion interupt
    LL_ADC_EnableIT_EOCS(ADC_FEEDBACK_HANDLE);

    LL_ADC_REG_SetFlagEndOfConversion(ADC_FEEDBACK_HANDLE, LL_ADC_REG_FLAG_EOC_UNITARY_CONV);

    // Enable ADC itself, since we using Continuos mode, to turn the ADC off
    // Use LL_ADC_Disable(ADC_FEEDBACK_HANDLE); to turn it off.
    LL_ADC_Enable(ADC_FEEDBACK_HANDLE);

    // Finally a signal to start the Continuos mode ADC.
    LL_ADC_REG_SetTriggerSource(ADC_FEEDBACK_HANDLE, LL_ADC_REG_TRIG_SOFTWARE);
    LL_ADC_REG_StartConversionSWStart(ADC_FEEDBACK_HANDLE);
}

static void Cap_PWM_Init(uint32_t freq_300V, uint32_t freq_50V)
{
    // Init 300V PWM PID
	PID_Init(&s_Cap_300V.charge_PID);

	//PWM_Init(&s_Cap_300V.charge_PWM);
    LL_TIM_OC_SetMode(s_Cap_300V.charge_PWM.TIMx, s_Cap_300V.charge_PWM.Channel,  LL_TIM_OCMODE_PWM1);
    LL_TIM_OC_SetPolarity(s_Cap_300V.charge_PWM.TIMx, s_Cap_300V.charge_PWM.Channel, LL_TIM_OCPOLARITY_HIGH);
    LL_TIM_OC_EnablePreload(s_Cap_300V.charge_PWM.TIMx, s_Cap_300V.charge_PWM.Channel);
    LL_TIM_CC_EnableChannel(s_Cap_300V.charge_PWM.TIMx, s_Cap_300V.charge_PWM.Channel);

    LL_TIM_SetOffStates(s_Cap_300V.charge_PWM.TIMx, LL_TIM_OSSI_ENABLE, LL_TIM_OSSR_ENABLE);

    LL_TIM_DisableAutomaticOutput(s_Cap_300V.charge_PWM.TIMx);

    LL_TIM_EnableAllOutputs(s_Cap_300V.charge_PWM.TIMx);

    LL_TIM_SetPrescaler(s_Cap_300V.charge_PWM.TIMx, 0);

    LL_TIM_EnableARRPreload(s_Cap_300V.charge_PWM.TIMx);

	Flyback_Set_Freq(&s_Cap_300V, freq_300V);
	LL_TIM_DisableIT_UPDATE(s_Cap_300V.charge_PWM.TIMx);

	// Init 50V PWM PID
	PID_Init(&s_Cap_50V.charge_PID);

	//PWM_Init(&s_Cap_50V.charge_PWM);
    LL_TIM_OC_SetMode(s_Cap_50V.charge_PWM.TIMx, s_Cap_50V.charge_PWM.Channel,  LL_TIM_OCMODE_PWM1);
    LL_TIM_OC_SetPolarity(s_Cap_50V.charge_PWM.TIMx, s_Cap_50V.charge_PWM.Channel, LL_TIM_OCPOLARITY_HIGH);
    LL_TIM_OC_EnablePreload(s_Cap_50V.charge_PWM.TIMx, s_Cap_50V.charge_PWM.Channel);
    LL_TIM_CC_EnableChannel(s_Cap_50V.charge_PWM.TIMx, s_Cap_50V.charge_PWM.Channel);

    LL_TIM_SetOffStates(s_Cap_50V.charge_PWM.TIMx, LL_TIM_OSSI_ENABLE, LL_TIM_OSSR_ENABLE);

    LL_TIM_DisableAutomaticOutput(s_Cap_50V.charge_PWM.TIMx);

    LL_TIM_EnableAllOutputs(s_Cap_50V.charge_PWM.TIMx);

    LL_TIM_SetPrescaler(s_Cap_50V.charge_PWM.TIMx, 0);

    LL_TIM_EnableARRPreload(s_Cap_50V.charge_PWM.TIMx);

	Flyback_Set_Freq(&s_Cap_50V, freq_50V);
	LL_TIM_DisableIT_UPDATE(s_Cap_50V.charge_PWM.TIMx);

	LL_GPIO_SetOutputPin(FLYBACK_SD1_PORT, FLYBACK_SD1_PIN);
	LL_GPIO_SetOutputPin(FLYBACK_SD2_PORT, FLYBACK_SD2_PIN);

    LL_GPIO_ResetOutputPin(DISCHARGE_300V_PORT, DISCHARGE_300V_PIN);
    LL_GPIO_ResetOutputPin(DISCHARGE_50V_PORT, DISCHARGE_50V_PIN);

    LL_TIM_EnableCounter(s_Cap_300V.charge_PWM.TIMx);
    LL_TIM_EnableCounter(s_Cap_50V.charge_PWM.TIMx);
}

static void Cap_Discharge_Init(void)
{
    LL_GPIO_ResetOutputPin(DISCHARGE_300V_PORT, DISCHARGE_300V_PIN);
    LL_GPIO_ResetOutputPin(DISCHARGE_50V_PORT, DISCHARGE_50V_PIN);
}

static inline void FlyBack_PID_Compute(volatile Cap_typedef* p_cap_x)
{
    if (p_cap_x->is_charge_on == true)
    {
        PID_Compute(&p_cap_x->charge_PID);
    
        volatile PWM_TypeDef* PWMx = &p_cap_x->charge_PWM;
        uint32_t _Duty = p_cap_x->charge_PWM_duty;

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
}

static inline void Flyback_Set_Freq(Cap_typedef* p_cap_x, uint32_t _Freq)
{
    PWM_TypeDef* PWMx = &p_cap_x->charge_PWM;

    LL_TIM_DisableUpdateEvent(PWMx->TIMx);

    // Set PWM FREQ
    //PWMx->Freq = __LL_TIM_CALC_ARR(APB1_TIMER_CLK, LL_TIM_GetPrescaler(PWMx->TIMx), _Freq);
    PWMx->Freq = (float)APB1_TIMER_CLK / ((float)(LL_TIM_GetPrescaler(PWMx->TIMx) + 1) * _Freq);
    LL_TIM_SetAutoReload(PWMx->TIMx, PWMx->Freq);

    LL_TIM_EnableUpdateEvent(PWMx->TIMx);
}

static inline void Flyback_Set_Duty(Cap_typedef* p_cap_x, uint32_t _Duty)
{
    PWM_TypeDef* PWMx = &p_cap_x->charge_PWM;

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

/* :::::::::: Cap Controller Notify :::::::: */
static void Cap_Controller_Charge_Monitor_300V(void)
{
	if (g_Cap_300V.is_charge_on == false)
	{
        s_Cap_300V.is_charge_on = g_Cap_300V.is_charge_on;
        return;
    }

    switch (g_Cap_300V.charge_state)
    {
    case CAP_SOFT_START_STATE:
    {   
        if (g_Cap_300V.soft_start_count == 0)
        {
            PID_SetOutputLimits(&s_Cap_300V.charge_PID, 0, 5);
            Cap_Set_Volt_Internal(&g_Cap_300V, 5);
            g_Cap_300V.soft_start_count++;
            break;
        }
        else if (g_Cap_300V.soft_start_count < 4000)
        {
            g_Cap_300V.soft_start_count++;
            break;
        }

        // if (Cap_300V_soft_start_count >= 50)
        for (uint8_t Idx = 0; Idx < g_Cap_300V.range_max; Idx++)
        {
            if (s_Cap_300V.ADC_Value <= g_Cap_300V.p_range[Idx].ADC_Value)
            {
                g_Cap_300V.range_index = Idx;
                break;
            }
        }

        g_Cap_300V.charge_state = CAP_CONTROL_CHARGE_STATE;
        break;
    }
    case CAP_CONTROL_CHARGE_STATE:
    {   
        if (g_Cap_300V.cap_state != CAP_IS_FINISH_CHARGING)
        {
            break;
        }

        if (g_Cap_300V.set_charge_voltage_USER > g_Cap_300V.p_range[g_Cap_300V.range_index].Volt_Value)
        {
            PID_SetOutputLimits(&s_Cap_300V.charge_PID, 0, g_Cap_300V.p_range[g_Cap_300V.range_index].Duty_Max);
            Cap_Set_Volt_Internal(&g_Cap_300V, g_Cap_300V.p_range[g_Cap_300V.range_index].Volt_Value);
            g_Cap_300V.cap_state = CAP_IS_CHARGING;
        }
        else
        {
            PID_SetOutputLimits(&s_Cap_300V.charge_PID, 0, g_Cap_300V.p_range[g_Cap_300V.range_index].Duty_Max);
            Cap_Set_Volt_Internal(&g_Cap_300V, g_Cap_300V.set_charge_voltage_USER);
            g_Cap_300V.cap_state = CAP_IS_CHARGING;
            g_Cap_300V.charge_state = CAP_SET_FREE_CHARGE_STATE;
            break;
        }

        g_Cap_300V.range_index++;
        break;
    }
    case CAP_SET_FREE_CHARGE_STATE:
    {   
        if (g_Cap_300V.cap_state != CAP_IS_FINISH_CHARGING)
        {
            break;
        }

        //PID_SetOutputLimits(&s_Cap_300V.charge_PID, 0, 5);

        if (is_measure_impedance_enable == true)
        {
            is_impedance_volt_complete = true;
        }

        break;
    }

    default:
        break;
    }

    s_Cap_300V.is_charge_on = g_Cap_300V.is_charge_on;

    // if (s_Cap_300V.ADC_Value >= (s_Cap_300V.set_charge_voltage_ADC * 0.99))
    // {
    //     if (g_Cap_300V.charge_state == CAP_FREE_CHARGE_STATE)
    //     {
    //         if (s_Cap_300V.is_notified_enable == true)
    //         {
    //             UART_Printf(CMD_line_handle, "HV CAP FINISHED CHARGING TO %dV\n", g_Cap_300V.set_charge_voltage_USER);
    //             UART_Send_String(CMD_line_handle, "> ");
            
    //             g_Cap_300V.is_notified_enable = false;
    //             s_Cap_300V.is_notified_enable = false;
    //         }
    //     }

    //     g_Cap_300V.cap_state = CAP_IS_FINISH_CHARGING;
    // }
}

static void Cap_Controller_Discharge_Monitor_300V(void)
{
	if (s_Cap_300V.is_discharge_on == false)
	{
        return;
    }

    if (s_Cap_300V.ADC_Value <= 20)
    {
        if (s_Cap_300V.is_notified_enable == true)
        {
            UART_Send_String(CMD_line_handle, "HV CAP FINISHED RELEASING TO 0V\n");
            UART_Send_String(CMD_line_handle, "> ");

            Cap_Set_Discharge(&g_Cap_300V, false, false);

            g_Cap_300V.is_notified_enable = false;
            s_Cap_300V.is_notified_enable = false;
        }

        g_Cap_300V.cap_state = CAP_IS_FINISH_DISCHARGING;
    }
}

static void Cap_Controller_Charge_Monitor_50V(void)
{ 
    if (g_Cap_50V.is_charge_on == false)
	{
        s_Cap_50V.is_charge_on = g_Cap_50V.is_charge_on;
        return;
    }

    switch (g_Cap_50V.charge_state)
    {
    case CAP_SOFT_START_STATE:
    {   
        if (g_Cap_50V.soft_start_count == 0)
        {
            PID_SetOutputLimits(&s_Cap_50V.charge_PID, 0, 5);
            Cap_Set_Volt_Internal(&g_Cap_50V, 5);
            g_Cap_50V.soft_start_count++;
            break;
        }
        else if (g_Cap_50V.soft_start_count < 4000)
        {
            g_Cap_50V.soft_start_count++;
            break;
        }

        // if (Cap_300V_soft_start_count >= 50)
        for (uint8_t Idx = 0; Idx < g_Cap_50V.range_max; Idx++)
        {
            if (s_Cap_50V.ADC_Value <= g_Cap_50V.p_range[Idx].ADC_Value)
            {
                g_Cap_50V.range_index = Idx;
                break;
            }
        }

        g_Cap_50V.charge_state = CAP_CONTROL_CHARGE_STATE;
        break;
    }
    case CAP_CONTROL_CHARGE_STATE:
    {
        if (g_Cap_50V.cap_state != CAP_IS_FINISH_CHARGING)
        {
            break;
        }

        if (g_Cap_50V.set_charge_voltage_USER > g_Cap_50V.p_range[g_Cap_50V.range_index].Volt_Value)
        {
            PID_SetOutputLimits(&s_Cap_50V.charge_PID, 0, g_Cap_50V.p_range[g_Cap_50V.range_index].Duty_Max);
            Cap_Set_Volt_Internal(&g_Cap_50V, g_Cap_50V.p_range[g_Cap_50V.range_index].Volt_Value);
            g_Cap_50V.cap_state = CAP_IS_CHARGING;
        }
        else
        {
            PID_SetOutputLimits(&s_Cap_50V.charge_PID, 0, g_Cap_50V.p_range[g_Cap_50V.range_index].Duty_Max);
            Cap_Set_Volt_Internal(&g_Cap_50V, g_Cap_50V.set_charge_voltage_USER);
            g_Cap_50V.cap_state = CAP_IS_CHARGING;
            g_Cap_50V.charge_state = CAP_SET_FREE_CHARGE_STATE;
            break;
        }

        g_Cap_50V.range_index++;
        break;
    }
    case CAP_SET_FREE_CHARGE_STATE:
    {   
        if (g_Cap_50V.cap_state != CAP_IS_FINISH_CHARGING)
        {
            break;
        }

        float set_duty_temp = ((float)g_Cap_50V.set_charge_voltage_USER * 100.0) / (120.0 + (float)g_Cap_50V.set_charge_voltage_USER);
        uint16_t cap_50V_set_duty    = (set_duty_temp * 1.112641084) + 0.5;

        PID_SetOutputLimits(&s_Cap_50V.charge_PID, 0, cap_50V_set_duty);
        g_Cap_50V.charge_state = CAP_IS_FREE_CHARGE_STATE;
        break;
    }

    case CAP_IS_FREE_CHARGE_STATE:
    {
        break;
    }

    default:
        break;
    }

    s_Cap_50V.is_charge_on = g_Cap_50V.is_charge_on;

    // if (s_Cap_50V.ADC_Value >= (s_Cap_50V.set_charge_voltage_ADC * 0.99))
    // {
    //     if ((s_Cap_50V.is_notified_enable == true) && (g_Cap_50V.charge_state == CAP_FREE_CHARGE_STATE))
    //     {
    //         UART_Printf(CMD_line_handle, "LV CAP FINISHED CHARGING TO %dV\n", g_Cap_50V.set_charge_voltage_USER);
    //         UART_Send_String(CMD_line_handle, "> ");

    //         g_Cap_50V.is_notified_enable = false;
    //         s_Cap_50V.is_notified_enable = false;
    //     }

    //     g_Cap_50V.cap_state = CAP_IS_FINISH_CHARGING;
    // }
}

static void Cap_Controller_Discharge_Monitor_50V(void)
{
	if (s_Cap_50V.is_discharge_on == false)
	{
        return;
    }

    if (s_Cap_50V.ADC_Value <= 67)
    {
        if (s_Cap_50V.is_notified_enable == true)
        {
            UART_Send_String(CMD_line_handle, "LV CAP FINISHED RELEASING TO 0V\n");
            UART_Send_String(CMD_line_handle, "> ");

            Cap_Set_Discharge(&g_Cap_50V, false, false);

            g_Cap_50V.is_notified_enable = false;
            s_Cap_50V.is_notified_enable = false;
        }

        g_Cap_50V.cap_state = CAP_IS_FINISH_DISCHARGING;
    }
}

static uint16_t Cap_Calib_Calculate(Cap_Controller_Task_typedef* p_cap_x, uint16_t raw_voltage)
{
    return raw_voltage / p_cap_x->calib.coefficient_value.average_value;
}

static uint16_t Cap_ADC_to_Volt(Cap_Controller_Task_typedef* p_cap_x, uint16_t ADC_value)
{
   return ADC_value * p_cap_x->calib.coefficient_value.average_value;
}

static void Cap_Set_Volt_Internal(Cap_Controller_Task_typedef* p_cap_x, uint16_t set_voltage)
{
    p_cap_x->set_charge_voltage_ADC = Cap_Calib_Calculate(p_cap_x, set_voltage);
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ End of the program ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
// static PID_TypeDef Discharge_300V_Cap_PID =
// {
// 	.PID_Mode 		= 	_PID_MODE_AUTOMATIC,
// 	.PON_Type 		= 	_PID_P_ON_E,
// 	.PID_Direction 	=	_PID_CD_DIRECT,
// 	.Kp				= 	(0.04 + 0.26),
// 	.Ki				= 	0.5,
// 	.Kd 			=	0.0,
// 	.MyInput		=	&Cap_ADC_Value[0],
// 	.MyOutput		= 	&PID_300V_PWM_duty,
// 	.MySetpoint		=	&g_PID_300V_set_voltage,
// 	.Output_Min		= 	0,
// 	.Output_Max		=	25,
// };

// static PID_TypeDef Discharge_50V_Cap_PID =
// {
// 	.PID_Mode 		= 	_PID_MODE_AUTOMATIC,
// 	.PON_Type 		= 	_PID_P_ON_E,
// 	.PID_Direction 	=	_PID_CD_DIRECT,
// 	.Kp				= 	(0.04 + 0.26),
// 	.Ki				= 	0.5,
// 	.Kd 			=	0.0,
// 	.MyInput		=	&Cap_ADC_Value[1],
// 	.MyOutput		= 	&PID_50V_PWM_duty,
// 	.MySetpoint		=	&g_PID_50V_set_voltage,
// 	.Output_Min		= 	0,
// 	.Output_Max		=	12,
// };

// static PWM_TypeDef 	Discharge_300V_PWM =
// {
//     .TIMx       =   FLYBACK_SW1_HANDLE,
//     .Channel    =   FLYBACK_SW1_CHANNEL,
//     .Prescaler  =   0,
//     .Mode       =   LL_TIM_OCMODE_PWM1,
//     .Polarity   =   LL_TIM_OCPOLARITY_HIGH,
//     .Duty       =   0,
//     .Freq       =   0,
// };

// static PWM_TypeDef	Discharge_50V_PWM =
// {
//     .TIMx       =   FLYBACK_SW2_HANDLE,
//     .Channel    =   FLYBACK_SW2_CHANNEL,
//     .Prescaler  =   0,
//     .Mode       =   LL_TIM_OCMODE_PWM1,
//     .Polarity   =   LL_TIM_OCPOLARITY_HIGH,
//     .Duty       =   0,
//     .Freq       =   0,
// };

			// if (is_notify_measure_impedance_enable == true)
			// {
			// 	ps_FSP_TX->CMD 									 	= FSP_CMD_MEASURE_IMPEDANCE;
			// 	ps_FSP_TX->Payload.measure_impedance.Pos_pole_index = Impedance_pos_pole;
			// 	ps_FSP_TX->Payload.measure_impedance.Neg_pole_index = Impedance_neg_pole;
			// 	ps_FSP_TX->Payload.measure_impedance.Period_low  	= Impedance_Period;
			// 	ps_FSP_TX->Payload.measure_impedance.Period_high 	= (Impedance_Period >> 8);
			// 	s_FSP_TX_Packet.sod 		= FSP_PKT_SOD;
			// 	s_FSP_TX_Packet.src_adr 	= fsp_my_adr;
			// 	s_FSP_TX_Packet.dst_adr 	= FSP_ADR_GPP;
			// 	s_FSP_TX_Packet.length 		= 5;
			// 	s_FSP_TX_Packet.type 		= FSP_PKT_TYPE_CMD_W_DATA;
			// 	s_FSP_TX_Packet.eof 		= FSP_PKT_EOF;
			// 	s_FSP_TX_Packet.crc16 		= crc16_CCITT(FSP_CRC16_INITIAL_VALUE, &s_FSP_TX_Packet.src_adr, s_FSP_TX_Packet.length + 4);

			// 	uint8_t encoded_frame[20] = { 0 };
			// 	uint8_t frame_len;
			// 	fsp_encode(&s_FSP_TX_Packet, encoded_frame, &frame_len);
			// 	UART_FSP(&GPP_UART, (char*)encoded_frame, frame_len);

			// 	s_Cap_300V.is_charge_on = false;
			// 	s_Cap_50V.is_charge_on = false;

			// 	is_notify_measure_impedance_enable = false;

			// 	// Enable Impedance task
			// 	// SchedulerTaskEnable(8, 1);
			// }
