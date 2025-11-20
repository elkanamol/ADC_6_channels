# ADC Conversion Helper

This project adds a small polling-based helper around `hadc1` to read up to six LISXXXALH accelerometer channels on STM32F7/H7 parts. The helper exposes a single shared buffer (`raw_LISXXXALH[]`) plus lightweight diagnostics so other modules can fetch samples without touching HAL state directly.

## What changed

- Added consistent channel configuration tables that match STM32F7 (default) and STM32H7 reference settings and prevent runtime mutation of `ADC_ChannelConfTypeDef`.
- Stored conversion results and failure markers in one global array defined in `adc_conversions.c` and declared `extern` in `adc_conversions.h`, eliminating duplicate definitions in `main.c`.
- Improved error tracking: every HAL failure updates `ADC_ErrorInfo_t` and writes explicit sentinel values (`0xFFFE`, `0xFFFD`, `0xFFFC`) into the sample slot so callers can spot faults without extra plumbing.
- Hardened parameter checking in `analogSensor_operation()` / `_all_channels()` and unified cleanup after timeouts to keep the ADC peripheral in a known state.

## Using the helper

1. Include `adc_conversions.h` after initializing `hadc1`.
2. Call `analogSensor_operation(channel)` or `analogSensor_operation_all_channels(ADC_CONVERSIONS_CHANNEL_COUNT)` from the main loop.
3. Inspect `raw_LISXXXALH[n]`: values `0–4095` are valid samples, values `>= 0xFFFC` indicate the error shown in the table above.
4. Use `analogSensor_getErrorCount()` / `analogSensor_getErrors()` for diagnostics and `analogSensor_resetErrors()` to clear the counters once handled.
