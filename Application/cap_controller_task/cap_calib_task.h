#ifndef CAP_CALIB_TASK_H_
#define CAP_CALIB_TASK_H_

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Include ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include <stdint.h>

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Defines ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
typedef struct _Cap_Calib_Coefficient_typedef_
{
    float default_value;
    float stage_1;
    float stage_2;
    float stage_3;
    float stage_4;
    float average_value;
} Cap_Calib_Coefficient_typedef;

typedef struct _Cap_Calib_Task_typdef_
{
    bool is_calib_running;
    bool is_measure_available;

    uint32_t measure_mv;
    char*    cap_name;

    Cap_Calib_Coefficient_typedef coefficient_value;

} Cap_Calib_Task_typdef;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
extern Cap_Calib_Task_typdef HV_Cap_Calib_Task;
extern Cap_Calib_Task_typdef LV_Cap_Calib_Task;

extern Cap_Calib_Coefficient_typedef HV_calib_coefficient;
extern Cap_Calib_Coefficient_typedef LV_calib_coefficient;

extern uint16_t g_HV_Calib_set;
extern uint16_t g_LV_Calib_set;

extern uint32_t g_HV_Measure_mv;
extern uint32_t g_LV_Measure_mv;

extern bool g_is_calib_running;
extern bool g_is_measure_available;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Enum ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Struct ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Class ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* :::::::::: Calib Task Init ::::::::::::: */
void Calib_Task_Init(void);

/* :::::::::: Calib Task ::::::::::::: */
void Calib_Task(void*);
void Calib_Calculate_All(uint16_t hv_set_voltage, uint16_t lv_set_voltage);
void Calib_Calculate_HV(uint16_t hv_set_voltage);
void Calib_Calculate_LV(uint16_t lv_set_voltage);

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ End of the program ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#endif /* CAP_CALIB_TASK_H_ */
