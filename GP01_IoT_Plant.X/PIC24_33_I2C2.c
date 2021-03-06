/*******************************************************************************
 * File: PIC24_PIC33_I2C.c
 * Author: Armstrong Subero (original Author unknown stated as "user")
 * PIC: 24FJ128GB204 @ 32 MHz, 3.3v
 * Compiler: XC16 (Pro) (v1.31, MPLAX X v3.55)
 * Program Version: 1.1
 *                * Added comments
 *                * Added intermediate level functions
 *                * Modified to work on PIC24FJ128GB204
 *                * Modified to use I2C2 module
 *                * Changed types
 *                
 * Program Description: This Program allows usage of I2C on PIC24 and dsPIC33
 *                      microcontrollers.
 * 
 * Hardware Description: Standard connections as per MCC or PPS
 *                      
 * Created May 8th, 2017, 9:00 PM
 * 
 *
 * License:
 * 
 * "Copyright (c) 2017 Armstrong Subero ("AUTHORS")"
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice, the following
 * two paragraphs and the authors appear in all copies of this software.
 *
 * IN NO EVENT SHALL THE "AUTHORS" BE LIABLE TO ANY PARTY for
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE "AUTHORS"
 * HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE "AUTHORS" SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS for A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE "AUTHORS" HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 *
 * Please maintain this header in its entirety when copying/modifying
 * these files.
 * 
 ******************************************************************************/

/*******************************************************************************
 * Includes and defines
 ******************************************************************************/
#include "mcc_generated_files/mcc.h"
#include "PIC24_PIC33_I2C2.h"
#include "dsPIC33_STD.h"


/*******************************************************************************
 * Function:        unsigned char MasterWriteI2C2(unsigned char data_out)
 *
 * PreCondition:    I2C must have been initialized
 *
 * Input:           Data to be written to I2C2 module
 *
 * Output:          Negative number on I2C collision
 *
 * Overview:        An intermediate library function that writes to the I2C2 
 *                  module
 * 
 * Usage:           MasterWriteI2C2(0x00);
 *
 * Note:            None
 ******************************************************************************/
char MasterWriteI2C2(unsigned char data_out) 
{ 
   I2C2TRN = data_out; 
   if(I2C2STATbits.IWCOL)        /* If write collision occurs,return -1 */ 
       return -1; 
   else 
   { 
       while(I2C2STATbits.TRSTAT);   // wait until write cycle is complete 
       I2C2_IDLE();                  // ensure module is idle 
       if ( I2C2STATbits.ACKSTAT )   // test for ACK condition received 
     return ( -2 ); 
    else return ( 0 );              // if WCOL bit is not set return non-negative # 
   } 
} 


/*******************************************************************************
 * Function:        char MasterReadI2C2(void)
 *
 * PreCondition:    I2C must have been initialized
 *
 * Input:           None
 *
 * Output:          Data read via I2C
 *
 * Overview:        An intermediate library function that reads from the I2C2
 *                  module
 * 
 * Usage:           MasterWriteI2C2(0x00);
 *
 * Note:            None
 ******************************************************************************/
char MasterReadI2C2(void)
{
    I2C2CONbits.RCEN = 1;
    while(I2C2CONbits.RCEN);
    I2C2STATbits.I2COV = 0;
    return(I2C2RCV);
}

/******************************************************************************/
void I2C2_INIT()
{
    I2C2BRG  =    141;     //400Khz
    I2C2STAT = 0x0000;
    I2C2CONbits.I2CSIDL  =   0;
    I2C2CONbits.SCLREL   =   0;
    I2C2CONbits.A10M     =   0;
    I2C2CONbits.DISSLW   =   0;
    I2C2CONbits.SMEN     =   0;
    I2C2CONbits.GCEN     =   0;
    I2C2CONbits.STREN    =   0;
    I2C2CONbits.ACKDT    =   0;
    I2C2CONbits.ACKEN    =   0;
    I2C2CONbits.RCEN     =   0;
    I2C2CONbits.PEN      =   0;
    I2C2CONbits.RSEN     =   0;
    I2C2CONbits.SEN      =   0;
    I2C2CONbits.I2CEN    =   1;
}

/******************************************************************************/
void I2C2_IDLE(void)
{
    // Wait until I2C Bus is Inactive
    uint8_t exit;
    while(1)
    {
        exit = 1;
        if(I2C2STATbits.IWCOL) I2C2STATbits.IWCOL=0;
        if(I2C2CONbits.SEN) exit = 0;
        if(I2C2CONbits.PEN) exit = 0;
        if(I2C2CONbits.RCEN) exit = 0;
        if(I2C2CONbits.RSEN) exit = 0;
        if(I2C2CONbits.ACKEN) exit = 0;
        if(I2C2STATbits.TRSTAT) exit = 0;
        if(exit) break;
    }
    Nop();
}

/*******************************************************************************/
void I2C2_Write(uint8_t devAddr,uint16_t regAddr,uint8_t data)
{
    I2C2_IDLE();
    I2C2CONbits.SEN = 1;
    while (I2C2CONbits.SEN);
    IFS3bits.MI2C2IF = 0;
    
    MasterWriteI2C2(devAddr|0);
    MasterWriteI2C2(regAddr&0x00FF);
    MasterWriteI2C2(data);

    I2C2CONbits.PEN = 1;
    while(I2C2CONbits.PEN);                     
    IFS3bits.MI2C2IF = 0;
    __delay_ms(1);
}

/******************************************************************************/
void I2C2_WriteBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t data) 
{
    uint8_t b;
    b = I2C2_Read(devAddr, regAddr);
    
    b = (data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));
    I2C2_Write(devAddr, regAddr, b);
}

/******************************************************************************/
bool I2C2_WriteBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data) 
{
    //      010 value to write
    // 76543210 bit numbers
    //    xxx   args: bitStart=4, length=3
    // 00011100 mask byte
    // 10101111 original value (sample)
    // 10100011 original & ~mask
    // 10101011 masked | value
    uint8_t b = I2C2_Read(devAddr, regAddr);
    uint8_t mask = ((1 << length) - 1) << (bitStart - length + 1);
    data <<= (bitStart - length + 1); // shift data into correct position
    data &= mask; // zero all non-important bits in data
    b &= ~(mask); // zero all important bits in existing byte
    b |= data; // combine data with existing byte
    I2C2_Write(devAddr, regAddr, b);
    return true;
}

/******************************************************************************/
void I2C2_WriteBytes(uint8_t devAddr,uint8_t regAddr,uint8_t len,uint8_t *dptr)
{
    while(len--)
    {
        I2C2_Write(devAddr,regAddr,*dptr++);
    }
}

/******************************************************************************/
void I2C2_WriteWord(uint8_t devAddr,uint8_t regAddr,uint16_t data)
{
    I2C2_Write(devAddr, regAddr++, (data>>8)&0xFF);
    I2C2_Write(devAddr, regAddr, data&0xFF);
}

/******************************************************************************/
uint8_t I2C2_Read(uint8_t devAddr,uint16_t regAddr)
{
    uint8_t read_data=0;

    I2C2CONbits.SEN = 1;
    while (I2C2CONbits.SEN);
    IFS3bits.MI2C2IF = 0; 
    
    MasterWriteI2C2(devAddr|0);
    MasterWriteI2C2(regAddr&0x00FF);
    
    I2C2CONbits.RSEN = 1;
    while(I2C2CONbits.RSEN);
    IFS3bits.MI2C2IF = 0;  
    
    MasterWriteI2C2(devAddr|1);
    read_data = MasterReadI2C2();

    I2C2CONbits.PEN = 1;
    while(I2C2CONbits.PEN);
    IFS3bits.MI2C2IF = 0;
    return  read_data;
}

/******************************************************************************/
uint8_t I2C2readBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, 
        uint8_t *data) 
{
    uint8_t b=0;
    uint8_t count = I2C2_Read(devAddr, regAddr);
    *data = b & (1 << bitNum);
    return count;
}

/******************************************************************************/
uint8_t I2C2readBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, 
        uint8_t length, uint8_t *data)
{
    // 01101001 read byte
    // 76543210 bit numbers
    //    xxx   args: bitStart=4, length=3
    //    010   masked
    //   -> 010 shifted
    uint8_t count=0, b=0;
    b = I2C2_Read(devAddr, regAddr);
    uint8_t mask = ((1 << length) - 1) << (bitStart - length + 1);
    b &= mask;
    b >>= (bitStart - length + 1);
    *data = b;
    return count;
}

/******************************************************************************/
void I2C2readBytes(uint8_t devAddr,uint8_t regAddr,uint8_t len,uint8_t *dptr)
{
    while(len--)
    {
        *dptr = I2C2_Read(devAddr,regAddr);
    }
}

/*******************************************************************************
* Function:        StartI2C2()
*
* Input:		None.
*
* Output:		None.
*
* Overview:		Generates an I2C Start Condition
*
* Note:			None
*******************************************************************************/
uint8_t StartI2C2(void)
{
	//This function generates an I2C start condition and returns status 
	//of the Start.

	I2C2CONbits.SEN = 1;		//Generate Start COndition
	while (I2C2CONbits.SEN);	//Wait for Start COndition
	//return(I2C2STATbits.S);	//Optionally return status
    return 0;
}


/*******************************************************************************
* Function:        RestartI2C2()
*
* Input:		None.
*
* Output:		None.
*
* Overview:		Generates a restart condition and optionally returns status
*
* Note:			None
*******************************************************************************/
uint8_t RestartI2C2(void)
{
	//This function generates an I2C Restart condition and returns status 
	//of the Restart.

	I2C2CONbits.RSEN = 1;		//Generate Restart		
	while (I2C2CONbits.RSEN);	//Wait for restart	
	//return(I2C2STATbits.S);	//Optional - return status
    return 0;
}


/*******************************************************************************
* Function:        StopI2C2()
*
* Input:		None.
*
* Output:		None.
*
* Overview:		Generates a bus stop condition
*
* Note:			None
*******************************************************************************/
uint8_t StopI2C2(void)
{
	//This function generates an I2C stop condition and returns status 
	//of the Stop.

	I2C2CONbits.PEN = 1;		//Generate Stop Condition
	while (I2C2CONbits.PEN);	//Wait for Stop
	//return(I2C2STATbits.P);	//Optional - return status
    return 0;
}


/*******************************************************************************
* Function:        WriteI2C2()
*
* Input:		Byte to write.
*
* Output:		None.
*
* Overview:		Writes a byte out to the bus
*
* Note:			None
*******************************************************************************/
uint8_t WriteI2C2(unsigned char byte)
{
	//This function transmits the byte passed to the function
	//while (I2C2STATbits.TRSTAT);	//Wait for bus to be idle
	I2C2TRN = byte;					//Load byte to I2C2 Transmit buffer
	while (I2C2STATbits.TBF);		//wait for data transmission
    return 0;
}


/*********************************************************************
* Function:        IdleI2C2()
*
* Input:		None.
*
* Output:		None.
*
* Overview:		Waits for bus to become Idle
*
* Note:			None
********************************************************************/
uint8_t IdleI2C2(void)
{
	while (I2C2STATbits.TRSTAT);		//Wait for bus Idle
    return 0;
}


/*********************************************************************
* Function:        LDByteWriteI2C2()
*
* Input:		Control Byte, 8 - bit address, data.
*
* Output:		None.
*
* Overview:		Write a byte to low density device at address LowAdd
*
* Note:			None
********************************************************************/
uint8_t LDByteWriteI2C2(unsigned char ControlByte, unsigned char LowAdd, unsigned char data)
{
	uint8_t ErrorCode;

	IdleI2C2();						//Ensure Module is Idle
	StartI2C2();						//Generate Start COndition
	WriteI2C2(ControlByte);			//Write Control byte
	IdleI2C2();

	ErrorCode = ACKStatus2();		//Return ACK Status
	
	WriteI2C2(LowAdd);				//Write Low Address
	IdleI2C2();

	ErrorCode = ACKStatus2();		//Return ACK Status

	WriteI2C2(data);					//Write Data
	IdleI2C2();
	StopI2C2();						//Initiate Stop Condition
//	EEAckPolling2(ControlByte);		//Perform ACK polling
	return(ErrorCode);
}


/*********************************************************************
* Function:        LDByteReadI2C2()
*
* Input:		Control Byte, Address, *Data, Length.
*
* Output:		None.
*
* Overview:		Performs a low density read of Length bytes and stores in *Data array
*				starting at Address.
*
* Note:			None
********************************************************************/
uint8_t LDByteReadI2C2(unsigned char ControlByte, unsigned char Address, unsigned char *Data, unsigned char Length)
{
	IdleI2C2();					//wait for bus Idle
	StartI2C2();					//Generate Start Condition
	WriteI2C2(ControlByte);		//Write Control Byte
	IdleI2C2();					//wait for bus Idle
	WriteI2C2(Address);			//Write start address
	IdleI2C2();					//wait for bus Idle

	RestartI2C2();				//Generate restart condition
	WriteI2C2(ControlByte | 0x01);	//Write control byte for read
	IdleI2C2();					//wait for bus Idle

	getsI2C2(Data, Length);		//read Length number of bytes
	NotAckI2C2();				//Send Not Ack
	StopI2C2();					//Generate Stop
    return 0;
}

/*********************************************************************
* Function:        LDPageWriteI2C2()
*
* Input:		ControlByte, LowAdd, *wrptr.
*
* Output:		None.
*
* Overview:		Write a page of data from array pointed to be wrptr
*				starting at LowAdd
*
* Note:			LowAdd must start on a page boundary
********************************************************************/
uint8_t LDPageWriteI2C2(unsigned char ControlByte, unsigned char LowAdd, unsigned char *wrptr,unsigned char len)
{
	IdleI2C2();					//wait for bus Idle
	StartI2C2();					//Generate Start condition
	WriteI2C2(ControlByte);		//send controlbyte for a write
	IdleI2C2();					//wait for bus Idle
	WriteI2C2(LowAdd);			//send low address
	IdleI2C2();					//wait for bus Idle
	putstringI2C2(wrptr,len);		//send data
	IdleI2C2();					//wait for bus Idle
	StopI2C2();					//Generate Stop
	return(0);
}

/*********************************************************************
* Function:        LDSequentialReadI2C()
*
* Input:		ControlByte, address, *rdptr, length.
*
* Output:		None.
*
* Overview:		Performs a sequential read of length bytes starting at address
*				and places data in array pointed to by *rdptr
*
* Note:			None
********************************************************************/
uint8_t LDSequentialReadI2C2(unsigned char ControlByte, unsigned char address, unsigned char *rdptr, unsigned char length)
{
    if(length)
    {
        IdleI2C2();						//Ensure Module is Idle
        StartI2C2();						//Initiate start condition
        WriteI2C2(ControlByte);			//write 1 byte
        IdleI2C2();						//Ensure module is Idle
        WriteI2C2(address);				//Write word address
        IdleI2C2();						//Ensure module is idle
        RestartI2C2();					//Generate I2C Restart Condition
        WriteI2C2(ControlByte | 0x01);	//Write 1 byte - R/W bit should be 1 for read
        IdleI2C2();						//Ensure bus is idle
        getsI2C2(rdptr, length);			//Read in multiple bytes
        NotAckI2C2();					//Send Not Ack
        StopI2C2();						//Send stop condition
    }
	return(0);
}

/*********************************************************************
* Function:        ACKStatus2()
*
* Input:		None.
*
* Output:		Acknowledge Status.
*
* Overview:		Return the Acknowledge status on the bus
*
* Note:			None
********************************************************************/
uint8_t ACKStatus2(void)
{
	return (!I2C2STATbits.ACKSTAT);		//Return Ack Status
}


/*********************************************************************
* Function:        NotAckI2C2()
*
* Input:		None.
*
* Output:		None.
*
* Overview:		Generates a NO Acknowledge on the Bus
*
* Note:			None
********************************************************************/
uint8_t NotAckI2C2(void)
{
	I2C2CONbits.ACKDT = 1;			//Set for NotACk
	I2C2CONbits.ACKEN = 1;
	while(I2C2CONbits.ACKEN);		//wait for ACK to complete
	I2C2CONbits.ACKDT = 0;			//Set for NotACk
    return 0;
}


/*********************************************************************
* Function:        AckI2C2()
*
* Input:		None.
*
* Output:		None.
*
* Overview:		Generates an Acknowledge.
*
* Note:			None
********************************************************************/
uint8_t AckI2C2(void)
{
	I2C2CONbits.ACKDT = 0;			//Set for ACk
	I2C2CONbits.ACKEN = 1;
	while(I2C2CONbits.ACKEN);		//wait for ACK to complete
    return 0;
}


/*********************************************************************
* Function:       getsI2C2()
*
* Input:		array pointer, Length.
*
* Output:		None.
*
* Overview:		read Length number of Bytes into array
*
* Note:			None
********************************************************************/
uint8_t getsI2C2(unsigned char *rdptr, unsigned char Length)
{
	while (Length --)
	{
		*rdptr++ = getI2C2();		//get a single byte
		
		if(I2C2STATbits.BCL)		//Test for Bus collision
		{
			return(-1);
		}

		if(Length)
		{
			AckI2C2();				//Acknowledge until all read
		}
	}
	return(0);
}


/*********************************************************************
* Function:        getI2C2()
*
* Input:		None.
*
* Output:		contents of I2C2 receive buffer.
*
* Overview:		Read a single byte from Bus
*
* Note:			None
********************************************************************/
uint8_t getI2C2(void)
{
	I2C2CONbits.RCEN = 1;			//Enable Master receive
	Nop();
	while(!I2C2STATbits.RBF);		//Wait for receive bufer to be full
	return(I2C2RCV);				//Return data in buffer
}


/*********************************************************************
* Function:        putstringI2C2()
*
* Input:		pointer to array.
*
* Output:		None.
*
* Overview:		writes a string of data upto PAGESIZE from array
*
* Note:			None
********************************************************************/
uint8_t putstringI2C2(unsigned char *wrptr,uint8_t len)
{
	unsigned char x;

	for(x = 0; x < len; x++)		//Transmit Data Until Pagesize
	{	
		if(WriteI2C2(*wrptr))			//Write 1 byte
		{
			return(-3);				//Return with Write Collision
		}
		IdleI2C2();					//Wait for Idle bus
		if(I2C2STATbits.ACKSTAT)
		{
			return(-2);				//Bus responded with Not ACK
		}
		wrptr++;
	}
	return(0);
}
//------------------------------------------------------------------------------


