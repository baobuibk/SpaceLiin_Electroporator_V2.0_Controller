#ifndef _SENSOR_INTERFACE_H_

#define _SENSOR_INTERFACE_H_

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Include ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include <stdbool.h>

#include "i2c.h"
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Defines ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
typedef enum _Sensor_Read_typedef_
{
	/* :::::::::: LSM6DSOX Read Data Type :::::::: */
	SENSOR_READ_GYRO,
	SENSOR_READ_ACCEL,
	SENSOR_READ_LSM6DSOX,

	/* :::::::::: BMP390 Read Data Type :::::::: */
	SENSOR_READ_TEMP,
	SENSOR_READ_PRESSURE,
	SENSOR_READ_ALTITUDE,
	SENSOR_READ_BMP390,

	/* :::::::::: H3LIS331DL Read Data Type :::::::: */
	SENSOR_READ_H3LIS331DL,

	/* :::::::::: H3LIS331DL Read Data Type :::::::: */
	ONBOARD_SENSOR_READ_H3LIS331DL,

} Sensor_Read_typedef;

typedef struct
{
    bool (*init)(i2c_stdio_typedef*);
    bool (*read_value)(i2c_stdio_typedef*, Sensor_Read_typedef);

    bool (*is_init_complete)(void);
    bool (*is_read_complete)(void);
} Sensor_Interface;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Enum ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Struct ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Class ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#endif //_SENSOR_INTERFACE_H_
