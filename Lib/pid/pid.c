/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Include~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include <stdint.h>
#include <stdbool.h>

#include "pid.h"
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Defines ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Enum ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Struct ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Class ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Private Types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* :::::::::: PID Init :::::::::: */
void PID_Init(PID_TypeDef *uPID)
{
    /* ~~~~~~~~~~ Set parameter ~~~~~~~~~~ */
    PID_SetOutputLimits(uPID, uPID->Output_Min, uPID->Output_Max);

    PID_SetControllerDirection(uPID, uPID->PID_Direction);

    PID_SetTunings2(uPID, uPID->Kp, uPID->Ki, uPID->Kd, uPID->PON_Type);

	uPID->OutputSum = *uPID->MyOutput;
	uPID->LastInput = *uPID->MyInput;
	
	if (uPID->OutputSum > uPID->Output_Max)
	{
		uPID->OutputSum = uPID->Output_Max;
	}
	else if (uPID->OutputSum < uPID->Output_Min)
	{
		uPID->OutputSum = uPID->Output_Min;
	}
}

/* :::::::::: PID Compute :::::::::: */
bool PID_Compute(PID_TypeDef *uPID)
{

	int16_t input;
	int16_t error;
	int16_t dInput;
	int16_t output;
	
	/* ~~~~~~~~~~ Check PID mode ~~~~~~~~~~ */
	if (!uPID->PID_Mode)
	{
		return false;
	}

	/* ..... Compute all the working error variables ..... */
	input   = *uPID->MyInput;
	error   = *uPID->MySetpoint - input;
	dInput  = (input - uPID->LastInput);
	
	uPID->OutputSum     += (uPID->Ki * error);
	
	/* ..... Add Proportional on Measurement, if P_ON_M is specified ..... */
	if (!uPID->PON_Type)
	{
		uPID->OutputSum -= uPID->Kp * dInput;
	}
	
	if (uPID->OutputSum > uPID->Output_Max)
	{
		uPID->OutputSum = uPID->Output_Max;
	}
	else if (uPID->OutputSum < uPID->Output_Min)
	{
		uPID->OutputSum = uPID->Output_Min;
	}
	else { }
	
	/* ..... Add Proportional on Error, if P_ON_E is specified ..... */
	if (uPID->PON_Type)
	{
		output = uPID->Kp * error;
	}
	else
	{
		output = 0;
	}
	
	/* ..... Compute Rest of PID Output ..... */
	output += uPID->OutputSum - uPID->Kd * dInput;
	
	if (output > uPID->Output_Max)
	{
		output = uPID->Output_Max;
	}
	else if (output < uPID->Output_Min)
	{
		output = uPID->Output_Min;
	}
	else { }
	
	*uPID->MyOutput = output;
    
    return true;
}

/* ::::::::::: PID Mode :::::::::::: */
void PID_SetMode(PID_TypeDef *uPID, PIDMode_TypeDef Mode)
{
	
	uint8_t newAuto = (Mode == _PID_MODE_AUTOMATIC);
	
	/* ~~~~~~~~~~ Initialize the PID ~~~~~~~~~~ */
	if (newAuto && !uPID->PID_Mode)
	{
		PID_Init(uPID);
	}
	
	uPID->PID_Mode = (PIDMode_TypeDef)newAuto;
}

PIDMode_TypeDef PID_GetMode(PID_TypeDef *uPID)
{
	return uPID->PID_Mode ? _PID_MODE_AUTOMATIC : _PID_MODE_MANUAL;
}

/* :::::::::: PID Limits ::::::::::: */
void PID_SetOutputLimits(PID_TypeDef *uPID, uint16_t Min, uint16_t Max)
{
	/* ~~~~~~~~~~ Check value ~~~~~~~~~~ */
	if (Min >= Max)
	{
		return;
	}
	
	uPID->Output_Min = Min;
	uPID->Output_Max = Max;
	
	/* ~~~~~~~~~~ Check PID Mode ~~~~~~~~~~ */
	if (uPID->PID_Mode)
	{
		
		/* ..... Check out value ..... */
		if (*uPID->MyOutput > uPID->Output_Max)
		{
			*uPID->MyOutput = uPID->Output_Max;
		}
		else if (*uPID->MyOutput < uPID->Output_Min)
		{
			*uPID->MyOutput = uPID->Output_Min;
		}
		else { }
		
		/* ..... Check out value ..... */
		if (uPID->OutputSum > uPID->Output_Max)
		{
			uPID->OutputSum = uPID->Output_Max;
		}
		else if (uPID->OutputSum < uPID->Output_Min)
		{
			uPID->OutputSum = uPID->Output_Min;
		}
		else { }
		
	}
	
}

/* :::::::::: PID Tunings :::::::::: */
void PID_SetTunings1(PID_TypeDef *uPID, float Kp, float Ki, float Kd)
{
	PID_SetTunings2(uPID, Kp, Ki, Kd, uPID->PON_Type);
}

void PID_SetTunings2(PID_TypeDef *uPID, float Kp, float Ki, float Kd, PIDPON_TypeDef POn)
{
	/* ~~~~~~~~~~ Check value ~~~~~~~~~~ */
	if (Kp < 0 || Ki < 0 || Kd < 0)
	{
		return;
	}
	
	/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
	uPID->PON_Type  = (PIDPON_TypeDef)(POn == _PID_P_ON_E);

	uPID->Kp = Kp;
	uPID->Ki = Ki;
	uPID->Kd = Kd;
	
	/* ~~~~~~~~ Check direction ~~~~~~~~ */
	if (uPID->PID_Direction == _PID_CD_REVERSE)
	{
		
		uPID->Kp = (0 - uPID->Kp);
		uPID->Ki = (0 - uPID->Ki);
		uPID->Kd = (0 - uPID->Kd);
		
	}
	
}

/* ::::::::: PID Direction ::::::::: */
void PID_SetControllerDirection(PID_TypeDef *uPID, PIDCD_TypeDef Direction)
{
	/* ~~~~~~~~~~ Check parameters ~~~~~~~~~~ */
	if ((uPID->PID_Mode) && (Direction !=uPID->PID_Direction))
	{
		
		uPID->Kp = (0 - uPID->Kp);
		uPID->Ki = (0 - uPID->Ki);
		uPID->Kd = (0 - uPID->Kd);
		
	}
	
	uPID->PID_Direction = Direction;
	
}

PIDCD_TypeDef PID_GetDirection(PID_TypeDef *uPID)
{
	return uPID->PID_Direction;
}

/* ::::::: Get Tunings Param ::::::: */
float PID_GetKp(PID_TypeDef *uPID)
{
	return uPID->Kp;
}

float PID_GetKi(PID_TypeDef *uPID)
{
	return uPID->Ki;
}

float PID_GetKd(PID_TypeDef *uPID)
{
	return uPID->Kd;
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ End of the program ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
