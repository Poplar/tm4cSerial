#define PART_TM4C1294NCPDT
#include "inc/tm4c1294ncpdt.h"

#include <stdint.h>
#include <stdbool.h>

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

void UARTIntHandler(void)
{
}

void delay(uint32_t cycles)
{
  volatile uint32_t i;
  for (i = 0; i < cycles; i++)
    ; // do nothing
}

void setup(void)
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
}

float doPID(float realVelocity, float targetVelocity)
{
  (void) realVelocity;
  (void) targetVelocity;

  return 0;
}

enum { CMD_NONE, CMD_STOP, CMD_CLOSEDLOOP, CMD_OPENLOOP };

float encoderGetVelocity(void)
{
  return 0.0;
}

void setPWM(float dutyCycle)
{
  (void) dutyCycle;

}

void sendStatus(int cmd, float velocity, float dutyCycle)
{
  uint8_t buf[12];

  buf[0] = 0xff;			// start byte 1
  buf[1] = 0xff;			// start byte 2
  buf[2] = cmd;				// command
  buf[3] = ((uint8_t *)&velocity)[0];	
  buf[4] = ((uint8_t *)&velocity)[1];
  buf[5] = ((uint8_t *)&velocity)[2];
  buf[6] = ((uint8_t *)&velocity)[3];
  buf[7] = ((uint8_t *)&dutyCycle)[0];
  buf[8] = ((uint8_t *)&dutyCycle)[1];
  buf[9] = ((uint8_t *)&dutyCycle)[2];
  buf[10] = ((uint8_t *)&dutyCycle)[3];
  buf[11] = 0xcc;			// checksum

  for (int i = 0; i < 12; i++)
    UARTCharPut(UART0_BASE, buf[i]);
}


int readCmd(int *cmd, float *velocity)
{
  static uint8_t led = GPIO_PIN_0;
  
  static int i, c;
  static char buf[8];

  if ((c = UARTCharGetNonBlocking(UART0_BASE)) != -1)
    buf[i++] = c;
  
  if (i == 8) {
    i = 0;
    *cmd = buf[2];
    *velocity = *((float *)&(buf[3]));

    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, led);
    led ^= GPIO_PIN_0;
    return 1;
  }

  return 0;
}


int main(void)
{
  setup();

  float realVelocity, targetVelocity, dutyCycle;
  int rv, cmd = CMD_STOP;

  targetVelocity = 21.3;
  
  while (1) {
    realVelocity = encoderGetVelocity();

    if (cmd == CMD_STOP)
      setPWM(0);
    else if (cmd == CMD_OPENLOOP) {
      setPWM(dutyCycle);
      sendStatus(cmd, targetVelocity, dutyCycle);
    }
    else if (cmd == CMD_CLOSEDLOOP) {
      dutyCycle = doPID(realVelocity, targetVelocity);
      setPWM(dutyCycle);
      sendStatus(cmd, targetVelocity, dutyCycle);
    }

    rv = readCmd(&cmd, &targetVelocity);
    if (rv == 1 && cmd == CMD_STOP)
      sendStatus(cmd, targetVelocity, dutyCycle);

    // replace this with sleep until timer interrupt
    // delay(2000000);
    delay(2000);
  }
}
