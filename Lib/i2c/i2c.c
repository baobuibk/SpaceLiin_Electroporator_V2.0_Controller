/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Include~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include <stdbool.h>

#include "i2c.h"

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Defines ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Enum ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Struct ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Class ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Private Types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/**
 * The function `I2CConfig` initializes a I2C configuration with a specified receive buffer size and
 * sets up interrupt-driven reception.
 * 
 * @param uart_p The `uart_p` parameter is a pointer to a structure of type `I2C_HandleTypeDef`, which
 * contains configuration settings for a I2C (Universal Asynchronous Receiver-Transmitter) peripheral.
 * @param rx_buffer_size The `rx_buffer_size` parameter specifies the size of the receive buffer used
 * for storing incoming data in the I2CConfig function. This buffer will be initialized using the
 * QUEUE_Init function to manage incoming data efficiently.
 */
void I2C_Init(  i2c_stdio_typedef* p_i2c, I2C_TypeDef* _handle,
                IRQn_Type _irqn, I2C_data_t* _p_request_buffer,
                uint16_t _size)
{
    p_i2c->handle  = _handle;
    p_i2c->irqn    = _irqn;

    p_i2c->p_request_buffer = _p_request_buffer;

    p_i2c->write_index = 0;
    p_i2c->read_index  = 0;
    p_i2c->buffer_size = _size;

	p_i2c->irqn_stage = 0;
	p_i2c->is_disable = true;

    LL_I2C_Enable(p_i2c->handle);

	LL_I2C_DisableIT_EVT(p_i2c->handle);
	LL_I2C_DisableIT_TX(p_i2c->handle);
    LL_I2C_DisableIT_RX(p_i2c->handle);
}

/* :::::::::: I2C Command :::::::: */
void I2C_Mem_Write_IT(
						i2c_stdio_typedef* p_i2c, uint8_t devAddress,
						uint8_t memAddress, uint8_t *p_data, uint8_t size, bool* p_is_complete)
{
	I2C_Add_to_request_buffer(p_i2c, devAddress, memAddress, 0, p_data, size, p_is_complete);
}

void I2C_Mem_Read_IT(
						i2c_stdio_typedef* p_i2c, uint8_t devAddress,
						uint8_t memAddress, uint8_t *p_data, uint8_t size, bool* p_is_complete)
{
	I2C_Add_to_request_buffer(p_i2c, devAddress, memAddress, 1, p_data, size, p_is_complete);
}

void I2C_Add_to_request_buffer(i2c_stdio_typedef* p_i2c, uint8_t devAddress,
		uint8_t memAddress, uint8_t write_or_read, uint8_t *p_data, uint8_t size, bool* p_is_complete)
{
	if (I2C_REQUEST_BUFFER_FULL(p_i2c))
    {
        return;
    }

	p_i2c->p_request_buffer[p_i2c->write_index].dev_address 	= devAddress;
	p_i2c->p_request_buffer[p_i2c->write_index].mem_address 	= memAddress;
	p_i2c->p_request_buffer[p_i2c->write_index].write_or_read	= write_or_read;

	p_i2c->p_request_buffer[p_i2c->write_index].p_dev_buffer 	= p_data;
	p_i2c->p_request_buffer[p_i2c->write_index].dev_buffer_size = size;
	p_i2c->p_request_buffer[p_i2c->write_index].p_is_complete  	= p_is_complete;

	I2C_ADVANCE_REQUEST_WRITE_INDEX(p_i2c);

	if (p_i2c->is_disable == true)
    {
        //LL_GPIO_ResetOutputPin(p_i2c->cs_port, p_i2c->cs_pin);

        I2C_Prime_Transmit(p_i2c);
    }
}

bool Is_I2C_Write_Complete(bool* p_is_complete)
{
	if (p_is_complete == NULL)
	{
		return 0;
	}
	
	if (*p_is_complete == true)
	{
		*p_is_complete = false;
		return 1;
	}
	
	return 0;
}

bool Is_I2C_Read_Complete(bool* p_is_complete)
{
	if (p_is_complete == NULL)
	{
		return 0;
	}
	
	if (*p_is_complete == true)
	{
		*p_is_complete = false;
		return 1;
	}
	
	return 0;
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
uint8_t I2C_is_buffer_full(volatile uint16_t *pui16Read,
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
uint8_t I2C_is_buffer_empty(volatile uint16_t *pui16Read,
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
uint16_t I2C_get_buffer_count(volatile uint16_t *pui16Read,
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
uint16_t I2C_advance_buffer_index(volatile uint16_t* pui16Index, uint16_t ui16Size)
{
    *pui16Index = (*pui16Index + 1) % ui16Size;

    return(*pui16Index);
}

void I2C_Prime_Transmit(i2c_stdio_typedef* p_i2c)
{
    if (I2C_REQUEST_BUFFER_EMPTY(p_i2c))
    {
        return;
    }
    
    NVIC_DisableIRQ(p_i2c->irqn);

	p_i2c->is_disable = false;

	LL_I2C_EnableIT_EVT(p_i2c->handle);

	if (p_i2c->p_request_buffer[p_i2c->read_index].write_or_read == 0)
	{
		LL_I2C_EnableIT_TX(p_i2c->handle);
	}
	else
	{
		LL_I2C_EnableIT_RX(p_i2c->handle);
	}

    // Generate Start Condition
	LL_I2C_GenerateStartCondition(p_i2c->handle);

    NVIC_EnableIRQ(p_i2c->irqn);
}

void I2C_EV_IRQHandler(i2c_stdio_typedef* p_i2c)
{
	switch (p_i2c->irqn_stage)
	{

	// Stage 0: Send Device Address with Write
	case 0:
	{
		if (LL_I2C_IsActiveFlag_SB(p_i2c->handle) == false)
		{
			return;
		}

		LL_I2C_TransmitData8(p_i2c->handle, p_i2c->p_request_buffer[p_i2c->read_index].dev_address);
		p_i2c->irqn_stage = 1;
		return;
	}

	// Stage 1: Send Memory Address
	case 1:
	{
		if (LL_I2C_IsActiveFlag_ADDR(p_i2c->handle) == false)
		{
			return;
		}

		LL_I2C_ClearFlag_ADDR(p_i2c->handle);
		LL_I2C_TransmitData8(p_i2c->handle, p_i2c->p_request_buffer[p_i2c->read_index].mem_address);
		p_i2c->irqn_stage = 2;
		return;
	}

	case 2: {
		// Stage 2: Send Data for Writing
		if (p_i2c->p_request_buffer[p_i2c->read_index].write_or_read == 0)
		{
			if (LL_I2C_IsActiveFlag_TXE(p_i2c->handle) == false)
			{
				return;
			}

			if (p_i2c->p_request_buffer[p_i2c->read_index].dev_buffer_size > 0)
			{
				LL_I2C_TransmitData8(p_i2c->handle, *p_i2c->p_request_buffer[p_i2c->read_index].p_dev_buffer++);
				p_i2c->p_request_buffer[p_i2c->read_index].dev_buffer_size--;
				return;
			} 

			// if (p_i2c->p_request_buffer[p_i2c->read_index].dev_buffer_size > 0)
			// {
			// 	p_i2c->p_request_buffer[p_i2c->read_index].mem_address += 1;
			// 	p_i2c->irqn_stage = 0;
			// 	LL_I2C_GenerateStartCondition(p_i2c->handle);
			// 	return;
			// }

			*p_i2c->p_request_buffer[p_i2c->read_index].p_is_complete = 1;
			p_i2c->irqn_stage = 0;
			LL_I2C_GenerateStopCondition(p_i2c->handle);
			LL_I2C_DisableIT_TX(p_i2c->handle);
			LL_I2C_DisableIT_EVT(p_i2c->handle);

			I2C_ADVANCE_REQUEST_READ_INDEX(p_i2c);

			if (I2C_REQUEST_BUFFER_EMPTY(p_i2c))
			{
				p_i2c->is_disable = true;
				return;
			}

			I2C_Prime_Transmit(p_i2c);
		}
		// Stage 2: Generate Re-Start for Reading
		else
		{
			if (LL_I2C_IsActiveFlag_TXE(p_i2c->handle) == false)
			{
				return;
			}

			LL_I2C_GenerateStartCondition(p_i2c->handle);
			p_i2c->irqn_stage = 3;
			return;
		}

	}

	// Stage 3: Send Device Address with Read
	case 3:
	{
		if (LL_I2C_IsActiveFlag_SB(p_i2c->handle) == false)
		{
			return;
		}

		LL_I2C_TransmitData8(p_i2c->handle, p_i2c->p_request_buffer[p_i2c->read_index].dev_address + 1);
		p_i2c->irqn_stage = 4;
		return;
	}

	// Stage 4: Check for the read mem addr and start read mem
	case 4:
	{
		if (LL_I2C_IsActiveFlag_ADDR(p_i2c->handle) == false)
		{
			return;
		}

		LL_I2C_ClearFlag_ADDR(p_i2c->handle);

		// Configure NACK for the last byte before receiving it
		if (p_i2c->p_request_buffer[p_i2c->read_index].dev_buffer_size == 1)
		{
			LL_I2C_AcknowledgeNextData(p_i2c->handle, LL_I2C_NACK);
		}
		// else
		// {
		// 	LL_I2C_AcknowledgeNextData(p_i2c->handle, LL_I2C_ACK);
		// }

		p_i2c->irqn_stage = 5;
		return;
	}

	case 5:
	{
		if (LL_I2C_IsActiveFlag_RXNE(p_i2c->handle) == false)
		{
			return;
		}

		// if (p_i2c->p_request_buffer[p_i2c->read_index].dev_buffer_size == 1)
		// {
		// 	LL_I2C_AcknowledgeNextData(p_i2c->handle, LL_I2C_NACK);
		// }

		if (p_i2c->p_request_buffer[p_i2c->read_index].dev_buffer_size > 0)
		{
			*p_i2c->p_request_buffer[p_i2c->read_index].p_dev_buffer++ = LL_I2C_ReceiveData8(p_i2c->handle);
			p_i2c->p_request_buffer[p_i2c->read_index].dev_buffer_size--;
		}

		if (p_i2c->p_request_buffer[p_i2c->read_index].dev_buffer_size > 0)
		{
			p_i2c->p_request_buffer[p_i2c->read_index].mem_address += 1;
			p_i2c->irqn_stage = 0;
			LL_I2C_GenerateStartCondition(p_i2c->handle);
			return;
		}

		*p_i2c->p_request_buffer[p_i2c->read_index].p_is_complete = 1;
		p_i2c->irqn_stage = 0;
		LL_I2C_GenerateStopCondition(p_i2c->handle);
		LL_I2C_DisableIT_RX(p_i2c->handle);
		LL_I2C_DisableIT_EVT(p_i2c->handle);

		I2C_ADVANCE_REQUEST_READ_INDEX(p_i2c);

		if (I2C_REQUEST_BUFFER_EMPTY(p_i2c))
		{
			p_i2c->is_disable = true;
			return;
		}

		I2C_Prime_Transmit(p_i2c);
	}

	default:
		break;
	}
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ End of the program ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
