
/**
 ******************************************************************************
 * @file    adc_conversions.h
 * @brief   Single-channel polling-based ADC conversion API
 * @date    November 19, 2025
 ******************************************************************************
 * @attention
 *
 * This module provides simple, blocking ADC conversions using polling mode.
 * Each channel is configured and read individually.
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
 * @brief ADC sample array (shared with other modules)
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

