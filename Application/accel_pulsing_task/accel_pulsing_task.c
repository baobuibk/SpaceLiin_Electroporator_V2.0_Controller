/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Include~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "app.h"
#include "stdbool.h"

#include "accel_pulsing_task.h"

#include "crc.h"

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Defines ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#define AUTO_PULSE_TIMEOUT 500

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Enum ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Struct ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Class ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Private Types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
typedef enum _auto_pulsing_state_typedef_
{
	DISABLE_AUTO_PULSING = 0,
	PREPARE_SEND_TO_SENSOR,
	SEND_RESQUEST_SENSOR,
	SENSOR_ERROR,
	SYSTERM_PULSING_RUN,
	SYSTERM_PULSING_DONE,

} auto_pulsing_state_typedef;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
static auto_pulsing_state_typedef auto_pulsing_state = DISABLE_AUTO_PULSING;

static bool    is_accel_pulsing_disable = true;

static uint16_t get_sensor_timeout = AUTO_PULSE_TIMEOUT;

static uint8_t frame_count = 0;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
static void fsp_print(uint8_t packet_length);

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
// Remember to change Threshold_Accel in .h file
H3LIS331DL_data_typedef Threshold_Accel = { 0, 0, 2000 };
H3LIS331DL_data_typedef Auto_Accel_Data;

#define ACCEL_SENSOR 	ONBOARD_SENSOR_READ_H3LIS331DL
#define P_ACCEL_REQUEST &Onboard_Sensor_H3LIS331DL_rb
#define ACCEL_DATA		H3LIS_Accel

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
void Accel_Pulsing_Task(void*)
{
	if (is_accel_pulsing_disable == true)
	{
		SchedulerTaskDisable(ACCEL_PULSING_TASK);
		auto_pulsing_state = DISABLE_AUTO_PULSING;
		return;
	}

	switch (auto_pulsing_state)
	{
	case PREPARE_SEND_TO_SENSOR:
	{
		// move up 1 line, turn off console
		UART_Send_String(CMD_line_handle, "\e[?25l\r> Accel is: X:       Y:       Z:       ");

		//Is_Sensor_Read_Complete();
		Sensor_Read_Value(ACCEL_SENSOR);
		
		auto_pulsing_state = SEND_RESQUEST_SENSOR;
		get_sensor_timeout = AUTO_PULSE_TIMEOUT;
		frame_count = 0;
		break;
	}

	case SEND_RESQUEST_SENSOR:
	{
		i2c_result_t return_value = Is_Sensor_Read_Complete(P_ACCEL_REQUEST);
		
		if (return_value == I2C_IS_RUNNING)
		{
			break;
		}
		else if (return_value != I2C_OK)
		{
			get_sensor_timeout = AUTO_PULSE_TIMEOUT;
			auto_pulsing_state = SENSOR_ERROR;
			break;
		}
		else if (return_value == I2C_OK)
		{
			// Lấy dữ liệu và in liên tục ra màn hình
			Auto_Accel_Data = ACCEL_DATA;

			frame_count++;

			if(frame_count == 251)
			{
				frame_count = 1;
			}	
				
			UART_Printf(CMD_line_handle, "\033[13G%5d\033[22G%5d\033[31G%5d  %d  ", ACCEL_DATA.x, ACCEL_DATA.y, ACCEL_DATA.z, frame_count);

			if (Auto_Accel_Data.z >= Threshold_Accel.z)
			{
				//start pulsing
				auto_pulsing_state = SYSTERM_PULSING_RUN;
			}
			else
			{
				Sensor_Read_Value(ACCEL_SENSOR);

				auto_pulsing_state = SEND_RESQUEST_SENSOR;
				get_sensor_timeout = AUTO_PULSE_TIMEOUT;
			}

			break;
		}

		if (get_sensor_timeout > 0)
		{
			get_sensor_timeout--;
			break;
		}

		auto_pulsing_state = SENSOR_ERROR;

		break;
	}

	case SYSTERM_PULSING_RUN:
	{
		if (is_h_bridge_enable == true)
		{
			return;
		}

		is_h_bridge_enable = true;

		ps_FSP_TX->CMD = FSP_CMD_SET_PULSE_CONTROL;
		ps_FSP_TX->Payload.set_pulse_control.State = true;
		fsp_print(2);

		auto_pulsing_state = SYSTERM_PULSING_DONE;
		break;
	}
		
	case SYSTERM_PULSING_DONE:
	{
		if (is_h_bridge_enable == true)
		{
			return;
		}

		Sensor_Read_Value(ACCEL_SENSOR);

		auto_pulsing_state = SEND_RESQUEST_SENSOR;

		get_sensor_timeout = AUTO_PULSE_TIMEOUT;
		break;
	}

	case SENSOR_ERROR:
	{
		is_accel_pulsing_disable = true;

		// move up 1 line, turn off console
		UART_Send_String(CMD_line_handle, "\e[?25h");

		SchedulerTaskDisable(ACCEL_PULSING_TASK);
		auto_pulsing_state = DISABLE_AUTO_PULSING;

		break;
	}

	case DISABLE_AUTO_PULSING:
	default:
		is_accel_pulsing_disable = true;
	}
}

void Enable_Auto_Pulsing()
{
	if (is_accel_pulsing_disable == true)
	{
		auto_pulsing_state = PREPARE_SEND_TO_SENSOR;
		is_accel_pulsing_disable = false;
		SchedulerTaskEnable(ACCEL_PULSING_TASK, 0);
		return;
	}
}

void Disable_Auto_Pulsing()
{
	is_accel_pulsing_disable = true;

	// move up 1 line, turn off console
	UART_Send_String(CMD_line_handle, "\e[?25h");
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
static void fsp_print(uint8_t packet_length)
{
	s_FSP_TX_Packet.sod 	= FSP_PKT_SOD;
	s_FSP_TX_Packet.src_adr = fsp_my_adr;
	s_FSP_TX_Packet.dst_adr = FSP_ADR_GPC;
	s_FSP_TX_Packet.length 	= packet_length;
	s_FSP_TX_Packet.type 	= FSP_PKT_TYPE_CMD_W_DATA;
	s_FSP_TX_Packet.eof 	= FSP_PKT_EOF;
	s_FSP_TX_Packet.crc16 	= crc16_CCITT(FSP_CRC16_INITIAL_VALUE, &s_FSP_TX_Packet.src_adr, s_FSP_TX_Packet.length + 4);

	uint8_t encoded_frame[100] = { 0 };
	uint8_t frame_len;
	fsp_encode(&s_FSP_TX_Packet, encoded_frame, &frame_len);

	UART_FSP(&GPP_UART, (char*) encoded_frame, frame_len);
}
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ End of the program ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
