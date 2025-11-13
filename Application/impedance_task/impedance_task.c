/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Include~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include "app.h"

#define MY_GPIO_H_
#include "stm32f4_header.h"

#include "impedance_task.h"

#include "fsp.h"
#include "crc.h"

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Defines ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#define VOLTAGE_RANGE_SIZE 5

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Enum ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Struct ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Class ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Private Types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
typedef enum
{
	IMPEDANCE_SET_VOLT_STATE,
    IMPEDANCE_SEND_COMMAND,
    IMPEDANCE_MEASURE_STATE,
} Impedance_State_typedef;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
static Impedance_State_typedef Impedance_State = IMPEDANCE_SET_VOLT_STATE;

static uint8_t  voltage_range_count = 0;
static uint16_t voltage_range_array[] =
{
    30,
    50,
    100,
    // 150,
    200,
    // 250,
    // 275,
    300,
};

static uint16_t impedance_range_count = 0;
static uint16_t impedance_range_array[] =
{
    600,
    1000,
    2000,
    // 1500,
    4000,
    // 2500,
    // 2750,
    6000,
};

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
static void fsp_print(uint8_t packet_length);

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
extern uint16_t Impedance;

extern uint16_t Impedance_Period;
extern uint8_t  Impedance_pos_pole, Impedance_neg_pole;

extern bool     is_cap_release_after_measure;
extern bool     is_measure_impedance_enable;

bool is_impedance_volt_complete = false;
bool is_measure_impedance_completed = false;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
void Impedance_Task_Init(void)
{
    ;
}

void Impedance_Task(void*)
{
    if (is_measure_impedance_enable == false)
    {
        voltage_range_count = 0;
        impedance_range_count = 0;
        is_impedance_volt_complete = false;
        is_measure_impedance_enable = false;

        Impedance_State = IMPEDANCE_SET_VOLT_STATE;

        SchedulerTaskDisable(IMPEDANCE_TASK);
        return;
    }

    switch (Impedance_State)
    {
    case IMPEDANCE_SET_VOLT_STATE:
    {
        Cap_Set_Volt(&g_Cap_300V, voltage_range_array[voltage_range_count], true);
        voltage_range_count++;

        Cap_Set_Charge(&g_Cap_300V, true, true);

        Impedance_State = IMPEDANCE_SEND_COMMAND;
        break;
    }

    case IMPEDANCE_SEND_COMMAND:
    {
        if (is_impedance_volt_complete == false)
        {
            break;
        }
        
        is_impedance_volt_complete = false;

        ps_FSP_TX->CMD 									 	= FSP_CMD_MEASURE_IMPEDANCE;
        ps_FSP_TX->Payload.measure_impedance.Pos_pole_index = Impedance_pos_pole;
        ps_FSP_TX->Payload.measure_impedance.Neg_pole_index = Impedance_neg_pole;
        ps_FSP_TX->Payload.measure_impedance.Period_low  	= 100;
        ps_FSP_TX->Payload.measure_impedance.Period_high 	= (100 >> 8);
        fsp_print(5);

        Impedance_State = IMPEDANCE_MEASURE_STATE;
        break;
    }

    case IMPEDANCE_MEASURE_STATE:
    {
        if (is_measure_impedance_completed == false)
        {
            break;
        }

        is_measure_impedance_completed = false;
        
        if ((Impedance > impedance_range_array[impedance_range_count]) && (voltage_range_count < VOLTAGE_RANGE_SIZE))
        {
            impedance_range_count++;

            Cap_Set_Charge(&g_Cap_300V, false, false);

            Impedance_State = IMPEDANCE_SET_VOLT_STATE;
            break;
        }

        if (is_cap_release_after_measure == true)
        {
            Cap_Set_Discharge(&g_Cap_300V, true, true);
        }

        UART_Send_String(CMD_line_handle, "MEASURING...OK\n");

		UART_Printf(CMD_line_handle, "> IMPEDANCE IS %d Ohm\n", Impedance);

		UART_Send_String(CMD_line_handle, "> CAP IS DISCHARGING\n> ");

	    Cap_Set_Discharge_All(true, false, true, false);

        voltage_range_count = 0;
        impedance_range_count = 0;
        is_impedance_volt_complete = false;
        is_measure_impedance_enable = false;

        Impedance_State = IMPEDANCE_SET_VOLT_STATE;
        break;
    }
    
    default:
        break;
    }
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

	uint8_t encoded_frame[20] = { 0 };
	uint8_t frame_len;
	fsp_encode(&s_FSP_TX_Packet, encoded_frame, &frame_len);

	UART_FSP(&GPP_UART, (char*)encoded_frame, frame_len);
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ End of the program ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
