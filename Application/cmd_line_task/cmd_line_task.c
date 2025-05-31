/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Include~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "cmd_line.h"
// #include "pid.h"
// #include "fsp.h"

#include "cmd_line_task.h"
#include "command.h"

#include "app.h"

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Defines ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Enum ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Struct ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Class ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Private Types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
struct _cmd_line_typedef
{
                uint16_t    buffer_size;
                char*       p_buffer;

    volatile    uint16_t    write_index;
    volatile    char        RX_char;
};
typedef struct _cmd_line_typedef cmd_line_typedef;
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
uart_stdio_typedef  *CMD_line_handle;

uart_stdio_typedef  RS232_UART;
char                g_RS232_UART_TX_buffer[2048];
char                g_RS232_UART_RX_buffer[64];

uart_stdio_typedef  RF_UART;
char                g_RF_UART_TX_buffer[2048];
char                g_RF_UART_RX_buffer[64];

cmd_line_typedef    RS232_CMD_line;
char                g_RS232_CMD_line_buffer[64];

cmd_line_typedef    RF_CMD_line;
char                g_RF_CMD_line_buffer[64];

static bool         is_warned_user = false;

static const char * ErrorCode[7] = 
{
    "OK\n",
    "CMDLINE_BAD_CMD\n",
    "CMDLINE_TOO_MANY_ARGS\n",
    "CMDLINE_TOO_FEW_ARGS\n",
    "CMDLINE_INVALID_ARG\n",
    "CMDLINE_INVALID_CMD\n",
    "CALIB IS RUNNING, USE CALIB_EXIT TO EXIT CALIB\n",
};

const char SPLASH[][65] = 
{
{"\r\n"},
{".........................................................\r\n"},
{".........................................................\r\n"},
{"..    ____                       _     _               ..\r\n"},
{"..   / ___| _ __   __ _  ___ ___| |   (_)_ __  _ __    ..\r\n"},
{"..   \\___ \\| '_ \\ / _` |/ __/ _ \\ |   | | '_ \\| '_ \\   ..\r\n"},
{"..    ___) | |_) | (_| | (_|  __/ |___| | | | | | | |  ..\r\n"},
{"..   |____/| .__/ \\__,_|\\___\\___|_____|_|_| |_|_| |_|  ..\r\n"},
{"..         |_|    _   _ _____ _____                    ..\r\n"},
{"..               | | | | ____| ____|                   ..\r\n"},
{"..               | |_| |  _| |  _|                     ..\r\n"},
{"..               |  _  | |___| |___                    ..\r\n"},
{"..               |_| |_|_____|_____|                   ..\r\n"},
{"..            __     _____   ___   ___                 ..\r\n"},
{"..            \\ \\   / / _ \\ / _ \\ / _ \\                ..\r\n"},
{"..             \\ \\ / / | | | | | | | | |               ..\r\n"},
{"..              \\ V /| |_| | |_| | |_| |               ..\r\n"},
{"..               \\_/  \\___(_)___(_)___/                ..\r\n"},
{".........................................................\r\n"},
{".........................................................\r\n"},                                                   
};

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
static void         CMD_send_splash(uart_stdio_typedef* p_uart);

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
uint8_t g_RS232_CMD_line_return = CMDLINE_OK;
uint8_t g_RF_CMD_line_return = CMDLINE_OK;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* :::::::::: CMD Line Task Init :::::::: */
void CMD_Line_Task_Init()
{

    UART_Init(  &RS232_UART, RS232_UART_HANDLE, RS232_UART_IRQ,
                g_RS232_UART_TX_buffer, g_RS232_UART_RX_buffer,
                sizeof(g_RS232_UART_TX_buffer), sizeof(g_RS232_UART_RX_buffer));

    UART_Init(  &RF_UART, RF_UART_HANDLE, RF_UART_IRQ,
                g_RF_UART_TX_buffer, g_RF_UART_RX_buffer,
                sizeof(g_RF_UART_TX_buffer), sizeof(g_RF_UART_RX_buffer));

    RS232_CMD_line.p_buffer         = g_RS232_CMD_line_buffer;
    RS232_CMD_line.buffer_size      = 64;
    RS232_CMD_line.write_index      = 0;

    if(RS232_CMD_line.buffer_size != 0)
    {
        memset((void *)RS232_CMD_line.p_buffer, 0, sizeof(RS232_CMD_line.p_buffer));
    }

    RF_CMD_line.p_buffer            = g_RF_CMD_line_buffer;
    RF_CMD_line.buffer_size         = 64;
    RF_CMD_line.write_index 	    = 0;

    if(RF_CMD_line.buffer_size != 0)
    {
        memset((void *)RF_CMD_line.p_buffer, 0, sizeof(RF_CMD_line.p_buffer));
    }

    UART_Send_String(&RS232_UART, "GPC FIRMWARE V1.0.0\n");
    UART_Send_String(&RS232_UART, "> ");
    //CMD_send_splash(&RS232_UART);

    UART_Send_Char(&RF_UART, 'B');

    // for (uint8_t i = 0; i < 10; i++)
    // {
    //     HB_sequence_array[i] = HB_sequence_default;
    // }
}

/* :::::::::: CMD Line Task ::::::::::::: */
void RS232_CMD_Line_Task(void*)
{
    uint8_t time_out;

    for(time_out = 50; (!RX_BUFFER_EMPTY(&RS232_UART)) && (time_out != 0); time_out--)
    {
        RS232_CMD_line.RX_char = UART_Get_Char(&RS232_UART);

        if (g_RS232_CMD_line_return == CMDLINE_IS_PROCESSING)
        {
            if (is_warned_user == false)
            {
                UART_Send_String(&RS232_UART, "> ");
            }

			UART_Send_String
            (
                &RS232_UART,
				"COMMAND IS BEING PROCESSED, PLEASE WAIT\n"
            );
			UART_Send_String(&RS232_UART, "> ");
			is_warned_user = true;

            break;
		}
        
        if(((RS232_CMD_line.RX_char == 8) || (RS232_CMD_line.RX_char == 127)))
        {
            if (RS232_CMD_line.write_index == 0)
                break;

            RS232_CMD_line.write_index--;
            UART_Send_Char(&RS232_UART, RS232_CMD_line.RX_char);
            break;
        }

        UART_Send_Char(&RS232_UART, RS232_CMD_line.RX_char);

        if (is_streaming_enable == true)
        {
            char buffer_temp[] = "SET_AUTO_ACCEL 0";
            CMD_line_handle = &RS232_UART;
            g_RS232_CMD_line_return = CmdLineProcess(buffer_temp);

            if (g_RS232_CMD_line_return == CMDLINE_NO_RESPONSE)
            {
                UART_Send_String(&RS232_UART, "\033[1;1H");
            }
            else
            {
                UART_Send_String(&RS232_UART, "> ");
                UART_Printf(&RS232_UART, ErrorCode[g_RS232_CMD_line_return]);
            }

            UART_Send_String(&RS232_UART, "> ");
            return;
        }

        if((RS232_CMD_line.RX_char == '\r') || (RS232_CMD_line.RX_char == '\n'))
        {
            if(RS232_CMD_line.write_index > 0)
            {
                // Add a NUL char at the end of the CMD
                RS232_CMD_line.p_buffer[RS232_CMD_line.write_index] = 0;
                // RS232_CMD_line.write_index++;

                CMD_line_handle   = &RS232_UART;
                g_RS232_CMD_line_return = CmdLineProcess(RS232_CMD_line.p_buffer);

                if (g_RS232_CMD_line_return == CMDLINE_IS_PROCESSING)
                {
					return;
				}

                RS232_CMD_line.write_index = 0;

                if (g_RS232_CMD_line_return == CMDLINE_NO_RESPONSE)
                {
					UART_Send_String(&RS232_UART, "\033[1;1H");
				}
                else
                {
					UART_Send_String(&RS232_UART, "> ");
					UART_Printf(&RS232_UART, ErrorCode[g_RS232_CMD_line_return]);
				}

                UART_Send_String(&RS232_UART, "> ");
            }
            else
            {
                UART_Send_String(&RS232_UART, "> ");
            }
        }
        else
        {
            RS232_CMD_line.p_buffer[RS232_CMD_line.write_index] = RS232_CMD_line.RX_char;
            RS232_CMD_line.write_index++;

            if (RS232_CMD_line.write_index > RS232_CMD_line.buffer_size)
            {
                // SDKLFJSDFKS
                // > CMD too long!
                // > 
                UART_Send_String(&RS232_UART, "\n> CMD too long!\n> ");
                //RS232_CMD_line.write_index = RS232_CMD_line.read_index;
                RS232_CMD_line.write_index    = 0;
            }
        }
    }

    if (g_RS232_CMD_line_return == CMDLINE_IS_PROCESSING)
    {
		g_RS232_CMD_line_return = CmdLineProcess(RS232_CMD_line.p_buffer);

		if (g_RS232_CMD_line_return == CMDLINE_IS_PROCESSING)
        {
			return;
		}

		RS232_CMD_line.write_index = 0;

		UART_Send_String(&RS232_UART, "> ");
		UART_Printf(&RS232_UART, ErrorCode[g_RS232_CMD_line_return]);
		UART_Send_String(&RS232_UART, "> ");

		if (is_warned_user == true)
        {
			UART_Send_String
            (
                &RS232_UART,
			    "FINISH PROCESS COMMAND, PLEASE CONTINUE\n"
            );

			UART_Send_String(&RS232_UART, "> ");
			is_warned_user = false;
		}

	}
}

void RF_CMD_Line_Task(void*)
{
    uint8_t time_out;

    for(time_out = 50; (!RX_BUFFER_EMPTY(&RF_UART)) && (time_out != 0); time_out--)
    {
        RF_CMD_line.RX_char = UART_Get_Char(&RF_UART);

        if (g_RF_CMD_line_return == CMDLINE_IS_PROCESSING)
        {
            if (is_warned_user == false)
            {
                UART_Send_String(&RF_UART, "> ");
            }
            
			UART_Send_String
            (
                &RF_UART,
				"COMMAND IS BEING PROCESSED, PLEASE WAIT\n"
            );
			UART_Send_String(&RF_UART, "> ");
			is_warned_user = true;

            break;
		}
        
        if(((RF_CMD_line.RX_char == 8) || (RF_CMD_line.RX_char == 127)))
        {
            if (RF_CMD_line.write_index == 0)
                break;

            RF_CMD_line.write_index--;
            UART_Send_Char(&RF_UART, RF_CMD_line.RX_char);
            break;
        }

        UART_Send_Char(&RF_UART, RF_CMD_line.RX_char);

        if (is_streaming_enable == true)
        {
            char buffer_temp[] = "SET_AUTO_ACCEL 0";
            CMD_line_handle = &RF_UART;
            g_RF_CMD_line_return = CmdLineProcess(buffer_temp);

            if (g_RF_CMD_line_return == CMDLINE_NO_RESPONSE)
            {
                UART_Send_String(&RF_UART, "\033[1;1H");
            }
            else
            {
                UART_Send_String(&RF_UART, "> ");
                UART_Printf(&RF_UART, ErrorCode[g_RF_CMD_line_return]);
            }

            UART_Send_String(&RF_UART, "> ");
            return;
        }

        if((RF_CMD_line.RX_char == '\r') || (RF_CMD_line.RX_char == '\n'))
        {
            if(RF_CMD_line.write_index > 0)
            {
                // Add a NUL char at the end of the CMD
                RF_CMD_line.p_buffer[RF_CMD_line.write_index] = 0;
                //RF_CMD_line.write_index++;

                CMD_line_handle   = &RF_UART;
                g_RF_CMD_line_return = CmdLineProcess(RF_CMD_line.p_buffer);

                if (g_RF_CMD_line_return == CMDLINE_IS_PROCESSING)
                {
					return;
				}

                RF_CMD_line.write_index = 0;

				if (g_RF_CMD_line_return == CMDLINE_NO_RESPONSE)
                {
					UART_Send_String(&RF_UART, "\033[1;1H");
				}
                else
                {
					UART_Send_String(&RF_UART, "> ");
					UART_Printf(&RF_UART, ErrorCode[g_RF_CMD_line_return]);
				}

                UART_Send_String(&RF_UART, "> ");
            }
            else
            {
                UART_Send_String(&RF_UART, "> ");
            }
        }
        else
        {
            RF_CMD_line.p_buffer[RF_CMD_line.write_index] = RF_CMD_line.RX_char;
            RF_CMD_line.write_index++;

            if (RF_CMD_line.write_index > RF_CMD_line.buffer_size)
            {
                // SDKLFJSDFKS
                // > CMD too long!
                // > 
                UART_Send_String(&RF_UART, "\n> CMD too long!\n> ");
                //RF_CMD_line.write_index = RF_CMD_line.read_index;
                RF_CMD_line.write_index    = 0;
            }
        }
    }

    if (g_RF_CMD_line_return == CMDLINE_IS_PROCESSING)
    {
		g_RF_CMD_line_return = CmdLineProcess(RF_CMD_line.p_buffer);

		if (g_RF_CMD_line_return == CMDLINE_IS_PROCESSING)
        {
			return;
		}

		RF_CMD_line.write_index = 0;

		UART_Send_String(&RF_UART, "> ");
		UART_Printf(&RF_UART, ErrorCode[g_RF_CMD_line_return]);
		UART_Send_String(&RF_UART, "> ");

		if (is_warned_user == true)
        {
			UART_Send_String
            (
                &RF_UART,
			    "FINISH PROCESS COMMAND, PLEASE CONTINUE\n"
            );

			UART_Send_String(&RF_UART, "> ");
			is_warned_user = false;
		}

	}
}

/* :::::::::: IRQ Handler ::::::::::::: */
void RS232_IRQHandler(void)
{
    if(LL_USART_IsActiveFlag_TXE(RS232_UART.handle) == true)
    {
        if(TX_BUFFER_EMPTY(&RS232_UART))
        {
            // Buffer empty, so disable interrupts
            LL_USART_DisableIT_TXE(RS232_UART.handle);
        }
        else
        {
            // There is more data in the output buffer. Send the next byte
            UART_Prime_Transmit(&RS232_UART);
        }
    }

    if(LL_USART_IsActiveFlag_RXNE(RS232_UART.handle) == true)
    {
        RS232_UART.RX_irq_char = LL_USART_ReceiveData8(RS232_UART.handle);

        // NOTE: On win 10, default PUTTY when hit enter only send back '\r',
        // while on default HERCULES when hit enter send '\r\n' in that order.
        // The code bellow is modified so that it can work on PUTTY and HERCULES.
        if((!RX_BUFFER_FULL(&RS232_UART)) && (RS232_UART.RX_irq_char != '\n'))
        {
            if (RS232_UART.RX_irq_char == '\r')
            {
                RS232_UART.p_RX_buffer[RS232_UART.RX_write_index] = '\n';
                ADVANCE_RX_WRITE_INDEX(&RS232_UART);
            }
            else
            {
                RS232_UART.p_RX_buffer[RS232_UART.RX_write_index] = RS232_UART.RX_irq_char;
                ADVANCE_RX_WRITE_INDEX(&RS232_UART);
            }
        }
    }
}

void RF_IRQHandler(void)
{
    if(LL_USART_IsActiveFlag_TXE(RF_UART.handle) == true)
    {
        if(TX_BUFFER_EMPTY(&RF_UART))
        {
            // Buffer empty, so disable interrupts
            LL_USART_DisableIT_TXE(RF_UART.handle);
        }
        else
        {
            // There is more data in the output buffer. Send the next byte
            UART_Prime_Transmit(&RF_UART);
        }
    }

    if(LL_USART_IsActiveFlag_RXNE(RF_UART.handle) == true)
    {
        RF_UART.RX_irq_char = LL_USART_ReceiveData8(RF_UART.handle);

        // NOTE: On win 10, default PUTTY when hit enter only send back '\r',
        // while on default HERCULES when hit enter send '\r\n' in that order.
        // The code bellow is modified so that it can work on PUTTY and HERCULES.
        if((!RX_BUFFER_FULL(&RF_UART)) && (RF_UART.RX_irq_char != '\n'))
        {
            if (RF_UART.RX_irq_char == '\r')
            {
                RF_UART.p_RX_buffer[RF_UART.RX_write_index] = '\n';
                ADVANCE_RX_WRITE_INDEX(&RF_UART);
            }
            else
            {
                RF_UART.p_RX_buffer[RF_UART.RX_write_index] = RF_UART.RX_irq_char;
                ADVANCE_RX_WRITE_INDEX(&RF_UART);
            }
        }
    }
}



static void CMD_send_splash(uart_stdio_typedef* p_uart)
{
    for(uint8_t i = 0 ; i < 21 ; i++)
    {
		UART_Send_String(p_uart, &SPLASH[i][0]);
	}
	UART_Send_String(p_uart, "> ");
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ End of the program ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
