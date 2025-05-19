/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Include~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include <stdint.h>
#include <stdbool.h>

#define MY_GPIO_H_
#include "stm32f4_header.h"

#include "cap_calib_task.h"

#include "app.h"

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Defines ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Enum ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Struct ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Class ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Private Types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/* :::::::::: Calib Task State ::::::::::::: */
typedef enum
{
	CALIB_OFF_STATE,
    CALIB_START_STATE,
    CALIB_PROCESS_STATE,
} Calib_State_typedef;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
static Calib_State_typedef  Calib_State = CALIB_OFF_STATE;
static uint8_t              Calib_Stage_Count = 0;

static Cap_Controller_Task_typedef* p_current_cap;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
static inline void Charge_Cap_Procedure(Cap_Controller_Task_typedef* p_cap_x, uint16_t set_volt);

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
uint32_t g_HV_Measure_mv = 0;
uint32_t g_LV_Measure_mv = 0;

bool g_is_calib_running     = false;
bool g_is_measure_available = false;

Cap_Calib_Coefficient_typedef HV_calib_coefficient =
{
    .default_value  = 8.32801533,
    .stage_1        = 0.0,
    .stage_2        = 0.0,
    .stage_3        = 0.0,
    .stage_4        = 0.0,
    //.average_value  = 8.32801533,
    .average_value  = 0.08137207,
};

Cap_Calib_Coefficient_typedef LV_calib_coefficient =
{
    .default_value  = 55.2,
    .stage_1        = 0.0,
    .stage_2        = 0.0,
    .stage_3        = 0.0,
    .stage_4        = 0.0,
    //.average_value  = 55.2,
    .average_value  = 0.01228327228 * 2.023221,
};

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* :::::::::: Discharge Task Init ::::::::::::: */
void Calib_Task_Init(void)
{
	p_current_cap = &g_Cap_300V;

    g_Cap_300V.calib = (Cap_Calib_Task_typdef)
    {
        .is_calib_running       = false,
        .is_measure_available   = false,

        .measure_mv     = 0,
        .cap_name       = "HV",
    };

    g_Cap_50V.calib = (Cap_Calib_Task_typdef)
    {
        .is_calib_running       = false,
        .is_measure_available   = false,

        .measure_mv = 0,
        .cap_name   = "LV",
    };

    g_Cap_300V.calib.coefficient_value = (Cap_Calib_Coefficient_typedef)
    {
        .default_value  = 8.32801533,
        .stage_1        = 0.0,
        .stage_2        = 0.0,
        .stage_3        = 0.0,
        .stage_4        = 0.0,
        //.average_value  = 8.32801533,
        //.average_value  = 0.08056641,
        //.average_value  = 0.09716993007,
        //.average_value  = 0.09768416035,
        //.average_value  = 0.098172043,
        .average_value  = 0.09837806874,
    };

    g_Cap_50V.calib.coefficient_value = (Cap_Calib_Coefficient_typedef)
    {
        .default_value  = 55.2,
        .stage_1        = 0.0,
        .stage_2        = 0.0,
        .stage_3        = 0.0,
        .stage_4        = 0.0,
        //.average_value  = 55.2,
        //.average_value  = 0.01228327228,
        //.average_value  = 0.014742014,
        .average_value  = 0.014792899,
        //.average_value  = 0.01394221998,
        //.average_value  = 0.025,
    };
}

/* :::::::::: Discharge Task ::::::::::::: */
void Calib_Task(void*)
{
    if (p_current_cap->calib.is_calib_running == false)
    {
        Cap_Set_Discharge_All(true, true, true, true);

        Calib_State = CALIB_OFF_STATE;
    }

    switch (Calib_State)
    {
    case CALIB_OFF_STATE:
        if (p_current_cap->calib.is_calib_running == true)
        {
            p_current_cap->calib.is_measure_available = 0;

            Calib_State = CALIB_START_STATE;
            Calib_Stage_Count = 0;
        }
        else
        {
            SchedulerTaskDisable(6);
        }
        
        break;

    case CALIB_START_STATE:
    	uint16_t set_cap_volt = 0;

        switch (Calib_Stage_Count)
        {
        case 0:
            set_cap_volt = p_current_cap->max_cap_volt * 0.1;
            UART_Printf(CMD_line_handle, "CALIB %s CAP AT %dV\n", p_current_cap->calib.cap_name, set_cap_volt);

            UART_Send_String(CMD_line_handle, "> ");

            Charge_Cap_Procedure(p_current_cap, set_cap_volt);
            Calib_State = CALIB_PROCESS_STATE;
            break;
        case 1:
            set_cap_volt = p_current_cap->max_cap_volt * 0.2;
            UART_Printf(CMD_line_handle, "CALIB %s CAP AT %dV\n", p_current_cap->calib.cap_name, set_cap_volt);

            UART_Send_String(CMD_line_handle, "> ");

            Charge_Cap_Procedure(p_current_cap, set_cap_volt);
            Calib_State = CALIB_PROCESS_STATE;
            break;
        case 2:
            set_cap_volt = p_current_cap->max_cap_volt * 0.5;
            UART_Printf(CMD_line_handle, "CALIB %s CAP AT %dV\n", p_current_cap->calib.cap_name, set_cap_volt);

            UART_Send_String(CMD_line_handle, "> ");

            Charge_Cap_Procedure(p_current_cap, set_cap_volt);
            Calib_State = CALIB_PROCESS_STATE;
            break;
        case 3:
            set_cap_volt = p_current_cap->max_cap_volt * 1;
            UART_Printf(CMD_line_handle, "CALIB %s CAP AT %dV\n", p_current_cap->calib.cap_name, set_cap_volt);

            UART_Send_String(CMD_line_handle, "> ");

            Charge_Cap_Procedure(p_current_cap, set_cap_volt);
            Calib_State = CALIB_PROCESS_STATE;
            break;

        default:
            break;
        }
            break;
    case CALIB_PROCESS_STATE:

        if (p_current_cap->calib.is_measure_available == true)
        {
            switch (Calib_Stage_Count)
            {
            case 0:
                p_current_cap->calib.coefficient_value.stage_1 = 1000.0 * ((float)g_Feedback_Voltage[0] / (float)g_HV_Measure_mv);

                Cap_Set_Charge(p_current_cap, false, false);
                Calib_Stage_Count = 1;
                p_current_cap->calib.is_measure_available = false;
                Calib_State = CALIB_START_STATE;
                break;
            case 1:
                p_current_cap->calib.coefficient_value.stage_2 = 1000.0 * ((float)g_Feedback_Voltage[0] / (float)g_HV_Measure_mv);

                Cap_Set_Charge(p_current_cap, false, false);
                Calib_Stage_Count = 2;
                p_current_cap->calib.is_measure_available = false;
                Calib_State = CALIB_START_STATE;
                break;
            case 2:
                p_current_cap->calib.coefficient_value.stage_3 = 1000.0 * ((float)g_Feedback_Voltage[0] / (float)g_HV_Measure_mv);

                Cap_Set_Charge(p_current_cap, false, false);
                Calib_Stage_Count = 3;
                p_current_cap->calib.is_measure_available = false;
                Calib_State = CALIB_START_STATE;
                break;
            case 3:
                p_current_cap->calib.coefficient_value.stage_4 = 1000.0 * ((float)g_Feedback_Voltage[0] / (float)g_HV_Measure_mv);

                p_current_cap->calib.coefficient_value.average_value = 
                (p_current_cap->calib.coefficient_value.stage_1 + p_current_cap->calib.coefficient_value.stage_2 + p_current_cap->calib.coefficient_value.stage_3 + p_current_cap->calib.coefficient_value.stage_4) / 4.0;

                UART_Send_String(CMD_line_handle, "FINNISH RUNNING CALIB, CAP IS DISCHARGING\n");

                UART_Send_String(CMD_line_handle, "> ");

                Cap_Set_Discharge_All(true, true, true, true);

                Calib_Stage_Count = 0;
                p_current_cap->calib.is_measure_available = false;
                p_current_cap->calib.is_calib_running = false;
                Calib_State = CALIB_OFF_STATE;
                SchedulerTaskDisable(6);
                break;

            default:
                break;
            }
        }
        break;

    default:
        break;
    }
}

void Calib_Calculate_All(uint16_t hv_set_voltage, uint16_t lv_set_voltage)
{
    ;
}

void Calib_Calculate_HV(uint16_t hv_set_voltage)
{
    ;
}

void Calib_Calculate_LV(uint16_t lv_set_voltage)
{
    ;
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
static inline void Charge_Cap_Procedure(Cap_Controller_Task_typedef* p_cap_x, uint16_t set_volt)
{
    Cap_Set_Volt(p_cap_x, set_volt, false);
    Cap_Set_Charge(p_cap_x, true, false);
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ End of the program ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
