/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Include~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include <stdio.h>
#include "command.h"

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Defines ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Enum ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Struct ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Class ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Private Types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
static uint8_t CMD_process_state = 0;

uint8_t ChannelMapping[8] = {0, 1, 2, 3, 4, 5, 6, 7};
uint8_t User_Channel_Mapping[8] = {1, 2, 3, 4, 5, 6, 7, 8};

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
static void fsp_print(uint8_t packet_length);
static void double_to_string(double value, char *buffer, uint8_t precision);

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
tCmdLineEntry g_psCmdTable[] =
{
		/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Cap Control Command ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
		{ "SET_CAP_VOLT",			CMD_SET_CAP_VOLT, 			" : Set cap voltage" },
		{ "SET_CAP_CONTROL",		CMD_SET_CAP_CONTROL, 		" : Control charger on/off" },
		{ "SET_CAP_RELEASE",		CMD_SET_CAP_RELEASE, 		" : Control releasing cap" },

		{ "GET_CAP_VOLT",			CMD_GET_CAP_VOLT, 			" : Get set voltage on cap" },
		{ "GET_CAP_CONTROL",		CMD_GET_CAP_CONTROL, 		" : Get info whether cap is charging or not" },
		{ "GET_CAP_RELEASE",		CMD_GET_CAP_RELEASE, 		" : Get info whether cap is releasing or not" },
		{ "GET_CAP_ALL",			CMD_GET_CAP_ALL, 			" : Get all info about cap" },

		/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Pulse Control Command ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
		{ "SET_SEQUENCE_INDEX",		CMD_SET_SEQUENCE_INDEX, 	" : Set current HB sequence index" },
		{ "SET_SEQUENCE_DELETE",	CMD_SET_SEQUENCE_DELETE, 	" : Delete a HB sequence index" },
		{ "SET_SEQUENCE_CONFIRM",	CMD_SET_SEQUENCE_CONFIRM, 	" : Confirm the setting of the current sequence index" },
		{ "SET_SEQUENCE_DELAY",		CMD_SET_SEQUENCE_DELAY, 	" : Set current HB index delay" },

		{ "SET_PULSE_POLE",			CMD_SET_PULSE_POLE,			" : Set pos and neg pole of a pulse" },
		{ "SET_PULSE_COUNT",		CMD_SET_PULSE_COUNT, 		" : Set number of pulse" },
		{ "SET_PULSE_DELAY",		CMD_SET_PULSE_DELAY, 		" : Set delay between pulse hv and lv" },
		{ "SET_PULSE_HV_POS", 		CMD_SET_PULSE_HV_POS, 		" : Set hs pulse on time and off time" },
		{ "SET_PULSE_HV_NEG", 		CMD_SET_PULSE_HV_NEG, 		" : Set hs pulse on time and off time" },
		{ "SET_PULSE_LV_POS", 		CMD_SET_PULSE_LV_POS, 		" : Set ls pulse on time and off time" },
		{ "SET_PULSE_LV_NEG", 		CMD_SET_PULSE_LV_NEG, 		" : Set ls pulse on time and off time" },
		{ "SET_PULSE_CONTROL", 		CMD_SET_PULSE_CONTROL, 		" : Start pulsing" },

		{ "GET_SEQUENCE_INDEX",		CMD_GET_SEQUENCE_INDEX, 	" : Get current sequence index" },
		{ "GET_SEQUENCE_DELAY",		CMD_GET_SEQUENCE_DELAY, 	" : Get current sequence delay" },
		{ "GET_SEQUENCE_ALL",		CMD_GET_SEQUENCE_ALL, 		" : Get all info about sequence" },

		{ "GET_PULSE_POLE",			CMD_GET_PULSE_POLE,			" : Get the pos and neg pole of a pulse" },
		{ "GET_PULSE_COUNT",		CMD_GET_PULSE_COUNT, 		" : Get number of pulse" },
		{ "GET_PULSE_DELAY",		CMD_GET_PULSE_DELAY, 		" : Get delay between pulse hv and lv" },
		{ "GET_PULSE_HV_POS", 		CMD_GET_PULSE_HV_POS, 		" : Get hs pulse on time and off time" },
		{ "GET_PULSE_HV_NEG", 		CMD_GET_PULSE_HV_NEG, 		" : Get hs pulse on time and off time" },
		{ "GET_PULSE_HV", 			CMD_GET_PULSE_HV, 			" : Get hs pulse on time and off time" },
		{ "GET_PULSE_LV_POS", 		CMD_GET_PULSE_LV_POS, 		" : Get ls pulse on time and off time" },
		{ "GET_PULSE_LV_NEG", 		CMD_GET_PULSE_LV_NEG, 		" : Get ls pulse on time and off time" },
		{ "GET_PULSE_LV", 			CMD_GET_PULSE_LV, 			" : Get ls pulse on time and off time" },
		{ "GET_PULSE_CONTROL", 		CMD_GET_PULSE_CONTROL, 		" : Get info whether pulse starting pulsing" },
		{ "GET_PULSE_ALL", 			CMD_GET_PULSE_ALL, 			" : Get all info about pulse" },

		/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Auto Pulsing Command ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
		{ "SET_THRESHOLD_ACCEL",	CMD_SET_THRESHOLD_ACCEL,	" : Set high threshold accel for auto pulsing" },
		{ "GET_THRESHOLD_ACCEL",	CMD_GET_THRESHOLD_ACCEL, 	" : Get high threshold accel for auto pulsing" },
		{ "SET_AUTO_ACCEL",			CMD_SET_AUTO_ACCEL,			" : Enable auto pulsing" },

		/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Manual Pulse Command ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
		{ "SET_MANUAL_POLE", 		CMD_SET_MANUAL_POLE, 		" : Choose which cap to turn on" },
		{ "SET_MANUAL_CAP", 		CMD_SET_MANUAL_CAP, 		" : Choose which cap to turn on" },
		{ "SET_MANUAL_PULSE", 		CMD_SET_MANUAL_PULSE, 		" : Turn on, off pulse manually" },

		/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ VOM Command ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
		{ "MEASURE_VOLT", 			CMD_MEASURE_VOLT,			" : Measure cap voltage"},
		{ "MEASURE_CURRENT",		CMD_MEASURE_CURRENT,		" : Measure cuvette current"},
		{ "MEASURE_IMPEDANCE", 		CMD_MEASURE_IMPEDANCE,		" : Measure cuvette impedance"},

		/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ I2C Sensor Command ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
		{ "GET_SENSOR_GYRO", 		CMD_GET_SENSOR_GYRO, 		" : Get gyro" },
		{ "GET_SENSOR_ACCEL", 		CMD_GET_SENSOR_ACCEL, 		" : Get accel" },
		{ "GET_SENSOR_LSM6DSOX", 	CMD_GET_SENSOR_LSM6DSOX, 	" : Get accel and gyro" },

		{ "GET_SENSOR_TEMP", 		CMD_GET_SENSOR_TEMP, 		" : Get temp" },
		{ "GET_SENSOR_PRESSURE", 	CMD_GET_SENSOR_PRESSURE, 	" : Get pressure" },
		{ "GET_SENSOR_ALTITUDE", 	CMD_GET_SENSOR_ALTITUDE, 	" : Get altitude" },
		{ "GET_SENSOR_BMP390", 		CMD_GET_SENSOR_BMP390, 		" : Get temp, pressure and altitude" },

		{ "GET_SENSOR_H3LIS", 		CMD_GET_SENSOR_H3LIS, 		" : Get accel from sensor H3LIS331DL" },

		/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Ultility Command ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
		{ "HELP", 					CMD_HELP,					" : Display list of commands" },
		{ "CALIB_RUN", 				CMD_CALIB_RUN, 				" : Start cap calib process" },
		{ "CALIB_MEASURE",			CMD_CALIB_MEASURE, 			" : Command to input VOM value" },
		{ "CALL_GPP", 				CMD_CALL_GPP,	    		" : Test communicate to GPP" },
		{ "CLC",           			CMD_CLEAR_SCREEN,              " " },

		{ 0, 0, 0 }


};

bool is_manual_mode_enable = false;
bool is_h_bridge_enable = false;

H_Bridge_task_typedef HB_sequence_default =
{
	.is_setted = 0,

	.sequence_delay_ms = 1,

	.pos_pole_index = 0,
	.neg_pole_index = 5,

	.pulse_delay_ms = 15,

	.hv_pos_count = 5,
	.hv_neg_count = 6,

	.hv_delay_ms = 5,

	.hv_pos_on_ms = 5,
	.hv_pos_off_ms = 15,
    .hv_neg_on_ms = 5,
	.hv_neg_off_ms = 15,

	.lv_pos_count = 7,
	.lv_neg_count = 8,

	.lv_delay_ms = 10,
    
	.lv_pos_on_ms = 50,
	.lv_pos_off_ms = 90,
    .lv_neg_on_ms = 50,
	.lv_neg_off_ms = 90,
};

H_Bridge_task_typedef HB_sequence_array[10];

uint8_t CMD_sequence_index = 0;
uint8_t CMD_total_sequence_index = 0;

bool is_measure_impedance_enable = false;
bool is_cap_release_after_measure = false;

bool is_streaming_enable = false;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* :::::::::: Cap Control Command :::::::: */
int CMD_SET_CAP_VOLT(int argc, char *argv[])
{
	if (g_is_calib_running == true)
	{
		return CMDLINE_CALIB_IS_RUNNING;
	}

	if (argc < 3)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 3)
		return CMDLINE_TOO_MANY_ARGS;

	int receive_argm[2];

	receive_argm[0] = atoi(argv[1]);
	receive_argm[1] = atoi(argv[2]);

	if ((receive_argm[0] > 300) || (receive_argm[0] < 0))
		return CMDLINE_INVALID_ARG;
	else if ((receive_argm[1] > 50) || (receive_argm[1] < 0))
		return CMDLINE_INVALID_ARG;

	Cap_Set_Volt_All(receive_argm[0], receive_argm[1], true, true);

	return CMDLINE_OK;
}

int CMD_SET_CAP_CONTROL(int argc, char *argv[])
{
	if (g_is_calib_running == true)
	{
		return CMDLINE_CALIB_IS_RUNNING;
	}

	if (argc < 3)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 3)
		return CMDLINE_TOO_MANY_ARGS;

	int receive_argm[2];

	receive_argm[0] = atoi(argv[1]);
	receive_argm[1] = atoi(argv[2]);

	if ((receive_argm[0] > 1) || (receive_argm[0] < 0))
		return CMDLINE_INVALID_ARG;
	else if ((receive_argm[1] > 1) || (receive_argm[1] < 0))
		return CMDLINE_INVALID_ARG;

	Cap_Set_Charge_All(receive_argm[0], receive_argm[1], true, true);

	return CMDLINE_OK;
}

int CMD_SET_CAP_RELEASE(int argc, char *argv[])
{
	if (g_is_calib_running == true) {
		return CMDLINE_CALIB_IS_RUNNING;
	}

	if (argc < 3)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 3)
		return CMDLINE_TOO_MANY_ARGS;

	int receive_argm[2];

	receive_argm[0] = atoi(argv[1]);
	receive_argm[1] = atoi(argv[2]);

	if ((receive_argm[0] > 1) || (receive_argm[0] < 0))
		return CMDLINE_INVALID_ARG;
	else if ((receive_argm[1] > 1) || (receive_argm[1] < 0))
		return CMDLINE_INVALID_ARG;

	Cap_Set_Discharge_All(receive_argm[0], receive_argm[1], true, true);

	return CMDLINE_OK;
}

int CMD_GET_CAP_VOLT(int argc, char *argv[])
{
	if (argc < 1)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 1)
		return CMDLINE_TOO_MANY_ARGS;

	uint16_t hv_cap_set_voltage, lv_cap_set_voltage;
	hv_cap_set_voltage = Cap_Get_Set_Volt(&g_Cap_300V);
	lv_cap_set_voltage = Cap_Get_Set_Volt(&g_Cap_50V);

	UART_Printf(CMD_line_handle, "> HV CAP IS SET AT: %dV, LV CAP IS SET AT: %dV\n", hv_cap_set_voltage, lv_cap_set_voltage);

	return CMDLINE_OK;
}

int CMD_GET_CAP_CONTROL(int argc, char *argv[])
{
	if (argc < 1)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 1)
		return CMDLINE_TOO_MANY_ARGS;

	if (Cap_Get_is_Charge(&g_Cap_300V) == true)
	{
		UART_Send_String(CMD_line_handle, "> HV CAP IS CHARGING\n");
	}
	else
	{
		UART_Send_String(CMD_line_handle, "> HV CAP IS NOT CHARGING\n");
	}

	if (Cap_Get_is_Charge(&g_Cap_50V) == true)
	{
		UART_Send_String(CMD_line_handle, "> LV CAP IS CHARGING\n");
	}
	else
	{
		UART_Send_String(CMD_line_handle, "> LV CAP IS NOT CHARGING\n");
	}
	
	return CMDLINE_OK;
}

int CMD_GET_CAP_RELEASE(int argc, char *argv[])
{
	if (argc < 1)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 1)
		return CMDLINE_TOO_MANY_ARGS;

	if (Cap_Get_is_Discharge(&g_Cap_300V) == true)
	{
		UART_Send_String(CMD_line_handle, "> HV CAP IS DISCHARGING\n");
	}
	else
	{
		UART_Send_String(CMD_line_handle, "> HV CAP IS NOT DISCHARGING\n");
	}

	if (Cap_Get_is_Discharge(&g_Cap_50V) == true)
	{
		UART_Send_String(CMD_line_handle, "> LV CAP IS DISCHARGING\n");
	}
	else
	{
		UART_Send_String(CMD_line_handle, "> LV CAP IS NOT DISCHARGING\n");
	}

	return CMDLINE_OK;
}

int CMD_GET_CAP_ALL(int argc, char *argv[])
{
	if (argc < 1)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 1)
		return CMDLINE_TOO_MANY_ARGS;
	
	uint16_t hv_cap_set_voltage, lv_cap_set_voltage;
	hv_cap_set_voltage = Cap_Get_Set_Volt(&g_Cap_300V);
	lv_cap_set_voltage = Cap_Get_Set_Volt(&g_Cap_300V);

	UART_Printf(CMD_line_handle, "> HV CAP IS SET AT: %dV, LV CAP IS SET AT: %dV\n", hv_cap_set_voltage, lv_cap_set_voltage);

	if (Cap_Get_is_Charge(&g_Cap_300V) == true)
	{
		UART_Send_String(CMD_line_handle, "> HV CAP IS CHARGING\n");
	}
	else
	{
		UART_Send_String(CMD_line_handle, "> HV CAP IS NOT CHARGING\n");
	}

	if (Cap_Get_is_Charge(&g_Cap_50V) == true)
	{
		UART_Send_String(CMD_line_handle, "> LV CAP IS CHARGING\n");
	}
	else
	{
		UART_Send_String(CMD_line_handle, "> LV CAP IS NOT CHARGING\n");
	}

	if (Cap_Get_is_Discharge(&g_Cap_300V) == true)
	{
		UART_Send_String(CMD_line_handle, "> HV CAP IS DISCHARGING\n");
	}
	else
	{
		UART_Send_String(CMD_line_handle, "> HV CAP IS NOT DISCHARGING\n");
	}

	if (Cap_Get_is_Discharge(&g_Cap_50V) == true)
	{
		UART_Send_String(CMD_line_handle, "> LV CAP IS DISCHARGING\n");
	}
	else
	{
		UART_Send_String(CMD_line_handle, "> LV CAP IS NOT DISCHARGING\n");
	}

	return CMDLINE_OK;
}

/* :::::::::: Pulse Control Command :::::::: */
int CMD_SET_SEQUENCE_INDEX(int argc, char *argv[])
{
	if (argc < 2)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 2)
		return CMDLINE_TOO_MANY_ARGS;

	int receive_argm;

	receive_argm = atoi(argv[1]);

	if (receive_argm > 10)
		return CMDLINE_INVALID_ARG;
	if (receive_argm < 1)
		return CMDLINE_INVALID_ARG;

	if ((receive_argm - 1) > CMD_total_sequence_index + 1)
	{
		UART_Printf(CMD_line_handle, "> ERROR YOUR NEXT SEQUENCE INDEX IS: %d, NOT %d\n", CMD_total_sequence_index + 2, receive_argm);

		UART_Printf(CMD_line_handle, "> CURRENT SEQUENCE INDEX: %d\n", CMD_sequence_index + 1);
		return CMDLINE_OK;
	}

	if (((HB_sequence_array[CMD_sequence_index].is_setted & (1 << 7)) == 0) && ((receive_argm - 1) != CMD_sequence_index))
	{
		UART_Printf(CMD_line_handle, "> ERROR CURRENT SEQUENCE INDEX: %d IS NOT CONFIRMED\n", CMD_sequence_index + 1);

		UART_Send_String(CMD_line_handle, "> EITHER CONFIRM IT OR DELETE IT\n");

		UART_Send_String(CMD_line_handle, "> \n");

		UART_Printf(CMD_line_handle, "> DELAY BETWEEN SEQUENCE: %dms\n", HB_sequence_array[CMD_sequence_index].sequence_delay_ms);

		UART_Send_String(CMD_line_handle, "> \n");

		UART_Printf(CMD_line_handle, "> POS HV PULSE COUNT: %d; NEG HV PULSE COUNT: %d\n",
		HB_sequence_array[CMD_sequence_index].hv_pos_count, HB_sequence_array[CMD_sequence_index].hv_neg_count);
		UART_Printf(CMD_line_handle, "> POS LV PULSE COUNT: %d; NEG LV PULSE COUNT: %d\n",
		HB_sequence_array[CMD_sequence_index].lv_pos_count, HB_sequence_array[CMD_sequence_index].lv_neg_count);

		UART_Send_String(CMD_line_handle, "> \n");

		UART_Printf(CMD_line_handle, "> DELAY BETWEEN HV POS AND NEG PULSE: %dms\n", HB_sequence_array[CMD_sequence_index].hv_delay_ms);
		UART_Printf(CMD_line_handle, "> DELAY BETWEEN LV POS AND NEG PULSE: %dms\n", HB_sequence_array[CMD_sequence_index].lv_delay_ms);
		UART_Printf(CMD_line_handle, "> DELAY BETWEEN HV PULSE AND LV PULSE: %dms\n", HB_sequence_array[CMD_sequence_index].pulse_delay_ms);

		UART_Send_String(CMD_line_handle, "> \n");

		UART_Printf(CMD_line_handle, "> HV PULSE POS ON TIME: %dms; HV PULSE POS OFF TIME: %dms\n",
		HB_sequence_array[CMD_sequence_index].hv_pos_on_ms, HB_sequence_array[CMD_sequence_index].hv_pos_off_ms);

		UART_Printf(CMD_line_handle, "> HV PULSE NEG ON TIME: %dms; HV PULSE NEG OFF TIME: %dms\n",
		HB_sequence_array[CMD_sequence_index].hv_neg_on_ms, HB_sequence_array[CMD_sequence_index].hv_neg_off_ms);

		UART_Printf(CMD_line_handle, "> LV PULSE POS ON TIME: %dms; LV PULSE POS OFF TIME: %dms\n",
		HB_sequence_array[CMD_sequence_index].lv_pos_on_ms, HB_sequence_array[CMD_sequence_index].lv_pos_off_ms);
		
		UART_Printf(CMD_line_handle, "> LV PULSE NEG ON TIME: %dms; LV PULSE NEG OFF TIME: %dms\n",
		HB_sequence_array[CMD_sequence_index].lv_neg_on_ms, HB_sequence_array[CMD_sequence_index].lv_neg_off_ms);

		UART_Send_String(CMD_line_handle, "> \n");
		return CMDLINE_OK;
	}

	UART_Send_String(CMD_line_handle, "> \n");

	UART_Printf(CMD_line_handle, "> CURRENT SEQUENCE INDEX: %d\n", receive_argm);

	UART_Send_String(CMD_line_handle, "> \n");

	CMD_sequence_index = receive_argm - 1;

	if (CMD_total_sequence_index < CMD_sequence_index)
	{
		CMD_total_sequence_index = CMD_sequence_index;
	}

	ps_FSP_TX->CMD	   						    = FSP_CMD_SET_SEQUENCE_INDEX;
	ps_FSP_TX->Payload.set_sequence_index.index = receive_argm;

	fsp_print(2);

	return CMDLINE_OK;
}

int CMD_SET_SEQUENCE_DELETE(int argc, char *argv[])
{
	if (argc < 1)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 1)
		return CMDLINE_TOO_MANY_ARGS;

	if (CMD_sequence_index < CMD_total_sequence_index)
	{
		UART_Send_String(CMD_line_handle, "> ERROR CANNOT DELETE IN-BETWEEN SEQUENCE\n");
		return CMDLINE_OK;
	}

	if (CMD_sequence_index == 0)
	{
		UART_Send_String(CMD_line_handle, "> ERROR CANNOT DELETE ANYMORE\n");

		UART_Printf(CMD_line_handle, "> TOTAL SEQUENCE: %d\n", CMD_total_sequence_index + 1);
		return CMDLINE_OK;
	}

	HB_sequence_array[(CMD_sequence_index)].is_setted &= ~(1 << 7);
	CMD_total_sequence_index -= 1;
	CMD_sequence_index = CMD_total_sequence_index;
	UART_Printf(CMD_line_handle, "> CURRENT SEQUENCE INDEX: %d\n", CMD_sequence_index + 1);

	ps_FSP_TX->CMD = FSP_CMD_SET_SEQUENCE_DELETE;
	fsp_print(1);

	return CMDLINE_OK;
}

int CMD_SET_SEQUENCE_CONFIRM(int argc, char *argv[])
{
	if (argc < 1)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 1)
		return CMDLINE_TOO_MANY_ARGS;

	HB_sequence_array[CMD_sequence_index].is_setted |= (1 << 7);

	ps_FSP_TX->CMD = FSP_CMD_SET_SEQUENCE_CONFIRM;
	fsp_print(1);

	return CMDLINE_OK;
}

int CMD_SET_SEQUENCE_DELAY(int argc, char *argv[])
{
	if (argc < 2)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 2)
		return CMDLINE_TOO_MANY_ARGS;

	int receive_argm;

	receive_argm = atoi(argv[1]);

	if (receive_argm > 100)
		return CMDLINE_INVALID_ARG;
	if (receive_argm < 1)
		return CMDLINE_INVALID_ARG;

	if ((HB_sequence_array[CMD_sequence_index].is_setted & (1 << 0)) == false)
	{
		HB_sequence_array[CMD_sequence_index].is_setted |= (1 << 0);
	}

	HB_sequence_array[CMD_sequence_index].sequence_delay_ms = receive_argm;

	ps_FSP_TX->CMD = FSP_CMD_SET_SEQUENCE_DELAY;
	ps_FSP_TX->Payload.set_sequence_delay.Delay_low  =  receive_argm;
	ps_FSP_TX->Payload.set_sequence_delay.Delay_high = (receive_argm >> 8);
	fsp_print(3);

	return CMDLINE_OK;
}

int CMD_SET_PULSE_POLE(int argc, char *argv[])
{
	if (g_is_calib_running == true) {
		return CMDLINE_CALIB_IS_RUNNING;
	}

	if (argc < 3)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 3)
		return CMDLINE_TOO_MANY_ARGS;

	int receive_argm[3];

	receive_argm[0] = atoi(argv[1]);
	receive_argm[1] = atoi(argv[2]);

	if (receive_argm[0] == receive_argm[1])
			return CMDLINE_INVALID_ARG;
	else if ((receive_argm[0] > 9) || (receive_argm[1] > 9))
		return CMDLINE_INVALID_ARG;
	else if ((receive_argm[0] < 1) || (receive_argm[1] < 1))
		return CMDLINE_INVALID_ARG;

	if ((HB_sequence_array[CMD_sequence_index].is_setted & (1 << 1)) == false)
	{
		HB_sequence_array[CMD_sequence_index].is_setted |= (1 << 1);
	}

	HB_sequence_array[CMD_sequence_index].pos_pole_index = ChannelMapping[receive_argm[0] - 1];
	HB_sequence_array[CMD_sequence_index].neg_pole_index = ChannelMapping[receive_argm[1] - 1];

	ps_FSP_TX->CMD	   						   = FSP_CMD_SET_PULSE_POLE;
	ps_FSP_TX->Payload.set_pulse_pole.pos_pole = ChannelMapping[receive_argm[0] - 1];
	ps_FSP_TX->Payload.set_pulse_pole.neg_pole = ChannelMapping[receive_argm[1] - 1];

	fsp_print(3);
	return CMDLINE_OK;
}

int CMD_SET_PULSE_COUNT(int argc, char *argv[])
{
	if (g_is_calib_running == true) {
		return CMDLINE_CALIB_IS_RUNNING;
	}

	if (argc < 5)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 5)
		return CMDLINE_TOO_MANY_ARGS;

	int receive_argm[4];

	receive_argm[0] = atoi(argv[1]);
	receive_argm[1] = atoi(argv[2]);
	receive_argm[2] = atoi(argv[3]);
	receive_argm[3] = atoi(argv[4]);

	if ((receive_argm[0] > 20) || (receive_argm[1] > 20) || (receive_argm[2] > 20) || (receive_argm[3] > 20))
		return CMDLINE_INVALID_ARG;

	if ((HB_sequence_array[CMD_sequence_index].is_setted & (1 << 2)) == false)
	{
		HB_sequence_array[CMD_sequence_index].is_setted |= (1 << 2);
	}

	HB_sequence_array[CMD_sequence_index].hv_pos_count = receive_argm[0];
	HB_sequence_array[CMD_sequence_index].hv_neg_count = receive_argm[1];

	HB_sequence_array[CMD_sequence_index].lv_pos_count = receive_argm[2];
	HB_sequence_array[CMD_sequence_index].lv_neg_count = receive_argm[3];

	ps_FSP_TX->CMD	   						 		= FSP_CMD_SET_PULSE_COUNT;
	ps_FSP_TX->Payload.set_pulse_count.HV_pos_count = receive_argm[0];
	ps_FSP_TX->Payload.set_pulse_count.HV_neg_count = receive_argm[1];

	ps_FSP_TX->Payload.set_pulse_count.LV_pos_count = receive_argm[2];
	ps_FSP_TX->Payload.set_pulse_count.LV_neg_count = receive_argm[3];

	fsp_print(5);
	return CMDLINE_OK;
}

int CMD_SET_PULSE_DELAY(int argc, char *argv[])
{
	if (g_is_calib_running == true) {
		return CMDLINE_CALIB_IS_RUNNING;
	}

	if (argc < 4)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 4)
		return CMDLINE_TOO_MANY_ARGS;

	int receive_argm[3];

	receive_argm[0] = atoi(argv[1]);
	receive_argm[1] = atoi(argv[2]);
	receive_argm[2] = atoi(argv[3]);

	if ((receive_argm[0] > 100) || (receive_argm[0] < 1))
		return CMDLINE_INVALID_ARG;
	else if ((receive_argm[1] > 100) || (receive_argm[1] < 1))
		return CMDLINE_INVALID_ARG;
	else if ((receive_argm[2] > 1000) || (receive_argm[2] < 1))
		return CMDLINE_INVALID_ARG;

	if ((HB_sequence_array[CMD_sequence_index].is_setted & (1 << 3)) == false)
	{
		HB_sequence_array[CMD_sequence_index].is_setted |= (1 << 3);
	}

	HB_sequence_array[CMD_sequence_index].hv_delay_ms = receive_argm[0];
	HB_sequence_array[CMD_sequence_index].lv_delay_ms	= receive_argm[1];

	HB_sequence_array[CMD_sequence_index].pulse_delay_ms = receive_argm[2];

	ps_FSP_TX->CMD									= FSP_CMD_SET_PULSE_DELAY;
	ps_FSP_TX->Payload.set_pulse_delay.HV_delay		= receive_argm[0];
	ps_FSP_TX->Payload.set_pulse_delay.LV_delay		= receive_argm[1];

	ps_FSP_TX->Payload.set_pulse_delay.Delay_low 	= receive_argm[2];
	ps_FSP_TX->Payload.set_pulse_delay.Delay_high 	= (receive_argm[2] >> 8);

	fsp_print(5);
	return CMDLINE_OK;
}

int CMD_SET_PULSE_HV_POS(int argc, char *argv[])
{
	if (g_is_calib_running == true) {
		return CMDLINE_CALIB_IS_RUNNING;
	}

	if (argc < 3)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 3)
		return CMDLINE_TOO_MANY_ARGS;

	int receive_argm[2];

	receive_argm[0] = atoi(argv[1]);
	receive_argm[1] = atoi(argv[2]);

	if ((receive_argm[0] > 20) || (receive_argm[0] < 1))
		return CMDLINE_INVALID_ARG;
	else if ((receive_argm[1] > 20) || (receive_argm[1] < 1))
		return CMDLINE_INVALID_ARG;

	if ((HB_sequence_array[CMD_sequence_index].is_setted & (1 << 4)) == false)
	{
		HB_sequence_array[CMD_sequence_index].is_setted |= (1 << 4);
	}

	HB_sequence_array[CMD_sequence_index].hv_pos_on_ms   = receive_argm[0];
	HB_sequence_array[CMD_sequence_index].hv_pos_off_ms  = receive_argm[1];

	ps_FSP_TX->CMD 	 			 		 	    = FSP_CMD_SET_PULSE_HV_POS;
	ps_FSP_TX->Payload.set_pulse_HV_pos.OnTime	= receive_argm[0];
	ps_FSP_TX->Payload.set_pulse_HV_pos.OffTime = receive_argm[1];

	fsp_print(3);
	return CMDLINE_OK;
}

int CMD_SET_PULSE_HV_NEG(int argc, char *argv[])
{
	if (g_is_calib_running == true) {
		return CMDLINE_CALIB_IS_RUNNING;
	}

	if (argc < 3)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 3)
		return CMDLINE_TOO_MANY_ARGS;

	int receive_argm[2];

	receive_argm[0] = atoi(argv[1]);
	receive_argm[1] = atoi(argv[2]);

	if ((receive_argm[0] > 20) || (receive_argm[0] < 1))
		return CMDLINE_INVALID_ARG;
	else if ((receive_argm[1] > 20) || (receive_argm[1] < 1))
		return CMDLINE_INVALID_ARG;

	if ((HB_sequence_array[CMD_sequence_index].is_setted & (1 << 4)) == false)
	{
		HB_sequence_array[CMD_sequence_index].is_setted |= (1 << 4);
	}

	HB_sequence_array[CMD_sequence_index].hv_neg_on_ms   = receive_argm[0];
	HB_sequence_array[CMD_sequence_index].hv_neg_off_ms  = receive_argm[1];

	ps_FSP_TX->CMD 	 			 		 	    = FSP_CMD_SET_PULSE_HV_NEG;
	ps_FSP_TX->Payload.set_pulse_HV_neg.OnTime	= receive_argm[0];
	ps_FSP_TX->Payload.set_pulse_HV_neg.OffTime = receive_argm[1];

	fsp_print(3);
	return CMDLINE_OK;
}

int CMD_SET_PULSE_LV_POS(int argc, char *argv[])
{
	if (g_is_calib_running == true) {
		return CMDLINE_CALIB_IS_RUNNING;
	}

	if (argc < 3)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 3)
		return CMDLINE_TOO_MANY_ARGS;

	int receive_argm[2];

	receive_argm[0] = atoi(argv[1]);
	receive_argm[1] = atoi(argv[2]);

	if ((receive_argm[0] > 1000) || (receive_argm[0] < 1))
		return CMDLINE_INVALID_ARG;
	else if ((receive_argm[1] > 1000) || (receive_argm[1] < 1))
		return CMDLINE_INVALID_ARG;

	if ((HB_sequence_array[CMD_sequence_index].is_setted & (1 << 5)) == false)
	{
		HB_sequence_array[CMD_sequence_index].is_setted |= (1 << 5);
	}

	HB_sequence_array[CMD_sequence_index].lv_pos_on_ms 	= receive_argm[0];
    HB_sequence_array[CMD_sequence_index].lv_pos_off_ms	= receive_argm[1];

	ps_FSP_TX->CMD 								     = FSP_CMD_SET_PULSE_LV_POS;
	ps_FSP_TX->Payload.set_pulse_LV_pos.OnTime_low 	 = receive_argm[0];
	ps_FSP_TX->Payload.set_pulse_LV_pos.OnTime_high  = (receive_argm[0] >> 8);

	ps_FSP_TX->Payload.set_pulse_LV_pos.OffTime_low  = receive_argm[1];
	ps_FSP_TX->Payload.set_pulse_LV_pos.OffTime_high = (receive_argm[1] >> 8);

	fsp_print(5);
	return CMDLINE_OK;
}

int CMD_SET_PULSE_LV_NEG(int argc, char *argv[])
{
	if (g_is_calib_running == true) {
		return CMDLINE_CALIB_IS_RUNNING;
	}

	if (argc < 3)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 3)
		return CMDLINE_TOO_MANY_ARGS;

	int receive_argm[2];

	receive_argm[0] = atoi(argv[1]);
	receive_argm[1] = atoi(argv[2]);

	if ((receive_argm[0] > 1000) || (receive_argm[0] < 1))
		return CMDLINE_INVALID_ARG;
	else if ((receive_argm[1] > 1000) || (receive_argm[1] < 1))
		return CMDLINE_INVALID_ARG;

	if ((HB_sequence_array[CMD_sequence_index].is_setted & (1 << 5)) == false)
	{
		HB_sequence_array[CMD_sequence_index].is_setted |= (1 << 5);
	}

	HB_sequence_array[CMD_sequence_index].lv_neg_on_ms 	= receive_argm[0];
    HB_sequence_array[CMD_sequence_index].lv_neg_off_ms	= receive_argm[1];

	ps_FSP_TX->CMD 								     = FSP_CMD_SET_PULSE_LV_NEG;
	ps_FSP_TX->Payload.set_pulse_LV_neg.OnTime_low 	 = receive_argm[0];
	ps_FSP_TX->Payload.set_pulse_LV_neg.OnTime_high  = (receive_argm[0] >> 8);

	ps_FSP_TX->Payload.set_pulse_LV_neg.OffTime_low  = receive_argm[1];
	ps_FSP_TX->Payload.set_pulse_LV_neg.OffTime_high = (receive_argm[1] >> 8);

	fsp_print(5);
	return CMDLINE_OK;
}

int CMD_SET_PULSE_CONTROL(int argc, char *argv[])
{
	if (g_is_calib_running == true) {
		return CMDLINE_CALIB_IS_RUNNING;
	}

	if (is_manual_mode_enable == true)
	{
		UART_Send_String(CMD_line_handle, "> TURNING OFF MANUAL MODE\n");
		UART_Send_String(CMD_line_handle, "> SET PULSE CONTROL ENABLE\n");

		is_manual_mode_enable = false;

		ps_FSP_TX->CMD = FSP_CMD_SET_MANUAL_PULSE;
		ps_FSP_TX->Payload.set_manual_pulse.State = false;

		fsp_print(2);
	}

	if (argc < 2)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 2)
		return CMDLINE_TOO_MANY_ARGS;

	int8_t receive_argm = atoi(argv[1]);

	if ((receive_argm > 1) || (receive_argm < 0))
		return CMDLINE_INVALID_ARG;

	if (HB_sequence_array[CMD_sequence_index].pos_pole_index == HB_sequence_array[CMD_sequence_index].neg_pole_index)
	{
		UART_Send_String(CMD_line_handle, "> ERROR CURRENT SEQUENCE PULSE POLE IS THE SAME\n");
		UART_Printf(CMD_line_handle, "> PULSE POS POLE: %d; PULSE NEG POLE: %d\n",
		User_Channel_Mapping[HB_sequence_array[CMD_sequence_index].pos_pole_index], User_Channel_Mapping[HB_sequence_array[CMD_sequence_index].neg_pole_index]);
		return CMDLINE_OK;
	}

	if (((HB_sequence_array[CMD_sequence_index].is_setted & (1 << 7)) == 0) && ((receive_argm - 1) != CMD_sequence_index))
	{
		UART_Printf(CMD_line_handle, "> ERROR CURRENT SEQUENCE INDEX: %d IS NOT CONFIRMED\n", CMD_sequence_index + 1);

		UART_Send_String(CMD_line_handle, "> EITHER CONFIRM IT OR DELETE IT\n");

		UART_Send_String(CMD_line_handle, "> \n");

		UART_Printf(CMD_line_handle, "> DELAY BETWEEN SEQUENCE: %dms\n", HB_sequence_array[CMD_sequence_index].sequence_delay_ms);

		UART_Send_String(CMD_line_handle, "> \n");

		UART_Printf(CMD_line_handle, "> POS HV PULSE COUNT: %d; NEG HV PULSE COUNT: %d\n",
		HB_sequence_array[CMD_sequence_index].hv_pos_count, HB_sequence_array[CMD_sequence_index].hv_neg_count);
		UART_Printf(CMD_line_handle, "> POS LV PULSE COUNT: %d; NEG LV PULSE COUNT: %d\n",
		HB_sequence_array[CMD_sequence_index].lv_pos_count, HB_sequence_array[CMD_sequence_index].lv_neg_count);

		UART_Send_String(CMD_line_handle, "> \n");

		UART_Printf(CMD_line_handle, "> DELAY BETWEEN HV POS AND NEG PULSE: %dms\n", HB_sequence_array[CMD_sequence_index].hv_delay_ms);
		UART_Printf(CMD_line_handle, "> DELAY BETWEEN LV POS AND NEG PULSE: %dms\n", HB_sequence_array[CMD_sequence_index].lv_delay_ms);
		UART_Printf(CMD_line_handle, "> DELAY BETWEEN HV PULSE AND LV PULSE: %dms\n", HB_sequence_array[CMD_sequence_index].pulse_delay_ms);

		UART_Send_String(CMD_line_handle, "> \n");

		UART_Printf(CMD_line_handle, "> HV PULSE POS ON TIME: %dms; HV PULSE POS OFF TIME: %dms\n",
		HB_sequence_array[CMD_sequence_index].hv_pos_on_ms, HB_sequence_array[CMD_sequence_index].hv_pos_off_ms);

		UART_Printf(CMD_line_handle, "> HV PULSE NEG ON TIME: %dms; HV PULSE NEG OFF TIME: %dms\n",
		HB_sequence_array[CMD_sequence_index].hv_neg_on_ms, HB_sequence_array[CMD_sequence_index].hv_neg_off_ms);

		UART_Printf(CMD_line_handle, "> LV PULSE POS ON TIME: %dms; LV PULSE POS OFF TIME: %dms\n",
		HB_sequence_array[CMD_sequence_index].lv_pos_on_ms, HB_sequence_array[CMD_sequence_index].lv_pos_off_ms);
		
		UART_Printf(CMD_line_handle, "> LV PULSE NEG ON TIME: %dms; LV PULSE NEG OFF TIME: %dms\n",
		HB_sequence_array[CMD_sequence_index].lv_neg_on_ms, HB_sequence_array[CMD_sequence_index].lv_neg_off_ms);

		UART_Send_String(CMD_line_handle, "> \n");
		return CMDLINE_OK;
	}

	is_h_bridge_enable = receive_argm;

	ps_FSP_TX->CMD = FSP_CMD_SET_PULSE_CONTROL;
	ps_FSP_TX->Payload.set_pulse_control.State = receive_argm;

	fsp_print(2);
	return CMDLINE_OK;
}

int CMD_GET_SEQUENCE_INDEX(int argc, char *argv[])
{
	if (argc < 1)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 1)
		return CMDLINE_TOO_MANY_ARGS;

	UART_Printf(CMD_line_handle, "> CURRENT SEQUENCE INDEX: %d\n", CMD_sequence_index + 1);

	return CMDLINE_OK;
}

int CMD_GET_SEQUENCE_DELAY(int argc, char *argv[])
{
	if (argc < 1)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 1)
		return CMDLINE_TOO_MANY_ARGS;

	UART_Printf(CMD_line_handle, "> CURRENT SEQUENCE DELAY: %dms\n", HB_sequence_array[CMD_sequence_index].sequence_delay_ms);

	return CMDLINE_OK;
}

int CMD_GET_SEQUENCE_ALL(int argc, char *argv[])
{
	if (argc < 1)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 1)
		return CMDLINE_TOO_MANY_ARGS;

	for (uint8_t i = 0; i <= CMD_total_sequence_index; i++)
	{
		UART_Printf(CMD_line_handle, "> CURRENT SEQUENCE INDEX: %d\n", i + 1);

		UART_Send_String(CMD_line_handle, "> \n");

		UART_Printf(CMD_line_handle, "> PULSE POS POLE: %d; PULSE NEG POLE: %d\n",
		User_Channel_Mapping[HB_sequence_array[i].pos_pole_index], User_Channel_Mapping[HB_sequence_array[i].neg_pole_index]);

		UART_Send_String(CMD_line_handle, "> \n");

		UART_Printf(CMD_line_handle, "> DELAY BETWEEN SEQUENCE: %dms\n", HB_sequence_array[i].sequence_delay_ms);

		UART_Send_String(CMD_line_handle, "> \n");

		UART_Printf(CMD_line_handle, "> POS HV PULSE COUNT: %d; NEG HV PULSE COUNT: %d\n",
		HB_sequence_array[i].hv_pos_count, HB_sequence_array[i].hv_neg_count);
		UART_Printf(CMD_line_handle, "> POS LV PULSE COUNT: %d; NEG LV PULSE COUNT: %d\n",
		HB_sequence_array[i].lv_pos_count, HB_sequence_array[i].lv_neg_count);

		UART_Send_String(CMD_line_handle, "> \n");

		UART_Printf(CMD_line_handle, "> DELAY BETWEEN HV POS AND NEG PULSE: %dms\n", HB_sequence_array[i].hv_delay_ms);
		UART_Printf(CMD_line_handle, "> DELAY BETWEEN LV POS AND NEG PULSE: %dms\n", HB_sequence_array[i].lv_delay_ms);
		UART_Printf(CMD_line_handle, "> DELAY BETWEEN HV PULSE AND LV PULSE: %dms\n", HB_sequence_array[i].pulse_delay_ms);

		UART_Send_String(CMD_line_handle, "> \n");

		UART_Printf(CMD_line_handle, "> HV PULSE POS ON TIME: %dms; HV PULSE POS OFF TIME: %dms\n",
		HB_sequence_array[CMD_sequence_index].hv_pos_on_ms, HB_sequence_array[CMD_sequence_index].hv_pos_off_ms);

		UART_Printf(CMD_line_handle, "> HV PULSE NEG ON TIME: %dms; HV PULSE NEG OFF TIME: %dms\n",
		HB_sequence_array[CMD_sequence_index].hv_neg_on_ms, HB_sequence_array[CMD_sequence_index].hv_neg_off_ms);

		UART_Printf(CMD_line_handle, "> LV PULSE POS ON TIME: %dms; LV PULSE POS OFF TIME: %dms\n",
		HB_sequence_array[CMD_sequence_index].lv_pos_on_ms, HB_sequence_array[CMD_sequence_index].lv_pos_off_ms);
		
		UART_Printf(CMD_line_handle, "> LV PULSE NEG ON TIME: %dms; LV PULSE NEG OFF TIME: %dms\n",
		HB_sequence_array[CMD_sequence_index].lv_neg_on_ms, HB_sequence_array[CMD_sequence_index].lv_neg_off_ms);

		UART_Send_String(CMD_line_handle, "> \n");
	}

	return CMDLINE_OK;
}

int CMD_GET_PULSE_POLE(int argc, char *argv[])
{
	if (argc < 1)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 1)
		return CMDLINE_TOO_MANY_ARGS;

	UART_Printf(CMD_line_handle, "> PULSE POS POLE: %d; PULSE NEG POLE: %d\n",
	User_Channel_Mapping[HB_sequence_array[CMD_sequence_index].pos_pole_index], User_Channel_Mapping[HB_sequence_array[CMD_sequence_index].neg_pole_index]);

	return CMDLINE_OK;
}

int CMD_GET_PULSE_COUNT(int argc, char *argv[])
{
	if (argc < 1)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 1)
		return CMDLINE_TOO_MANY_ARGS;

	UART_Printf(CMD_line_handle, "> POS HV PULSE COUNT: %d; NEG HV PULSE COUNT: %d\n",
	HB_sequence_array[CMD_sequence_index].hv_pos_count, HB_sequence_array[CMD_sequence_index].hv_neg_count);

	UART_Printf(CMD_line_handle, "> POS LV PULSE COUNT: %d; NEG LV PULSE COUNT: %d\n",
	HB_sequence_array[CMD_sequence_index].lv_pos_count, HB_sequence_array[CMD_sequence_index].lv_neg_count);

	return CMDLINE_OK;
}

int CMD_GET_PULSE_DELAY(int argc, char *argv[])
{
	if (argc < 1)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 1)
		return CMDLINE_TOO_MANY_ARGS;

	UART_Printf(CMD_line_handle, "> DELAY BETWEEN HV POS AND NEG PULSE: %dms\n", HB_sequence_array[CMD_sequence_index].hv_delay_ms);

	UART_Printf(CMD_line_handle, "> DELAY BETWEEN LV POS AND NEG PULSE: %dms\n", HB_sequence_array[CMD_sequence_index].lv_delay_ms);

	UART_Printf(CMD_line_handle, "> DELAY BETWEEN HV PULSE AND LV PULSE: %dms\n", HB_sequence_array[CMD_sequence_index].pulse_delay_ms);

	return CMDLINE_OK;		
}

int CMD_GET_PULSE_HV_POS(int argc, char *argv[])
{
	if (argc < 1)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 1)
		return CMDLINE_TOO_MANY_ARGS;

	UART_Printf(CMD_line_handle, "> HV PULSE POS ON TIME: %dms; HV PULSE POS OFF TIME: %dms\n",
	HB_sequence_array[CMD_sequence_index].hv_pos_on_ms, HB_sequence_array[CMD_sequence_index].hv_pos_off_ms);

	return CMDLINE_OK;
}

int CMD_GET_PULSE_HV_NEG(int argc, char *argv[])
{
	if (argc < 1)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 1)
		return CMDLINE_TOO_MANY_ARGS;

	UART_Printf(CMD_line_handle, "> HV PULSE NEG ON TIME: %dms; HV PULSE NEG OFF TIME: %dms\n",
	HB_sequence_array[CMD_sequence_index].hv_neg_on_ms, HB_sequence_array[CMD_sequence_index].hv_neg_off_ms);

	return CMDLINE_OK;
}

int CMD_GET_PULSE_HV(int argc, char *argv[])
{
	if (argc < 1)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 1)
		return CMDLINE_TOO_MANY_ARGS;

	UART_Printf(CMD_line_handle, "> HV PULSE POS ON TIME: %dms; HV PULSE POS OFF TIME: %dms\n",
	HB_sequence_array[CMD_sequence_index].hv_pos_on_ms, HB_sequence_array[CMD_sequence_index].hv_pos_off_ms);

	UART_Printf(CMD_line_handle, "> HV PULSE NEG ON TIME: %dms; HV PULSE NEG OFF TIME: %dms\n",
	HB_sequence_array[CMD_sequence_index].hv_neg_on_ms, HB_sequence_array[CMD_sequence_index].hv_neg_off_ms);

	return CMDLINE_OK;
}

int CMD_GET_PULSE_LV_POS(int argc, char *argv[])
{
	if (argc < 1)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 1)
		return CMDLINE_TOO_MANY_ARGS;

	UART_Printf(CMD_line_handle, "> LV PULSE POS ON TIME: %dms; LV PULSE POS OFF TIME: %dms\n",
	HB_sequence_array[CMD_sequence_index].lv_pos_on_ms, HB_sequence_array[CMD_sequence_index].lv_pos_off_ms);
	
	return CMDLINE_OK;
}

int CMD_GET_PULSE_LV_NEG(int argc, char *argv[])
{
	if (argc < 1)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 1)
		return CMDLINE_TOO_MANY_ARGS;

	UART_Printf(CMD_line_handle, "> LV PULSE NEG ON TIME: %dms; LV PULSE NEG OFF TIME: %dms\n",
	HB_sequence_array[CMD_sequence_index].lv_neg_on_ms, HB_sequence_array[CMD_sequence_index].lv_neg_off_ms);
	
	return CMDLINE_OK;
}

int CMD_GET_PULSE_LV(int argc, char *argv[])
{
	if (argc < 1)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 1)
		return CMDLINE_TOO_MANY_ARGS;

	UART_Printf(CMD_line_handle, "> LV PULSE POS ON TIME: %dms; LV PULSE POS OFF TIME: %dms\n",
	HB_sequence_array[CMD_sequence_index].lv_pos_on_ms, HB_sequence_array[CMD_sequence_index].lv_pos_off_ms);
	
	UART_Printf(CMD_line_handle, "> LV PULSE NEG ON TIME: %dms; LV PULSE NEG OFF TIME: %dms\n",
	HB_sequence_array[CMD_sequence_index].lv_neg_on_ms, HB_sequence_array[CMD_sequence_index].lv_neg_off_ms);

	return CMDLINE_OK;
}

int CMD_GET_PULSE_CONTROL(int argc, char *argv[])
{
	if (argc < 1)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 1)
		return CMDLINE_TOO_MANY_ARGS;

	if (is_h_bridge_enable == 1)
    {
        UART_Send_String(CMD_line_handle, "> H BRIDGE IS PULSING\n");
    }
    else
    {
        UART_Send_String(CMD_line_handle, "> H BRIDGE IS NOT PULSING\n");
    }

	return CMDLINE_OK;
}

int CMD_GET_PULSE_ALL(int argc, char *argv[])
{
	if (argc < 1)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 1)
		return CMDLINE_TOO_MANY_ARGS;

	UART_Send_String(CMD_line_handle, "> \n");

	UART_Printf(CMD_line_handle, "> PULSE POS POLE: %d; PULSE NEG POLE: %d\n",
	User_Channel_Mapping[HB_sequence_array[CMD_sequence_index].pos_pole_index], User_Channel_Mapping[HB_sequence_array[CMD_sequence_index].neg_pole_index]);

	UART_Send_String(CMD_line_handle, "> \n");

	UART_Printf(CMD_line_handle, "> DELAY BETWEEN SEQUENCE: %dms\n", HB_sequence_array[CMD_sequence_index].sequence_delay_ms);

	UART_Send_String(CMD_line_handle, "> \n");

	UART_Printf(CMD_line_handle, "> POS HV PULSE COUNT: %d; NEG HV PULSE COUNT: %d\n",
	HB_sequence_array[CMD_sequence_index].hv_pos_count, HB_sequence_array[CMD_sequence_index].hv_neg_count);
	UART_Printf(CMD_line_handle, "> POS LV PULSE COUNT: %d; NEG LV PULSE COUNT: %d\n",
	HB_sequence_array[CMD_sequence_index].lv_pos_count, HB_sequence_array[CMD_sequence_index].lv_neg_count);

	UART_Send_String(CMD_line_handle, "> \n");

	UART_Printf(CMD_line_handle, "> DELAY BETWEEN HV POS AND NEG PULSE: %dms\n", HB_sequence_array[CMD_sequence_index].hv_delay_ms);
	UART_Printf(CMD_line_handle, "> DELAY BETWEEN LV POS AND NEG PULSE: %dms\n", HB_sequence_array[CMD_sequence_index].lv_delay_ms);
	UART_Printf(CMD_line_handle, "> DELAY BETWEEN HV PULSE AND LV PULSE: %dms\n", HB_sequence_array[CMD_sequence_index].pulse_delay_ms);

	UART_Send_String(CMD_line_handle, "> \n");

	UART_Printf(CMD_line_handle, "> HV PULSE POS ON TIME: %dms; HV PULSE POS OFF TIME: %dms\n",
	HB_sequence_array[CMD_sequence_index].hv_pos_on_ms, HB_sequence_array[CMD_sequence_index].hv_pos_off_ms);

	UART_Printf(CMD_line_handle, "> HV PULSE NEG ON TIME: %dms; HV PULSE NEG OFF TIME: %dms\n",
	HB_sequence_array[CMD_sequence_index].hv_neg_on_ms, HB_sequence_array[CMD_sequence_index].hv_neg_off_ms);

	UART_Printf(CMD_line_handle, "> LV PULSE POS ON TIME: %dms; LV PULSE POS OFF TIME: %dms\n",
	HB_sequence_array[CMD_sequence_index].lv_pos_on_ms, HB_sequence_array[CMD_sequence_index].lv_pos_off_ms);
	
	UART_Printf(CMD_line_handle, "> LV PULSE NEG ON TIME: %dms; LV PULSE NEG OFF TIME: %dms\n",
	HB_sequence_array[CMD_sequence_index].lv_neg_on_ms, HB_sequence_array[CMD_sequence_index].lv_neg_off_ms);

	UART_Send_String(CMD_line_handle, "> \n");

//	if (is_h_bridge_enable == 1)
//    {
//        UART_Send_String(CMD_line_handle, "> H BRIDGE IS PULSING\n");
//    }
//    else
//    {
//        UART_Send_String(CMD_line_handle, "> H BRIDGE IS NOT PULSING\n");
//    }

	return CMDLINE_OK;
}

/* :::::::::: Auto Pulsing Command :::::::::: */
int CMD_SET_THRESHOLD_ACCEL(int argc, char *argv[])
{
	if (argc < 4)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 4)
		return CMDLINE_TOO_MANY_ARGS;

	int16_t x = (int16_t)atoi(argv[1]);
	int16_t y = (int16_t)atoi(argv[2]);
	int16_t z = (int16_t)atoi(argv[3]);

	Threshold_Accel.x = x;
	Threshold_Accel.y = y;
	Threshold_Accel.z = z;

	return CMDLINE_OK;
}

int CMD_GET_THRESHOLD_ACCEL(int argc, char *argv[])
{
	if (argc < 1)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 1)
		return CMDLINE_TOO_MANY_ARGS;

	// In giá trị ra
	UART_Printf(CMD_line_handle, "Threshold Accel is: %d %d %d\n", Threshold_Accel.x, Threshold_Accel.y, Threshold_Accel.z);

	return CMDLINE_OK;
}

int CMD_SET_AUTO_ACCEL(int argc, char *argv[])
{
	if (argc < 2)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 2)
		return CMDLINE_TOO_MANY_ARGS;
	
	int8_t receive_argm = atoi(argv[1]);

	if ((receive_argm > 1) || (receive_argm < 0))
		return CMDLINE_INVALID_ARG;

	if (HB_sequence_array[CMD_sequence_index].pos_pole_index == HB_sequence_array[CMD_sequence_index].neg_pole_index)
	{
		UART_Send_String(CMD_line_handle, "> ERROR CURRENT SEQUENCE PULSE POLE IS THE SAME\n");
		UART_Printf(CMD_line_handle, "> PULSE POS POLE: %d; PULSE NEG POLE: %d\n",
		User_Channel_Mapping[HB_sequence_array[CMD_sequence_index].pos_pole_index], User_Channel_Mapping[HB_sequence_array[CMD_sequence_index].neg_pole_index]);
		return CMDLINE_OK;
	}

	if (((HB_sequence_array[CMD_sequence_index].is_setted & (1 << 7)) == 0) && ((receive_argm - 1) != CMD_sequence_index))
	{
		UART_Printf(CMD_line_handle, "> ERROR CURRENT SEQUENCE INDEX: %d IS NOT CONFIRMED\n", CMD_sequence_index + 1);

		UART_Send_String(CMD_line_handle, "> EITHER CONFIRM IT OR DELETE IT\n");

		UART_Send_String(CMD_line_handle, "> \n");

		UART_Printf(CMD_line_handle, "> DELAY BETWEEN SEQUENCE: %dms\n", HB_sequence_array[CMD_sequence_index].sequence_delay_ms);

		UART_Send_String(CMD_line_handle, "> \n");

		UART_Printf(CMD_line_handle, "> POS HV PULSE COUNT: %d; NEG HV PULSE COUNT: %d\n",
		HB_sequence_array[CMD_sequence_index].hv_pos_count, HB_sequence_array[CMD_sequence_index].hv_neg_count);
		UART_Printf(CMD_line_handle, "> POS LV PULSE COUNT: %d; NEG LV PULSE COUNT: %d\n",
		HB_sequence_array[CMD_sequence_index].lv_pos_count, HB_sequence_array[CMD_sequence_index].lv_neg_count);

		UART_Send_String(CMD_line_handle, "> \n");

		UART_Printf(CMD_line_handle, "> DELAY BETWEEN HV POS AND NEG PULSE: %dms\n", HB_sequence_array[CMD_sequence_index].hv_delay_ms);
		UART_Printf(CMD_line_handle, "> DELAY BETWEEN LV POS AND NEG PULSE: %dms\n", HB_sequence_array[CMD_sequence_index].lv_delay_ms);
		UART_Printf(CMD_line_handle, "> DELAY BETWEEN HV PULSE AND LV PULSE: %dms\n", HB_sequence_array[CMD_sequence_index].pulse_delay_ms);

		UART_Send_String(CMD_line_handle, "> \n");

		UART_Printf(CMD_line_handle, "> HV PULSE POS ON TIME: %dms; HV PULSE POS OFF TIME: %dms\n",
		HB_sequence_array[CMD_sequence_index].hv_pos_on_ms, HB_sequence_array[CMD_sequence_index].hv_pos_off_ms);

		UART_Printf(CMD_line_handle, "> HV PULSE NEG ON TIME: %dms; HV PULSE NEG OFF TIME: %dms\n",
		HB_sequence_array[CMD_sequence_index].hv_neg_on_ms, HB_sequence_array[CMD_sequence_index].hv_neg_off_ms);

		UART_Printf(CMD_line_handle, "> LV PULSE POS ON TIME: %dms; LV PULSE POS OFF TIME: %dms\n",
		HB_sequence_array[CMD_sequence_index].lv_pos_on_ms, HB_sequence_array[CMD_sequence_index].lv_pos_off_ms);
		
		UART_Printf(CMD_line_handle, "> LV PULSE NEG ON TIME: %dms; LV PULSE NEG OFF TIME: %dms\n",
		HB_sequence_array[CMD_sequence_index].lv_neg_on_ms, HB_sequence_array[CMD_sequence_index].lv_neg_off_ms);

		UART_Send_String(CMD_line_handle, "> \n");
		return CMDLINE_OK;
	}

	if (receive_argm == 1)
	{
		is_streaming_enable = true;
		Enable_Auto_Pulsing();
	}
	else
	{
		is_streaming_enable = false;
		Disable_Auto_Pulsing();
	}

	return CMDLINE_OK;
}

/* :::::::::: Manual Pulse Command :::::::::: */
int CMD_SET_MANUAL_POLE(int argc, char *argv[])
{
	if (argc < 3)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 3)
		return CMDLINE_TOO_MANY_ARGS;

	int receive_argm[2];

	receive_argm[0] = atoi(argv[1]);
	receive_argm[1] = atoi(argv[2]);

	if (receive_argm[0] == receive_argm[1])
		return CMDLINE_INVALID_ARG;
	else if ((receive_argm[0] > 8) || (receive_argm[0] < 1) || (receive_argm[0] == 9))
		return CMDLINE_INVALID_ARG;
	else if ((receive_argm[1] > 8) || (receive_argm[1] < 1) || (receive_argm[1] == 9))
		return CMDLINE_INVALID_ARG;

	ps_FSP_TX->CMD = FSP_CMD_SET_MANUAL_POLE;
	ps_FSP_TX->Payload.set_manual_pole.pos_pole = ChannelMapping[receive_argm[0] - 1];
	ps_FSP_TX->Payload.set_manual_pole.neg_pole = ChannelMapping[receive_argm[1] - 1];

	fsp_print(3);

	return CMDLINE_OK;
}

int CMD_SET_MANUAL_CAP(int argc, char *argv[])
{
	if (is_h_bridge_enable == true)
	{
		UART_Send_String(&RS232_UART, "> ERROR: H BRIDGE IS RUNNING\n");
		return CMDLINE_INVALID_CMD;
	}
	
	if (argc < 2)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 2)
		return CMDLINE_TOO_MANY_ARGS;

	uint8_t receive_argm;

	receive_argm = atoi(argv[1]);

	if ((receive_argm > 3) || (receive_argm < 0))
		return CMDLINE_INVALID_ARG;
	
	ps_FSP_TX->CMD = FSP_CMD_SET_MANUAL_CAP;
	ps_FSP_TX->Payload.set_manual_cap.Which_Cap = receive_argm;

	fsp_print(2);
	return CMDLINE_OK;
}

int CMD_SET_MANUAL_PULSE(int argc, char *argv[])
{
	if (is_h_bridge_enable == true)
	{
		UART_Send_String(&RS232_UART, "> ERROR: H BRIDGE IS RUNNING\n");
		return CMDLINE_INVALID_CMD;
	}

	if (argc < 2)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 2)
		return CMDLINE_TOO_MANY_ARGS;

	uint8_t receive_argm;

	receive_argm = atoi(argv[1]);

	if ((receive_argm > 1) || (receive_argm < 0))
		return CMDLINE_INVALID_ARG;


	is_manual_mode_enable = receive_argm;

	ps_FSP_TX->CMD = FSP_CMD_SET_MANUAL_PULSE;
	ps_FSP_TX->Payload.set_manual_pulse.State = receive_argm;

	fsp_print(2);
	return CMDLINE_OK;
}

/* :::::::::: VOM Command :::::::: */
int CMD_MEASURE_VOLT(int argc, char *argv[])
{
	if (argc < 1)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 1)
		return CMDLINE_TOO_MANY_ARGS;

	uint16_t data_tmp[2];

	data_tmp[0] = Cap_Measure_Volt(&g_Cap_300V);
	data_tmp[1] = Cap_Measure_Volt(&g_Cap_50V);

	UART_Printf(CMD_line_handle, "> HV cap: %dV, LV cap: %dV\r\n", data_tmp[0], data_tmp[1]);

	return CMDLINE_OK;
}

int CMD_MEASURE_CURRENT(int argc, char *argv[])
{
	if (argc < 1)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 1)
		return CMDLINE_TOO_MANY_ARGS;

	return CMDLINE_OK;
}

uint16_t Impedance_Period = 0;
uint8_t  Impedance_pos_pole, Impedance_neg_pole;
int CMD_MEASURE_IMPEDANCE(int argc, char *argv[])
{
	if (argc < 3)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 3)
		return CMDLINE_TOO_MANY_ARGS;

	int receive_argm[2];

	receive_argm[0]  = atoi(argv[1]);
	receive_argm[1]  = atoi(argv[2]);
	// receive_argm[2]  = atoi(argv[3]);
	// receive_argm[3]  = atoi(argv[4]);
	// receive_argm[4]  = atoi(argv[5]);

	if ((receive_argm[0] > 8) || (receive_argm[0] < 1))
		return CMDLINE_INVALID_ARG;

	if ((receive_argm[1] > 8) || (receive_argm[1] < 1))
		return CMDLINE_INVALID_ARG;

	// if ((receive_argm[2] > 300) || (receive_argm[2] < 0))
	// 	return CMDLINE_INVALID_ARG;

	// if ((receive_argm[3] > 2000) || (receive_argm[3] < 0))
	// 	return CMDLINE_INVALID_ARG;
	
	// if ((receive_argm[4] > 1) || (receive_argm[4] < 0))
	// 	return CMDLINE_INVALID_ARG;
	
	Impedance_pos_pole = ChannelMapping[receive_argm[0] - 1];
	Impedance_neg_pole = ChannelMapping[receive_argm[1] - 1];

	// Impedance_Period = receive_argm[3];
	// is_cap_release_after_measure = receive_argm[4];

	// g_is_Discharge_300V_On = 0;
	// g_is_Discharge_50V_On = 0;

	// g_PID_is_300V_on = 0;
	// Calib_Calculate_HV(receive_argm[2]);
	// UART_Printf(&CMD_line_handle, "> CHARGING HV CAP TO %dV\r\n", receive_argm[2]);
	// g_PID_is_300V_on = 1;

	Cap_Set_Discharge(&g_Cap_300V, false, false);

	is_measure_impedance_enable = true;
	//is_300V_notified_enable = true;

	SchedulerTaskEnable(IMPEDANCE_TASK, 1);

	return CMDLINE_OK;
}

/* :::::::::: I2C Sensor Command :::::::: */
int CMD_GET_SENSOR_GYRO(int argc, char *argv[])
{
{
switch (CMD_process_state)
{
case 0:
{
	if (argc < 1)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 1)
		return CMDLINE_TOO_MANY_ARGS;

	Sensor_Read_Value(SENSOR_READ_GYRO);
	CMD_process_state = 1;
	return CMDLINE_IS_PROCESSING;
}


case 1:
{
	if (Is_Sensor_Read_Complete(&Sensor_LSM6DSOX_rb) == false)
	{
		return CMDLINE_IS_PROCESSING;
	}
	
	UART_Printf(CMD_line_handle, "> GYRO x: %dmpds; GYRO y: %dmpds; GYRO z: %dmpds\n", Sensor_Gyro.x, Sensor_Gyro.y, Sensor_Gyro.z);
	CMD_process_state = 0;
    return CMDLINE_OK;
}

default:
	break;
}
return CMDLINE_BAD_CMD;
}
}

int CMD_GET_SENSOR_ACCEL(int argc, char *argv[])
{
switch (CMD_process_state)
{
case 0:
{
	if (argc < 1)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 1)
		return CMDLINE_TOO_MANY_ARGS;

	Sensor_Read_Value(SENSOR_READ_ACCEL);
	CMD_process_state = 1;
	return CMDLINE_IS_PROCESSING;
}


case 1:
{
	if (Is_Sensor_Read_Complete(&Sensor_LSM6DSOX_rb) == false)
	{
		return CMDLINE_IS_PROCESSING;
	}
	
	UART_Printf(CMD_line_handle, "> ACCEL x: %dmg; ACCEL y: %dmg; ACCEL z: %dmg\n", Sensor_Accel.x, Sensor_Accel.y, Sensor_Accel.z);
	CMD_process_state = 0;
    return CMDLINE_OK;
}

default:
	break;
}
return CMDLINE_BAD_CMD;
}

int CMD_GET_SENSOR_LSM6DSOX(int argc, char *argv[])
{
switch (CMD_process_state)
{
case 0:
{
	if (argc < 1)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 1)
		return CMDLINE_TOO_MANY_ARGS;

	Sensor_Read_Value(SENSOR_READ_LSM6DSOX);
	CMD_process_state = 1;
	return CMDLINE_IS_PROCESSING;
}


case 1:
{
	if (Is_Sensor_Read_Complete(&Sensor_LSM6DSOX_rb) == false)
	{
		return CMDLINE_IS_PROCESSING;
	}
	
	UART_Printf(CMD_line_handle, "> GYRO x: %dmpds; GYRO y: %dmpds; GYRO z: %dmpds\n", Sensor_Gyro.x, Sensor_Gyro.y, Sensor_Gyro.z);
	UART_Printf(CMD_line_handle, "> ACCEL x: %dmg; ACCEL y: %dmg; ACCEL z: %dmg\n", Sensor_Accel.x, Sensor_Accel.y, Sensor_Accel.z);
	CMD_process_state = 0;
    return CMDLINE_OK;
}

default:
	break;
}
return CMDLINE_BAD_CMD;
}

int CMD_GET_SENSOR_TEMP(int argc, char *argv[])
{
switch (CMD_process_state)
{
case 0:
{
	if (argc < 1)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 1)
		return CMDLINE_TOO_MANY_ARGS;

	Sensor_Read_Value(SENSOR_READ_TEMP);

	CMD_process_state = 1;
	return CMDLINE_IS_PROCESSING;
}


case 1:
{
	if (Is_Sensor_Read_Complete(&Sensor_BMP390_rb) == false)
	{
		return CMDLINE_IS_PROCESSING;
	}
	
	char fractional_string[16] = {0};
	double_to_string(Sensor_Temp, fractional_string, 3);

	UART_Printf(CMD_line_handle, "> TEMPERATURE: %s C\n", fractional_string);

	CMD_process_state = 0;
    return CMDLINE_OK;
}

default:
	break;
}
return CMDLINE_BAD_CMD;
}

int CMD_GET_SENSOR_PRESSURE(int argc, char *argv[])
{
switch (CMD_process_state)
{
case 0:
{
	if (argc < 1)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 1)
		return CMDLINE_TOO_MANY_ARGS;

	Sensor_Read_Value(SENSOR_READ_PRESSURE);

	CMD_process_state = 1;
	return CMDLINE_IS_PROCESSING;
}


case 1:
{
	if (Is_Sensor_Read_Complete(&Sensor_BMP390_rb) == false)
	{
		return CMDLINE_IS_PROCESSING;
	}

	char fractional_string[16] = {0};
	double_to_string(Sensor_Pressure, fractional_string, 3);
	
	UART_Printf(CMD_line_handle, "> PRESSURE: %s Pa\n", fractional_string);

	CMD_process_state = 0;
    return CMDLINE_OK;
}

default:
	break;
}
return CMDLINE_BAD_CMD;
}

int CMD_GET_SENSOR_ALTITUDE(int argc, char *argv[])
{
switch (CMD_process_state)
{
case 0:
{
	if (argc < 1)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 1)
		return CMDLINE_TOO_MANY_ARGS;

	Sensor_Read_Value(SENSOR_READ_ACCEL);

	CMD_process_state = 1;
	return CMDLINE_IS_PROCESSING;
}


case 1:
{
	if (Is_Sensor_Read_Complete(&Sensor_BMP390_rb) == false)
	{
		return CMDLINE_IS_PROCESSING;
	}
	
	char fractional_string[16] = {0};
	double_to_string(Sensor_Altitude, fractional_string, 3);

	UART_Printf(CMD_line_handle, "> ALTITUDE: %s m\n", fractional_string);

	CMD_process_state = 0;
    return CMDLINE_OK;
}

default:
	break;
}
return CMDLINE_BAD_CMD;
}

int CMD_GET_SENSOR_BMP390(int argc, char *argv[])
{
switch (CMD_process_state)
{
case 0:
{
	if (argc < 1)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 1)
		return CMDLINE_TOO_MANY_ARGS;

	Sensor_Read_Value(SENSOR_READ_BMP390);

	CMD_process_state = 1;
	return CMDLINE_IS_PROCESSING;
}


case 1:
{
	if (Is_Sensor_Read_Complete(&Sensor_BMP390_rb) == false)
	{
		return CMDLINE_IS_PROCESSING;
	}

	char fractional_string[16] = {0};

	double_to_string(Sensor_Temp, fractional_string, 3);
	UART_Printf(CMD_line_handle, "> TEMPERATURE: %s C\n", fractional_string);

	double_to_string(Sensor_Pressure, fractional_string, 3);
	UART_Printf(CMD_line_handle, "> PRESSURE: %s Pa\n", fractional_string);

	double_to_string(Sensor_Altitude, fractional_string, 3);
	UART_Printf(CMD_line_handle, "> ALTITUDE: %s m\n", fractional_string);

	CMD_process_state = 0;
    return CMDLINE_OK;
}

default:
	break;
}
return CMDLINE_BAD_CMD;
}

int CMD_GET_SENSOR_H3LIS(int argc, char *argv[])
{
	if (argc < 1)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 1)
		return CMDLINE_TOO_MANY_ARGS;

	switch (CMD_process_state)
	{
	case 0:
	{
		if (argc < 1)
			return CMDLINE_TOO_FEW_ARGS;
		else if (argc > 1)
			return CMDLINE_TOO_MANY_ARGS;

		Sensor_Read_Value(ONBOARD_SENSOR_READ_H3LIS331DL);
		CMD_process_state = 1;
		return CMDLINE_IS_PROCESSING;
	}

	case 1:
	{
		if (Is_Sensor_Read_Complete(&Onboard_Sensor_H3LIS331DL_rb) == false)
		{
			return CMDLINE_IS_PROCESSING;
		}

		UART_Printf(CMD_line_handle, "> ACCEL x: %dmg; ACCEL y: %dmg; ACCEL z: %dmg\n", H3LIS_Accel.x, H3LIS_Accel.y, H3LIS_Accel.z);
		CMD_process_state = 0;
		return CMDLINE_OK;
	}

	default:
		break;
	}

	return CMDLINE_BAD_CMD;
}

/* :::::::::: Ultility Command :::::::: */
int CMD_HELP(int argc, char *argv[])
{
	tCmdLineEntry *pEntry;

	UART_Send_String(&RS232_UART, "\nAvailable commands\r\n");
	UART_Send_String(&RS232_UART, "------------------\r\n");

	UART_Send_String(&RF_UART, "\nAvailable commands\r\n");
	UART_Send_String(&RF_UART, "------------------\r\n");

	// Point at the beginning of the command table.
	pEntry = &g_psCmdTable[0];

	// Enter a loop to read each entry from the command table.  The
	// end of the table has been reached when the command name is NULL.
	while (pEntry->pcCmd) {
		// Print the command name and the brief description.
		UART_Send_String(&RS232_UART, pEntry->pcCmd);
		UART_Send_String(&RS232_UART, pEntry->pcHelp);
		UART_Send_String(&RS232_UART, "\r\n");

		UART_Send_String(&RF_UART, pEntry->pcCmd);
		UART_Send_String(&RF_UART, pEntry->pcHelp);
		UART_Send_String(&RF_UART, "\r\n");

		// Advance to the next entry in the table.
		pEntry++;

	}
	// Return success.
	return (CMDLINE_OK);
}

int CMD_CALIB_RUN(int argc, char *argv[])
{
	if (argc < 2)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 2)
		return CMDLINE_TOO_MANY_ARGS;

	int receive_argm = atoi(argv[1]);

	if ((receive_argm > 1) || (receive_argm < 0))
		return CMDLINE_INVALID_ARG;

	g_is_calib_running = receive_argm;

	if (receive_argm == 1) {
		SchedulerTaskEnable(CALIB_TASK, 1);
	}

	return CMDLINE_OK;
}

int CMD_CALIB_MEASURE(int argc, char *argv[])
{
	if (argc < 3)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 3)
		return CMDLINE_TOO_MANY_ARGS;

	int receive_argm[2];

	receive_argm[0] = atoi(argv[1]);
	receive_argm[1] = atoi(argv[2]);

	g_HV_Measure_mv = receive_argm[0];
	g_LV_Measure_mv = receive_argm[1];

	g_is_measure_available = true;

	return CMDLINE_OK;
}

int CMD_CALL_GPP(int argc, char *argv[])
{
	if (argc < 1)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 1)
		return CMDLINE_TOO_MANY_ARGS;

	ps_FSP_TX->CMD 			= FSP_CMD_HANDSHAKE;
	ps_FSP_TX->Payload.handshake.Check = 0xAB;

	fsp_print(2);
	return CMDLINE_OK;
}

int CMD_CLEAR_SCREEN(int argc, char *argv[])
{
    UART_Send_String(CMD_line_handle, "\033[2J");
    return CMDLINE_NO_RESPONSE;
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
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

static void double_to_string(double value, char *buffer, uint8_t precision)
{
    // Handle negative numbers
    if (value < 0)
	{
        *buffer++ = '-';
        value = -value;
    }

    // Extract the integer part
    uint32_t integer_part  = (uint32_t)value;
    double fractional_part = value - integer_part;

    // Convert integer part to string
    sprintf(buffer, "%ld", integer_part);
    while (*buffer) buffer++; // Move pointer to the end of the integer part

    // Add decimal point
    if (precision > 0)
	{
        *buffer++ = '.';

        // Extract and convert the fractional part
        for (uint8_t i = 0; i < precision; i++)
		{
            fractional_part *= 10;
            uint8_t digit = (uint8_t)fractional_part;
            *buffer++ = '0' + digit;
            fractional_part -= digit;
        }
    }

    // Null-terminate the string
    *buffer = '\0';
}
