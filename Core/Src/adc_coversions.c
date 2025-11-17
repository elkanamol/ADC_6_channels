//#include "LISXXXALH.h"
#include "adc.h"
#include "main.h"

// foward declarations
void ADC_Select_CH1(void);
void ADC_Select_CH2(void);
void ADC_Select_CH3(void);
void ADC_Select_CH4(void);
void ADC_Select_CH5(void);
void ADC_Select_CH6(void);


/* Variables */
static ADC_ChannelConfTypeDef _sConfig = {.Rank = ADC_REGULAR_RANK_1,
										  .SamplingTime = ADC_SAMPLETIME_5CYCLE, 
										  .SingleDiff = ADC_SINGLE_ENDED,
										  .OffsetNumber = ADC_OFFSET_NONE, 
										  .Offset = 0}; // 12.11.2024
/* Functions */
void analogSensor_operation(uint8_t snsrID)
{
	if (snsrID == 0) {
		ADC_Select_CH1();
	} else if (snsrID == 1) {
		ADC_Select_CH2();
	} else if (snsrID == 2) {
		ADC_Select_CH3();
	} else if (snsrID == 3) {
		ADC_Select_CH4();
	} else if (snsrID == 4) {
		ADC_Select_CH5();
	} else 				    {
		ADC_Select_CH6();
	}
	HAL_ADC_ConfigChannel(&hadc1, &_sConfig); // 12.11.2024

	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, 10);
	raw_LISXXXALH[snsrID] = HAL_ADC_GetValue(&hadc1);
//	if (snsrID == 4) raw_LISXXXALH[snsrID] = 60000;// 16.12.2023 - tmp debug to remove (simulate 'sensor-cut' on sensor 6)
	HAL_ADC_Stop(&hadc1);
}

void ADC_Select_CH1()
{
//	  ADC_ChannelConfTypeDef sConfig = {0};
	  _sConfig.Channel = ADC_CHANNEL_1;
//	  sConfig.Rank = ADC_REGULAR_RANK_1;
//	  sConfig.SamplingTime = ADC_SAMPLETIME_5CYCLE;
//	  sConfig.SingleDiff = ADC_SINGLE_ENDED;
//	  sConfig.OffsetNumber = ADC_OFFSET_NONE;
//	  sConfig.Offset = 0;
//	  if (HAL_ADC_ConfigChannel(&hadc1, &_sConfig) != HAL_OK)
//	  {
//	//	    Error_Handler();
//	//		__disable_irq();
//	//		while (1) {
//	//		}
//	  }
}

void ADC_Select_CH2()
{
//	  ADC_ChannelConfTypeDef sConfig = {0};
	  _sConfig.Channel = ADC_CHANNEL_2;
//	  sConfig.Rank = ADC_REGULAR_RANK_1;
//	  sConfig.SamplingTime = ADC_SAMPLETIME_5CYCLE;
//	  sConfig.SingleDiff = ADC_SINGLE_ENDED;
//	  sConfig.OffsetNumber = ADC_OFFSET_NONE;
//	  sConfig.Offset = 0;
//	  if (HAL_ADC_ConfigChannel(&hadc1, &_sConfig) != HAL_OK)
//	  {
////		  //	    Error_Handler();
////		  		__disable_irq();
////		  		while (1) {
////		  		}
//	  }
}

void ADC_Select_CH3()
{
//	  ADC_ChannelConfTypeDef sConfig = {0};
	  _sConfig.Channel = ADC_CHANNEL_3;
//	  sConfig.Rank = ADC_REGULAR_RANK_1;
//	  sConfig.SamplingTime = ADC_SAMPLETIME_5CYCLE;
//	  sConfig.SingleDiff = ADC_SINGLE_ENDED;
//	  sConfig.OffsetNumber = ADC_OFFSET_NONE;
//	  sConfig.Offset = 0;
//	  if (HAL_ADC_ConfigChannel(&hadc1, &_sConfig) != HAL_OK)
//	  {
////		  //	    Error_Handler();
////		  		__disable_irq();
////		  		while (1) {
////		  		}
//	  }
}

void ADC_Select_CH4()
{
//	  ADC_ChannelConfTypeDef sConfig = {0};
	  _sConfig.Channel = ADC_CHANNEL_4;
//	  sConfig.Rank = ADC_REGULAR_RANK_1;
//	  sConfig.SamplingTime = ADC_SAMPLETIME_5CYCLE;
//	  sConfig.SingleDiff = ADC_SINGLE_ENDED;
//	  sConfig.OffsetNumber = ADC_OFFSET_NONE;
//	  sConfig.Offset = 0;
//	  if (HAL_ADC_ConfigChannel(&hadc1, &_sConfig) != HAL_OK)
//	  {
////		  //	    Error_Handler();
////		  		__disable_irq();
////		  		while (1) {
////		  		}
//	  }
}

void ADC_Select_CH5()
{
//	  ADC_ChannelConfTypeDef sConfig = {0};
	  _sConfig.Channel = ADC_CHANNEL_5;
//	  sConfig.Rank = ADC_REGULAR_RANK_1;
//	  sConfig.SamplingTime = ADC_SAMPLETIME_5CYCLE;
//	  sConfig.SingleDiff = ADC_SINGLE_ENDED;
//	  sConfig.OffsetNumber = ADC_OFFSET_NONE;
//	  sConfig.Offset = 0;
//	  if (HAL_ADC_ConfigChannel(&hadc1, &_sConfig) != HAL_OK)
//	  {
////		  //	    Error_Handler();
////		  		__disable_irq();
////		  		while (1) {
////		  		}
//	  }
}

void ADC_Select_CH6()
{
//	  ADC_ChannelConfTypeDef sConfig = {0};
	  _sConfig.Channel = ADC_CHANNEL_6;
//	  sConfig.Rank = ADC_REGULAR_RANK_1;
//	  sConfig.SamplingTime = ADC_SAMPLETIME_5CYCLE;
//	  sConfig.SingleDiff = ADC_SINGLE_ENDED;
//	  sConfig.OffsetNumber = ADC_OFFSET_NONE;
//	  sConfig.Offset = 0;
//	  if (HAL_ADC_ConfigChannel(&hadc1, &_sConfig) != HAL_OK)
//	  {
////		  //	    Error_Handler();
////		  		__disable_irq();
////		  		while (1) {
////		  		}
//	  }
}

