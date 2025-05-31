// APP HEADER //
#include "app.h"

#define MY_GPIO_H_
#include "stm32f4_header.h"

static void Status_Led(void*);

#define         SCHEDULER_TASK_COUNT  9
uint32_t        g_ui32SchedulerNumTasks = SCHEDULER_TASK_COUNT;
tSchedulerTask 	g_psSchedulerTable[SCHEDULER_TASK_COUNT] =
                {
                        {
                                &Cap_Controller_Charge_Task,
                                (void *) 0,
                                2,                          //call every 248us
                                0,			                //count from start
                                true		                //is active
                        },
                        {
                                &Cap_Controller_Monitor_Task,
                                (void *) 0,
                                10,                        //call every 248us
                                0,			                //count from start
                                true		                //is active
                        },
                        {
                                &Sensor_Read_Task,
                                (void *) 0,
                                10,                        //call every 248us
                                0,			                //count from start
                                true		                //is active
                        },
                        {
                                &Impedance_Task,
                                (void *) 0,
                                10,                         //call every 248us
                                0,			                //count from start
                                true		                //is active
                        },
                        {
                                &RS232_CMD_Line_Task,
                                (void *) 0,
                                20,                         //call every 500us
                                0,                          //count from start
                                true                        //is active
                        },
                        {
                                &RF_CMD_Line_Task,
                                (void *) 0,
                                20,                         //call every 500us
                                0,                          //count from start
                                true                        //is active
                        },
                        {
                                &FSP_Line_Task,
                                (void *) 0,
                                20,                          //call every 500us
                                0,                          //count from start
                                true                        //is active
                        },
                        {
                                &Calib_Task,
                                (void *) 0,
                                50,                         //call every 500us
                                0,                          //count from start
                                false                       //is active
                        },
                        {
                                &Status_Led,
                                (void *) 0,
                                10000,                      //call every 500us
                                0,                          //count from start
                                true                        //is active
                        },
                };

void App_Main(void)
{   
    // STM32F030CCT6 @ 36MHz, 
    // can run scheduler tick max @ 100us.
    SchedulerInit(10000);

    Cap_Controller_Init();
    Sensor_Read_Init();
    Impedance_Task_Init();
    CMD_Line_Task_Init();
    FSP_Line_Task_Init();

    while (1)
    {
        SchedulerRun();
    }
}

static void Status_Led(void*)
{
    LL_GPIO_TogglePin(DEBUG_LED_PORT, DEBUG_LED_PIN);
}
