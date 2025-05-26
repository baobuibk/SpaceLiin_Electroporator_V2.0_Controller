/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Include~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include <string.h>
#include <stdlib.h>

#include "app.h"

#include "fsp_frame.h"
#include "crc.h"
//#include "pwm.h"

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Defines ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Enum ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Struct ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Class ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Private Types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
// extern Accel_Gyro_DataTypedef _gyro, _accel;
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
static void fsp_print(uint8_t packet_length);
static void convertArrayToInteger(uint8_t array[], void* p_output, uint8_t length, uint8_t output_size);

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
extern uint8_t is_cap_release_after_measure;
extern bool    is_streaming_enable;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
extern uart_stdio_typedef *CMD_line_handle;

uint16_t Avr_Current;
uint16_t Impedance;
uint32_t Voltage;
uint16_t delay_ms = 0;
uint16_t FSP_lv_on_time_ms, FSP_lv_off_time_ms;

void FSP_Line_Process() {
	int16_t x,y,z;
	switch (ps_FSP_RX->CMD) {

	case FSP_CMD_SET_PULSE_CONTROL: {
		is_h_bridge_enable = ps_FSP_RX->Payload.set_pulse_control.State;
		break;
	}

	case FSP_CMD_SET_MANUAL_PULSE:
	{
		is_manual_mode_enable = ps_FSP_RX->Payload.set_manual_pulse.State;

		UART_Send_String(CMD_line_handle, "MANUAL PULSE MODE TURNED OFF\n");
		UART_Send_String(CMD_line_handle, "> ");

		break;
	}

	case FSP_CMD_MEASURE_VOLT: {
		ps_FSP_TX->CMD = FSP_CMD_MEASURE_VOLT;

		Voltage = g_Feedback_Voltage[0] * 1000 / HV_calib_coefficient.average_value;

		ps_FSP_TX->Payload.measure_volt.HV_volt[0] =  Voltage;
		ps_FSP_TX->Payload.measure_volt.HV_volt[1] = (Voltage >> 8);
		ps_FSP_TX->Payload.measure_volt.HV_volt[2] = (Voltage >> 16);
		ps_FSP_TX->Payload.measure_volt.HV_volt[3] = (Voltage >> 24);

		fsp_print(5);
		break;
	}

	case FSP_CMD_MEASURE_CURRENT: {
		Avr_Current = ps_FSP_RX->Payload.measure_current.Value_high;
		Avr_Current = Avr_Current << 8;
		Avr_Current |= ps_FSP_RX->Payload.measure_current.Value_low;

		if (Avr_Current < 1000) {
			UART_Printf(CMD_line_handle, "CURRENT IS %dmA\n", Avr_Current);
		} else {
			UART_Printf(CMD_line_handle, "CURRENT IS %d.%dA\n",
					Avr_Current / 1000, Avr_Current % 1000);
		}

		UART_Send_String(CMD_line_handle, "> ");
		break;
	}

	case FSP_CMD_MEASURE_IMPEDANCE:
	{	
		Impedance = ps_FSP_RX->Payload.measure_impedance.Value_high;
		Impedance = Impedance << 8;
		Impedance |= ps_FSP_RX->Payload.measure_impedance.Value_low;

		is_measure_impedance_completed = true;
		break;
	}

		/* :::::::::: I2C Sensor Command :::::::: */
	case FSP_CMD_GET_SENSOR_GYRO: {
		UART_Send_String(CMD_line_handle, "Received FSP_CMD_GET_SENSOR_GYRO\n");

		uint16_t gyro_x = 0, gyro_y = 0, gyro_z = 0;

		convertArrayToInteger(ps_FSP_RX->Payload.get_sensor_gyro.gyro_x,
				&gyro_x, 2, sizeof(uint16_t));
		convertArrayToInteger(ps_FSP_RX->Payload.get_sensor_gyro.gyro_y,
				&gyro_y, 2, sizeof(uint16_t));
		convertArrayToInteger(ps_FSP_RX->Payload.get_sensor_gyro.gyro_z,
				&gyro_z, 2, sizeof(uint16_t));

		UART_Printf(CMD_line_handle,
				"> GYRO x: %dmpds; GYRO y: %dmpds; GYRO z: %dmpds\n", gyro_x,
				gyro_y, gyro_z);

		UART_Send_String(CMD_line_handle, "> ");
		break;
	}

	case FSP_CMD_GET_SENSOR_ACCEL: {
		UART_Send_String(CMD_line_handle,
				"Received FSP_CMD_GET_SENSOR_ACCEL\n");

		uint16_t accel_x = 0, accel_y = 0, accel_z = 0;

		convertArrayToInteger(ps_FSP_RX->Payload.get_sensor_accel.accel_x,
				&accel_x, 2, sizeof(uint16_t));
		convertArrayToInteger(ps_FSP_RX->Payload.get_sensor_accel.accel_y,
				&accel_y, 2, sizeof(uint16_t));
		convertArrayToInteger(ps_FSP_RX->Payload.get_sensor_accel.accel_z,
				&accel_z, 2, sizeof(uint16_t));

		UART_Printf(CMD_line_handle,
				"> ACCEL x: %dmg; ACCEL y: %dmg; ACCEL z: %dmg\n",
				accel_x, accel_y, accel_z);

		UART_Send_String(CMD_line_handle, "> ");
		break;
	}

	case FSP_CMD_GET_SENSOR_LSM6DSOX: {
		UART_Send_String(CMD_line_handle, "Received FSP_CMD_GET_LSM6DSOX\n");

		uint16_t gyro_x = 0, gyro_y = 0, gyro_z = 0, accel_x = 0, accel_y = 0,
				accel_z = 0;

		convertArrayToInteger(ps_FSP_RX->Payload.get_sensor_LSM6DSOX.gyro_x,
				&gyro_x, 2, sizeof(uint16_t));
		convertArrayToInteger(ps_FSP_RX->Payload.get_sensor_LSM6DSOX.gyro_y,
				&gyro_y, 2, sizeof(uint16_t));
		convertArrayToInteger(ps_FSP_RX->Payload.get_sensor_LSM6DSOX.gyro_z,
				&gyro_z, 2, sizeof(uint16_t));

		convertArrayToInteger(ps_FSP_RX->Payload.get_sensor_LSM6DSOX.accel_x,
				&accel_x, 2, sizeof(uint16_t));
		convertArrayToInteger(ps_FSP_RX->Payload.get_sensor_LSM6DSOX.accel_y,
				&accel_y, 2, sizeof(uint16_t));
		convertArrayToInteger(ps_FSP_RX->Payload.get_sensor_LSM6DSOX.accel_z,
				&accel_z, 2, sizeof(uint16_t));

		UART_Printf(CMD_line_handle,
				"> GYRO x: %dmpds; GYRO y: %dmpds; GYRO z: %dmpds\n", gyro_x,
				gyro_y, gyro_z);

		UART_Printf(CMD_line_handle,
				"> ACCEL x: %dmg; ACCEL y: %dmg; ACCEL z: %dmg\n",
				accel_x, accel_y, accel_z);

		UART_Send_String(CMD_line_handle, "> ");
		break;
	}

	case FSP_CMD_GET_SENSOR_TEMP: {
		UART_Send_String(CMD_line_handle, "Received FSP_CMD_GET_SENSOR_TEMP\n");

		UART_Printf(CMD_line_handle, "> TEMPERATURE: %s Celsius\n",
				ps_FSP_RX->Payload.get_sensor_temp.temp);

		UART_Send_String(CMD_line_handle, "> ");
		break;
	}

	case FSP_CMD_GET_SENSOR_PRESSURE: {
		UART_Send_String(CMD_line_handle,
				"Received FSP_CMD_GET_SENSOR_PRESSURE\n");

		UART_Printf(CMD_line_handle, "> PRESSURE: %s Pa\n",
				ps_FSP_RX->Payload.get_sensor_pressure.pressure);

		UART_Send_String(CMD_line_handle, "> ");
		break;
	}

	case FSP_CMD_GET_SENSOR_ALTITUDE: {
		UART_Send_String(CMD_line_handle,
				"Received FSP_CMD_GET_SENSOR_ALTITUDE\n");

		UART_Printf(CMD_line_handle, "> APPROX ALTIUDE: %s m\n",
				ps_FSP_RX->Payload.get_sensor_altitude.altitude);

		UART_Send_String(CMD_line_handle, "> ");
		break;
	}

	case FSP_CMD_GET_SENSOR_BMP390: {
		UART_Send_String(CMD_line_handle, "Received FSP_CMD_GET_BMP390\n");

		UART_Printf(CMD_line_handle, "> TEMPERATURE: %s Celsius\n",
				ps_FSP_RX->Payload.get_sensor_BMP390.temp);

		UART_Printf(CMD_line_handle, "> PRESSURE: %s Pa\n",
				ps_FSP_RX->Payload.get_sensor_BMP390.pressure);

		UART_Printf(CMD_line_handle, "> APPROX ALTIUDE: %s m\n",
				ps_FSP_RX->Payload.get_sensor_BMP390.altitude);

		UART_Send_String(CMD_line_handle, "> ");
		break;
	}

	case FSP_CMD_GET_SENSOR_H3LIS331DL:
	{
		UART_Send_String(CMD_line_handle,
				"Received FSP_CMD_GET_SENSOR_H3LIS331DL\n");

		uint32_t H3LIS_x = 0, H3LIS_y = 0, H3LIS_z = 0;

		convertArrayToInteger(ps_FSP_RX->Payload.get_sensor_H3LIS331DL.accel_x,
				&H3LIS_x, 3, sizeof(uint32_t));
		convertArrayToInteger(ps_FSP_RX->Payload.get_sensor_H3LIS331DL.accel_y,
				&H3LIS_y, 3, sizeof(uint32_t));
		convertArrayToInteger(ps_FSP_RX->Payload.get_sensor_H3LIS331DL.accel_z,
				&H3LIS_z, 3, sizeof(uint32_t));

		UART_Printf(CMD_line_handle,
				"> H3LIS331DL ACCEL x: %dmg; H3LIS331DL ACCEL y: %dmg; H3LIS331DL ACCEL z: %dmg\n",
				H3LIS_x, H3LIS_y, H3LIS_z);

		UART_Send_String(CMD_line_handle, "> ");
		break;
	}
	
	/* :::::::::: Auto Accel Command :::::::: */
	case FSP_CMD_GET_THRESHOLD_ACCEL:
		// Đọc giá trị kiểu int16_t từ byte thấp và byte cao
		x = (int16_t)((ps_FSP_RX->Payload.get_threshold_accel.XH << 8) |
		                      ps_FSP_RX->Payload.get_threshold_accel.XL);
		y = (int16_t)((ps_FSP_RX->Payload.get_threshold_accel.YH << 8) |
		                      ps_FSP_RX->Payload.get_threshold_accel.YL);
		z = (int16_t)((ps_FSP_RX->Payload.get_threshold_accel.ZH << 8) |
		                      ps_FSP_RX->Payload.get_threshold_accel.ZL);

		// In giá trị ra
		UART_Printf(CMD_line_handle, "Threshold Accel is: %d %d %d\n", x, y, z);

		break;
	case FSP_CMD_STREAM_ACCEL:
		x = (int16_t)((ps_FSP_RX->Payload.stream_accel.XH << 8) |
		                      ps_FSP_RX->Payload.stream_accel.XL);
		y = (int16_t)((ps_FSP_RX->Payload.stream_accel.YH << 8) |
		                      ps_FSP_RX->Payload.stream_accel.YL);
		z = (int16_t)((ps_FSP_RX->Payload.stream_accel.ZH << 8) |
		                      ps_FSP_RX->Payload.stream_accel.ZL);

		uint8_t count = ps_FSP_RX->Payload.stream_accel.count;
		if (count == 0)
		{
		    UART_Send_String(CMD_line_handle, "\e[?25l\r> Accel is: X:       Y:       Z:       ");
		}

		if (is_streaming_enable == false)
		{
			break;
		}
		
		UART_Printf(CMD_line_handle, "\033[13G%5d\033[22G%5d\033[31G%5d  %d  ", x, y, z, count);
		break;
		
	default:
		break;
	}
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
static void fsp_print(uint8_t packet_length)
{
	s_FSP_TX_Packet.sod 		= FSP_PKT_SOD;
	s_FSP_TX_Packet.src_adr 	= fsp_my_adr;
	s_FSP_TX_Packet.dst_adr 	= FSP_ADR_GPP;
	s_FSP_TX_Packet.length 		= packet_length;
	s_FSP_TX_Packet.type 		= FSP_PKT_TYPE_CMD_W_DATA;
	s_FSP_TX_Packet.eof 		= FSP_PKT_EOF;
	s_FSP_TX_Packet.crc16 		= crc16_CCITT(FSP_CRC16_INITIAL_VALUE, &s_FSP_TX_Packet.src_adr, s_FSP_TX_Packet.length + 4);

	uint8_t encoded_frame[25] = { 0 };
	uint8_t frame_len;
	fsp_encode(&s_FSP_TX_Packet, encoded_frame, &frame_len);

	UART_FSP(&GPP_UART, (char*)encoded_frame, frame_len);
}

static void convertArrayToInteger(uint8_t array[], void* p_output, uint8_t length, uint8_t output_size)
{
	uint32_t temp_output = 0;

	for (int8_t i = (length - 1); i >= 0; i--)
	{
		temp_output <<= 8;
		temp_output |=  array[i];
	}

	switch (output_size)
	{
	case sizeof(uint16_t):
	{
		*(uint16_t *)p_output = (uint16_t)temp_output;
		break;
	}

	case sizeof(uint32_t):
	{
		*(uint32_t *)p_output = temp_output;
		break;
	}
		
	default:
		break;
	}
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ End of the program ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
