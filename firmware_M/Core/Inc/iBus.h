/*
 * ibus.h
 *
 *  Created on: Sep 25, 2023
 *      Author: Jongwook Baek
 */

#ifndef IBUS_H_
#define IBUS_H_

#include "main.h"

#define IBUS_RX_BUFFER_SIZE 32
#define IBUS_CHANNEL_SIZE 8

#define IBUS_HEADER_1 0
#define IBUS_HEADER_2 1
#define IBUS_CHECKSUM_2 31

#define PI 3.141592
#define WHEEL_WIDTH 0.494 //m
#define WHEEL_DIAMETER 0.236 //m

#define DUTY_MIN 0
#define DUTY_MAX 1000
#define RPM_MIN 0
#define RPM_MAX 98
#define GEAR_RATIO 1/1.8
#define WHEEL_DIFFERENCE 1.234

typedef struct _IBUS
{
	uint8_t state;
	uint8_t rxBuffer[IBUS_RX_BUFFER_SIZE];
	uint16_t checksum_buffer;
	uint16_t checksum_packet;
	uint16_t rawdata[IBUS_CHANNEL_SIZE];
	uint8_t failsafe;

	double channel[IBUS_CHANNEL_SIZE];

	int leftRPM;
	int rightRPM;
	int leftRPM2;
	int rightRPM2;

	int leftDuty;
	int rightDuty;
	int leftDuty2;
	int rightDuty2;

	int pan;
	int tilt;

	int fire;
	int light;
}IBUS;

uint16_t iBus_combineByte(uint8_t lowByte, uint8_t highByte);
uint16_t iBus_checksum(uint8_t *packet);
void iBus_parsing(IBUS *ibus);
double iBus_rangeTransform(double input, double xMIN, double xMAX, double yMIN, double yMAX);
double iBus_limiting(double input, double limit);
void iBus_normalization(IBUS *ibus);
void iBus_cmd_vel(IBUS *ibus);
uint8_t iBus_readPacket(IBUS *ibus, uint8_t *packet);
uint8_t iBus_readByte(IBUS *ibus, uint8_t byte);

#endif
