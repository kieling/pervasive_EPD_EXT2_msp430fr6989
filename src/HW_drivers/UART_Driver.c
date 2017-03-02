/**
* Copyright (c) Pervasive Displays Inc. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
*
* 3. The name of Atmel may not be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
* 4. This software may only be redistributed and used in connection with an
*    Atmel microcontroller product.
*
* THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
* EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
* OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
* ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
**/

#include <ringbuf.h>
#include <UART_Driver.h>
#include "driverlib.h"

//static Receive_handler _OnRecvHandle;
static tRingBufObject 				recvbufObj;
static uint8_t 						Recv_Buffer[Receive_Buf_Len];
static tRingBufObject           	transmitbufObj;
static  uint8_t  					transmit_Buffer[Transmit_Buf_Len];
void ConfigureUART(uint32_t Baudrate)
{
	//_OnRecvHandle=OnRecvHandle;
	//P4.4,5 = USCI_A1 TXD/RXD
	GPIO_setAsPeripheralModuleFunctionInputPin(
	    GPIO_PORT_P4,
	    GPIO_PIN4 + GPIO_PIN5,GPIO_PRIMARY_MODULE_FUNCTION
	);
	//Baudrate = 9600, clock freq = 24969216Hz
	//UCBRx = 0xa29, UCBRFx = 0, UCBRSx = 0, UCOS16 = 0
	//Baudrate = 115200, clock freq = 24969216Hz
	//UCBRx = 0xD8, UCBRFx = 0x0d, UCBRSx = 0x0c, UCOS16 = 0
	EUSCI_A_UART_initParam param = {0};
	param.selectClockSource = EUSCI_A_UART_CLOCKSOURCE_SMCLK;
	param.clockPrescalar =13;;	//UCAxBRW
	param.firstModReg =9;;		//UCAxMCTL=>UCBRFx
	param.secondModReg =0;;		//UCAxMCTL=>UCBRSx
	param.parity = EUSCI_A_UART_NO_PARITY;
	param.msborLsbFirst = EUSCI_A_UART_LSB_FIRST;
	param.numberofStopBits = EUSCI_A_UART_ONE_STOP_BIT;
	param.uartMode = EUSCI_A_UART_MODE;
	param.overSampling = EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION;//UCAxMCTL=>UCOS16
	EUSCI_A_UART_init(EUSCI_A1_BASE, &param);
	//Enable UART module for operation
	EUSCI_A_UART_enable(EUSCI_A1_BASE);

	//Enable Receive Interrupt
	EUSCI_A_UART_clearInterrupt(EUSCI_A1_BASE,
	                           EUSCI_A_UART_RECEIVE_INTERRUPT);
	EUSCI_A_UART_enableInterrupt(EUSCI_A1_BASE,
	                            EUSCI_A_UART_RECEIVE_INTERRUPT);

	RingBufInit(&recvbufObj,(uint8_t *)&Recv_Buffer,sizeof(Recv_Buffer));
	RingBufInit(&transmitbufObj,(uint8_t *)&transmit_Buffer,sizeof(transmit_Buffer));
}

void UartSend(const uint8_t *pui8Buffer, uint32_t ui32Count)
{
	RingBufWrite(&transmitbufObj,(uint8_t *)pui8Buffer,ui32Count);
	EUSCI_A_UART_enableInterrupt(EUSCI_A1_BASE, EUSCI_A_UART_TRANSMIT_INTERRUPT);
	/*
	while(ui32Count--)
	{
		RingBuf
		RingBufWriteOne(&transmitbufObj,receivedData);
		USCI_A_UART_transmitData(USCI_A1_BASE,*pui8Buffer++);
	}
	*/
}
void Uart_PeekRecvDatas(uint8_t *datas,uint8_t len)
{
	RingBufPeek(&recvbufObj,datas,len);
}
uint32_t Uart_RecvAvail(void)
{

	return RingBufUsed(&recvbufObj);
}

bool UartRecv(uint8_t * RecvBuf,uint32_t len)
{
	uint8_t flag=false;
	if(len>0)
	{
		//__disable_interrupt();
		if(len<=RingBufUsed(&recvbufObj))
		{
			RingBufRead(&recvbufObj,RecvBuf, len);
			flag=true;
		}
		//__enable_interrupt();
	}
	return flag;

}

void ClearUartBuffer(void)
{
	RingBufFlush(&recvbufObj);
	RingBufFlush(&transmitbufObj);
}
void handle_uartA1_tx_interrupt(void)
{
	if (RingBufUsed(&transmitbufObj)>0 )
	{

		EUSCI_A_UART_transmitData(EUSCI_A1_BASE, RingBufReadOne(&transmitbufObj));

	}
	else
	{

		EUSCI_A_UART_disableInterrupt(EUSCI_A1_BASE, EUSCI_A_UART_TRANSMIT_INTERRUPT);
	}
}
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_A1_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(USCI_A1_VECTOR)))
#endif
void USCI_A1_ISR(void)
{
	if (EUSCI_A_UART_getInterruptStatus(EUSCI_A1_BASE, EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG))
	{
		handle_uartA1_tx_interrupt();
	}
	if (EUSCI_A_UART_getInterruptStatus(EUSCI_A1_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG))
	{
		RingBufWriteOne(&recvbufObj, EUSCI_A_UART_receiveData(EUSCI_A1_BASE));
		/*
		uint8_t receivedData;
		switch(__even_in_range(UCA1IV,4))
		{
			//Vector 2 - RXIFG
			case 2:
				receivedData = USCI_A_UART_receiveData(USCI_A1_BASE);
				RingBufWriteOne(&recvbufObj,receivedData);
				//if(_OnRecvHandle!=NULL)_OnRecvHandle(receivedData);
				break;
			default: break;
		}
		*/
	}
}
