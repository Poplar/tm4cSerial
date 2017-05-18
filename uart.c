#define PART_TM4C1294NCPDT
#include "inc/tm4c1294ncpdt.h"
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"

uint32_t g_ui32SysClock;

#ifdef DEBUG
  void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif

  void
UARTIntHandler(void)
{
  uint32_t ui32Status;
  uint32_t buff[100];
  uint32_t index = 0,size;
  // Get the interrrupt status.
  ui32Status = UARTIntStatus(UART0_BASE, true);

  // Clear the asserted interrupts.
  uartintclear(UART0_base, ui32status);

  // loop while there are characters in the receive fifo.
  while(uartcharsavail(uart0_base))
  {
    // read the next character from the uart and write it back to the uart.
/*    uartcharputnonblocking(uart0_base,
        uartchargetnonblocking(uart0_base)); */ //important part
    
    buff[index] = uartchargetnonblocking(uart0_base);
    index++;
    size++;
    /* blink led on receive; use for debug only
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, GPIO_PIN_0);
    SysCtlDelay(g_ui32SysClock / (1000 * 3));
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, 0); 
    */
  }
  UARTSend(buff, size);
  size = 0;
  index = 0;
}

  void
UARTSend(const uint8_t *pui8Buffer, uint32_t ui32Count)
{
  // Loop while there are more characters to send.
  while(ui32Count--)
  {
    // Write the next character to the UART.
    UARTCharPutNonBlocking(UART0_BASE, *pui8Buffer++);
  }
}

int main(void)
{
  // Set the clocking to run directly from the crystal at 120MHz.
  g_ui32SysClock = MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
        SYSCTL_OSC_MAIN |
        SYSCTL_USE_PLL |
        SYSCTL_CFG_VCO_480), 120000000);
  // Enable the GPIO port that is used for the on-board LED.
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);

  // Enable the GPIO pins for the LED (PN0).
  GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0);

  // Enable the peripherals used by this example.
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

  // Enable processor interrupts.
  IntMasterEnable();

  // Set GPIO A0 and A1 as UART pins.
  GPIOPinConfigure(GPIO_PA0_U0RX);
  GPIOPinConfigure(GPIO_PA1_U0TX);
  GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

  // Configure the UART for 115,200, 8-N-1 operation.
  UARTConfigSetExpClk(UART0_BASE, g_ui32SysClock, 115200,
      (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
       UART_CONFIG_PAR_NONE));

  // Enable the UART interrupt.
  IntEnable(INT_UART0);
  UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);

  // Prompt for text to be entered.
  UARTSend((uint8_t *)"\033[2JEnter text: ", 16);

  // Loop forever echoing data through the UART.
  while(1)
  {
  }
}
