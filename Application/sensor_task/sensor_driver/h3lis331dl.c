/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Include~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "board.h"

#include "sensor_interface.h"

#include "h3lis331dl.h"

#include "i2c.h"

#include "stm32f4xx_ll_gpio.h"

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Defines ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#define H3LIS331DL_ADDR                 0x32
#define H3LIS331DL_STATUS_REG_ADDR      0x27

#define H3LIS331DL_ACCEL_REG_ADDR_BASE	0x28
#define H3LIS331DL_ACCEL_REG_SIZE		6

#define H3LIS331DL_READ_ALL_ADDR_BASE	0x28
#define H3LIS331DL_READ_ALL_SIZE		6

#define H3LIS331DL_X_OFS_USR 			0x73
#define H3LIS331DL_Y_OFS_USR 			0x74
#define H3LIS331DL_Z_OFS_USR 			0x75

#define H3LIS331DL_SENSITIVITY_100g	    49
#define H3LIS331DL_SENSITIVITY_200g	    98
#define H3LIS331DL_SENSITIVITY_400g	    195

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

static float H3LIS331DL_Sensivity = H3LIS331DL_SENSITIVITY_100g;

static bool is_H3LIS331DL_Init_Complete  = false;
static bool is_H3LIS331DL_Write_Complete  = false;
static bool is_H3LIS331DL_Read_Complete  = false;
//static bool is_H3LIS331DL_Calib_Complete = false;

static bool is_H3LIS331DL_Data_Complete  = false;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
static bool H3LIS331DL_is_value_ready(Sensor_Read_typedef read_type, uint8_t *p_status_value);
static void H3LIS331DL_read_raw_value(i2c_stdio_typedef* p_i2c, Sensor_Read_typedef read_type, uint8_t *p_H3LIS331DL_RX_buffer);
static void H3LIS331DL_read_accel_value(Sensor_Read_typedef read_type, uint8_t *p_H3LIS331DL_RX_buffer);

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
H3LIS331DL_data_typedef H3LIS_Accel;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* :::::::::: H3LIS331DL Command :::::::: */
bool H3LIS331DL_init(i2c_stdio_typedef* p_i2c)
{
    switch (Sensor_Read_State)
    {
        case 0:
        {
            LL_GPIO_SetOutputPin(ONBOARD_SENSOR_INT1_PORT, ONBOARD_SENSOR_INT1_PIN);
            LL_GPIO_SetOutputPin(ONBOARD_SENSOR_INT2_PORT, ONBOARD_SENSOR_INT2_PIN);
            memset(Sensor_temp_buffer, 0, sizeof(Sensor_temp_buffer));
            I2C_Mem_Read_IT(p_i2c, H3LIS331DL_ADDR, 0x0F, Sensor_temp_buffer, 1, &is_H3LIS331DL_Read_Complete);
            Sensor_Read_State = 1;
            return 0;
        }
        case 1:
        {
            if (Is_I2C_Read_Complete(&is_H3LIS331DL_Read_Complete) == false)
            {
                return 0;
            }
            if (Sensor_temp_buffer[0] != 0x32) // Kiểm tra WHO_AM_I
            {
                Sensor_Read_State = 0;
                return 0;
            }
            // CTRL_REG2: BOOT = 1 để reboot
            Sensor_temp_buffer[0] = 0x80;
            I2C_Mem_Write_IT(p_i2c, H3LIS331DL_ADDR, 0x21, Sensor_temp_buffer, 1, &is_H3LIS331DL_Write_Complete);
            Sensor_Read_State = 2;
            return 0;
        }
        case 2:
        {
            if (Is_I2C_Write_Complete(&is_H3LIS331DL_Write_Complete) == false)
            {
                return 0;
            }
            // Kiểm tra BOOT = 0
            I2C_Mem_Read_IT(p_i2c, H3LIS331DL_ADDR, 0x21, Sensor_temp_buffer, 1, &is_H3LIS331DL_Read_Complete);
            Sensor_Read_State = 3;
            return 0;
        }
        case 3:
        {
            if (Is_I2C_Read_Complete(&is_H3LIS331DL_Read_Complete) == false)
            {
                return 0;
            }
            if (Sensor_temp_buffer[0] != 0x00) // BOOT chưa hoàn tất
            {
                Sensor_Read_State = 2;
                is_H3LIS331DL_Write_Complete = true;
                return 0;
            }
            // CTRL_REG2: Bật HPF cho data out, HPM = 01 (Reference mode)
            Sensor_temp_buffer[0] = 0x00;
            Sensor_temp_buffer[0] |= (1 << 4); // HPen1 = 1 (HPF cho data out)
            Sensor_temp_buffer[0] |= (0 << 3); // HPM1 = 0
            Sensor_temp_buffer[0] |= (1 << 2); // HPM0 = 1 (HPM = 01, Reference mode)
            I2C_Mem_Write_IT(p_i2c, H3LIS331DL_ADDR, 0x21, Sensor_temp_buffer, 1, &is_H3LIS331DL_Write_Complete);
            Sensor_Read_State = 4;
            return 0;
        }
        case 4:
        {
            if (Is_I2C_Write_Complete(&is_H3LIS331DL_Write_Complete) == false)
            {
                return 0;
            }
            // REFERENCE: Đặt giá trị tham chiếu cho HPF (0x14)
            Sensor_temp_buffer[0] = 0x14;
            I2C_Mem_Write_IT(p_i2c, H3LIS331DL_ADDR, 0x26, Sensor_temp_buffer, 1, &is_H3LIS331DL_Write_Complete);
            Sensor_Read_State = 5;
            return 0;
        }
        case 5:
        {
            if (Is_I2C_Write_Complete(&is_H3LIS331DL_Write_Complete) == false)
            {
                return 0;
            }
            // CTRL_REG4: FS = ±100g, BDU = 1
            Sensor_temp_buffer[0] = 0x80; // BDU = 1
            I2C_Mem_Write_IT(p_i2c, H3LIS331DL_ADDR, 0x23, Sensor_temp_buffer, 1, &is_H3LIS331DL_Write_Complete);
            Sensor_Read_State = 6;
            return 0;
        }
        case 6:
        {
            if (Is_I2C_Write_Complete(&is_H3LIS331DL_Write_Complete) == false)
            {
                return 0;
            }
            // CTRL_REG1: ODR = 1000 Hz, Normal Mode, bật X, Y, Z
            Sensor_temp_buffer[0] = 0x37; // PM0=1, DR1=DR0=1, Zen=Yen=Xen=1
            I2C_Mem_Write_IT(p_i2c, H3LIS331DL_ADDR, 0x20, Sensor_temp_buffer, 1, &is_H3LIS331DL_Write_Complete);
            Sensor_Read_State = 7;
            return 0;
        }
        case 7:
        {
            if (Is_I2C_Write_Complete(&is_H3LIS331DL_Write_Complete) == false)
            {
                return 0;
            }
            // Đọc lại CTRL_REG1 để xác nhận
            I2C_Mem_Read_IT(p_i2c, H3LIS331DL_ADDR, 0x20, Sensor_temp_buffer, 1, &is_H3LIS331DL_Read_Complete);
            Sensor_Read_State = 8;
            return 0;
        }
        case 8:
        {
            if (Is_I2C_Read_Complete(&is_H3LIS331DL_Read_Complete) == false)
            {
                return 0;
            }
            if (Sensor_temp_buffer[0] != 0x37)
            {
                Sensor_Read_State = 6; // Thử ghi lại
                return 0;
            }
            for (volatile uint32_t i = 0; i < 100000; i++); // Delay 10ms
            // CTRL_REG3: Tắt ngắt
            Sensor_temp_buffer[0] = 0x00;
            I2C_Mem_Write_IT(p_i2c, H3LIS331DL_ADDR, 0x22, Sensor_temp_buffer, 1, &is_H3LIS331DL_Write_Complete);
            Sensor_Read_State = 9;
            return 0;
        }
        case 9:
        {
            if (Is_I2C_Write_Complete(&is_H3LIS331DL_Write_Complete) == false)
            {
                return 0;
            }
            // INT1_CFG: Tắt ngắt
            Sensor_temp_buffer[0] = 0x00;
            I2C_Mem_Write_IT(p_i2c, H3LIS331DL_ADDR, 0x30, Sensor_temp_buffer, 1, &is_H3LIS331DL_Write_Complete);
            Sensor_Read_State = 10;
            return 0;
        }
        case 10:
        {
            if (Is_I2C_Write_Complete(&is_H3LIS331DL_Write_Complete) == false)
            {
                return 0;
            }
            // INT2_CFG: Tắt ngắt
            Sensor_temp_buffer[0] = 0x00;
            I2C_Mem_Write_IT(p_i2c, H3LIS331DL_ADDR, 0x34, Sensor_temp_buffer, 1, &is_H3LIS331DL_Write_Complete);
            Sensor_Read_State = 11;
            return 0;
        }
        case 11:
        {
            if (Is_I2C_Write_Complete(&is_H3LIS331DL_Write_Complete) == false)
            {
                return 0;
            }
            // Kiểm tra CTRL_REG3
            I2C_Mem_Read_IT(p_i2c, H3LIS331DL_ADDR, 0x22, Sensor_temp_buffer, 1, &is_H3LIS331DL_Read_Complete);
            Sensor_Read_State = 12;
            return 0;
        }
        case 12:
        {
            if (Is_I2C_Read_Complete(&is_H3LIS331DL_Read_Complete) == false)
            {
                return 0;
            }
            if (Sensor_temp_buffer[0] != 0x00)
            {
                Sensor_Read_State = 0;
                return 0;
            }
            // Kiểm tra INT1_CFG
            I2C_Mem_Read_IT(p_i2c, H3LIS331DL_ADDR, 0x30, Sensor_temp_buffer, 1, &is_H3LIS331DL_Read_Complete);
            Sensor_Read_State = 13;
            return 0;
        }
        case 13:
        {
            if (Is_I2C_Read_Complete(&is_H3LIS331DL_Read_Complete) == false)
            {
                return 0;
            }
            if (Sensor_temp_buffer[0] != 0x00)
            {
                Sensor_Read_State = 0;
                return 0;
            }
            // Kiểm tra INT2_CFG
            I2C_Mem_Read_IT(p_i2c, H3LIS331DL_ADDR, 0x34, Sensor_temp_buffer, 1, &is_H3LIS331DL_Read_Complete);
            Sensor_Read_State = 14;
            return 0;
        }
        case 14:
        {
            if (Is_I2C_Read_Complete(&is_H3LIS331DL_Read_Complete) == false)
            {
                return 0;
            }
            if (Sensor_temp_buffer[0] != 0x00)
            {
                Sensor_Read_State = 0;
                return 0;
            }
            // Reset HPF
            I2C_Mem_Read_IT(p_i2c, H3LIS331DL_ADDR, 0x25, Sensor_temp_buffer, 1, &is_H3LIS331DL_Read_Complete); // Đọc HP_FILTER_RESET
            Sensor_Read_State = 15;
            return 0;
        }
        case 15:
        {
            if (Is_I2C_Read_Complete(&is_H3LIS331DL_Read_Complete) == false)
            {
                return 0;
            }

			is_H3LIS331DL_Init_Complete = true;

			Sensor_Read_State = 0;
            return 1;
        }

        default:
            return 0;
    }
}

bool H3LIS331DL_read_value(i2c_stdio_typedef* p_i2c, Sensor_Read_typedef read_type)
{
    switch (Sensor_Read_Value_State)
    {

	case 0:
    {
        memset(Sensor_temp_buffer, 0, sizeof(Sensor_temp_buffer));
        I2C_Mem_Read_IT(p_i2c, H3LIS331DL_ADDR, 0x0F, Sensor_temp_buffer, 1, &is_H3LIS331DL_Read_Complete);
        Sensor_Read_Value_State = 1;
        return 0;
    }

	case 1:
    {
		if (Is_I2C_Read_Complete(&is_H3LIS331DL_Read_Complete) == false)
        {
            return 0;
        }
        if (Sensor_temp_buffer[0] != 0x32) // Check WHO_AM_I
        {
            Sensor_Read_Value_State = 0;
            return 0;
        }

		I2C_Mem_Read_IT
        (
            p_i2c, H3LIS331DL_ADDR,
            H3LIS331DL_STATUS_REG_ADDR,
			Sensor_temp_buffer,
            1, &is_H3LIS331DL_Read_Complete
        );

		Sensor_Read_Value_State = 2;
		return 0;
	}

	case 2:
    {
		if (Is_I2C_Read_Complete(&is_H3LIS331DL_Read_Complete) == false)
        {
            return 0;
        }

		if (H3LIS331DL_is_value_ready(read_type, Sensor_temp_buffer) == false)
       	{
			Sensor_Read_Value_State = SENSOR_CHECK_STATUS_STATE;
			return 0;
		}

		I2C_Mem_Read_IT
		(
			p_i2c,
			H3LIS331DL_ADDR,
			H3LIS331DL_ACCEL_REG_ADDR_BASE + 0,
			&Sensor_temp_buffer[1],
			1,
			&is_H3LIS331DL_Read_Complete
		);

		Sensor_Read_Value_State = 3;
		return 0;
	}

	case 3:
    {
		if (Is_I2C_Read_Complete(&is_H3LIS331DL_Read_Complete) == false)
        {
            return 0;
        }

		I2C_Mem_Read_IT
		(
			p_i2c,
			H3LIS331DL_ADDR,
			H3LIS331DL_ACCEL_REG_ADDR_BASE + 1,
			&Sensor_temp_buffer[2],
			1,
			&is_H3LIS331DL_Read_Complete
		);

		Sensor_Read_Value_State = 4;
		return 0;
	}

	case 4:
    {
		if (Is_I2C_Read_Complete(&is_H3LIS331DL_Read_Complete) == false)
        {
            return 0;
        }

		I2C_Mem_Read_IT
		(
			p_i2c,
			H3LIS331DL_ADDR,
			H3LIS331DL_ACCEL_REG_ADDR_BASE + 2,
			&Sensor_temp_buffer[3],
			1,
			&is_H3LIS331DL_Read_Complete
		);

		Sensor_Read_Value_State = 5;
		return 0;
	}

	case 5:
    {
		if (Is_I2C_Read_Complete(&is_H3LIS331DL_Read_Complete) == false)
        {
            return 0;
        }

		I2C_Mem_Read_IT
		(
			p_i2c,
			H3LIS331DL_ADDR,
			H3LIS331DL_ACCEL_REG_ADDR_BASE + 3,
			&Sensor_temp_buffer[4],
			1,
			&is_H3LIS331DL_Read_Complete
		);

		Sensor_Read_Value_State = 6;
		return 0;
	}

	case 6:
    {
		if (Is_I2C_Read_Complete(&is_H3LIS331DL_Read_Complete) == false)
        {
            return 0;
        }

		I2C_Mem_Read_IT
		(
			p_i2c,
			H3LIS331DL_ADDR,
			H3LIS331DL_ACCEL_REG_ADDR_BASE + 4,
			&Sensor_temp_buffer[5],
			1,
			&is_H3LIS331DL_Read_Complete
		);

		Sensor_Read_Value_State = 7;
		return 0;
	}

	case 7:
    {
		if (Is_I2C_Read_Complete(&is_H3LIS331DL_Read_Complete) == false)
        {
            return 0;
        }

		I2C_Mem_Read_IT
		(
			p_i2c,
			H3LIS331DL_ADDR,
			H3LIS331DL_ACCEL_REG_ADDR_BASE + 5,
			&Sensor_temp_buffer[6],
			1,
			&is_H3LIS331DL_Read_Complete
		);

		Sensor_Read_Value_State = 8;
		return 0;
	}

	case 8:
    {
		if (Is_I2C_Read_Complete(&is_H3LIS331DL_Read_Complete) == false)
        {
            return 0;
        }

		H3LIS331DL_read_accel_value(read_type, &Sensor_temp_buffer[1]);

		Sensor_Read_Value_State = 0;

        is_H3LIS331DL_Data_Complete = true;
		return 1;
	}

	default:
		return 0;
	}
}

/* :::::::::: H3LIS331DL Flag Check Command :::::::: */
bool Is_H3LIS331DL_Init_Complete(void)
{
    if (is_H3LIS331DL_Init_Complete == true)
    {
        is_H3LIS331DL_Init_Complete = false;
        return 1;
    }
    
    return 0;
}

bool Is_H3LIS331DL_Read_Complete(void)
{
    if (is_H3LIS331DL_Data_Complete == true)
    {
        is_H3LIS331DL_Data_Complete = false;
        return 1;
    }
    
    return 0;
}

/* :::::::::: Sensor_LSM6DSOX Interface :::::::: */
Sensor_Interface Sensor_H3LIS331DL =
{
    .init       = &H3LIS331DL_init,
    .read_value = &H3LIS331DL_read_value,

    .is_init_complete = &Is_H3LIS331DL_Init_Complete,
    .is_read_complete = &Is_H3LIS331DL_Read_Complete,
};

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
static bool H3LIS331DL_is_value_ready(Sensor_Read_typedef read_type, uint8_t *p_status_value)
{
	switch (read_type)
    {
	case SENSOR_READ_H3LIS331DL:
	case ONBOARD_SENSOR_READ_H3LIS331DL:
		return ((*p_status_value & (1 << 3)) == (1 << 3)) ;

	default:
		break;
	}
	return 0;
}

static void H3LIS331DL_read_raw_value(i2c_stdio_typedef* p_i2c, Sensor_Read_typedef read_type, uint8_t *p_H3LIS331DL_RX_buffer)
{
	switch (read_type)
    {
	case SENSOR_READ_H3LIS331DL:
	case ONBOARD_SENSOR_READ_H3LIS331DL:
	{
		// I2C_Mem_Read_IT
        // (
        //     p_i2c,
        //     H3LIS331DL_ADDR,
		// 	H3LIS331DL_ACCEL_REG_ADDR_BASE,
		// 	p_H3LIS331DL_RX_buffer,
        //     H3LIS331DL_ACCEL_REG_SIZE,
		// 	&is_H3LIS331DL_Read_Complete
        // );

		// for (uint8_t i = 0; i < H3LIS331DL_ACCEL_REG_SIZE; i++)
		// {
		// 	I2C_Mem_Read_IT
		// 	(
		// 		p_i2c,
		// 		H3LIS331DL_ADDR,
		// 		H3LIS331DL_ACCEL_REG_ADDR_BASE + i,
		// 		&p_H3LIS331DL_RX_buffer[i],
		// 		1,
		// 		&is_H3LIS331DL_Read_Complete
		// 	);
		// }
		
		break;
	}

	default:
		break;
	}
}

// static void H3LIS331DL_read_accel_value(Sensor_Read_typedef read_type, uint8_t *p_H3LIS331DL_RX_buffer)
// {
// 	switch (read_type)
//     {

// 	case SENSOR_READ_H3LIS331DL:
// 	case ONBOARD_SENSOR_READ_H3LIS331DL:
//     {
// 		H3LIS_Accel.x = ((p_H3LIS331DL_RX_buffer[1] << 8) | p_H3LIS331DL_RX_buffer[0]);
// 		//H3LIS_Accel.x *= H3LIS331DL_Sensivity;

// 		H3LIS_Accel.y = ((p_H3LIS331DL_RX_buffer[3] << 8) | p_H3LIS331DL_RX_buffer[2]);
// 		//H3LIS_Accel.y *= H3LIS331DL_Sensivity;

// 		H3LIS_Accel.z = ((p_H3LIS331DL_RX_buffer[5] << 8) | p_H3LIS331DL_RX_buffer[4]);
// 		//H3LIS_Accel.z *= H3LIS331DL_Sensivity;
// 		break;
// 	}

// 	default:
// 		break;
// 	}
// }

static void H3LIS331DL_read_accel_value(Sensor_Read_typedef read_type, uint8_t *p_H3LIS331DL_RX_buffer)
{
    switch (read_type)
    {
        case SENSOR_READ_H3LIS331DL:
        case ONBOARD_SENSOR_READ_H3LIS331DL:
        {
            // Ghép MSB và LSB, lấy giá trị thô 16-bit
            int16_t raw_x = (int16_t)((p_H3LIS331DL_RX_buffer[1] << 8) | p_H3LIS331DL_RX_buffer[0]);
            int16_t raw_y = (int16_t)((p_H3LIS331DL_RX_buffer[3] << 8) | p_H3LIS331DL_RX_buffer[2]);
            int16_t raw_z = (int16_t)((p_H3LIS331DL_RX_buffer[5] << 8) | p_H3LIS331DL_RX_buffer[4]);

            // Dịch phải 4 bit để right-align 12-bit
            raw_x >>= 4;
            raw_y >>= 4;
            raw_z >>= 4;

            // Chuyển đổi sang gia tốc (mg) với độ nhạy 49 mg/digit
            int32_t accel_x = raw_x * H3LIS331DL_Sensivity; // mg
            int32_t accel_y = raw_y * H3LIS331DL_Sensivity; // mg
            int32_t accel_z = raw_z * H3LIS331DL_Sensivity; // mg

            // Chuyển đổi sang gia tốc (g) với độ nhạy 49 mg/digit (0.049 g/digit)
            // float sensitivity = H3LIS331DL_Sensivity / 1000.0f; // 49 mg/digit = 0.049 g/digit
			// uint8_t sensitivity = H3LIS331DL_Sensivity; // 49 mg/digit = 0.049 g/digit
            H3LIS_Accel.x = accel_x;
            H3LIS_Accel.y = accel_y;
            H3LIS_Accel.z = accel_z;

            // Hiệu chỉnh offset thủ công (dựa trên giá trị đo được)
            // float offset_x = 52.0f;  // Đo khi cảm biến nằm ngang
            // float offset_y = -24.0f; // Đo khi cảm biến nằm ngang
            // float offset_z = 21.0f;  // 22g - 1g để Z ~ 1g
            // H3LIS_Accel.x -= offset_x;
            // H3LIS_Accel.y -= offset_y;
            // H3LIS_Accel.z -= offset_z;

            break;
        }
        default:
            break;
    }
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ End of the program ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
