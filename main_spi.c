#include <p24f16KA301.h>

_FOSCSEL (FNOSC_FRCDIV);   //8mHz with Post-scaling

unsigned long microseconds = 0;
unsigned long milliseconds = 0;


void spi_config()
{
	_DISSCK = 0;	//Internal SPIx clock is enabled
	_DISSDO = 0;	//SDOx pin is controlled by the module
	_MODE16 = 0;	//Communication is byte-wide (8 bits)
	_SMP = 1;		//Input data is sampled at the end of data output time
	_CKE = 1;		//Serial output data changes on transition from active clock state to Idle clock state (see bit 6)
	_SSEN = 0;		//SSx pin is not used by the module; pin is controlled by port function
	_CKP = 0;		//Idle state for clock is a low level; active state is a high level
	_MSTEN = 1;		//Master mode
	_SPRE = 0b110;	//Secondary prescale 2:1
	_PPRE = 0b10;	//Primary prescale 4:1
	
	_FRMEN = 0;		//Framed SPIx support is disabled
	_SPIBEN = 0;	//Enhanced buffer is disabled (Legacy mode)

	_SPIROV = 0;	//No overflow has occurred
	_SPIEN = 1;		//Enables module and configures SCKx, SDOx, SDIx and SSx as serial port pins
}


void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void)
{
    // Remember to clear the Timer1 interrupt flag when this ISR is entered.
    _T1IF = 0; // Clear interrupt flag
    
	microseconds = microseconds + 100;
	
	countTimer++;
	
	if(countTimer >= 10)
    {
		milliseconds++;
		countTimer = 0;
	}
}


void timing_interrupt_config()
{
	_TON = 1;	//Turn on timer 1, for general use
	
    _TCKPS = 0b00;  //Timer pre-scaler set to 1:1, page 139
    _RCDIV = 0b011; //1mHz (with 8mHz), divide by 8
    
    // Configure Timer1 interrupt
    _T1IP = 5;      // Select interrupt priority
    _T1IE = 1;      // Enable interrupt
    _T1IF = 0;      // Clear interrupt flag
    PR1 = 50;    // Count to 100 micro-sec at 1 mHz, instruct at 500 kHz
}

/***********************************************************************************************/

int main()
{
	spi_config();
	timing_interrupt_config();
	
	unsigned long startTime;
	
	while(1)
	{
		_SPI1BUF = 0xBA;
		
		startTime = milliseconds;
		while((milliseconds - startTime) <= 10)
		{
			//do nothing
		}
		
	}
}


/* To set up the SPI1 module for the Standard Master
mode of operation:
1. If using interrupts:
a) Clear the SPI1IF bit in the IFS0 register.
b) Set the SPI1IE bit in the IEC0 register.
c) Write the respective SPI1IPx bits in the
IPC2 register to set the interrupt priority.
2. Write the desired settings to the SPI1CON1 and
SPI1CON2 registers with the MSTEN bit
(SPI1CON1<5>) = 1.
3. Clear the SPIROV bit (SPI1STAT<6>).
4. Enable SPI operation by setting the SPIEN bit
(SPI1STAT<15>).
5. Write the data to be transmitted to the SPI1BUF
register. Transmission (and reception) will start
as soon as data is written to the SPI1BUF
register.
To set up the SPI1 module for the Standard Slave mode
of operation:
1. Clear the SPI1BUF register.
2. If using interrupts:
a) Clear the SPI1IF bit in the IFS0 register.
b) Set the SPI1IE bit in the IEC0 register.
c) Write the respective SPI1IPx bits in the
IPC2 register to set the interrupt priority.
3. Write the desired settings to the SPI1CON1
and SPI1CON2 registers with the MSTEN bit
(SPI1CON1<5>) = 0.
4. Clear the SMP bit.
5. If the CKE bit is set, then the SSEN bit
(SPI1CON1<7>) must be set to enable the SS1
pin.
6. Clear the SPIROV bit (SPI1STAT<6>).
7. Enable SPI operation by setting the SPIEN bit
(SPI1STAT<15>). */