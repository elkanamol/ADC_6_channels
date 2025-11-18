/**
 * ============================================================================
 * STEP-BY-STEP ADC DEBUG - FRESH START
 * ============================================================================
 * 
 * STATUS: Reset to simplest possible ADC read with full error tracking
 * 
 * ============================================================================
 * WHY YOU'RE GETTING 0xFFFF
 * ============================================================================
 * 
 * Most likely cause: Your ADC configuration has ContinuousConvMode = ENABLE
 * This makes HAL_ADC_PollForConversion() behave differently!
 * 
 * In continuous mode:
 * - ADC starts converting immediately after HAL_ADC_Start()
 * - EOC flag is set/cleared automatically
 * - Polling for conversion may timeout because flag was already cleared
 * 
 * ============================================================================
 * IMMEDIATE FIX (99% will solve the problem)
 * ============================================================================
 * 
 * In adc.c, line ~49, change:
 * 
 *   hadc1.Init.ContinuousConvMode = ENABLE;   // ← CHANGE THIS
 * 
 * To:
 * 
 *   hadc1.Init.ContinuousConvMode = DISABLE;  // ← Use single conversion
 * 
 * Then rebuild and test!
 * 
 * ============================================================================
 * ERROR MARKERS IN raw_LISXXXALH[]
 * ============================================================================
 * 
 * 0xFFFF = Invalid channel ID (snsrID >= 6)
 * 0xFFFE = HAL_ADC_ConfigChannel() failed
 * 0xFFFD = HAL_ADC_Start() failed
 * 0xFFFC = HAL_ADC_PollForConversion() timeout
 * 0-4095 = Valid ADC reading ✅
 * 
 * ============================================================================
 * DEBUGGING VARIABLES (watch in debugger or check UART)
 * ============================================================================
 * 
 * last_config_status : Last HAL_ADC_ConfigChannel return (0=OK, 1=ERROR)
 * last_start_status  : Last HAL_ADC_Start return (0=OK, 1=ERROR)  
 * last_poll_status   : Last HAL_ADC_PollForConversion return (0=OK, 3=TIMEOUT)
 * conversion_errors  : Total error count
 * 
 * ============================================================================
 * CURRENT CODE FLOW
 * ============================================================================
 * 
 * 1. HAL_ADC_Stop()           - Ensure clean state
 * 2. HAL_ADC_ConfigChannel()  - Select which channel to read
 * 3. HAL_ADC_Start()          - Start conversion
 * 4. HAL_ADC_PollForConversion() - Wait for completion (100ms timeout)
 * 5. HAL_ADC_GetValue()       - Read the result
 * 6. HAL_ADC_Stop()           - Stop ADC
 * 
 * This is the SIMPLEST possible approach - no optimizations yet!
 * 
 * ============================================================================
 * UART DEBUG OUTPUT
 * ============================================================================
 * 
 * You should see (every 1 second):
 * 
 * CH0=2048 (0x0800) | Errors=0 | Status: Cfg=0 Start=0 Poll=0
 * 
 * If working properly:
 * - CH0 value should be 0-4095
 * - Errors should be 0
 * - All status codes should be 0
 * 
 * If seeing 0xFFFF:
 * - Check which status is non-zero
 * - See DEBUG_GUIDE.md for solutions
 * 
 * ============================================================================
 * TESTING PLAN
 * ============================================================================
 * 
 * STEP 1: Get single channel (CH0) reading properly
 *   - Fix ContinuousConvMode to DISABLE
 *   - Should see values 0-4095
 * 
 * STEP 2: Test all 6 channels
 *   - Uncomment the section in main.c marked "UNCOMMENT THIS AFTER..."
 *   - All channels should read properly
 * 
 * STEP 3: Optimize for speed
 *   - Remove HAL_ADC_Stop() between samples
 *   - Use scan mode for all channels at once
 *   - Reduce sampling time
 * 
 * ============================================================================
 * ADC CONFIGURATION REVIEW
 * ============================================================================
 * 
 * Your current settings (from adc.c):
 *   ClockPrescaler: DIV8
 *   Resolution: 12-bit (0-4095)
 *   ScanConvMode: ENABLE (ready for multi-channel)
 *   ContinuousConvMode: ENABLE ← CHANGE TO DISABLE FOR DEBUGGING
 *   NbrOfConversion: 6
 *   EOCSelection: SINGLE_CONV
 *   SamplingTime: 3 CYCLES (very fast, may need to increase)
 * 
 * Recommended for debugging:
 *   ContinuousConvMode: DISABLE
 *   SamplingTime: 480 CYCLES (slower but more stable)
 * 
 * ============================================================================
 * HARDWARE CONNECTION
 * ============================================================================
 * 
 * STM32F746ZG ADC channels:
 *   PA0 = ADC1_IN0 (Channel 0)
 *   PA1 = ADC1_IN1 (Channel 1)
 *   PA2 = ADC1_IN2 (Channel 2)
 *   PA3 = ADC1_IN3 (Channel 3)
 *   PA4 = ADC1_IN4 (Channel 4)
 *   PA5 = ADC1_IN5 (Channel 5)
 * 
 * Quick hardware test:
 *   - Connect PA0 to GND → should read ~0
 *   - Connect PA0 to 3.3V → should read ~4095
 *   - Leave PA0 floating → may read random or ~2048
 * 
 * ============================================================================
 * FILES MODIFIED
 * ============================================================================
 * 
 * 1. adc_coversions.c  - Simple single-channel read with error tracking
 * 2. adc_conversions.h - Updated function prototypes
 * 3. main.c           - Debug output with detailed status
 * 4. DEBUG_GUIDE.md   - Detailed troubleshooting guide
 * 
 * ============================================================================
 */
