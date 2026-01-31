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
	ONBOARD_SENSOR_SET_FS_100G,
	ONBOARD_SENSOR_SET_FS_200G,
	ONBOARD_SENSOR_SET_FS_400G,
	ONBOARD_SENSOR_GET_FS,

} Sensor_Read_typedef;

typedef struct
{
    uint8_t (*init)(i2c_stdio_typedef*);
    uint8_t (*read_value)(i2c_stdio_typedef*, Sensor_Read_typedef);

    uint8_t (*set_full_scale)(i2c_stdio_typedef*, uint32_t);
	uint8_t (*get_full_scale)(i2c_stdio_typedef*);

    bool (*is_init_complete)(void);
    bool (*is_read_complete)(void);
} Sensor_Interface;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Enum ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Struct ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Class ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#endif //_SENSOR_INTERFACE_H_
