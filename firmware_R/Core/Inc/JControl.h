/*
 *  JControl.h
 *
 *  Created on: FEB 11, 2023
 *      Author: Jong Wook Baek
 *
 *  DC Motor Control Library For SRCIRC.
 */

#ifndef JCONTROL_H_
#define JCONTROL_H_

#include "main.h"

#define CW 0
#define CCW 1
#define RESET 0
#define SET 1

#define MAF_SIZE 15
#define CPR 8192 //Encoder PPR * 4
#define Tc 0.02 //TMR IT

#define sumELimit 1000 //Don't touch
#define division 1000 //Don't touch

typedef struct _MAF
{
	float OutputBuffer[MAF_SIZE];
	float FilteredData;
	int count;
}MAF;

typedef struct _DIRECTION
{
	int FrontMotorDirection;
	int FrontEncoderDirection;
}DIRECTION;

typedef struct _ENCODER
{
	int now;
	int past;
	int m1;

	float RPM;
	long long DEGREE;
}ENCODER;

typedef struct _DUTY
{
	int duty;
	int dutylimit;
}DUTY;

typedef struct _PID
{
    float input;
	float target;
	
	float E;
	float E_old;

	float sumE;
	float diffE;

	float kP;
	float kI;
	float kD;

	float output;
	float outputlimit;

	MAF filter;
	float FilteredOutput;
}PID;

float MAF_Filter(MAF* dst, float rawdata);
void Get_Motor_Status(ENCODER* dst, TIM_TypeDef* TIMx);
void Duty_Control_Velocity(DUTY* dst, DIRECTION* DIRx, GPIO_TypeDef* GPIOx, uint16_t PINx, TIM_TypeDef* TIMx, uint8_t CHx, int target);
void PID_Control(PID* dst, float target, float input);
void PID_Control_Velocity(PID* dst, DIRECTION* DIRx, ENCODER* ENx, GPIO_TypeDef* GPIOx, uint16_t PINx, TIM_TypeDef* TIMx, uint8_t CHx, int target);

#endif
