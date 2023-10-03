/*
 * Protocol2.h
 *
 *  Created on: Sep 27, 2023
 *      Author: Jongwook Baek
 */

#ifndef INC_PROTOCOL2_H_
#define INC_PROTOCOL2_H_

#include "main.h"
#include "iBus.h"

#define PROTOCOL2_RX_BUFFER_SIZE 64
#define PROTOCOL2_TX_BUFFER_SIZE 64
#define PROTOCOL2_CHANNEL_SIZE 8

#define PROTOCOL2_HEADER_1 0
#define PROTOCOL2_HEADER_2 1
#define PROTOCOL2_HEADER_3 2
#define PROTOCOL2_HEADER_4 3
#define PROTOCOL2_CRC_2 31

#define PROTOCOL2_WRITE 0x03
#define PROTOCOL2_SYNC_WRITE 0x83
#define PROTOCOL2_BULK_WRITE 0x93

typedef struct _PROTOCOL2
{
	uint8_t rxState;
	uint8_t txState;
	uint8_t rxBuffer[PROTOCOL2_RX_BUFFER_SIZE];
	uint8_t txBuffer[PROTOCOL2_TX_BUFFER_SIZE];
	uint8_t inited;
	uint16_t crc_buffer;
	uint16_t crc_packet;
	uint16_t rawdata[PROTOCOL2_CHANNEL_SIZE];

	double channel[PROTOCOL2_CHANNEL_SIZE];

}PROTOCOL2;

uint16_t Protocol2_combineByte(uint8_t lowByte, uint8_t highByte);
void Protocol2_divideByte(PROTOCOL2 *protocol2, int value, int len);
void Protocol2_update_header(PROTOCOL2 *protocol2);
void Protocol2_torqueOn(PROTOCOL2 *protocol2);
void Protocol2_writePosition(PROTOCOL2 *protocol2, IBUS *ibus);
void Protocol2_update_length(PROTOCOL2 *protocol2);
uint16_t Protocol2_update_crc(uint16_t crc_accum, uint8_t *data_blk_ptr, uint16_t data_blk_size);
void Protocol2_writeByte(PROTOCOL2 *protocol2, IBUS *ibus);
void Protocol2_writePacket(PROTOCOL2 *protocol2, IBUS *ibus);

#endif /* INC_PROTOCOL2_H_ */
