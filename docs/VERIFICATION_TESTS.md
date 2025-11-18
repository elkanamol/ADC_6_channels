# ADC Sampling Verification Tests

## How to Verify the Solution Works

### ✅ Test 1: Measure Sampling Time (Oscilloscope Required)

**Setup:**
1. Connect oscilloscope probe to **PG0** (GPIO pin)
2. Connect ground to board GND

**Expected Results:**
- **Original code**: Pulse width ~186µs (6 channels × 31µs)
- **New optimized code**: Pulse width ~80-100µs (45% faster!)

**Code in `main.c`:**
```c
while (1) {
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_0, GPIO_PIN_SET);
    analogSensor_operation_all_channels();  // All 6 channels
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_Delay(10);
}
```

**Oscilloscope settings:**
- Timebase: 50µs/div
- Trigger: Rising edge on PG0
- Measure: Pulse width

---

### ✅ Test 2: Verify Continuous Sampling Under Load

**Purpose:** Prove that sampling continues even when CPU is busy with communication

**Test Code:**
```c
// Add this to main.c:
while (1) {
    // Sample all channels
    analogSensor_operation_all_channels();
    
    // Simulate heavy communication load
    char msg[100];
    for (int i = 0; i < 20; i++) {
        sprintf(msg, "UART load test message #%d - This is a long string to create load\n", i);
        // HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
    }
    
    // Check error count
    uint32_t errors = analogSensor_getErrorCount();
    if (errors > 0) {
        printf("WARNING: %u ADC conversion errors detected!\n", (unsigned int)errors);
    }
    
    HAL_Delay(100);
}
```

**Expected Results:**
- **Original code**: High error count, missed samples
- **New code**: Zero or very low error count, sampling continues

---

### ✅ Test 3: Maximum Sampling Rate Test

**Purpose:** Measure how many complete 6-channel scans per second

**Test Code:**
```c
// Add a 1-second timer (TIM2) configured in CubeMX
// Then add this:

volatile uint32_t sample_count = 0;
volatile uint32_t samples_per_second = 0;

// In main loop:
while (1) {
    analogSensor_operation_all_channels();
    sample_count++;
    // No delay - sample as fast as possible!
}

// In stm32f7xx_it.c, add timer interrupt:
void TIM2_IRQHandler(void) {
    if (__HAL_TIM_GET_FLAG(&htim2, TIM_FLAG_UPDATE)) {
        __HAL_TIM_CLEAR_FLAG(&htim2, TIM_FLAG_UPDATE);
        
        samples_per_second = sample_count;
        sample_count = 0;
        
        printf("Samples/sec: %u\n", (unsigned int)samples_per_second);
    }
}
```

**Expected Results:**
| Method | Samples/Second | Time per Cycle |
|--------|---------------|----------------|
| Original (buggy) | ~5,300 | 186µs |
| Solution 1 (optimized polling) | ~10,000 | 100µs |
| Solution 2 (DMA) | ~20,000 | 50µs |

---

### ✅ Test 4: Data Integrity Check

**Purpose:** Verify all channels produce valid, reasonable values

**Test Code:**
```c
while (1) {
    analogSensor_operation_all_channels();
    
    // Check for error markers (0xFFFF = timeout/error)
    int errors_detected = 0;
    for (int i = 0; i < 6; i++) {
        if (raw_LISXXXALH[i] == 0xFFFF) {
            printf("ERROR: Channel %d returned error marker!\n", i);
            errors_detected = 1;
        }
        
        // Check if values are within ADC range (0-4095 for 12-bit)
        if (raw_LISXXXALH[i] > 4095) {
            printf("ERROR: Channel %d out of range: %u\n", 
                   i, (unsigned int)raw_LISXXXALH[i]);
            errors_detected = 1;
        }
    }
    
    if (!errors_detected) {
        printf("✓ All channels OK\n");
    }
    
    HAL_Delay(100);
}
```

---

### ✅ Test 5: Single-Channel API Compatibility Test

**Purpose:** Verify backward compatibility with original API

**Test Code:**
```c
// Test original API still works:
while (1) {
    // Use old-style API (but with bug fixes)
    for (int i = 0; i < 6; i++) {
        analogSensor_operation(i);
    }
    
    printf("Channel 0: %u, Channel 5: %u\n",
           (unsigned int)raw_LISXXXALH[0],
           (unsigned int)raw_LISXXXALH[5]);
    
    HAL_Delay(100);
}
```

---

## 🔬 Advanced Verification: Logic Analyzer

If you have a logic analyzer, you can capture:

1. **ADC_EOC (End of Conversion)** signal
2. **GPIO toggle** timing
3. **UART TX** activity
4. **DMA request** signals

This allows precise measurement of:
- Conversion timing jitter
- Impact of interrupts on sampling
- DMA transfer timing

---

## 📊 Success Criteria

Your solution is successful if:

1. ✅ Sampling time reduced by **>40%** (oscilloscope test)
2. ✅ **Zero or minimal errors** under communication load
3. ✅ Sampling rate **>8,000 samples/sec** (full 6-channel cycles)
4. ✅ All channels produce **valid data** (0-4095 range)
5. ✅ **Backward compatible** with original API

---

## 🐛 Troubleshooting

### Problem: High error count
**Solution:** Increase polling timeout in `adc_coversions.c`:
```c
if (HAL_ADC_PollForConversion(&hadc1, 100) == HAL_OK) // Increase from 10 to 100
```

### Problem: All values read as zero
**Solution:** Check ADC is properly started:
```c
// Add debug in analogSensor_operation_all_channels():
if (!adc_initialized) {
    printf("Starting ADC...\n");
    if (HAL_ADC_Start(&hadc1) != HAL_OK) {
        printf("ERROR: ADC failed to start!\n");
        return;
    }
    adc_initialized = 1;
}
```

### Problem: Values seem to be from wrong channels
**Solution:** Verify channel mapping in `adc_coversions.c`:
```c
// Check channels match your hardware:
// PA0 = ADC_CHANNEL_0
// PA1 = ADC_CHANNEL_1
// etc.
```

---

## 📈 Performance Benchmarking

To compare solutions, use this test framework:

```c
#include "stm32f7xx_hal.h"

typedef struct {
    uint32_t start_tick;
    uint32_t end_tick;
    uint32_t duration_us;
} TimingResult;

TimingResult measure_sampling_time(void) {
    TimingResult result;
    
    // Use SysTick or DWT cycle counter for microsecond precision
    result.start_tick = HAL_GetTick() * 1000; // Convert to µs
    
    analogSensor_operation_all_channels();
    
    result.end_tick = HAL_GetTick() * 1000;
    result.duration_us = result.end_tick - result.start_tick;
    
    return result;
}

// In main:
while (1) {
    TimingResult t = measure_sampling_time();
    printf("Sampling took: %u µs\n", (unsigned int)t.duration_us);
    HAL_Delay(1000);
}
```

---

## 🎯 Final Verification Checklist

Before submitting your homework:

- [ ] Oscilloscope shows **~80-100µs** pulse width (PG0)
- [ ] Error counter stays at **zero** during normal operation
- [ ] Sampling continues under **heavy UART load**
- [ ] All 6 channels produce **valid values** (0-4095)
- [ ] Code compiles with **no warnings**
- [ ] Backward compatible with `analogSensor_operation(snsrID)` API
- [ ] Code includes **proper comments** explaining bug fixes
- [ ] Performance is **~2x faster** than original

✅ **You're ready to submit!**
