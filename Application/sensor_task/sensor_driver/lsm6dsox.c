/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Include~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
// #include <math.h>

// #include "app.h"

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "board.h"

#include "sensor_interface.h"

#include "lsm6dsox.h"

#include "i2c.h"

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Defines ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#define LSM6DSOX_ADDR                   0xD4
#define LSM6DSOX_STATUS_REG_ADDR		0x1E

#define LSM6DSOX_GYRO_REG_ADDR_BASE		0x22
#define LSM6DSOX_GYRO_REG_SIZE			6

#define LSM6DSOX_ACCEL_REG_ADDR_BASE	0x28
#define LSM6DSOX_ACCEL_REG_SIZE			6

#define LSM6DSOX_READ_ALL_ADDR_BASE		0x22
#define LSM6DSOX_READ_ALL_SIZE			12

#define LSM6DSOX_X_OFS_USR 				0x73
#define LSM6DSOX_Y_OFS_USR 				0x74
#define LSM6DSOX_Z_OFS_USR 				0x75

#define LSM6DSOX_ACCEL_FS_2G			0.061f
#define LSM6DSOX_ACCEL_FS_4G			0.122f
#define LSM6DSOX_ACCEL_FS_8G			0.244f
#define LSM6DSOX_ACCEL_FS_16G			0.488f

#define GYRO_SENSITIVITY_125DPS		    4.375f
#define GYRO_SENSITIVITY_250DPS		    8.75f
#define GYRO_SENSITIVITY_500DPS		    17.50f
#define GYRO_SENSITIVITY_1000DPS		35.0f
#define GYRO_SENSITIVITY_2000DPS		70.0f

#define LSM6DSOX_CALIB_TIMEOUT			5000

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Enum ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Struct ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Class ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Private Types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
typedef enum _Sensor_common_read_state_typedef_
{
	SENSOR_READ_RESET_STATE,
	SENSOR_CHECK_STATUS_STATE,
	SENSOR_READ_DATA_STATE,
	SENSOR_PROCESS_DATA_STATE,

} Sensor_common_read_state_typedef;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
static uint8_t Sensor_temp_buffer[30];
static uint8_t Sensor_Read_State = 0;
static uint8_t Sensor_Read_Value_State = 0;
// static uint8_t Sensor_Init_State = 0;

// static uint8_t Sensor_Calib_State = 0;
// static uint8_t Sensor_Calib_Count = 0;
// static int16_t Sensor_Calib_Timeout = LSM6DSOX_CALIB_TIMEOUT;
// static LSM6DSOX_data_typedef Sensor_Calib_Data;
static float Accel_Calib_Factor = LSM6DSOX_ACCEL_FS_2G;
static float Gyro_Calib_Factor 	= GYRO_SENSITIVITY_500DPS;

static bool is_LSM6DSOX_Init_Complete  = false;
static bool is_LSM6DSOX_Write_Complete  = false;
static bool is_LSM6DSOX_Read_Complete  = false;
static bool is_LSM6DSOX_Calib_Complete = false;

static bool is_LSM6DSOX_Data_Complete = false;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
static bool LSM6DSOX_is_value_ready
(
    Sensor_Read_typedef read_type,
	uint8_t *p_LSM6DSOX_RX_buffer
);

static void LSM6DSOX_read_uncompensated_value
(
	i2c_stdio_typedef*  p_i2c,
    Sensor_Read_typedef read_type,
	uint8_t *p_LSM6DSOX_RX_buffer
);

static void LSM6DSOX_compensate_value
(
    Sensor_Read_typedef read_type,
	uint8_t *p_LSM6DSOX_RX_buffer
);

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
LSM6DSOX_data_typedef Sensor_Gyro;
LSM6DSOX_data_typedef Sensor_Accel;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* :::::::::: LSM6DSOX Command :::::::: */
bool LSM6DSOX_init(i2c_stdio_typedef* p_i2c)
{
	switch (Sensor_Read_State)
    {

	case 0:
    {
		memset(Sensor_temp_buffer, 0, sizeof(Sensor_temp_buffer));

		Sensor_temp_buffer[0] =  0;
		Sensor_temp_buffer[0] |= (0x1001 << 7); // ACCEL ODR 3.33 kHZ
		Sensor_temp_buffer[0] |= (0x10 	 << 3); // ACCEL FULL-SCALE +-4g
		Sensor_temp_buffer[0] |= (0x0    << 1); // ACCEL HIGH-RESOLUTION DISABLE

		I2C_Mem_Write_IT(p_i2c, LSM6DSOX_ADDR, 0x10, Sensor_temp_buffer, 1, &is_LSM6DSOX_Write_Complete);
		
		Accel_Calib_Factor = LSM6DSOX_ACCEL_FS_4G;
		Sensor_Read_State = 1;
		return 0;
	}

	case 1:
    {
        if (Is_I2C_Write_Complete(&is_LSM6DSOX_Write_Complete) == false)
        {
            return 0;
        }
        
		//Sensor_temp_buffer[0] = 0x4C;

		Sensor_temp_buffer[0] =  0;
		Sensor_temp_buffer[0] |= (0x1001 << 7); // GYRO ODR 3.33 kHZ
		Sensor_temp_buffer[0] |= (0x01 	 << 3); // GYRO FULL-SCALE +-500dps
		Sensor_temp_buffer[0] |= (0x0    << 1); // FS_125 DISABLE

		I2C_Mem_Write_IT(p_i2c, LSM6DSOX_ADDR, 0x11, Sensor_temp_buffer, 1, &is_LSM6DSOX_Write_Complete);
		
		Gyro_Calib_Factor = GYRO_SENSITIVITY_500DPS;
		Sensor_Read_State = 2;
		return 0;
	}

	case 2:
    {
		if (Is_I2C_Write_Complete(&is_LSM6DSOX_Write_Complete) == false)
        {
            return 0;
        }

		Sensor_temp_buffer[0] = 0x00;		//0x02 for use user offset
		I2C_Mem_Write_IT(p_i2c, LSM6DSOX_ADDR, 0x16, Sensor_temp_buffer, 1, &is_LSM6DSOX_Write_Complete);
		Sensor_Read_State = 3;
		return 0;
	}

	case 3:
    {
		if (Is_I2C_Write_Complete(&is_LSM6DSOX_Write_Complete) == false)
        {
            return 0;
        }

		Sensor_temp_buffer[0] = 0x00;
		I2C_Mem_Write_IT(p_i2c, LSM6DSOX_ADDR, 0x17, Sensor_temp_buffer, 1, &is_LSM6DSOX_Write_Complete);
		Sensor_Read_State = 4;
		return 0;
	}

	case 4:
    {
		if (Is_I2C_Write_Complete(&is_LSM6DSOX_Write_Complete) == false)
        {
            return 0;
        }

		Sensor_temp_buffer[0] = 0x00;
		I2C_Mem_Write_IT(p_i2c, LSM6DSOX_ADDR, 0x75, Sensor_temp_buffer, 1, &is_LSM6DSOX_Write_Complete);
		Sensor_Read_State = 5;
		return 0;
	}

	case 5:
    {
        if (Is_I2C_Write_Complete(&is_LSM6DSOX_Write_Complete) == false)
        {
            return 0;
        }

		Sensor_Read_State = 0;
        is_LSM6DSOX_Init_Complete = true;
		return 1;
    }

	default:
		return 0;
	}
}

bool LSM6DSOX_read_value(i2c_stdio_typedef* p_i2c, Sensor_Read_typedef read_type)
{
	switch (Sensor_Read_Value_State)
    {

	case SENSOR_READ_RESET_STATE:
    {
		memset(Sensor_temp_buffer, 0, sizeof(Sensor_temp_buffer));

		Sensor_Read_Value_State = SENSOR_CHECK_STATUS_STATE;
		return 0;
	}

	case SENSOR_CHECK_STATUS_STATE:
    {
		I2C_Mem_Read_IT(p_i2c, LSM6DSOX_ADDR, LSM6DSOX_STATUS_REG_ADDR,
				Sensor_temp_buffer, 1, &is_LSM6DSOX_Read_Complete);

		Sensor_Read_Value_State = SENSOR_READ_DATA_STATE;
		return 0;
	}

	case SENSOR_READ_DATA_STATE:
    {
		if (Is_I2C_Read_Complete(&is_LSM6DSOX_Read_Complete) == false)
        {
            return 0;
        }

		if (LSM6DSOX_is_value_ready(read_type, Sensor_temp_buffer) == false)
        {
			Sensor_Read_State = SENSOR_CHECK_STATUS_STATE;
			return 0;
		}

		LSM6DSOX_read_uncompensated_value(p_i2c, read_type, &Sensor_temp_buffer[1]);

		Sensor_Read_Value_State = SENSOR_PROCESS_DATA_STATE;
		return 0;
	}

	case SENSOR_PROCESS_DATA_STATE:
    {
		if (Is_I2C_Read_Complete(&is_LSM6DSOX_Read_Complete) == false)
        {
            return 0;
        }

		LSM6DSOX_compensate_value(read_type, &Sensor_temp_buffer[1]);

		Sensor_Read_Value_State = SENSOR_READ_RESET_STATE;

        is_LSM6DSOX_Data_Complete = true;
		return 1;
	}

	default:
		return 0;
	}
}

/* :::::::::: LSM6DSOX Flag Check Command :::::::: */
bool Is_LSM6DSOX_Init_Complete()
{
    if (is_LSM6DSOX_Init_Complete == true)
    {
        is_LSM6DSOX_Init_Complete = false;
        return 1;
    }
    
    return 0;
}

bool Is_LSM6DSOX_Read_Complete()
{
    if (is_LSM6DSOX_Data_Complete == true)
    {
        is_LSM6DSOX_Data_Complete = false;
        return 1;
    }
    
    return 0;
}

bool Is_LSM6DSOX_Calib_Complete()
{
    if (is_LSM6DSOX_Calib_Complete == true)
    {
        is_LSM6DSOX_Calib_Complete = false;
        return 1;
    }
    
    return 0;
}

/* :::::::::: Sensor_LSM6DSOX Interface :::::::: */
Sensor_Interface Sensor_LSM6DSOX =
{
    .init       = &LSM6DSOX_init,
    .read_value = &LSM6DSOX_read_value,

    .is_init_complete = &Is_LSM6DSOX_Init_Complete,
    .is_read_complete = &Is_LSM6DSOX_Read_Complete,
};

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
static bool LSM6DSOX_is_value_ready(Sensor_Read_typedef read_type, uint8_t *p_status_value)
{
	switch (read_type)
    {
	case SENSOR_READ_GYRO:
		return (*p_status_value & (1 << 1));

	case SENSOR_READ_ACCEL:
		return (*p_status_value & (1 << 0));

	case SENSOR_READ_LSM6DSOX:
		return (*p_status_value & (1 << 0)) && (*p_status_value & (1 << 1));

	default:
		break;
	}
	return 0;
}

static void LSM6DSOX_read_uncompensated_value(i2c_stdio_typedef* p_i2c, Sensor_Read_typedef read_type, uint8_t *p_LSM6DSOX_RX_buffer)
{
	switch (read_type)
    {
	case SENSOR_READ_GYRO:
		I2C_Mem_Read_IT(p_i2c, LSM6DSOX_ADDR, LSM6DSOX_GYRO_REG_ADDR_BASE,
				p_LSM6DSOX_RX_buffer, LSM6DSOX_GYRO_REG_SIZE, &is_LSM6DSOX_Read_Complete);
		break;

	case SENSOR_READ_ACCEL:
		I2C_Mem_Read_IT(p_i2c, LSM6DSOX_ADDR, LSM6DSOX_ACCEL_REG_ADDR_BASE,
				p_LSM6DSOX_RX_buffer, LSM6DSOX_ACCEL_REG_SIZE, &is_LSM6DSOX_Read_Complete);
		break;

	case SENSOR_READ_LSM6DSOX:
		I2C_Mem_Read_IT(p_i2c, LSM6DSOX_ADDR, LSM6DSOX_READ_ALL_ADDR_BASE,
				p_LSM6DSOX_RX_buffer, LSM6DSOX_READ_ALL_SIZE, &is_LSM6DSOX_Read_Complete);
		break;

	default:
		break;
	}
}

static void LSM6DSOX_compensate_value(Sensor_Read_typedef read_type, uint8_t *p_LSM6DSOX_RX_buffer)
{
	switch (read_type)
    {

	case SENSOR_READ_GYRO:
    {
		Sensor_Gyro.x = ((p_LSM6DSOX_RX_buffer[1] << 8)
				| p_LSM6DSOX_RX_buffer[0]);
		Sensor_Gyro.x *= Gyro_Calib_Factor;
		Sensor_Gyro.y = ((p_LSM6DSOX_RX_buffer[3] << 8)
				| p_LSM6DSOX_RX_buffer[2]);
		Sensor_Gyro.y *= Gyro_Calib_Factor;
		Sensor_Gyro.z = ((p_LSM6DSOX_RX_buffer[5] << 8)
				| p_LSM6DSOX_RX_buffer[4]);
		Sensor_Gyro.z *= Gyro_Calib_Factor;
		break;
	}

	case SENSOR_READ_ACCEL:
    {
		Sensor_Accel.x = ((p_LSM6DSOX_RX_buffer[1] << 8)
				| p_LSM6DSOX_RX_buffer[0]);
		Sensor_Accel.x *= Accel_Calib_Factor;
		Sensor_Accel.y = ((p_LSM6DSOX_RX_buffer[3] << 8)
				| p_LSM6DSOX_RX_buffer[2]);
		Sensor_Accel.y *= Accel_Calib_Factor;
		Sensor_Accel.z = ((p_LSM6DSOX_RX_buffer[5] << 8)
				| p_LSM6DSOX_RX_buffer[4]);
		Sensor_Accel.z *= Accel_Calib_Factor;
		break;
	}

	case SENSOR_READ_LSM6DSOX:
    {
		Sensor_Gyro.x = ((p_LSM6DSOX_RX_buffer[1] << 8)
				| p_LSM6DSOX_RX_buffer[0]);
		Sensor_Gyro.x *= Gyro_Calib_Factor;
		Sensor_Gyro.y = ((p_LSM6DSOX_RX_buffer[3] << 8)
				| p_LSM6DSOX_RX_buffer[2]);
		Sensor_Gyro.y *= Gyro_Calib_Factor;
		Sensor_Gyro.z = ((p_LSM6DSOX_RX_buffer[5] << 8)
				| p_LSM6DSOX_RX_buffer[4]);
		Sensor_Gyro.z *= Gyro_Calib_Factor;

		Sensor_Accel.x = ((p_LSM6DSOX_RX_buffer[7] << 8)
				| p_LSM6DSOX_RX_buffer[6]);
		Sensor_Accel.x *= Accel_Calib_Factor;
		Sensor_Accel.y = ((p_LSM6DSOX_RX_buffer[9] << 8)
				| p_LSM6DSOX_RX_buffer[8]);
		Sensor_Accel.y *= Accel_Calib_Factor;
		Sensor_Accel.z = ((p_LSM6DSOX_RX_buffer[11] << 8)
				| p_LSM6DSOX_RX_buffer[10]);
		Sensor_Accel.z *= Accel_Calib_Factor;
		break;
	}

	default:
		break;
	}
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ End of the program ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
