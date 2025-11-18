
#ifndef ADC_CONVERSIONS_H
#define ADC_CONVERSIONS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Simple single-channel ADC read with full error tracking
 * Error markers in raw_LISXXXALH[]:
 *   0xFFFF = Invalid channel ID
 *   0xFFFE = Config error
 *   0xFFFD = Start error
 *   0xFFFC = Timeout error
 */
void analogSensor_operation(uint8_t snsrID);

/**
 * Read all 6 channels sequentially
 */
void analogSensor_operation_all_channels(uint8_t total_channels);

/**
 * Get total conversion errors (for diagnostics)
 */
uint32_t analogSensor_getErrorCount(void);

/**
 * Get last HAL status codes for debugging
 * config: last HAL_ADC_ConfigChannel return (0=OK, 1=ERROR)
 * start: last HAL_ADC_Start return (0=OK, 1=ERROR)
 * poll: last HAL_ADC_PollForConversion return (0=OK, 1=ERROR/TIMEOUT)
 */
void analogSensor_getDebugStatus(uint32_t *config, uint32_t *start, uint32_t *poll);

/**
 * Reset error counter
 */
void analogSensor_resetErrors(void);

/* Expose the sample array for read-only consumers */
extern volatile uint32_t raw_LISXXXALH[6];

#ifdef __cplusplus
}
#endif

#endif /* ADC_CONVERSIONS_H */

