/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Include~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include <stdint.h>
#include <stdbool.h>

#include "fsp_line_task.h"

#include "app.h"

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Defines ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Enum ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Struct ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Class ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Private Types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
struct _fsp_line_typedef {
				uint16_t 	buffer_size;
				char 		*p_buffer;

	volatile 	uint16_t 	write_index;
	volatile 	char 		RX_char;
};
typedef struct _fsp_line_typedef fsp_line_typedef;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
static const char * ErrorCode[7] =
{
    "FSP_PKT_RECEIVED_OK\n",
    "FSP_PKT_NOT_READY\n",
    "FSP_PKT_INVALID\n",
    "FSP_PKT_WRONG_ADR\n",
    "FSP_PKT_ERROR\n",
    "FSP_PKT_CRC_FAIL\n",
    "FSP_PKT_WRONG_LENGTH\n"
};

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
uart_stdio_typedef 	GPP_UART;
char 				g_GPP_UART_TX_buffer[GPP_TX_SIZE];
char 				g_GPP_UART_RX_buffer[GPP_RX_SIZE];

fsp_packet_t 		s_FSP_TX_Packet;
fsp_packet_t 		s_FSP_RX_Packet;
FSP_Payload 		*ps_FSP_TX = (FSP_Payload*) (&s_FSP_TX_Packet.payload);		//for TX
FSP_Payload 		*ps_FSP_RX = (FSP_Payload*) (&s_FSP_RX_Packet.payload);		//for RX

fsp_line_typedef 	FSP_line;
char 				g_FSP_line_buffer[FSP_BUF_LEN];

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* :::::::::: CMD Line Task Init :::::::: */
void FSP_Line_Task_Init()
{
	UART_Init(	&GPP_UART, GPP_UART_HANDLE, GPP_UART_IRQ, g_GPP_UART_TX_buffer,
				g_GPP_UART_RX_buffer, GPP_TX_SIZE, GPP_RX_SIZE);

	FSP_line.p_buffer 		= g_FSP_line_buffer;
	FSP_line.buffer_size 	= FSP_BUF_LEN;
	FSP_line.write_index 	= 0;

	if (FSP_line.buffer_size != 0)
	{
		memset((void*) FSP_line.p_buffer, 0, sizeof(FSP_line.p_buffer));
	}

	fsp_init(FSP_ADR_GPC);
}

/* :::::::::: CMD Line Task ::::::::::::: */
void FSP_Line_Task(void*)
{
	uint8_t FSP_return, time_out = 0;

	for (time_out = 50; (!RX_BUFFER_EMPTY(&GPP_UART)) && (time_out != 0); time_out--)
	{
		FSP_line.RX_char = UART_Get_Char(&GPP_UART);

		if (FSP_line.RX_char == FSP_PKT_SOD)
		{
			FSP_line.write_index = 0;
		}
		else if (FSP_line.RX_char == FSP_PKT_EOF)
		{
			FSP_return = frame_decode((uint8_t*) FSP_line.p_buffer, FSP_line.write_index, &s_FSP_RX_Packet);
			
			UART_Printf(&RS232_UART, "%s> ", ErrorCode[FSP_return]);

			if (FSP_return == FSP_PKT_READY)
				FSP_Line_Process();

			FSP_line.write_index = 0;
		}
		else 
		{
			FSP_line.p_buffer[FSP_line.write_index] = FSP_line.RX_char;
			FSP_line.write_index++;

			if (FSP_line.write_index > FSP_line.buffer_size)
				FSP_line.write_index = 0;

		}
	}
}

/* :::::::::: IRQ Handler ::::::::::::: */
// Private helper - does NOT touch NVIC
static void UART_Write_Byte_Internal(uart_stdio_typedef* p_uart)
{
    // Write data to the hardware
    LL_USART_TransmitData8(p_uart->handle, p_uart->p_TX_buffer[p_uart->TX_read_index]);
    
    // Advance the index safely
    UART_advance_buffer_index(&(p_uart)->TX_read_index, p_uart->TX_size);
}

void GPP_UART_IRQHandler(void)
{
	if(LL_USART_IsActiveFlag_TXE(GPP_UART.handle) && LL_USART_IsEnabledIT_TXE(GPP_UART.handle))
	{
		if (TX_BUFFER_EMPTY(&GPP_UART))
		{
			// Buffer empty, so disable interrupts
			LL_USART_DisableIT_TXE(GPP_UART.handle);
		} 
		else
		{
			// There is more data in the output buffer. Send the next byte
			UART_Write_Byte_Internal(&GPP_UART);
		}
	}

	if (LL_USART_IsActiveFlag_RXNE(GPP_UART.handle) == true)
	{
		GPP_UART.RX_irq_char = LL_USART_ReceiveData8(GPP_UART.handle);

		// NOTE: On win 10, default PUTTY when hit enter only send back '\r',
		// while on default HERCULES when hit enter send '\r\n' in that order.
		// The code bellow is modified so that it can work on PUTTY and HERCULES.
		if ((!RX_BUFFER_FULL(&GPP_UART))) 
		{
			GPP_UART.p_RX_buffer[GPP_UART.RX_write_index] = GPP_UART.RX_irq_char;
			ADVANCE_RX_WRITE_INDEX(&GPP_UART);
		}
	}
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ End of the program ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
