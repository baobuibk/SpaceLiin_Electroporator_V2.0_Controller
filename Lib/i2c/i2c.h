#ifndef _I2C_H_

#define _I2C_H_

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Include ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include <stdint.h>

#define MY_CORE_H_
#define MY_I2C_H_
#include "stm32f4_header.h"

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Defines ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
typedef enum
{
	I2C_WAIT,
	I2C_OK,
	I2C_FAIL,
} i2c_result_t;

typedef struct
{
	volatile uint8_t dev_address;
	volatile uint8_t mem_address;
	volatile uint8_t write_or_read;

	volatile uint8_t *p_dev_buffer;
	volatile uint8_t dev_buffer_size;
	volatile bool*   p_is_complete;
} I2C_data_t;

typedef struct _i2c_stdio_typedef
{
    I2C_TypeDef*            handle;
    IRQn_Type		        irqn;

	volatile uint8_t 		irqn_stage;

    I2C_data_t*        		p_request_buffer;

    volatile uint16_t       write_index;
    volatile uint16_t       read_index;
             uint16_t       buffer_size;

    volatile bool           is_disable;

} i2c_stdio_typedef;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Enum ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Struct ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Class ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
void I2C_Init
(
    i2c_stdio_typedef* p_i2c,
    I2C_TypeDef* _handle,
    IRQn_Type _irqn,
    I2C_data_t* _p_request_buffer,
    uint16_t _size
);

/* :::::::::: I2C Command :::::::: */
void I2C_Mem_Write_IT
(
	i2c_stdio_typedef* p_i2c,
    uint8_t devAddress,
	uint8_t memAddress,
    uint8_t *p_data,
    uint8_t size,
    bool* p_is_complete
);

void I2C_Mem_Read_IT
(
	i2c_stdio_typedef* p_i2c,
    uint8_t devAddress,
	uint8_t memAddress,
    uint8_t *p_data,
    uint8_t size,
    bool* p_is_complete
);

bool Is_I2C_Write_Complete(bool* p_is_complete);

bool Is_I2C_Read_Complete(bool* p_is_complete);

uint8_t     I2C_is_buffer_full(volatile uint16_t *pui16Read,
                volatile uint16_t *pui16Write, uint16_t ui16Size);

uint8_t     I2C_is_buffer_empty(volatile uint16_t *pui16Read,
                volatile uint16_t *pui16Write);

void I2C_Add_to_request_buffer
(
    i2c_stdio_typedef* p_i2c,
    uint8_t devAddress,
	uint8_t memAddress,
    uint8_t write_or_read,
    uint8_t *p_data,
    uint8_t size,
    bool* p_is_complete
);

uint16_t    I2C_get_buffer_count(volatile uint16_t *pui16Read,
                volatile uint16_t *pui16Write, uint16_t ui16Size);

uint16_t    I2C_advance_buffer_index(volatile uint16_t* pui16Index, uint16_t ui16Size);

void        I2C_Prime_Transmit(i2c_stdio_typedef* p_i2c);

/* :::::::::: IRQ Handler ::::::::::::: */
void I2C_EV_IRQHandler(i2c_stdio_typedef* p_i2c);

//*****************************************************************************
//
// Macros to determine number of free and used bytes in the transmit buffer.
//
//*****************************************************************************
#define I2C_REQUEST_BUFFER_SIZE(p_i2c)          ((p_i2c)->buffer_size)

#define I2C_REQUEST_BUFFER_USED(p_i2c)          (I2C_get_buffer_count(&(p_i2c)->read_index,  \
                                                        &(p_i2c)->write_index, \
                                                        (p_i2c)->buffer_size))

#define I2C_REQUEST_BUFFER_FREE(p_i2c)          (I2C_REQUEST_BUFFER_SIZE(p_i2c) - I2C_REQUEST_BUFFER_USED(p_i2c))

#define I2C_REQUEST_BUFFER_EMPTY(p_i2c)         (I2C_is_buffer_empty(&(p_i2c)->read_index,   \
                                                       &(p_i2c)->write_index))

#define I2C_REQUEST_BUFFER_FULL(p_i2c)          (I2C_is_buffer_full(&(p_i2c)->read_index,  \
                                                      &(p_i2c)->write_index, \
                                                      (p_i2c)->buffer_size))

#define I2C_ADVANCE_REQUEST_WRITE_INDEX(p_i2c)  (I2C_advance_buffer_index(&(p_i2c)->write_index, \
                                                            (p_i2c)->buffer_size))

#define I2C_ADVANCE_REQUEST_READ_INDEX(p_i2c)   (I2C_advance_buffer_index(&(p_i2c)->read_index, \
                                                            (p_i2c)->buffer_size))

#endif //_I2C_H_
