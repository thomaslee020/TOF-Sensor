#include <stdint.h>
#include "tm4c1294ncpdt.h"
#include "Systick.h"
#include "PLL.h"


volatile uint32_t ADCvalue;

//-ADC0_InSeq3-
// Busy-wait analog to digital conversion. 0 to 3.3V maps to 0 to 4095 
// Input: none 
// Output: 12-bit result of ADC conversion 
uint32_t ADC0_InSeq3(void){
	uint32_t result;
	
	ADC0_PSSI_R = 0x0008;														// 1) initiate SS3   
	while((ADC0_RIS_R&0x08)==0){}										// 2) wait for conversion done   
	result = ADC0_SSFIFO3_R&0xFFF;									// 3) read 12-bit result   
	ADC0_ISC_R = 0x0008;														// 4) acknowledge completion   
	
	return result; 
} 

void ADC_Init(void){
	//config the ADC from Valvano textbook
	SYSCTL_RCGCGPIO_R |= 0b00010000;								// 1. activate clock for port E
	while ((SYSCTL_PRGPIO_R & 0b00010000) == 0) {}	//		wait for clock/port to be ready
	GPIO_PORTE_DIR_R &= ~0x10;											// 2) make PE4 input   
	GPIO_PORTE_AFSEL_R |= 0x10;											// 3) enable alternate function on PE4   
	GPIO_PORTE_DEN_R &= ~0x10;											// 4) disable digital I/O on PE4   
	GPIO_PORTE_AMSEL_R |= 0x10;											// 5) enable analog function on PE4   
	SYSCTL_RCGCADC_R |= 0x01;												// 6) activate ADC0   
	ADC0_PC_R = 0x01;																// 7) maximum speed is 125K samples/sec   
	ADC0_SSPRI_R = 0x0123;													// 8) Sequencer 3 is highest priority   
	ADC0_ACTSS_R &= ~0x0008;												// 9) disable sample sequencer 3   
	ADC0_EMUX_R &= ~0xF000;													// 10) seq3 is software trigger 
	ADC0_SSMUX3_R = 9;															// 11) set channel Ain9 (PE4)   
	ADC0_SSCTL3_R = 0x0006;													// 12) no TS0 D0, yes IE0 END0   
	ADC0_IM_R &= ~0x0008;														// 13) disable SS3 interrupts   
	ADC0_ACTSS_R |= 0x0008;													// 14) enable sample sequencer 3 
	//==============================================================================	
	
	return;
}

void PortH_Init(void){
   //Use PortH0
	 SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R7; //ACTIVATE CLOCK FOR PORT H
	 while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R7)==0){}; //ALLOW TIME FOR CLOCK TO STABILIZE
	 GPIO_PORTH_DIR_R |= 0x0F; //PH0,PH1,PH2,PH3 out
	 GPIO_PORTH_AFSEL_R &= ~0x0F; //DISABLE ALT FUNCT ON PH0,PH1,PH2,PH3 OUT
   GPIO_PORTH_DEN_R |= 0x0F; //ENABLE DIGITAL i/O ON PH0,PH1,PH2,PH3
   GPIO_PORTH_AMSEL_R &= ~0x0F;		 
   
	 GPIO_PORTH_DATA_R ^= 0b00001111; 								//hello world!

	 return;
}	


uint32_t func_debug[100];// array used to store realtime data 	


//Displacement
void PortL0_Init(void){	//keypad ouput
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R10;		              // activate the clock for Port L
	while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R10) == 0){};	        // allow time for clock to stabilize
  GPIO_PORTL_DEN_R = 0b00001111; 		// Enabled both as digital outputs
	GPIO_PORTL_DIR_R = 0b00000001;
	GPIO_PORTL_DATA_R = 0b0000001;
  return;
	}

//Distance
void PortN0_Init(void){
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R12;                 //activate the clock for Port N
	while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R12) == 0){};
	GPIO_PORTN_DIR_R=		 0b00000011;
	GPIO_PORTN_DEN_R=		 0b00000011;
	GPIO_PORTN_DATA_R ^= 0b00000000; 
	return;
}

void PortM0_Init(void){ //button input
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R11;                 //activate the clock for Port M
	while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R11) == 0){};        //allow time for clock to stabilize 
	GPIO_PORTM_DIR_R = 0b00000000;       								    // make PM0 an input, PM0 is reading if the button is pressed or not 
  GPIO_PORTM_DEN_R = 0b00001111;

	return;
}


void Distance(i){ //64 inner or 8 outer (i)
	if (i%8 == 0){
  GPIO_PORTN_DATA_R &= 0b00000000; //ensure 0
	GPIO_PORTN_DATA_R |= 0b00000001; //light on
	SysTick_Wait10ms(5);	 					 //delay
	GPIO_PORTN_DATA_R = 0b00000000;	 //light off
}
}

void Status(){ //called after rotation finished
	GPIO_PORTL_DATA_R = 0b00000001; //light on 
}


int main(void){

	PortL0_Init();
	PortM0_Init();
	PortN0_Init();
	//PortF4F0_Init();
	PortH_Init();
	//PortE0_Init();
	
	PLL_Init();
	SysTick_Init();
	ADC_Init();
	
	uint8_t delay = 2;
	uint8_t count = 0;

while(1){
		if (GPIO_PORTM_DATA_R == 0b00000000){ //evaluates true if zero
		GPIO_PORTN_DATA_R &= 0b00000000; //
		for(int i =0; i<64; i++){
			GPIO_PORTL_DATA_R &= 0b00000000;
			GPIO_PORTL_DATA_R |= 0b00000000;
			count = 0;
			while(count<8){
			GPIO_PORTH_DATA_R &= 0b00000000;
			GPIO_PORTH_DATA_R |= 0b00001100;
			SysTick_Wait10ms(delay);
			
			GPIO_PORTH_DATA_R &= 0b00000000;
			GPIO_PORTH_DATA_R |= 0b00000110;
			SysTick_Wait10ms(delay);
			
			GPIO_PORTH_DATA_R &= 0b00000000;
			GPIO_PORTH_DATA_R |= 0b00000011;
			SysTick_Wait10ms(delay);

			GPIO_PORTH_DATA_R &= 0b00000000;
			GPIO_PORTH_DATA_R |= 0b00001001;
			SysTick_Wait10ms(delay);	
			count++;
			}
			//for(int j=1;i<=8;i++){ 8*64 = 512 points per 360 degrees scanned, datasheet will correspond to 20 scans at different x values
				//while(data==0){ when data is able to be taken in
				//flash LED (indicate data taken)
				//set data to 0 again
				//implement VL53LIX distance functionS which returns the sensored distance to the surface
				//interrupt
				//stop tof sensor with built in function
				//reloop at next point
			Distance(i); //onboard
		}	
		Status(); //external
	}
}	
}






	
		

