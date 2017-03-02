#include "conf_EPD.h"

/* Glossary of Acronyms
 * GU = Global Update
 * PU = Partial Update
 * LUT = Look-up table, the array to define the parameters of driving waveform
 * OTP = One Time Programming, the LUTs are pre-programmed in driver IC
 */

void main(void)
{
	//Stop WDT
	WDT_A_hold(WDT_A_BASE);

    // Set all GPIO pins to output low to prevent floating input and reduce power consumption
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

	// Initialize EINK (using GPIO as VCC pin)
    GPIO_setAsOutputPin(GPIO_PORT_P2,GPIO_PIN7);
    GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN7);

    system_init();
    EPD_display_init();
    Systick_Configuration();
    EPD_delay_ms(1000);
    Set_AssignEPD_Drive(EPD_Type, EPD_Size, USE_Temperature_Sensor);

    while(true)
    {
    	// Partial update
        EPD_display_PU_from_pointer(Image2,Image1,true,0);
        EPD_delay_ms(3000);

        // Global update
        EPD_display_GU_from_pointer(Image1,Image2,GU_Mode,true);
        EPD_delay_ms(3000);
    }
}

