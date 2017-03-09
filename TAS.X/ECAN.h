/* 
 * File:   ECAN.h
 * Author: Ctrl+A and Ctrl+X
 *
 * Created on 2017年2月20日, 上午11:12
 */

#ifndef ECAN_H
#define	ECAN_H

#ifdef	__cplusplus
extern "C" {
#endif

#define NUM_OF_ECAN_BUFFERS 4       //ECAN buffer number

/* defines used in ecan.c */
/* ECAN message buffer length */
#define ECAN1_MSG_BUF_LENGTH 	4

/* ECAN message type identifiers */
#define CAN_MSG_DATA	0x01
#define CAN_MSG_RTR	0x02
#define CAN_FRAME_EXT	0x03
#define CAN_FRAME_STD	0x04

#define CAN_BUF_FULL	0x05
//#define CAN_BUF_FULLOVER 0x08
#define CAN_BUF_EMPTY	0x06

typedef struct
{
	/* keep track of the buffer status */
	unsigned char buffer_status;

        /* RTR message or data message */
	unsigned char message_type;

        /* frame type extended or standard */
	unsigned char frame_type;
	/* buffer being used to send and receive messages */
        
	unsigned char buffer;
	/* 29 bit id max of 0x1FFF FFFF
	*  11 bit id max of 0x7FF */
	unsigned long id;
	unsigned char data[8];
	unsigned char data_length;
} EcanData;

void InitECAN(void);
void ECANSendData(EcanData *ECANMsg);
void ECANProcessRevData(EcanData *ECANMsg);
void ECANRevDataConfig(void);
void clearRxFlags(unsigned char buffer_number);


#ifdef	__cplusplus
}
#endif

#endif	/* ECAN_H */

