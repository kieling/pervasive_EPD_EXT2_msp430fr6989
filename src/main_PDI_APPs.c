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
#include "conf_EPD.h"
#include "BootTask_Module.h"
#include "Data_Module.h"

void main(void)
{
	//Stop WDT
	WDT_A_hold(WDT_A_BASE);

	GPIO_setOutputLowOnPin(GPIO_PORT_PA, 0xFFFF);
	GPIO_setAsOutputPin(GPIO_PORT_PA, 0xFFFF);
	GPIO_setOutputLowOnPin(GPIO_PORT_PB, 0xFFFF);
	GPIO_setAsOutputPin(GPIO_PORT_PB, 0xFFFF);
	GPIO_setOutputLowOnPin(GPIO_PORT_PC, 0xFFFF);
	GPIO_setAsOutputPin(GPIO_PORT_PC, 0xFFFF);
	GPIO_setOutputLowOnPin(GPIO_PORT_PD, 0xFFFF);
	GPIO_setAsOutputPin(GPIO_PORT_PD, 0xFFFF);
	GPIO_setOutputLowOnPin(GPIO_PORT_PE, 0xFFFF);
	GPIO_setAsOutputPin(GPIO_PORT_PE, 0xFFFF);
	GPIO_setOutputLowOnPin(GPIO_PORT_PJ, 0xFF);
	GPIO_setAsOutputPin(GPIO_PORT_PJ, 0xFF);

	// Configure PJ.4 and PJ.5 as input pins for LFXIN and LFXOUT mode
	GPIO_setAsPeripheralModuleFunctionInputPin(
		GPIO_PORT_PJ,
		GPIO_PIN4 + GPIO_PIN5,
		GPIO_PRIMARY_MODULE_FUNCTION
	);

    // Disable the GPIO power-on default high-impedance mode to activate previously configured port settings
    PMM_unlockLPM5();

	// Initialize EINK
    GPIO_setAsOutputPin(GPIO_PORT_P2,GPIO_PIN7);
    GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN7);

	system_init();
	EPD_display_init();

	Systick_Configuration();
	Rtos_init();
	Data_Module_init();
	BootTask_Module_init();

    Set_AssignEPD_Drive(EPD_Type, EPD_Size, USE_Temperature_Sensor);
    EPD_display_GU_from_pointer(Image2,Image1,GU_Mode,true);

	while(1)
	{
		EPD_display_GU_from_pointer(Image1,Image2,GU_Mode,true);
		EPD_delay_ms(3000); // approx 1 seg

		EPD_display_GU_from_pointer(Image2,Image1,GU_Mode,true);
		EPD_delay_ms(3000); // approx 1 seg
	}

}
