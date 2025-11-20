
/**
 ******************************************************************************
 * @file    adc_conversions.h
 * @brief   Single-channel polling-based ADC conversion API
 * @date    November 16, 2025
 * @author  Elkana Molson
 ******************************************************************************
 * @attention
 *
 * This module provides simple, blocking ADC conversions using polling mode.
 * Each channel is configured and read individually.
 * @note This code assumes that the ADC peripheral (hadc1) is already
 * initialized and calibrated.
 *
 * Assumptions to guide this design:
 *   - The code was targeting STM32H7 series with HAL library per the HAL
 macros.
 *   - The sensors is MEMS accelerometer LISXXXALH series.
 *   - The code using private LISXXXALH.h header for sensor-specific settings.
 *   - ADC is configured for single-ended input
 *   - 12-bit/14-bit resolution is used (per the original code line #30)
 *   - raw_LISXXXALH[] type of uint32_t per the HAL_ADC_GetValue() return value.
 *   - Polling mode is acceptable (no DMA). 
 *   - per the DS of LIS331ALH,LIS344ALH, the sampling BW frequency is 1-2 kHz
 *   - the timeout of polling was 10ms. 
 *   - Therefore, a simple blocking read is acceptable for this use case.
 * 
 * Bug fixes and improvements:
 *   - Fixed overflow writing to raw_LISXXXALH[] when snsrID invalid
 *   - Fixed runtime mutation of sConfig (optimizer-safe)
 *   - Reduce complexity and if/else checks. using fall through method.
 *   - Added return checks for HAL functions, with error tracking
 *   - Fixed missing channel config per AN2834
 *   - Added comprehensive error tracking
 *   - Increased sampling time (5→15 cycles) for stability
 *   - Improved code readability and maintainability
 *   - 
 *
 * Features:
 *   - Simple polling-based operation (blocking)
 *   - Full error tracking and diagnostics
 *   - Error codes stored in data array for easy detection
 *   - Lightweight and easy to debug
 * 
 * Usage Example:
 *   // Read single channel
 *   analogSensor_operation(0);
 *   if (raw_LISXXXALH[0] <= 4095) {
 *     // Valid ADC value
 *   } else {
 *     // Error occurred (value >= 0xFFFC)
 *   }
 *
 *   // Read all channels
 *   analogSensor_operation_all_channels(6);
 *
 *   // Check for errors
 *   if (analogSensor_getErrorCount() > 0) {
 *     ADC_ErrorInfo_t errors;
 *     analogSensor_getErrors(&errors);
 *     printf("Channel %u failed\n", errors.last_failed_channel);
 *   }
 *
 ******************************************************************************
 */

#ifndef ADC_CONVERSIONS_H
#define ADC_CONVERSIONS_H

#include "stm32f7xx_hal.h"
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/

/**
 * @brief Number of ADC channels
 */
#define ADC_CONVERSIONS_CHANNEL_COUNT 6

/* Exported types ------------------------------------------------------------*/

/**
 * @brief ADC error information structure (simplified tracking)
 */
typedef struct {
  uint32_t total_errors;               ///< Total conversion failures
  HAL_StatusTypeDef last_error_status; ///< Last HAL error status
  uint8_t last_failed_channel;         ///< Which channel failed (0xFF = none)
} ADC_ErrorInfo_t;

/* Exported variables --------------------------------------------------------*/

/**
 * @brief ADC sample array (volatile for ISR/main access), declared here, use as
 * global in main.c.
 * @note Values 0-4095 are valid ADC readings (12-bit)
 * @note Values >= 0xFFFC indicate errors:
 *       - 0xFFFE: Configuration error
 *       - 0xFFFD: Start error
 *       - 0xFFFC: Timeout error
 */
extern volatile uint32_t raw_LISXXXALH[ADC_CONVERSIONS_CHANNEL_COUNT];
/* Exported functions --------------------------------------------------------*/

/**
 * @brief Acquire one ADC channel (blocking, polling mode)
 *
 * @param snsrID Channel index (0..5)
 *
 * @note Result stored in raw_LISXXXALH[snsrID]
 * @note Not reentrant / not thread-safe
 */
void analogSensor_operation(uint8_t snsrID);

/**
 * @brief Read all channels sequentially
 *
 * @param total_channels Number of channels to read (typically 6)
 *
 * @note Results stored in raw_LISXXXALH[] array
 */
void analogSensor_operation_all_channels(uint8_t total_channels);

/**
 * @brief Get total error count
 *
 * @return uint32_t Total number of conversion errors
 */
uint32_t analogSensor_getErrorCount(void);

/**
 * @brief Get complete error information
 *
 * @param error_info Pointer to receive error info
 *
 * @return HAL_StatusTypeDef
 *   @retval HAL_OK    Success
 *   @retval HAL_ERROR NULL pointer
 */
HAL_StatusTypeDef analogSensor_getErrors(ADC_ErrorInfo_t *error_info);

/**
 * @brief Reset error tracking
 */
void analogSensor_resetErrors(void);

#ifdef __cplusplus
}
#endif

#endif /* ADC_CONVERSIONS_H */

