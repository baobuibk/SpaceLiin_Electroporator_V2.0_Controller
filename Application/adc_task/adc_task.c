// /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Include~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
// #include <stdint.h>
// #include <stdbool.h>

// #define MY_ADC_H_
// #include "stm32f4_header.h"

// #include "adc_task.h"

// #include "app.h"

// /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Defines ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
// /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
// /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
// /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Enum ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
// /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Struct ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
// /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Class ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
// /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Private Types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
// /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
// static uint8_t  ADC_channel_index = 0;
// static uint16_t ADC_Value[ADC_CHANNEL_COUNT] = {0};

// static bool is_ADC_read_completed       = false;
// //static bool is_ADC_sequence_completed   = false;

// /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
// /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
// uint16_t g_Feedback_Voltage[ADC_CHANNEL_COUNT] = {0};

// /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
// /* :::::::::: ADC Task Init :::::::: */
// void ADC_Task_Init(uint32_t Sampling_Time)
// {
//     LL_ADC_REG_SetTriggerSource(ADC_FEEDBACK_HANDLE, LL_ADC_REG_TRIG_SOFTWARE);

//     LL_ADC_REG_SetSequencerLength(ADC_FEEDBACK_HANDLE, LL_ADC_REG_SEQ_SCAN_DISABLE);

//     LL_ADC_REG_SetSequencerRanks(ADC_FEEDBACK_HANDLE, LL_ADC_REG_RANK_1, ADC_300V_CHANNEL);

//     LL_ADC_REG_SetSequencerRanks(ADC_FEEDBACK_HANDLE, LL_ADC_REG_RANK_2, ADC_50V_CHANNEL);

//     LL_ADC_REG_SetSequencerDiscont(ADC_FEEDBACK_HANDLE, LL_ADC_REG_SEQ_DISCONT_1RANK);

//     LL_ADC_SetChannelSamplingTime(ADC_FEEDBACK_HANDLE, ADC_300V_CHANNEL, Sampling_Time);

//     LL_ADC_SetChannelSamplingTime(ADC_FEEDBACK_HANDLE, ADC_50V_CHANNEL, Sampling_Time);

//     LL_ADC_EnableIT_EOCS(ADC_FEEDBACK_HANDLE);

//     LL_ADC_REG_SetFlagEndOfConversion(ADC_FEEDBACK_HANDLE, LL_ADC_REG_FLAG_EOC_UNITARY_CONV);

//     LL_ADC_Enable(ADC_FEEDBACK_HANDLE);

//     LL_ADC_REG_StartConversionSWStart(ADC_FEEDBACK_HANDLE);
// }

// /* :::::::::: ADC Task ::::::::::::: */
// //TODO: Áp dụng ring buffer và mạch lọc cho ADC.
// void ADC_Task(void*)
// {
//     if (is_ADC_read_completed == true)
//     {
//         is_ADC_read_completed = false;

//         LL_ADC_REG_StartConversionSWStart(ADC_FEEDBACK_HANDLE);
//     }
// }

// /* :::::::::: ADC Interupt Handler ::::::::::::: */
// void ADC_Task_IRQHandler(void)
// {
//     if(LL_ADC_IsActiveFlag_EOCS(ADC_FEEDBACK_HANDLE) == true)
//     {
//         is_ADC_read_completed = true;
//         LL_ADC_ClearFlag_EOCS(ADC_FEEDBACK_HANDLE);

//         ADC_Value[ADC_channel_index] = LL_ADC_REG_ReadConversionData12(ADC_FEEDBACK_HANDLE);
//         g_Feedback_Voltage[ADC_channel_index] = 
//             __LL_ADC_CALC_DATA_TO_VOLTAGE(3300, ADC_Value[ADC_channel_index], LL_ADC_RESOLUTION_12B);

//         if(ADC_channel_index >= (ADC_CHANNEL_COUNT - 1))
//         {
//             ADC_channel_index = 0;
//         }
//         else
//         {
//             ADC_channel_index = ADC_channel_index + 1;
//         }
//     }
// }

// /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
// /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ End of the program ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
