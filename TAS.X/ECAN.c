
/*
 * ECAN.c
 *
 *  Created on: Dec 27, 2013
 *      Author: Copy wang.xiang
 */
/* Device header file */

#include <xc.h>
#include <p33Exxxx.h>
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */
#include <stdlib.h>
#include "ECAN.h"

//ECAN buffer Init
/* This is the ECAN message buffer declaration. For this example the message buffers are
placed in EDS memory. Note that for dsPIC33E ECAN message buffers can be placed anywhere in
Flash memory. Note the buffer alignment. */
__eds__ unsigned int ecan1MsgBuf[NUM_OF_ECAN_BUFFERS][8]
__attribute__((aligned(NUM_OF_ECAN_BUFFERS * 16)));
volatile unsigned int BufferNo = 0;
unsigned int RevData[8] = {0, 0, 0, 0, 0, 0, 0, 0};
EcanData ECANMsg;

//*****************************************************************************
//
// configure ECAN--for CAN Communication
//
//*****************************************************************************

void InitECAN()
{
	PMD1bits.C1MD = 0;
	unsigned long address_DMA0;
	unsigned long address_DMA1;
	/* The dsPIC33E device features I/O remap. This I/O remap configuration for the ECAN
	module can be performed here. */
	//SetIORemapForECANModule();
	TRISBbits.TRISB9 = 1;
	TRISCbits.TRISC6 = 0;
	RPOR5 = 0x000E; //RP54 assign to can_c1tx
	RPINR26 = 0x0029; //can_c1rx assign to RP41



      // ////// RPOR6bits.RP56R = 0x000E;   //RP56      CAN_T
     ////////   RPINR26 = 55;               //RP55      CAN_R



	/* put the module in configuration mode */
	C1CTRL1bits.REQOP = 4;
	while(C1CTRL1bits.OPMODE != 4);

	/* FCAN is selected to be FCY
	FCAN = FCY = 60MHz */
	C1CTRL1bits.CANCKS = 0x0;
	/*
	    Bit Time = (Sync Segment + Propagation Delay + Phase Segment 1 + Phase Segment 2)=20*TQ
	    Phase Segment 1 = 8TQ
	    Phase Segment 2 = 6Tq
	    Propagation Delay = 5Tq
	    Sync Segment = 1TQ
	    BIT RATE OF 500kbps
	    6(2*3)/60Mhz * 20Tq
	 */
	C1CFG1bits.BRP = 0x2;
	/* Synchronization Jump Width set to 4 TQ */
	C1CFG1bits.SJW = 0x3;
	/* Phase Segment 1 time is 8 TQ */
	C1CFG2bits.SEG1PH = 0x7;
	/* Phase Segment 2 time is set to be programmable */
	C1CFG2bits.SEG2PHTS = 0x1;
	/* Phase Segment 2 time is 6 TQ */
	C1CFG2bits.SEG2PH = 0x5;
	/* Propagation Segment time is 5 TQ */
	C1CFG2bits.PRSEG = 0x4;
	/* Bus line is sampled three times at the sample point */
	C1CFG2bits.SAM = 0x1;

	/* 4 CAN Messages to be buffered in DMA RAM */
	C1FCTRLbits.DMABS = 0b000;

	/* Filter configuration */
	/* enable window to access the filter configuration registers */
	C1CTRL1bits.WIN = 0b1;

	/* select acceptance mask 0 filter 0 buffer 1 */
	C1FMSKSEL1bits.F0MSK = 0;
	/* Configure Acceptance Filter Mask 0 register to mask SID<2:0>
	 * Mask Bits (11-bits) : 0b111 1111 1000 */
	C1RXM0SIDbits.SID = 0x000; //This is example number
	/* Configure Acceptance Filter 0 to match standard identifier
	Filter Bits (11-bits): 0b011 1010 xxx with the mask setting, message with SID
	range 0x1D0-0x1D7 will be accepted by the ECAN module. */
	C1RXF0SIDbits.SID = 0x0000; //This is example number
	/* Acceptance Filter 0 to check for Standard Identifier */
	C1RXM0SIDbits.MIDE = 0x1;
	C1RXF0SIDbits.EXIDE = 0x0;
	/* acceptance filter to use buffer 1 for incoming messages */
	C1BUFPNT1bits.F0BP = 0b0001;
	/* enable filter 0 */
	C1FEN1bits.FLTEN0 = 1;

	/* select acceptance mask 1 filter 1 and buffer 2 */
	C1FMSKSEL1bits.F1MSK = 0b01;
	C1RXM1SIDbits.SID = 0x000; //This is example number
	C1RXF1SIDbits.SID = 0x0000; //This is example number
	/* Acceptance Filter 0 to check for Standard Identifier */
	C1RXM1SIDbits.MIDE = 0x1;
	C1RXF1SIDbits.EXIDE = 0x0;
	/* acceptance filter to use buffer 2 for incoming messages */
	C1BUFPNT1bits.F1BP = 0b0010;
	/* enable filter 1 */
	C1FEN1bits.FLTEN1 = 1;

	/* select acceptance mask 1 filter 2 and buffer 3 */
	C1FMSKSEL1bits.F2MSK = 0b10;
	C1RXM2SIDbits.SID = 0x000; //This is example number
	C1RXF2SIDbits.SID = 0x0000; //This is example number
	/* Acceptance Filter 0 to check for Standard Identifier */
	C1RXM2SIDbits.MIDE = 0x1;
	C1RXF2SIDbits.EXIDE = 0x0;
	/* acceptance filter to use buffer 3 for incoming messages */
	C1BUFPNT1bits.F2BP = 0b0011;
	/* enable filter 2 */
	C1FEN1bits.FLTEN2 = 1;

	/* clear window bit to access ECAN control registers */
	C1CTRL1bits.WIN = 0;

	/* put the module in normal mode */
	C1CTRL1bits.REQOP = 0;
	while(C1CTRL1bits.OPMODE != 0);

	/* clear the buffer and overflow flags */
	C1RXFUL1 = C1RXFUL2 = C1RXOVF1 = C1RXOVF2 = 0x0000;
	/* ECAN1, Buffer 0 is a Transmit Buffer */
	C1TR01CONbits.TXEN0 = 1;
	/* ECAN1, Buffer 1 is a Receive Buffer */
	C1TR01CONbits.TXEN1 = 1;
	/* ECAN1, Buffer 2 is a Receive Buffer */
	C1TR23CONbits.TXEN2 = 1;
	/* ECAN1, Buffer 3 is a Receive Buffer */
	C1TR23CONbits.TXEN3 = 0;
	/* Message Buffer 0 Priority Level */
	C1TR01CONbits.TX0PRI = 0b11;

	/* configure the device to interrupt on the receive buffer full flag */
	/* clear the buffer full flags */
	C1RXFUL1 = 0;
	C1INTFbits.RBIF = 0;

	//------------------------- initDMAECAN --------------------------------------
	/* Assign 32x8word Message Buffers for ECAN1 in device RAM. This example uses DMA0 for TX.
	Refer to 21.8.1"DMA Operation for Transmitting Data" for details on DMA channel
	configuration for ECAN transmit. */
	/* Data Transfer Size: Word Transfer Mode */
	DMA0CONbits.SIZE = 0x0;
	/* Data Transfer Direction: device RAM to Peripheral */
	DMA0CONbits.DIR = 0x1;
	/* DMA Addressing Mode: Peripheral Indirect Addressing mode */
	DMA0CONbits.AMODE = 0x2;
	/* Operating Mode: Continuous, Ping-Pong modes disabled */
	DMA0CONbits.MODE = 0x0;
	/* Assign ECAN1 Transmit event for DMA Channel 0 */
	DMA0REQ = 70;
	/* Set Number of DMA Transfer per ECAN message to 8 words */
	DMA0CNT = 7;
	/* Peripheral Address: ECAN1 Transmit Register */
	DMA0PAD = (volatile unsigned int) &C1TXD;
	address_DMA0 = __builtin_edsoffset(ecan1MsgBuf);
	address_DMA0 = address_DMA0 & 0x7FFF;
	address_DMA0 += __builtin_edspage(ecan1MsgBuf) << 15;
	DMA0STAL = address_DMA0 & 0xFFFF;
	DMA0STAH = address_DMA0 >> 16;
	/* Channel Enable: Enable DMA Channel 0 */
	DMA0CONbits.CHEN = 0x1;

	/*Assign 32x8word Message Buffers for ECAN1 in device RAM. This example uses DMA1 for RX.
	Refer to 21.8.1"DMA Operation for Transmitting Data" for details on DMA channel
	configuration for ECAN transmit. */
	/* Data Transfer Size: Word Transfer Mode */
	DMA1CONbits.SIZE = 0x0;
	/* Data Transfer Direction: Peripheral to device RAM */
	DMA1CONbits.DIR = 0x0;
	/* DMA Addressing Mode: Peripheral Indirect Addressing mode */
	DMA1CONbits.AMODE = 0x2;
	/* Operating Mode: Continuous, Ping-Pong modes disabled */
	DMA1CONbits.MODE = 0x0;
	/* Assign ECAN1 Receive event for DMA Channel 0 */
	DMA1REQ = 34;
	/* Set Number of DMA Transfer per ECAN message to 8 words */
	DMA1CNT = 7;
	/* Peripheral Address: ECAN1 Receive Register */
	DMA1PAD = (volatile unsigned int) &C1RXD;
	address_DMA1 = __builtin_edsoffset(ecan1MsgBuf);
	address_DMA1 = address_DMA1 & 0x7FFF;
	address_DMA1 += __builtin_edspage(ecan1MsgBuf) << 15;
	DMA1STAL = address_DMA1 & 0xFFFF;
	DMA1STAH = address_DMA1 >> 16;

	/* Channel Enable: Enable DMA Channel 1 */
	DMA1CONbits.CHEN = 0x1;
}

void ECANProcessRevData(EcanData *ECANMsg)
{
	unsigned int ide = 0;
	unsigned int rtr = 0;
	unsigned long id = 0;

	/*
	Standard Message Format:
	Word0 : 0bUUUx xxxx xxxx xxxx
	                     |____________|||
	                                SID10:0   SRR IDE(bit 0)
	Word1 : 0bUUUU xxxx xxxx xxxx
	                           |____________|
	                                        EID17:6
	Word2 : 0bxxxx xxx0 UUU0 xxxx
	                  |_____||	     |__|
	                  EID5:0 RTR   	  DLC
	word3-word6: data bytes
	word7: filter hit code bits

	Remote Transmission Request Bit for standard frames
	SRR->	"0"	 Normal Message
	                "1"  Message will request remote transmission
	Substitute Remote Request Bit for extended frames
	SRR->	should always be set to "1" as per CAN specification

	Extended  Identifier Bit
	IDE-> 	"0"  Message will transmit standard identifier
	                "1"  Message will transmit extended identifier

	Remote Transmission Request Bit for extended frames
	RTR-> 	"0"  Message transmitted is a normal message
	                "1"  Message transmitted is a remote message
	Don't care for standard frames
	 */
	/* read word 0 to see the message type */
	ide = ecan1MsgBuf[ECANMsg->buffer][0] & 0x0001;
	/* check to see what type of message it is */
	/* message is standard identifier */
	if(ide == 0)
	{                                                     //0001 1111 1111 1100
		ECANMsg->id = (ecan1MsgBuf[ECANMsg->buffer][0] & 0x1FFC) >> 2;
		ECANMsg->frame_type = CAN_FRAME_STD;
		rtr = ecan1MsgBuf[ECANMsg->buffer][0] & 0x0002;
	}/* mesage is extended identifier */
	else
	{
		id = ecan1MsgBuf[ECANMsg->buffer][0] & 0x1FFC;
		ECANMsg->id = id << 16;
		id = ecan1MsgBuf[ECANMsg->buffer][1] & 0x0FFF;
		ECANMsg->id = ECANMsg->id + (id << 6);
		id = (ecan1MsgBuf[ECANMsg->buffer][2] & 0xFC00) >> 10;
		ECANMsg->id = ECANMsg->id + id;
		ECANMsg->frame_type = CAN_FRAME_EXT;
		rtr = ecan1MsgBuf[ECANMsg->buffer][2] & 0x0200;
	}
	/* check to see what type of message it is */
	/* RTR message */
	if(rtr == 1)
	{
		ECANMsg->message_type = CAN_MSG_RTR;
	}        /* normal message */
	else
	{
		ECANMsg->message_type = CAN_MSG_DATA;
		ECANMsg->data[0] = (unsigned char) ecan1MsgBuf[ECANMsg->buffer][3];
		ECANMsg->data[1] = (unsigned char)((ecan1MsgBuf[ECANMsg->buffer][3] & 0xFF00) >> 8);
		ECANMsg->data[2] = (unsigned char) ecan1MsgBuf[ECANMsg->buffer][4];
		ECANMsg->data[3] = (unsigned char)((ecan1MsgBuf[ECANMsg->buffer][4] & 0xFF00) >> 8);
		ECANMsg->data[4] = (unsigned char) ecan1MsgBuf[ECANMsg->buffer][5];
		ECANMsg->data[5] = (unsigned char)((ecan1MsgBuf[ECANMsg->buffer][5] & 0xFF00) >> 8);
		ECANMsg->data[6] = (unsigned char) ecan1MsgBuf[ECANMsg->buffer][6];
		ECANMsg->data[7] = (unsigned char)((ecan1MsgBuf[ECANMsg->buffer][6] & 0xFF00) >> 8);
		ECANMsg->data_length = (unsigned char)(ecan1MsgBuf[ECANMsg->buffer][2] & 0x000F);
	}
	clearRxFlags(ECANMsg->buffer);
}

/******************************************************************************
*    Function:			clearRxFlags
*    Description:       clears the rxfull flag after the message is read
*    Arguments:			buffer number to clear
******************************************************************************/
void clearRxFlags(unsigned char buffer_number)
{
	if((C1RXFUL1bits.RXFUL1) && (buffer_number == 1))
	{
		/* clear flag */
		C1RXFUL1bits.RXFUL1 = 0;
	}
	/* check to see if buffer 2 is full */
	else if((C1RXFUL1bits.RXFUL2) && (buffer_number == 2))
	{
		/* clear flag */
		C1RXFUL1bits.RXFUL2 = 0;
	}
	/* check to see if buffer 3 is full */
	else if((C1RXFUL1bits.RXFUL3) && (buffer_number == 3))
	{
		/* clear flag */
		C1RXFUL1bits.RXFUL3 = 0;
	}
	else
	{

	}

}

//*****************************************************************************
//
// ECAN--Send Data
//
//*****************************************************************************

void ECANSendData(EcanData *ECANMsg)
{
	unsigned long word0 = 0;
	unsigned long word1 = 0;
	unsigned long word2 = 0;
	/*
	Message Format:
	Word0 : 0bUUUx xxxx xxxx xxxx
	         |____________|||
	        SID10:0   SRR IDE(bit 0)
	Word1 : 0bUUUU xxxx xxxx xxxx
	           |____________|
	          EID17:6
	Word2 : 0bxxxx xxx0 UUU0 xxxx
	      |_____||	     |__|
	     EID5:0 RTR      DLC

	Remote Transmission Request Bit for standard frames
	SRR->	"0"	 Normal Message
	"1"  Message will request remote transmission
	Substitute Remote Request Bit for extended frames
	SRR->	should always be set to "1" as per CAN specification

	Extended  Identifier Bit
	IDE-> 	"0"  Message will transmit standard identifier
	                "1"  Message will transmit extended identifier

	Remote Transmission Request Bit for extended frames
	RTR-> 	"0"  Message transmitted is a normal message
	"1"  Message transmitted is a remote message
	Don't care for standard frames
	 */
	/* check to see if the message has an extended ID */
	if(ECANMsg->frame_type == CAN_FRAME_EXT)
	{
		/* get the extended message id EID28..18*/
		word0 = (ECANMsg->id & 0x1FFC0000) >> 16;
		/* set the SRR and IDE bit */
		word0 = word0 + 0x0003;
		/* the the value of EID17..6 */
		word1 = (ECANMsg->id & 0x0003FFC0) >> 6;
		/* get the value of EID5..0 for word 2 */
		word2 = (ECANMsg->id & 0x0000003F) << 10;
	}
	else
	{
		/* get the SID */
		word0 = ((ECANMsg->id & 0x000007FF) << 2);
	}
	/* check to see if the message is an RTR message */
	if(ECANMsg->message_type == CAN_MSG_RTR)
	{
		if(ECANMsg->frame_type == CAN_FRAME_EXT)
			word2 = word2 | 0x0200;
		else
			word0 = word0 | 0x0002;

		ecan1MsgBuf[ECANMsg->buffer][0] = word0;
		ecan1MsgBuf[ECANMsg->buffer][1] = word1;
		ecan1MsgBuf[ECANMsg->buffer][2] = word2;
	}
	else
	{
		word2 = word2 + (ECANMsg->data_length & 0x0F);
		ecan1MsgBuf[ECANMsg->buffer][0] = word0;
		ecan1MsgBuf[ECANMsg->buffer][1] = word1;
		ecan1MsgBuf[ECANMsg->buffer][2] = word2;
		/* fill the data */
		ecan1MsgBuf[ECANMsg->buffer][3] = ((ECANMsg->data[1] << 8) + ECANMsg->data[0]);
		ecan1MsgBuf[ECANMsg->buffer][4] = ((ECANMsg->data[3] << 8) + ECANMsg->data[2]);
		ecan1MsgBuf[ECANMsg->buffer][5] = ((ECANMsg->data[5] << 8) + ECANMsg->data[4]);
		ecan1MsgBuf[ECANMsg->buffer][6] = ((ECANMsg->data[7] << 8) + ECANMsg->data[6]);
	}
	/* set the message for transmission */
	C1TR01CONbits.TXREQ0 = 1;
}












