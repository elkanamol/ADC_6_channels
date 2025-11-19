/**
 ******************************************************************************
 * @file    adc_conversions.c
 * @brief   Implementation of single-channel polling-based ADC conversions
 * @date    November 19, 2025
 * @author  Elkana Molson
 ******************************************************************************
 */

#include "adc.h"
#include "main.h"
#include "adc_conversions.h"

/* Private defines -----------------------------------------------------------*/
#define ADC_POLL_TIMEOUT_MS 10

/* External variables --------------------------------------------------------*/
extern ADC_HandleTypeDef hadc1;
extern volatile uint32_t raw_LISXXXALH[ADC_CONVERSIONS_CHANNEL_COUNT];

/* Private types -------------------------------------------------------------*/

/* Error markers for raw_LISXXXALH[] array (values > 4095) */
typedef enum {
  ADC_ERROR_INVALID_CHANNEL = 0xFFFF,
  ADC_ERROR_CONFIG = 0xFFFE,
  ADC_ERROR_START = 0xFFFD,
  ADC_ERROR_TIMEOUT = 0xFFFC
} ADC_ErrorTypeDef;

/* Private variables ---------------------------------------------------------*/

/* Error tracking */
static ADC_ErrorInfo_t adc_errors = {.total_errors = 0,
                                     .last_error_status = HAL_OK,
                                     .last_failed_channel = 0xFF};

/* ADC channel configurations (C99 designated initializers) */
static ADC_ChannelConfTypeDef sConfig[ADC_CONVERSIONS_CHANNEL_COUNT] = {
    [0] = {.Channel = ADC_CHANNEL_0,
           .Rank = ADC_REGULAR_RANK_1,
           .SamplingTime = ADC_SAMPLETIME_15CYCLES,
           .Offset = 0},
    [1] = {.Channel = ADC_CHANNEL_1,
           .Rank = ADC_REGULAR_RANK_1,
           .SamplingTime = ADC_SAMPLETIME_15CYCLES,
           .Offset = 0},
    [2] = {.Channel = ADC_CHANNEL_2,
           .Rank = ADC_REGULAR_RANK_1,
           .SamplingTime = ADC_SAMPLETIME_15CYCLES,
           .Offset = 0},
    [3] = {.Channel = ADC_CHANNEL_3,
           .Rank = ADC_REGULAR_RANK_1,
           .SamplingTime = ADC_SAMPLETIME_15CYCLES,
           .Offset = 0},
    [4] = {.Channel = ADC_CHANNEL_4,
           .Rank = ADC_REGULAR_RANK_1,
           .SamplingTime = ADC_SAMPLETIME_15CYCLES,
           .Offset = 0},
    [5] = {.Channel = ADC_CHANNEL_5,
           .Rank = ADC_REGULAR_RANK_1,
           .SamplingTime = ADC_SAMPLETIME_15CYCLES,
           .Offset = 0}};

/* Public functions ----------------------------------------------------------*/

void analogSensor_operation(uint8_t snsrID) {
  HAL_StatusTypeDef status;

  if (snsrID >= ADC_CONVERSIONS_CHANNEL_COUNT) {
    adc_errors.total_errors++;
    adc_errors.last_error_status = HAL_ERROR;
    adc_errors.last_failed_channel = snsrID;
    return;
  }

  status = HAL_ADC_ConfigChannel(&hadc1, &sConfig[snsrID]);
  if (status != HAL_OK) {
    adc_errors.total_errors++;
    adc_errors.last_error_status = status;
    adc_errors.last_failed_channel = snsrID;
    raw_LISXXXALH[snsrID] = ADC_ERROR_CONFIG;
    return;
  }

  status = HAL_ADC_Start(&hadc1);
  if (status != HAL_OK) {
    raw_LISXXXALH[snsrID] = ADC_ERROR_START;
    adc_errors.total_errors++;
    adc_errors.last_error_status = status;
    adc_errors.last_failed_channel = snsrID;
    return;
  }

  status = HAL_ADC_PollForConversion(&hadc1, ADC_POLL_TIMEOUT_MS);
  if (status != HAL_OK) {
    raw_LISXXXALH[snsrID] = ADC_ERROR_TIMEOUT;
    adc_errors.total_errors++;
    adc_errors.last_error_status = status;
    adc_errors.last_failed_channel = snsrID;
    HAL_ADC_Stop(&hadc1);
    return;
  }

  raw_LISXXXALH[snsrID] = HAL_ADC_GetValue(&hadc1);
  HAL_ADC_Stop(&hadc1);
}

void analogSensor_operation_all_channels(uint8_t total_channels) {
  if (total_channels > ADC_CONVERSIONS_CHANNEL_COUNT) {
    total_channels = ADC_CONVERSIONS_CHANNEL_COUNT;
  }
  for (uint8_t i = 0; i < total_channels; i++) {
    analogSensor_operation(i);
  }
}

uint32_t analogSensor_getErrorCount(void) { return adc_errors.total_errors; }

HAL_StatusTypeDef analogSensor_getErrors(ADC_ErrorInfo_t *error_info) {
  if (error_info == NULL) {
    return HAL_ERROR;
  }
  *error_info = adc_errors;
  return HAL_OK;
}

void analogSensor_resetErrors(void) {
  adc_errors.total_errors = 0;
  adc_errors.last_error_status = HAL_OK;
  adc_errors.last_failed_channel = 0xFF;
}
