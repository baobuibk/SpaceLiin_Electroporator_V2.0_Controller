/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Include~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include <string.h>
#include <stdio.h>
#include "app.h"

#include "i2c.h"
#include "sensor_task.h"

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Defines ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Enum ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Struct ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Class ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Private Types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
static uint8_t      is_buffer_full(volatile uint16_t *pui16Read,
                            volatile uint16_t *pui16Write, uint16_t ui16Size);

static uint8_t      is_buffer_empty(volatile uint16_t *pui16Read,
                            volatile uint16_t *pui16Write);

static uint16_t     get_buffer_count(volatile uint16_t *pui16Read,
                            volatile uint16_t *pui16Write, uint16_t ui16Size);

static uint16_t     advance_buffer_index(volatile uint16_t* pui16Index, uint16_t ui16Size);

static uint8_t 		sensor_I2C_error_handle(sensor_request_rb_t* p_current_sensor, i2c_result_t* p_return_value);

static void 		reset_buffer(volatile uint16_t *pui16Read, volatile uint16_t *pui16Write);

//*****************************************************************************
//
// Macros to determine number of free and used bytes in the receive buffer.
//
//*****************************************************************************
#define SENSOR_REQUEST_BUFFER_SIZE(p_sensor)          	((p_sensor)->buffer_size)

#define SENSOR_REQUEST_BUFFER_USED(p_sensor)          	(get_buffer_count(	&(p_sensor)->read_index,  \
                                                                  				&(p_sensor)->write_index, \
                                                                  			 	 (p_sensor)->buffer_size))

#define IS_SENSOR_REQUEST_BUFFER_FREE(p_sensor)           (SENSOR_REQUEST_BUFFER_SIZE(p_sensor) - SENSOR_REQUEST_BUFFER_USED(p_sensor))

#define IS_SENSOR_REQUEST_BUFFER_EMPTY(p_sensor)          (is_buffer_empty(	&(p_sensor)->read_index,   \
                                                                  				&(p_sensor)->write_index))

#define IS_SENSOR_REQUEST_BUFFER_FULL(p_sensor)           (is_buffer_full(	&(p_sensor)->read_index,  \
                                                                 			    &(p_sensor)->write_index, \
                                                                 		 	     (p_sensor)->buffer_size))

#define ADVANCE_SENSOR_REQUEST_WRITE_INDEX(p_sensor) 	    (advance_buffer_index( &(p_sensor)->write_index, \
                                                                      			    (p_sensor)->buffer_size))

#define ADVANCE_SENSOR_REQUEST_READ_INDEX(p_sensor)   	(advance_buffer_index( &(p_sensor)->read_index, \
                                                                      				(p_sensor)->buffer_size))

#define RESET_SENSOR_REQUEST_BUFFER(p_sensor)			(reset_buffer(	&(p_sensor)->read_index,   \
                                                                  				&(p_sensor)->write_index))

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
i2c_stdio_typedef Sensor_I2C;
#define			  SENSOR_DATA_ARRAY_SIZE 2 * 16
I2C_data_t 		  g_sensor_I2C_data_array[SENSOR_DATA_ARRAY_SIZE];

request_buffer_t g_sensor_BMP390_request_buffer[16];
request_buffer_t g_sensor_LSM6DSOX_request_buffer[16];
// request_buffer_t g_sensor_H3LIS331DL_request_buffer[16];

char g_sensor_BMP390_name[] 	= "BMP390";
char g_sensor_LSM6DSOX_name[] 	= "LSM6DSOX";

sensor_request_rb_t Sensor_BMP390_rb;
sensor_request_rb_t Sensor_LSM6DSOX_rb;
// sensor_request_rb_t Sensor_H3LIS331DL_rb;

i2c_stdio_typedef   Onboard_Sensor_I2C;
#define			    ONBOARD_SENSOR_DATA_ARRAY_SIZE 1 * 16
I2C_data_t 		    g_onboard_sensor_I2C_data_array[ONBOARD_SENSOR_DATA_ARRAY_SIZE];

request_buffer_t    g_onboard_sensor_H3LIS331DL_request_buffer[16];

char g_onboard_sensor_H3LIS331DL_name[] = "ONBOARD H3LIS331DL";

sensor_request_rb_t Onboard_Sensor_H3LIS331DL_rb;

#define NUMBER_OF_SENSOR 3
sensor_request_rb_t* Sensor_List[NUMBER_OF_SENSOR] =
{
	&Sensor_BMP390_rb,
	&Sensor_LSM6DSOX_rb,
	&Onboard_Sensor_H3LIS331DL_rb,
};

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
void Sensor_Read_Init(void)
{
	I2C_Init
	(
		&Sensor_I2C,
		SENSOR_I2C_HANDLE,
		SENSOR_I2C_IRQ,
		g_sensor_I2C_data_array,
		SENSOR_DATA_ARRAY_SIZE
	);

	I2C_Init
	(
		&Onboard_Sensor_I2C,
		ONBOARD_SENSOR_I2C_HANDLE,
		ONBOARD_SENSOR_I2C_IRQ,
		g_onboard_sensor_I2C_data_array,
		ONBOARD_SENSOR_DATA_ARRAY_SIZE
	);

	sensor_request_rb_t *p_current_sensor;

	Sensor_BMP390_rb.p_i2c				= &Sensor_I2C;
	Sensor_BMP390_rb.p_request_buffer	= g_sensor_BMP390_request_buffer;
	Sensor_BMP390_rb.buffer_size    	= 16;
	Sensor_BMP390_rb.write_index 		= 0;
	Sensor_BMP390_rb.read_index			= 0;
	Sensor_BMP390_rb.is_init			= 0;
	Sensor_BMP390_rb.p_sensor_name		= g_sensor_BMP390_name;

	Sensor_BMP390_rb.pfn_sensor_function = &Sensor_BMP390;

	for (uint8_t i = 0; i < Sensor_BMP390_rb.buffer_size; i++)
	{
		Sensor_BMP390_rb.p_request_buffer[i].is_requested = false;
		Sensor_BMP390_rb.p_request_buffer[i].request = 0;
	}
	
	Sensor_LSM6DSOX_rb.p_i2c			= &Sensor_I2C;
	Sensor_LSM6DSOX_rb.p_request_buffer	= g_sensor_LSM6DSOX_request_buffer;
	Sensor_LSM6DSOX_rb.buffer_size    	= 16;
	Sensor_LSM6DSOX_rb.write_index 		= 0;
	Sensor_LSM6DSOX_rb.read_index		= 0;
	Sensor_LSM6DSOX_rb.is_init			= 0;
	Sensor_LSM6DSOX_rb.p_sensor_name	= g_sensor_LSM6DSOX_name;

	Sensor_LSM6DSOX_rb.pfn_sensor_function = &Sensor_LSM6DSOX;

	for (uint8_t i = 0; i < Sensor_LSM6DSOX_rb.buffer_size; i++)
	{
		Sensor_LSM6DSOX_rb.p_request_buffer[i].is_requested = false;
		Sensor_LSM6DSOX_rb.p_request_buffer[i].request = 0;
	}

	Onboard_Sensor_H3LIS331DL_rb.p_i2c				= &Onboard_Sensor_I2C;
	Onboard_Sensor_H3LIS331DL_rb.p_request_buffer	= g_onboard_sensor_H3LIS331DL_request_buffer;
	Onboard_Sensor_H3LIS331DL_rb.buffer_size    	= 16;
	Onboard_Sensor_H3LIS331DL_rb.write_index 		= 0;
	Onboard_Sensor_H3LIS331DL_rb.read_index			= 0;
	Onboard_Sensor_H3LIS331DL_rb.is_init			= 0;
	Onboard_Sensor_H3LIS331DL_rb.p_sensor_name		= g_onboard_sensor_H3LIS331DL_name;

	Onboard_Sensor_H3LIS331DL_rb.pfn_sensor_function = &Sensor_H3LIS331DL;

	for (uint8_t i = 0; i < Onboard_Sensor_H3LIS331DL_rb.buffer_size; i++)
	{
		Onboard_Sensor_H3LIS331DL_rb.p_request_buffer[i].is_requested = false;
		Onboard_Sensor_H3LIS331DL_rb.p_request_buffer[i].request = 0;
	}

	for (uint8_t sensor_Idx = 0; sensor_Idx < NUMBER_OF_SENSOR; sensor_Idx++)
	{
		p_current_sensor = Sensor_List[sensor_Idx];

		if (p_current_sensor == NULL)
		{
			return;
		}

		if (IS_SENSOR_REQUEST_BUFFER_FULL(p_current_sensor))
		{
			return;
		}

		p_current_sensor->p_request_buffer[p_current_sensor->write_index].is_requested = true;
		p_current_sensor->p_request_buffer[p_current_sensor->write_index].request_type = SENSOR_INIT_TYPE;
		ADVANCE_SENSOR_REQUEST_WRITE_INDEX(p_current_sensor);
	}
}

void Sensor_Read_Task(void*)
{
    uint8_t sensor_Idx;
    uint8_t current_request;
    sensor_request_rb_t *p_current_sensor;
	uint8_t	is_complete = false;

    // Loop through each sensor in the list
    for (sensor_Idx = 0; sensor_Idx < NUMBER_OF_SENSOR; sensor_Idx++)
    {
        // Get pointer to current sensor entry in the sensor list
        p_current_sensor = Sensor_List[sensor_Idx];

        // Get a pointer to the current request in the buffer
        current_request = p_current_sensor->p_request_buffer[p_current_sensor->read_index].request;

        // Call the function pointer associated with this sensor,
        // passing it the current request, if it's set
        if (p_current_sensor->pfn_sensor_function == NULL)
        {
            continue;
        }

		if (p_current_sensor->p_request_buffer[p_current_sensor->read_index].is_requested == false)
		{
			continue;
		}

		switch (p_current_sensor->p_request_buffer[p_current_sensor->read_index].request_type)
		{
		case SENSOR_READ_TYPE:
			is_complete = p_current_sensor->pfn_sensor_function->read_value(p_current_sensor->p_i2c ,current_request);
			break;
		case SENSOR_INIT_TYPE:
			is_complete = p_current_sensor->pfn_sensor_function->init(p_current_sensor->p_i2c);
			break;
		case SENSOR_SET_FS_TYPE:
			is_complete = p_current_sensor->pfn_sensor_function->set_full_scale(p_current_sensor->p_i2c, current_request);
			break;
		case SENSOR_GET_FS_TYPE:
			is_complete = p_current_sensor->pfn_sensor_function->get_full_scale(p_current_sensor->p_i2c);
			break;

		default:
			break;
		}

		if (sensor_I2C_error_handle(p_current_sensor, &is_complete) == 1)
		{
			return;
		}
		
		// p_current_sensor->p_request_buffer[p_current_sensor->read_index].is_requested = false;

		// If there are no more requests in the buffer, skip to next sensor
		if (IS_SENSOR_REQUEST_BUFFER_EMPTY(p_current_sensor))
		{
			continue;
		}

		// Advance the buffer read index to get the next request
		ADVANCE_SENSOR_REQUEST_READ_INDEX(p_current_sensor);
    }
}

bool Sensor_Read_Value(Sensor_Read_typedef read_type)
{
	sensor_request_rb_t* p_sensor_rb = NULL;
	bool is_set_fs = false;
	bool is_get_fs = false;

	switch (read_type)
    {

	case SENSOR_READ_TEMP:
	case SENSOR_READ_PRESSURE:
	case SENSOR_READ_ALTITUDE:
	case SENSOR_READ_BMP390:
    {
		p_sensor_rb = &Sensor_BMP390_rb;
        break;
	}

	case SENSOR_READ_GYRO:
	case SENSOR_READ_ACCEL:
	case SENSOR_READ_LSM6DSOX:
    {
		p_sensor_rb = &Sensor_LSM6DSOX_rb;
        break;
	}

	// case SENSOR_READ_H3LIS331DL:
	// {
	// 	p_sensor_rb = &Sensor_H3LIS331DL_rb;
	// 	break;
	// }

	case ONBOARD_SENSOR_READ_H3LIS331DL:
	{
		p_sensor_rb = &Onboard_Sensor_H3LIS331DL_rb;
		break;
	}

	case ONBOARD_SENSOR_SET_FS_100G:
	case ONBOARD_SENSOR_SET_FS_200G:
	case ONBOARD_SENSOR_SET_FS_400G:
	{
		is_set_fs = true;
		p_sensor_rb = &Onboard_Sensor_H3LIS331DL_rb;
		break;
	}

	case ONBOARD_SENSOR_GET_FS:
	{
		is_get_fs = true;
		p_sensor_rb = &Onboard_Sensor_H3LIS331DL_rb;
		break;
	}

	default:
		return 0;
	}

	if (p_sensor_rb == NULL)
	{
		return false;
	}

    if (IS_SENSOR_REQUEST_BUFFER_FULL(p_sensor_rb))
	{
		return false;
	}

	if (p_sensor_rb->is_init == false)
	{
		p_sensor_rb->p_request_buffer[p_sensor_rb->write_index].is_requested = true;
		p_sensor_rb->p_request_buffer[p_sensor_rb->write_index].request_type = SENSOR_INIT_TYPE;
		ADVANCE_SENSOR_REQUEST_WRITE_INDEX(p_sensor_rb);
	}
	else if (is_set_fs == true)
	{
		p_sensor_rb->p_request_buffer[p_sensor_rb->write_index].is_requested = true;
		p_sensor_rb->p_request_buffer[p_sensor_rb->write_index].request_type = SENSOR_SET_FS_TYPE;
		p_sensor_rb->p_request_buffer[p_sensor_rb->write_index].request 	 = read_type;
		ADVANCE_SENSOR_REQUEST_WRITE_INDEX(p_sensor_rb);
		return true;
	}
	else if (is_get_fs == true)
	{
		p_sensor_rb->p_request_buffer[p_sensor_rb->write_index].is_requested = true;
		p_sensor_rb->p_request_buffer[p_sensor_rb->write_index].request_type = SENSOR_GET_FS_TYPE;
		ADVANCE_SENSOR_REQUEST_WRITE_INDEX(p_sensor_rb);
		return true;
	}

	p_sensor_rb->p_request_buffer[p_sensor_rb->write_index].is_requested = true;
	p_sensor_rb->p_request_buffer[p_sensor_rb->write_index].request_type = SENSOR_READ_TYPE;
	p_sensor_rb->p_request_buffer[p_sensor_rb->write_index].request 	 = read_type;
	ADVANCE_SENSOR_REQUEST_WRITE_INDEX(p_sensor_rb);
	return true;
}

uint8_t Is_Sensor_Read_Complete(sensor_request_rb_t* p_sensor_rb)
{
	if (p_sensor_rb == NULL)
	{
		return false;
	}

	i2c_result_t return_value = p_sensor_rb->is_complete;
	p_sensor_rb->is_complete = I2C_IS_RUNNING;
	
	return return_value;
}

void Sensor_I2C_IRQHandler(void)
{
	I2C_EV_IRQHandler(&Sensor_I2C);
}

void Onboard_Sensor_I2C_IRQHandler(void)
{
	I2C_EV_IRQHandler(&Onboard_Sensor_I2C);
}

void Sensor_I2C_ER_IRQHandler(void)
{
	I2C_ER_IRQHandler(&Sensor_I2C);
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
static uint8_t sensor_I2C_error_handle(sensor_request_rb_t* p_current_sensor, i2c_result_t* p_return_value)
{
	if (p_return_value == NULL)
	{
		return 1;
	}

	p_current_sensor->is_complete  = *p_return_value;
	i2c_result_t return_value_temp = *p_return_value;
	*p_return_value = false;
	
	// If the current request has been marked as complete
	// update completion flag for that sensor
	if (return_value_temp == I2C_OK)
	{
		 if (p_current_sensor->p_request_buffer[p_current_sensor->read_index].request_type == SENSOR_INIT_TYPE)
		 {
		 	p_current_sensor->is_init = true;

		 	// UART_Printf(CMD_line_handle, "> SENSOR %s INIT SUCCESSFULLY\n", p_current_sensor->p_sensor_name);
		 }

		p_current_sensor->p_request_buffer[p_current_sensor->read_index].is_requested = false;

		return 0;
	}
	else if (return_value_temp == I2C_IS_RUNNING)
	{
		return 1;
	}

	// if (return_value_temp == I2C_ERROR)
	p_current_sensor->is_init = false;

	p_current_sensor->p_request_buffer[p_current_sensor->read_index].is_requested = false;
	
	// if (p_current_sensor->p_request_buffer[p_current_sensor->read_index].request_type == SENSOR_INIT_TYPE)
	// {
	// 	UART_Printf(CMD_line_handle, "> ERROR: SENSOR %s FAIL TO INIT\n", p_current_sensor->p_sensor_name);
	// }

	switch (return_value_temp)
	{

	case I2C_ERROR_SENSOR_NOT_CONNECTED:
	{
		RESET_SENSOR_REQUEST_BUFFER(p_current_sensor);

		p_current_sensor->p_request_buffer[p_current_sensor->read_index].is_requested = false;

		UART_Printf(CMD_line_handle, "> ERROR: SENSOR %s IS NOT RESPONDING ON I2C BUS (NO ACK)\n", p_current_sensor->p_sensor_name);

		return 0;
	}

	case I2C_ERROR_BUS_ERROR:
	{
		RESET_SENSOR_REQUEST_BUFFER(p_current_sensor);

		p_current_sensor->p_request_buffer[p_current_sensor->read_index].is_requested = false;

		UART_Printf(CMD_line_handle, "> ERROR: SENSOR %s I2C BUS ERROR (SIGNAL ERROR OR MISWIRED)\n", p_current_sensor->p_sensor_name);

		return 0;
	}

	case I2C_ERROR_BUS_BUSY:
	{
		RESET_SENSOR_REQUEST_BUFFER(p_current_sensor);

		p_current_sensor->p_request_buffer[p_current_sensor->read_index].is_requested = false;

		UART_Printf(CMD_line_handle, "> ERROR: SENSOR %s I2C BUS CURRENTLY BUSY (SIGNAL ERROR OR MISWIRED)\n", p_current_sensor->p_sensor_name);

		return 0;
	}

	case I2C_FAIL:
	{
		RESET_SENSOR_REQUEST_BUFFER(p_current_sensor);

		UART_Printf(CMD_line_handle, "> ERROR: INTERNAL ERROR, PLEASE RESET\n", p_current_sensor->p_sensor_name);

		return 0;
	}
	
	default:
		break;
	}

	return 1;
}

//*****************************************************************************
//
//! Determines whether the ring buffer whose pointers and size are provided
//! is full or not.
//!
//! \param pui16Read points to the read index for the buffer.
//! \param pui16Write points to the write index for the buffer.
//! \param ui16Size is the size of the buffer in bytes.
//!
//! This function is used to determine whether or not a given ring buffer is
//! full.  The structure of the code is specifically to ensure that we do not
//! see warnings from the compiler related to the order of volatile accesses
//! being undefined.
//!
//! \return Returns \b 1 if the buffer is full or \b 0 otherwise.
//
//*****************************************************************************

static uint8_t is_buffer_full(volatile uint16_t *pui16Read,
             volatile uint16_t *pui16Write, uint16_t ui16Size)
{
    uint16_t ui16Write;
    uint16_t ui16Read;

    ui16Write = *pui16Write;
    ui16Read = *pui16Read;

    return((((ui16Write + 1) % ui16Size) == ui16Read) ? 1 : 0);
}


//*****************************************************************************
//
//! Determines whether the ring buffer whose pointers and size are provided
//! is empty or not.
//!
//! \param pui16Read points to the read index for the buffer.
//! \param pui16Write points to the write index for the buffer.
//!
//! This function is used to determine whether or not a given ring buffer is
//! empty.  The structure of the code is specifically to ensure that we do not
//! see warnings from the compiler related to the order of volatile accesses
//! being undefined.
//!
//! \return Returns \b 1 if the buffer is empty or \b 0 otherwise.
//
//*****************************************************************************

static uint8_t is_buffer_empty(volatile uint16_t *pui16Read,
              volatile uint16_t *pui16Write)
{
    uint16_t ui16Write;
    uint16_t ui16Read;

    ui16Write = *pui16Write;
    ui16Read = *pui16Read;

    return((ui16Read == ui16Write) ? 1 : 0);
}


//*****************************************************************************
//
//! Determines the number of bytes of data contained in a ring buffer.
//!
//! \param pui16Read points to the read index for the buffer.
//! \param pui16Write points to the write index for the buffer.
//! \param ui16Size is the size of the buffer in bytes.
//!
//! This function is used to determine how many bytes of data a given ring
//! buffer currently contains.  The structure of the code is specifically to
//! ensure that we do not see warnings from the compiler related to the order
//! of volatile accesses being undefined.
//!
//! \return Returns the number of bytes of data currently in the buffer.
//
//*****************************************************************************

static uint16_t get_buffer_count(volatile uint16_t *pui16Read,
               volatile uint16_t *pui16Write, uint16_t ui16Size)
{
    uint16_t ui16Write;
    uint16_t ui16Read;

    ui16Write = *pui16Write;
    ui16Read = *pui16Read;

    return((ui16Write >= ui16Read) ? (ui16Write - ui16Read) :
           (ui16Size - (ui16Read - ui16Write)));
}

//*****************************************************************************
//
//! Adding +1 to the index
//!
//! \param pui16Read points to the read index for the buffer.
//! \param pui16Write points to the write index for the buffer.
//! \param ui16Size is the size of the buffer in bytes.
//!
//! This function is use to advance the index by 1, if the index
//! already hit the uart size then it will reset back to 0.
//!
//! \return Returns the number of bytes of data currently in the buffer.
//
//*****************************************************************************

static uint16_t advance_buffer_index(volatile uint16_t* pui16Index, uint16_t ui16Size)
{
    *pui16Index = (*pui16Index + 1) % ui16Size;

    return(*pui16Index);
}

//*****************************************************************************
//
//! Resets the buffer by aligning the read index to the write index.
//!
//! \param pui16Read points to the read index of the buffer.
//! \param pui16Write points to the write index of the buffer.
//!
//! This function sets the read index equal to the write index,
//! effectively marking the buffer as empty without modifying the
//! underlying data.
//!
//! \return None.
//
//*****************************************************************************
static void reset_buffer(volatile uint16_t *pui16Read,
              volatile uint16_t *pui16Write)
{
    uint16_t ui16Write;

    ui16Write  = *pui16Write;
    *pui16Read = ui16Write;
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ End of the program ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
