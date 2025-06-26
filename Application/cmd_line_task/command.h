#ifndef COMMAND_H_
#define COMMAND_H_

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Include ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include <stdint.h>

#include "fsp.h"
#include "crc.h"

#include "cmd_line.h"

#include "fsp_line_task.h"

#include "app.h"

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Defines ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Enum ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Struct ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Class ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
typedef struct _H_Bridge_task_typedef_
{
    uint8_t     is_setted;

    uint16_t    sequence_delay_ms;

    uint8_t     pos_pole_index;
    uint8_t     neg_pole_index;

    uint16_t    pulse_delay_ms;

    uint8_t     hv_pos_count;
    uint8_t     hv_neg_count;

    uint8_t     hv_delay_ms;

    uint8_t     hv_pos_on_ms;
    uint8_t     hv_pos_off_ms;
    uint8_t     hv_neg_on_ms;
    uint8_t     hv_neg_off_ms;

    uint8_t     lv_pos_count;
    uint8_t     lv_neg_count;

    uint8_t     lv_delay_ms;

    uint8_t     lv_pos_on_ms;
    uint8_t     lv_pos_off_ms;
    uint8_t     lv_neg_on_ms;
    uint8_t     lv_neg_off_ms;
} H_Bridge_task_typedef;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
extern bool                     is_manual_mode_enable;
extern bool						is_h_bridge_enable;
extern H_Bridge_task_typedef    HB_sequence_default;
extern H_Bridge_task_typedef    HB_sequence_array[10];

extern uint8_t CMD_sequence_index;
extern uint8_t CMD_total_sequence_index;

extern bool is_streaming_enable;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* :::::::::: Cap Control Command :::::::: */
int CMD_SET_CAP_VOLT(int argc, char *argv[]);
int CMD_SET_CAP_CONTROL(int argc, char *argv[]);
int CMD_SET_CAP_RELEASE(int argc, char *argv[]);

int CMD_GET_CAP_VOLT(int argc, char *argv[]);
int CMD_GET_CAP_CONTROL(int argc, char *argv[]);
int CMD_GET_CAP_RELEASE(int argc, char *argv[]);
int CMD_GET_CAP_ALL(int argc, char *argv[]);

/* :::::::::: Pulse Control Command :::::::: */
int CMD_SET_SEQUENCE_INDEX(int argc, char *argv[]);
int CMD_SET_SEQUENCE_DELETE(int argc, char *argv[]);
int CMD_SET_SEQUENCE_CONFIRM(int argc, char *argv[]);
int CMD_SET_SEQUENCE_DELAY(int argc, char *argv[]);

int CMD_SET_PULSE_POLE(int argc, char *argv[]);
int CMD_SET_PULSE_COUNT(int argc, char *argv[]);
int CMD_SET_PULSE_DELAY(int argc, char *argv[]);
int CMD_SET_PULSE_HV_POS(int argc, char *argv[]);
int CMD_SET_PULSE_HV_NEG(int argc, char *argv[]);
int CMD_SET_PULSE_LV_POS(int argc, char *argv[]);
int CMD_SET_PULSE_LV_NEG(int argc, char *argv[]);
int CMD_SET_PULSE_CONTROL(int argc, char *argv[]);

int CMD_GET_SEQUENCE_INDEX(int argc, char *argv[]);
int CMD_GET_SEQUENCE_DELAY(int argc, char *argv[]);
int CMD_GET_SEQUENCE_ALL(int argc, char *argv[]);

int CMD_GET_PULSE_POLE(int argc, char *argv[]);
int CMD_GET_PULSE_COUNT(int argc, char *argv[]);
int CMD_GET_PULSE_DELAY(int argc, char *argv[]);
int CMD_GET_PULSE_HV_POS(int argc, char *argv[]);
int CMD_GET_PULSE_HV_NEG(int argc, char *argv[]);
int CMD_GET_PULSE_HV(int argc, char *argv[]);
int CMD_GET_PULSE_LV_POS(int argc, char *argv[]);
int CMD_GET_PULSE_LV_NEG(int argc, char *argv[]);
int CMD_GET_PULSE_LV(int argc, char *argv[]);
int CMD_GET_PULSE_CONTROL(int argc, char *argv[]);
int CMD_GET_PULSE_ALL(int argc, char *argv[]);

/* :::::::::: Auto Pulsing Command :::::::::: */
int CMD_SET_THRESHOLD_ACCEL(int argc, char *argv[]);
int CMD_GET_THRESHOLD_ACCEL(int argc, char *argv[]);
int CMD_SET_AUTO_ACCEL(int argc, char *argv[]);

/* :::::::::: Manual Pulse Command :::::::::: */
int CMD_SET_MANUAL_POLE(int argc, char *argv[]);
int CMD_SET_MANUAL_CAP(int argc, char *argv[]);
int CMD_SET_MANUAL_PULSE(int argc, char *argv[]);

/* :::::::::: VOM Command :::::::: */
int CMD_MEASURE_VOLT(int argc, char *argv[]);
int CMD_MEASURE_CURRENT(int argc, char *argv[]);
int CMD_MEASURE_IMPEDANCE(int argc, char *argv[]);

int CMD_SET_CURRENT_LIMIT(int argc, char *argv[]);
int CMD_RESET_OVC_FLAG(int argc, char *argv[]);

int CMD_GET_CURRENT_LIMIT(int argc, char *argv[]);
int CMD_GET_OVC_FLAG(int argc, char *argv[]);

/* :::::::::: I2C Sensor Command :::::::: */
int CMD_GET_SENSOR_GYRO(int argc, char *argv[]);
int CMD_GET_SENSOR_ACCEL(int argc, char *argv[]);
int CMD_GET_SENSOR_LSM6DSOX(int argc, char *argv[]);

int CMD_GET_SENSOR_TEMP(int argc, char *argv[]);
int CMD_GET_SENSOR_PRESSURE(int argc, char *argv[]);
int CMD_GET_SENSOR_ALTITUDE(int argc, char *argv[]);
int CMD_GET_SENSOR_BMP390(int argc, char *argv[]);

int CMD_GET_SENSOR_H3LIS(int argc, char *argv[]);

/* :::::::::: Ultility Command :::::::: */
int CMD_HELP(int argc, char *argv[]);
int CMD_CALIB_RUN(int argc, char *argv[]);
int CMD_CALIB_MEASURE(int argc, char *argv[]);
int CMD_CALL_GPP(int argc, char *argv[]);
int CMD_CLEAR_SCREEN(int argc, char *argv[]);
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ End of the program ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#endif /* COMMAND_H_ */
