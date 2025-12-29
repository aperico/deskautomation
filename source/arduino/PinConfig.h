#ifndef PINCONFIG_H
#define PINCONFIG_H

#define DRIVER_L298N 1
#define DRIVER_BTS7960 2

#ifndef DRIVER_TYPE
#define DRIVER_TYPE DRIVER_L298N
#endif

#if DRIVER_TYPE == DRIVER_L298N
static const int IN1 = 8;
static const int IN2 = 9;
static const int ENA = 10;
#elif DRIVER_TYPE == DRIVER_BTS7960
static const int RPWM = 5;
static const int LPWM = 6;
static const int R_EN = 7;
static const int L_EN = 8;
#endif

#endif
