/*
 * ibus.c
 *
 *  Created on: Sep 25, 2023
 *      Author: Jongwook Baek
 */

#include "iBus.h"

uint16_t iBus_combineByte(uint8_t lowByte, uint8_t highByte)
{
	return (highByte << 8) + lowByte;
}

uint16_t iBus_checksum(uint8_t *packet)
{
	uint16_t checksum = 0xffff;

	for(int i = 0; i < IBUS_RX_BUFFER_SIZE-2; i++)
	{
		checksum -= packet[i];
	}

	return checksum;
}

void iBus_parsing(IBUS *ibus)
{
	ibus->rawdata[0] = iBus_combineByte(ibus->rxBuffer[2], ibus->rxBuffer[3]);
	ibus->rawdata[1] = iBus_combineByte(ibus->rxBuffer[4], ibus->rxBuffer[5]);
	ibus->rawdata[2] = iBus_combineByte(ibus->rxBuffer[6], ibus->rxBuffer[7]);
	ibus->rawdata[3] = iBus_combineByte(ibus->rxBuffer[8], ibus->rxBuffer[9]);
	ibus->rawdata[4] = iBus_combineByte(ibus->rxBuffer[10], ibus->rxBuffer[11]);
	ibus->rawdata[5] = iBus_combineByte(ibus->rxBuffer[12], ibus->rxBuffer[13]);
	ibus->rawdata[6] = iBus_combineByte(ibus->rxBuffer[14], ibus->rxBuffer[15]);
	ibus->rawdata[7] = iBus_combineByte(ibus->rxBuffer[16], ibus->rxBuffer[17]);

	ibus->failsafe = (ibus->rxBuffer[13] >> 4);
}

double iBus_rangeTransform(double input, double xMIN, double xMAX, double yMIN, double yMAX)
{
	return ((yMAX - yMIN) / (xMAX - xMIN)) * (input - xMIN) + yMIN;
}

void iBus_normalization(IBUS *ibus)
{
	for(int i = 0; i < IBUS_CHANNEL_SIZE; i++)
	{
		ibus->channel[i] = iBus_rangeTransform(ibus->rawdata[i], 1000, 2000, -1, 1);

		if(ibus->channel[i] < 0.1 && ibus->channel[i] > -0.1)
		{
			ibus->channel[i] = 0.0;
		}
		else if(ibus->channel[i] > 0.9)
		{
			ibus->channel[i] = 1.0;
		}
		else if(ibus->channel[i] < -0.9)
		{
			ibus->channel[i] = -1.0;
		}
	}
}

void iBus_cmd_vel(IBUS *ibus)
{
	double linear = ibus->channel[1];
	double angular = ibus->channel[0];
	double pan = ibus->channel[3];
	double tilt = ibus->channel[2];
	double fire = ibus->channel[5];
	double light = ibus->channel[4];

	ibus->leftRPM = (linear - angular * WHEEL_WIDTH / 2) / (PI * WHEEL_DIAMETER) * 60;
	ibus->rightRPM = (linear + angular * WHEEL_WIDTH / 2) / (PI * WHEEL_DIAMETER) * 60;

	ibus->leftRPM = iBus_rangeTransform(ibus->leftRPM, 0, 100, RPM_MIN, RPM_MAX) * GEAR_RATIO / WHEEL_DIFFERENCE;
	ibus->rightRPM = iBus_rangeTransform(ibus->rightRPM, 0, 100, RPM_MIN, RPM_MAX) * GEAR_RATIO / WHEEL_DIFFERENCE;
	ibus->leftRPM2 = ibus->leftRPM * WHEEL_DIFFERENCE;
	ibus->rightRPM2 = ibus->rightRPM * WHEEL_DIFFERENCE;

	ibus->leftDuty = iBus_rangeTransform(ibus->leftRPM, RPM_MIN, RPM_MAX * GEAR_RATIO, DUTY_MIN, DUTY_MAX);
	ibus->rightDuty = iBus_rangeTransform(ibus->rightRPM, RPM_MIN, RPM_MAX * GEAR_RATIO, DUTY_MIN, DUTY_MAX);
	ibus->leftDuty2 = iBus_rangeTransform(ibus->leftRPM2, RPM_MIN, RPM_MAX * GEAR_RATIO, DUTY_MIN, DUTY_MAX);
	ibus->rightDuty2 = iBus_rangeTransform(ibus->rightRPM2, RPM_MIN, RPM_MAX * GEAR_RATIO, DUTY_MIN, DUTY_MAX);

	ibus->pan = iBus_rangeTransform(pan, -1, 1, 0, 4095);
	ibus->tilt = iBus_rangeTransform(tilt, -1, 1, 0, 4095);

	ibus->fire = iBus_rangeTransform(fire, -1, 1, 0, 1);
	ibus->light = iBus_rangeTransform(light, -1, 1, 0, 1);
}

uint8_t iBus_readPacket(IBUS *ibus, uint8_t *packet)
{
	ibus->state = IBUS_HEADER_1;

	for(int i = 0; i < IBUS_RX_BUFFER_SIZE; i++)
	{
		if(iBus_readByte(ibus, packet[i]))
		{
			return 1;
		}
	}

	return 0;
}

uint8_t iBus_readByte(IBUS *ibus, uint8_t byte)
{
	switch (ibus->state)
	{
	case IBUS_HEADER_1:
	{
		if(byte == 0x20)
		{
			ibus->rxBuffer[ibus->state] = byte;
			ibus->state++;
		}
		break;
	}
	case IBUS_HEADER_2:
	{
		if(byte == 0x40)
		{
			ibus->rxBuffer[ibus->state] = byte;
			ibus->state++;
		}
		else
		{
			ibus->state = IBUS_HEADER_1;
		}
		break;
	}
	case IBUS_CHECKSUM_2:
	{
		ibus->rxBuffer[ibus->state] = byte;
		ibus->state = IBUS_HEADER_1;

		ibus->checksum_buffer = iBus_checksum(ibus->rxBuffer);
		ibus->checksum_packet = iBus_combineByte(ibus->rxBuffer[IBUS_CHECKSUM_2-1], ibus->rxBuffer[IBUS_CHECKSUM_2]);

		if(ibus->checksum_buffer == ibus->checksum_packet)
		{
			iBus_parsing(ibus);
			iBus_normalization(ibus);
			iBus_cmd_vel(ibus);

			return 1;
		}
		else
		{
			return 0;
		}
		break;
	}
	default:
	{
		ibus->rxBuffer[ibus->state] = byte;
		ibus->state++;
		break;
	}
	}

	return 0;
}
