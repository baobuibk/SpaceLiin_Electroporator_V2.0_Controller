#ifndef CALIB_TASK_H_
#define CALIB_TASK_H_

// /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Include ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
// #include <stdint.h>

// /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Defines ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
// /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
// struct _hv_calib_coefficient
// {
//     float default_value;
//     float at_50v;
//     float at_100v;
//     float at_200v;
//     float at_300v;
//     float average_value;
// };

// struct _lv_calib_coefficient
// {
//     float default_value;
//     float at_5v;
//     float at_10v;
//     float at_25v;
//     float at_50v;
//     float average_value;
// };

// /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
// extern struct _hv_calib_coefficient HV_calib_coefficient;
// extern struct _lv_calib_coefficient LV_calib_coefficient;

// extern uint16_t g_HV_Calib_set;
// extern uint16_t g_LV_Calib_set;

// extern uint32_t g_HV_Measure_mv;
// extern uint32_t g_LV_Measure_mv;

// extern bool g_is_calib_running;
// extern bool g_is_measure_available;

// /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Enum ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
// /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Struct ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
// /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Class ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
// /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
// /* :::::::::: Calib Task Init ::::::::::::: */
// void Calib_Task_Init(void);

// /* :::::::::: Calib Task ::::::::::::: */
// void Calib_Task(void*);
// void Calib_Calculate_All(uint16_t hv_set_voltage, uint16_t lv_set_voltage);
// void Calib_Calculate_HV(uint16_t hv_set_voltage);
// void Calib_Calculate_LV(uint16_t lv_set_voltage);

// /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ End of the program ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#endif /* CALIB_TASK_H_ */
