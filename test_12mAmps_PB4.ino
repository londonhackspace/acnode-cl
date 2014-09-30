
//#include <inc/hw_gpio.h>
#include <inc/tm4c129xnczad.h>

const int myPB4 = PB_4;
  
// the setup routine runs once when you press reset:
void setup() {                
 
  PB4_extra_mAmp_Init();  

}

// the loop routine runs over and over again forever:
void loop() {
  digitalWrite(myPB4, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);               // wait for a second
  digitalWrite(myPB4, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);               // wait for a second
}
void PB4_extra_mAmp_Init(){//TBD get input values 4,8,12 mA
  SYSCTL_RCGCGPIO_R |= 0x02;//Activate on port B
  while((SYSCTL_PRGPIO_R & SYSCTL_RCGCGPIO_R1) == 0){};//stabilize
  GPIO_PORTB_AHB_AMSEL_R &= ~0x10;//disable analog on PB4
  GPIO_PORTB_AHB_PCTL_R &= ~ 0x10;//PB4 as GPIO
  GPIO_PORTB_AHB_DIR_R |= 0x10;//PB4 output(default 2 mA outpur current)
  GPIO_PORTB_AHB_AFSEL_R &= ~0x10;//disable alt function
  GPIO_PORTB_AHB_DEN_R |= 0x10;//enable digital on PB4

  /*----Add this line if you want 4 mA---*/
  GPIO_PORTB_AHB_DR4R_R |= 0x10;//2 mA more total 4 so far
  /*----Add also this line if you want 8 mA---*/
  GPIO_PORTB_AHB_DR8R_R |= 0x10;//+4 mA more total 8 so far
  /*----Add also this line if you want 12 mA---*/
  GPIO_PORTB_AHB_DR12R_R |= 0x10;//+4 mA more total 12 so far 
}
