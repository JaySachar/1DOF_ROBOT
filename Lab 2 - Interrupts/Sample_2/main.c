#include <stdint.h>
#include <stdio.h>
#include "PLL.h"
#include "tm4c123gh6pm.h"

void SysTickInit(void);
void SysTickHandler(void);
void PortFInit(void);
void GPIOInterruptInit(void);
void SW2_Handler(void);
void enable_interrupts(void);
int main(void)
{
    PLL_Init();                 // bus clock at 80 MHz
    PortFInit();
    SysTickInit();
    while(1){};
}

void SysTickHandler(void){

    // Using XOR about the PF2 pin. The other pins will stay the same
    // since 0 XOR 0 = 0, and 1 XOR 0 is 1.
    // When PF2 is 1, 1 XOR 1 is 0. When PF2 is 0, 0 XOR 1 is 1
    GPIO_PORTF_DATA_R ^= 0x04; // 0000 0010
   // toggle bit at PF2
}
void SysTickInit(void){
    // Disable timer
    NVIC_ST_CTRL_R = 0;
    // Set reload value to be exactly 0.2 [ms]
    NVIC_ST_RELOAD_R = 0x00F42400;
    // Clear the current value register. This register contains the current value at the time the register is accessed
    NVIC_ST_CURRENT_R = 0;
    NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x40000000;
    // 0x00000007 -> for the last 4 bits: 0111, which sets CLK_SRC to system clock
    // Enables ITERN (interrupts generated to the NVIC when SysTick counts to 0)
    // Enables the counter. Here, it loads in the reload value, and begins counting down. When reaching 0, the count bit is set, and an interrupt is generated
    NVIC_ST_CTRL_R = 0x00000007;
}

void PortFInit(void) {
    SYSCTL_RCGC2_R |= 0x00000020;
    while ((SYSCTL_PRGPIO_R & 0x00000020) == 0) {};
    GPIO_PORTF_LOCK_R = 0x4C4F434B;
    GPIO_PORTF_CR_R = 0x1F;
    GPIO_PORTF_AMSEL_R = 0x00;
    GPIO_PORTF_PCTL_R = 0x00000000;
    GPIO_PORTF_DIR_R = 0x0E;
    GPIO_PORTF_AFSEL_R = 0x00;
    GPIO_PORTF_PUR_R = 0x11;
    GPIO_PORTF_DEN_R = 0x1F;

    // Enable the Interrupt for SW2
    // Interrupts are enabled/disabled via GPIO_IM (GPIOIM Register pg. 667)
    GPIO_PORTF_IS_R &= ~0x10; // Set PF4 to be edge-sensitive (~0001.0000 -> 1110.1111)
    GPIO_PORTF_IBE_R &= ~0x10; // Set PF4 to not be triggered by both edges
    GPIO_PORTF_IEV_R |= 0x10; // Set PF4 to be rising edge base, so it is normally high level, button pressed, low level, button is not longer depressed, high level, ISR called
    GPIO_PORTF_ICR_R |= 0x10; // Clear any Interrupts on PF4
    GPIO_PORTF_IM_R |= 0x10; // Port 4 (SW1 Interrupt Enabled)
    // Port F is Vector Number 46, Interrupt Number 30
    // Interrupt Number 30, so Register 36 (PRI7)
    NVIC_PRI7_R =(NVIC_PRI7_R & 0xFF1EFFFF)|0x00A00000;



    // Bits 23:21 are set to 101 (5, middle priority),and are associated with 4n+2 (4*7+2 = 30) so interrupt 30 is set to a priority of 5
 /*   000 XXXXX 000 XXXXX 000 XXXXX 000 XXXXX // NVIOC_PRI7_R initially. We want to target bits 23:21 while preserving the reserved bits values (XXXXX)
   &111 11111 000 11111 111 11111 111 11111 // Preserves all other bits, while setting bits 23:21 to 0
   _________________________________
    000 XXXXX 000 XXXXX 000 XXXXX 000 XXXXX
   |000 00000 101 00000 000 00000 000 00000
   _________________________________
    000 XXXXX 101 XXXXX 000 XXXXX 000 XXXXX
*/
    // Enable interrupt for interrupt 30 (EN0)
    NVIC_EN0_R = (1 << 30); // Enable interrupts for Port F (Interrupt 30)
                            // NVIC_EN0_R = 0100 0000 0000 0000 0000 0000 0000 0000 == 0x4000.0000


}

void SW2_Handler(void){
    GPIO_PORTF_DATA_R ^= 0x02; // 0000 0100
    GPIO_PORTF_ICR_R |= 0x10; // Clear any Interrupts on PF4
    GPIO_PORTF_IM_R |= 0x10; // Port 4 (SW1 Interrupt Enabled)

}

void enable_interrupts(void) {
    __asm("    CPSIE  I\n"
          "    BX     LR");
}
