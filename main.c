/**
 * PROYECTO 3 - ELECTRONICA DIGITAL 2
 * PARQUEO INTELIGENTE
 *
 * DESARROLLADO POR:
 *      JUDAH PÉREZ - 21536
 *      DANIEL VALDEZ -
 *
 * HARDWARE:
 *      SENSORES (LDR)  -> PORTE[0:3] = Analog Read[3:0]
 *      LEDS ROJOS      -> PORTD[0:3]
 *      LEDS VERDES     -> PORTC[4:7]
 *      UART ESP32      -> PORTD[6:7] = UART2 {Rx,Tx}
 *
 */

//**************************************************************************************************************
// Librerías
//**************************************************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
//#include "inc/hw_ints.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/systick.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"

//**************************************************************************************************************
// Variables Globales
//**************************************************************************************************************
#define SENSORES_BASE  GPIO_PORTE_BASE
#define LED_ROJO_BASE  GPIO_PORTD_BASE
#define LED_VERDE_BASE GPIO_PORTC_BASE
#define SENSORES_MASK  GPIO_PIN_3|GPIO_PIN_2|GPIO_PIN_1|GPIO_PIN_0
#define LED_ROJO_MASK  GPIO_PIN_3|GPIO_PIN_2|GPIO_PIN_1|GPIO_PIN_0
#define LED_VERDE_MASK GPIO_PIN_7|GPIO_PIN_6|GPIO_PIN_5|GPIO_PIN_4

uint8_t sensorValues = 0x02;


int main(void)
{
    // configuración del reloj a 40MHz con el oscilador externo de 16MHz y PLL
    SysCtlClockSet(SYSCTL_OSC_MAIN|SYSCTL_USE_PLL|SYSCTL_SYSDIV_5|SYSCTL_XTAL_16MHZ);

    // Configurar Puerto E - Sensores
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    GPIOPinTypeGPIOInput(SENSORES_BASE, SENSORES_MASK);

    // Configurar Puerto D - LEDS rojos
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    GPIOPinTypeGPIOOutput(LED_ROJO_BASE, LED_ROJO_MASK);

    // Configurar Puerto C - LEDS verdes
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    GPIOPinTypeGPIOOutput(LED_VERDE_BASE, LED_VERDE_MASK);

    //Habilitar módulo UART2
    //SysCtlPeripheralEnable(SYSCTL_PERIPH_UART2);

    //**********************************************************************************************************
    // Loop Principal
    //**********************************************************************************************************
    while (1)
    {
        GPIOPinWrite(LED_ROJO_BASE, LED_ROJO_MASK, sensorValues); //Sensor indica ocupado
        GPIOPinWrite(LED_VERDE_BASE, LED_VERDE_MASK, (~sensorValues)<<4);
        sensorValues++;
        SysCtlDelay(90000000);
    }
}
