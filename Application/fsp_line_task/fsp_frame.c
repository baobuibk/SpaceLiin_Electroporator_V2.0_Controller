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
	switch (ps_FSP_RX->CMD)
	{

	case FSP_CMD_SET_PULSE_CONTROL:
	{
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

	case FSP_CMD_MEASURE_VOLT:
	{
		ps_FSP_TX->CMD = FSP_CMD_MEASURE_VOLT;

		Voltage = g_Feedback_Voltage[0] * 1000 / HV_calib_coefficient.average_value;

		ps_FSP_TX->Payload.measure_volt.HV_volt[0] =  Voltage;
		ps_FSP_TX->Payload.measure_volt.HV_volt[1] = (Voltage >> 8);
		ps_FSP_TX->Payload.measure_volt.HV_volt[2] = (Voltage >> 16);
		ps_FSP_TX->Payload.measure_volt.HV_volt[3] = (Voltage >> 24);

		fsp_print(5);
		break;
	}

	case FSP_CMD_MEASURE_CURRENT:
	{
		Avr_Current = ps_FSP_RX->Payload.measure_current.Value_high;
		Avr_Current = Avr_Current << 8;
		Avr_Current |= ps_FSP_RX->Payload.measure_current.Value_low;

		if (Avr_Current < 1000)
		{
			UART_Printf(CMD_line_handle, "CURRENT IS %dmA\n", Avr_Current);
		}
		else
		{
			UART_Printf(CMD_line_handle, "CURRENT IS %d.%dA\n", Avr_Current / 1000, Avr_Current % 1000);
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

	case FSP_CMD_OVER_CURRENT_DETECT:
	{
		if (ps_FSP_RX->Payload.ovc_current_detect.OVC_signal == false)
		{
			break;
		}
		
		// if (true)
		Cap_Set_Discharge_All(true, true, true, true);

		UART_Send_String(&RF_UART, "OVER CURRENT DETECTED\n> ");
		UART_Send_String(&RS232_UART, "OVER CURRENT DETECTED\n> ");

		break;
	}
		
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
