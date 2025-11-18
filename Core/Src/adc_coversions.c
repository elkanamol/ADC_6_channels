// #include "LISXXXALH.h" // replaced with adc_conversions.h
#include "adc.h"
#include "main.h"

/* Constants */
#define ADC_CHANNEL_COUNT 6    // now 6 per the example
#define ADC_POLL_TIMEOUT_MS 10 // Timeout for ADC polling in milliseconds

/* Externals */

// from adc.c
extern ADC_HandleTypeDef hadc1; 
// from main.c/LISXXXALH.h
extern volatile uint32_t raw_LISXXXALH[ADC_CHANNEL_COUNT]; 

/* Constants */
static uint32_t last_config_status = 0;
static uint32_t last_start_status = 0;
static uint32_t last_poll_status = 0;
static uint32_t conversion_errors = 0;

typedef enum {
  ADC_ERROR_NONE = 0xFFFF,
  ADC_ERROR_CONFIG = 0xFFFE,
  ADC_ERROR_START = 0xFFFD,
  ADC_ERROR_TIMEOUT = 0xFFFC
} ADC_ErrorTypeDef;

/* Functions */

/**
 * @brief Acquire and process one analog sensor (single ADC channel).
 *
 * @note the ADC must be initialized. Not reentrant / not thread-safe.
 * Error markers in raw_LISXXXALH[]:
 * 0xFFFF = Invalid channel ID,
 * 0xFFFE = Config error,
 * 0xFFFD = Start error,
 * 0xFFFC = Timeout error
 *
 * @param[in] snsrID Channel index (0..5).
 *
 * @note ADC must be initialized. Not reentrant / not thread-safe.
 */
void analogSensor_operation(uint8_t snsrID) {
  if (snsrID >= ADC_CHANNEL_COUNT) { // prevent overflow
    return;
  }

  // (void)HAL_ADC_Stop(&hadc1);
  ADC_ChannelConfTypeDef sConfig = {0};
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES;

  // Map sensor ID to ADC channel
  switch (snsrID) {
  case 0:
    sConfig.Channel = ADC_CHANNEL_0;
    break;
  case 1:
    sConfig.Channel = ADC_CHANNEL_1;
    break;
  case 2:
    sConfig.Channel = ADC_CHANNEL_2;
    break;
  case 3:
    sConfig.Channel = ADC_CHANNEL_3;
    break;
  case 4:
    sConfig.Channel = ADC_CHANNEL_4;
    break;
  case 5:
    sConfig.Channel = ADC_CHANNEL_5;
    break;
  default:
    return;
  }

  // Configure ADC channel
  last_config_status = HAL_ADC_ConfigChannel(&hadc1, &sConfig);
  if (last_config_status != HAL_OK) {
    raw_LISXXXALH[snsrID] = ADC_ERROR_CONFIG;
    conversion_errors++;
    return;
  }

  // Start ADC conversion
  last_start_status = HAL_ADC_Start(&hadc1);
  if (last_start_status != HAL_OK) {
    raw_LISXXXALH[snsrID] = ADC_ERROR_START;
    conversion_errors++;
    return;
  }

  // Poll for conversion completion
  last_poll_status = HAL_ADC_PollForConversion(&hadc1, ADC_POLL_TIMEOUT_MS);
  if (last_poll_status != HAL_OK) {
    raw_LISXXXALH[snsrID] = ADC_ERROR_TIMEOUT;
    conversion_errors++;
    HAL_ADC_Stop(&hadc1);
    return;
  }

  raw_LISXXXALH[snsrID] = HAL_ADC_GetValue(&hadc1);

  HAL_ADC_Stop(&hadc1);
}

void analogSensor_operation_all_channels(uint8_t total_channels) {
  for (uint8_t i = 0; i < total_channels; i++) {
    analogSensor_operation(i);
  }
}

/**
 * Get error count for debugging
 */
uint32_t analogSensor_getErrorCount(void) { return conversion_errors; }

/**s
 * Get last HAL status codes for debugging
 */
void analogSensor_getDebugStatus(uint32_t *config, uint32_t *start,
                                 uint32_t *poll) {
  *config = last_config_status;
  *start = last_start_status;
  *poll = last_poll_status;
}

/**
 * Reset error counter
 */
void analogSensor_resetErrors(void) {
  conversion_errors = 0;
  last_config_status = 0;
  last_start_status = 0;
  last_poll_status = 0;
}
