#ifndef PID_H_
#define PID_H_

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Include ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include <stdint.h>

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Defines ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Enum ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Struct ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Class ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* :::::::::: PID Mode ::::::::::::: */
typedef enum
{
	_PID_MODE_MANUAL    = 0,
	_PID_MODE_AUTOMATIC = 1
	
}PIDMode_TypeDef;

/* :::::::::: PID Proportional on Type ::::::::::::: */
typedef enum
{
	_PID_P_ON_M = 0, /* Proportional on Measurement */
	_PID_P_ON_E = 1
	
}PIDPON_TypeDef;

/* :::::::::: PID Control Direction ::::::::::::: */
typedef enum
{
	_PID_CD_DIRECT  = 0,
	_PID_CD_REVERSE = 1
	
}PIDCD_TypeDef;

/* :::::::::: PID Structure ::::::::::::: */
typedef struct
{
	PIDMode_TypeDef PID_Mode;
	PIDPON_TypeDef  PON_Type;
	PIDCD_TypeDef   PID_Direction;

	float          	Kp;
	float          	Ki;
	float          	Kd;

	uint16_t       	*MyInput;
	uint8_t        	*MyOutput;
	uint16_t       	*MySetpoint;

	int16_t         OutputSum;
	int16_t         LastInput;

	int16_t         Output_Min;
	int16_t         Output_Max;
	
}PID_TypeDef;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* :::::::::: PID Init :::::::::: */
void PID_Init(PID_TypeDef *uPID);

/* :::::::::: PID Compute :::::::::: */
bool PID_Compute(PID_TypeDef *uPID);

/* ::::::::::: PID Mode :::::::::::: */
void            PID_SetMode(PID_TypeDef *uPID, PIDMode_TypeDef Mode);
PIDMode_TypeDef PID_GetMode(PID_TypeDef *uPID);

/* :::::::::: PID Limits ::::::::::: */
void PID_SetOutputLimits(PID_TypeDef *uPID, uint16_t Min, uint16_t Max);

/* :::::::::: PID Tunings :::::::::: */
void PID_SetTunings1(PID_TypeDef *uPID, float Kp, float Ki, float Kd);
void PID_SetTunings2(PID_TypeDef *uPID, float Kp, float Ki, float Kd, PIDPON_TypeDef POn);

/* ::::::::: PID Direction ::::::::: */
void          PID_SetControllerDirection(PID_TypeDef *uPID, PIDCD_TypeDef Direction);
PIDCD_TypeDef PID_GetDirection(PID_TypeDef *uPID);

/* ::::::: Get Tunings Param ::::::: */
float PID_GetKp(PID_TypeDef *uPID);
float PID_GetKi(PID_TypeDef *uPID);
float PID_GetKd(PID_TypeDef *uPID);

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ End of the program ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#endif /* PID_H_ */
