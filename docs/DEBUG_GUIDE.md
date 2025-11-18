# ADC Debug Guide - Step by Step

## Current Status: BASIC DEBUGGING MODE

The code is now reset to the **simplest possible ADC read** with full error tracking.

---

## 🔍 How to Debug the 0xFFFF Issue

### Step 1: Check UART Output

Run the program and check the UART output. You should see something like:

```
CH0=1234 (0x04D2) | Errors=0 | Status: Cfg=0 Start=0 Poll=0
```

### What Each Error Marker Means:

| Value | Meaning | Likely Cause |
|-------|---------|--------------|
| **0xFFFF** | Invalid channel ID | snsrID >= 6 (should not happen) |
| **0xFFFE** | ConfigChannel failed | ADC not initialized properly |
| **0xFFFD** | Start failed | ADC in wrong state or locked |
| **0xFFFC** | Timeout | Conversion didn't complete in 100ms |
| **0-4095** | Valid data! | ADC working correctly ✅ |

### HAL Status Codes:

| Status | Value | Meaning |
|--------|-------|---------|
| HAL_OK | 0 | Success ✅ |
| HAL_ERROR | 1 | Generic error |
| HAL_BUSY | 2 | Resource busy |
| HAL_TIMEOUT | 3 | Timeout occurred |

---

## 🐛 Common Issues and Solutions

### Issue 1: Getting 0xFFFE (Config Error)

**Cause:** `HAL_ADC_ConfigChannel()` is failing

**Solutions:**
1. Check that `MX_ADC1_Init()` was called in main
2. Verify ADC clock is enabled
3. Check that GPIO pins are configured as analog

**How to verify:**
```c
// Add this in main.c before the while loop:
if (hadc1.Instance == NULL) {
    printf("ERROR: ADC not initialized!\r\n");
}
```

---

### Issue 2: Getting 0xFFFD (Start Error)

**Cause:** `HAL_ADC_Start()` is failing

**Possible reasons:**
- ADC already running (ContinuousConvMode = ENABLE conflict)
- ADC calibration not done
- Wrong state

**Solution:** Try adding calibration before first use:
```c
// In adc.c, in USER CODE BEGIN ADC1_Init 2:
if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED) != HAL_OK) {
    Error_Handler();
}
```

---

### Issue 3: Getting 0xFFFC (Timeout)

**Cause:** `HAL_ADC_PollForConversion()` times out

**This is the most likely cause of your 0xFFFF!**

**Reasons:**
1. **ContinuousConvMode = ENABLE** - ADC never stops, EOC flag behaves differently
2. **EOCSelection = ADC_EOC_SINGLE_CONV** - Flag set after each channel, not sequence
3. ADC clock not running
4. GPIO not configured properly

**Solution:** Change ADC configuration in CubeMX/adc.c:

```c
// Option 1: Use single conversion mode (RECOMMENDED for debugging)
hadc1.Init.ContinuousConvMode = DISABLE;  // ← Change this
hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV; // OK

// Option 2: Keep continuous but change EOC
hadc1.Init.ContinuousConvMode = ENABLE;
hadc1.Init.EOCSelection = ADC_EOC_SEQ_CONV;  // ← Change this
```

---

## ✅ Expected Output When Working

```
CH0=2048 (0x0800) | Errors=0 | Status: Cfg=0 Start=0 Poll=0
CH0=2051 (0x0803) | Errors=0 | Status: Cfg=0 Start=0 Poll=0
CH0=2049 (0x0801) | Errors=0 | Status: Cfg=0 Start=0 Poll=0
```

Values should:
- Be between **0-4095** (12-bit ADC)
- Change slightly if you're reading a floating input
- Be stable around mid-range (~2048) if input is not connected
- Be close to 0 if pin is grounded
- Be close to 4095 if pin is at 3.3V

---

## 🔧 Quick Fix Checklist

If you're getting 0xFFFF, try these in order:

### 1. Check ADC Configuration (in adc.c):
```c
// Change ContinuousConvMode to DISABLE for debugging
hadc1.Init.ContinuousConvMode = DISABLE;  // ← Add this line
```

### 2. Increase Sampling Time (in adc.c):
```c
// In all 6 channel configurations:
sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;  // ← Slower but more reliable
```

### 3. Add ADC Calibration (in adc.c, before `/* USER CODE END ADC1_Init 2 */`):
```c
/* USER CODE BEGIN ADC1_Init 2 */
// Calibrate ADC before first use (STM32F7 specific)
if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED) != HAL_OK) {
    Error_Handler();
}
/* USER CODE END ADC1_Init 2 */
```

### 4. Verify GPIO is Analog (should already be OK):
```c
GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5;
GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;  // ✅ Should be ANALOG
GPIO_InitStruct.Pull = GPIO_NOPULL;       // ✅ Should be NOPULL
```

---

## 🎯 Action Plan

### STEP 1: Fix ADC Configuration
1. Open `adc.c`
2. Find line: `hadc1.Init.ContinuousConvMode = ENABLE;`
3. Change to: `hadc1.Init.ContinuousConvMode = DISABLE;`
4. Rebuild and test

### STEP 2: Test Single Channel
- Run program
- Check UART output
- Look for values 0-4095 instead of 0xFFFF

### STEP 3: Once Working, Test All Channels
- Uncomment the "UNCOMMENT THIS AFTER SINGLE CHANNEL WORKS" section in main.c
- Should see all 6 channels reading properly

---

## 📊 Debugging Variables You Can Monitor

Add these to your watch window in debugger:

```c
raw_LISXXXALH[0]      // Should be 0-4095 when working
last_config_status    // Should be 0 (HAL_OK)
last_start_status     // Should be 0 (HAL_OK)
last_poll_status      // Should be 0 (HAL_OK)
conversion_errors     // Should be 0 or low
hadc1.Instance        // Should be 0x40012000 (ADC1 base address)
hadc1.State           // Check ADC state
```

---

## 🚀 Next Steps After It Works

Once you get valid ADC readings (0-4095):

1. Test all 6 channels
2. Optimize for speed (reduce delays, use continuous mode properly)
3. Add scan mode for faster multi-channel reading
4. Consider DMA if needed

---

## 💡 Quick Test: Verify ADC Hardware

Connect PA0 to GND:
- Should read close to **0**

Connect PA0 to 3.3V:
- Should read close to **4095**

Leave PA0 floating:
- May read random values or mid-range (~2048)

This proves the ADC hardware is working!
