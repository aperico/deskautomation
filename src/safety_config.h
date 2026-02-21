#ifndef SAFETY_CONFIG_H
#define SAFETY_CONFIG_H

#include <stdint.h>

// Motor current sense configuration
static const uint16_t MOTOR_SENSE_THRESHOLD_MA = 150U;
static const uint32_t MOTOR_SENSE_FAULT_TIME_MS = 100U;

// ADC conversion parameters
static const uint16_t ADC_REF_MV = 5000U;
static const uint16_t SHUNT_MILLIOHMS = 500U;

#endif // SAFETY_CONFIG_H
