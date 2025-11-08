#ifndef CAP_CONTROLLER_TASK_H_
#define CAP_CONTROLLER_TASK_H_

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Include ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include <stdint.h>
#include <string.h>

#include "board.h"

#include "cap_calib_task.h"

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Defines ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#define CAP_300V   0
#define CAP_50V    1

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
typedef enum
{
    CAP_IS_IDLE,                // NO CHARGE OR DISCHARGE
    CAP_IS_CHARGING,
    CAP_IS_FINISH_CHARGING,
    CAP_IS_DISCHARGING,
    CAP_IS_FINISH_DISCHARGING,

} CAP_State_t;

typedef enum
{
    CAP_SOFT_START_STATE,
    CAP_CONTROL_CHARGE_STATE,
    CAP_SET_FREE_CHARGE_STATE,
    CAP_IS_FREE_CHARGE_STATE,
} Cap_Charge_State_t;

typedef struct
{
    uint16_t Volt_Value;
    uint16_t ADC_Value;
    uint8_t  Duty_Max;
} Charge_Range_t;

typedef struct _Cap_Controller_Task_typedef_
{
    CAP_State_t cap_state;

    uint16_t    max_cap_volt;
    uint16_t    feedback_ADC;
    uint16_t    feedback_VOLT;

    Cap_Charge_State_t  charge_state;
    Charge_Range_t*     p_range;
    uint8_t             range_max;
    uint8_t             range_set;
    uint8_t             range_index;
    uint16_t            soft_start_count;

    bool        is_charge_on;
    uint16_t    set_charge_voltage_USER;
    uint16_t    set_charge_voltage_ADC;

    bool        is_discharge_on;
    uint16_t    set_discharge_voltage_USER;
    uint16_t    set_discharge_voltage_ADC;

    bool        is_notified_enable;

    Cap_Calib_Task_typdef calib;

} Cap_Controller_Task_typedef;

typedef enum _Set_Voltage_Type_
{
    ADC_VALUE_TYPE,
    VOLT_VALUE_TYPE,

} Set_Voltage_Type;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
extern uint16_t     g_Feedback_Voltage[ADC_CHANNEL_COUNT];

extern  bool		g_PID_is_300V_on;
extern	uint16_t 	g_PID_300V_set_voltage;
extern  bool        g_is_Discharge_300V_On;
extern  bool        is_300V_notified_enable;

extern	bool		g_PID_is_50V_on;
extern	uint16_t 	g_PID_50V_set_voltage;
extern  bool        g_is_Discharge_50V_On;
extern  bool        is_50V_notified_enable;

extern Cap_Controller_Task_typedef g_Cap_300V;
extern Cap_Controller_Task_typedef g_Cap_50V;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Enum ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Struct ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Class ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* :::::::::: Cap Controller Driver Init :::::::: */
void Cap_Controller_Init(void);

/* :::::::::: Cap Controller Charge Task ::::::::::::: */
void Cap_Controller_Charge_Task(void*);

/* :::::::::: Cap Controller Ultility Task ::::::::::::: */
void Cap_Controller_Monitor_Task(void*);

/* :::::::::: Cap Controller Command :::::::: */
void Cap_Set_Volt(Cap_Controller_Task_typedef* cap_x, uint16_t set_voltage, bool is_notified);
void Cap_Set_Volt_All(uint16_t HV_set_voltage, uint16_t LV_set_voltage, bool HV_is_notified, bool LV_is_notified);
void Cap_Set_Charge(Cap_Controller_Task_typedef* cap_x, bool charge_state, bool is_notified);
void Cap_Set_Charge_All(bool HV_charge_state, bool LV_charge_state, bool HV_is_notified, bool LV_is_notified);
void Cap_Set_Discharge(Cap_Controller_Task_typedef* cap_x, bool discharge_state, bool is_notified);
void Cap_Set_Discharge_All(bool HV_discharge_state, bool LV_discharge_state, bool HV_is_notified, bool LV_is_notified);

uint16_t Cap_Get_Set_Volt(Cap_Controller_Task_typedef* cap_x);
bool     Cap_Get_is_Charge(Cap_Controller_Task_typedef* cap_x);
bool     Cap_Get_is_Discharge(Cap_Controller_Task_typedef* cap_x);
bool     Cap_Get_is_Notified(Cap_Controller_Task_typedef* cap_x);

uint16_t Cap_Measure_Volt(Cap_Controller_Task_typedef* p_cap_x);

/* :::::::::: Cap Controller Driver Interupt Handler ::::::::::::: */
void Cap_Controller_ADC_IRQHandler(void);

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ End of the program ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#endif /* CAP_CONTROLLER_TASK_H_ */
