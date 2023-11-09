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
#define SENSORES_PINS  GPIO_PIN_3|GPIO_PIN_2|GPIO_PIN_1|GPIO_PIN_0
#define LED_ROJO_MASK  GPIO_PIN_3|GPIO_PIN_2|GPIO_PIN_1|GPIO_PIN_0
#define LED_VERDE_MASK GPIO_PIN_7|GPIO_PIN_6|GPIO_PIN_5|GPIO_PIN_4
#define UART2_PINS     GPIO_PIN_7|GPIO_PIN_6

uint8_t sensorValues;
uint8_t parqueosDisponibles;
uint8_t parqueosOcupados;

//**************************************************************************************************************
// Prototipos de Función
//**************************************************************************************************************
void sendUART0(void);
void sendUART2(void);
uint8_t countSetBits(uint8_t bitwise);

//**************************************************************************************************************
// Código Principal
//**************************************************************************************************************
int main(void)
{
    // configuración del reloj a 40MHz con el oscilador externo de 16MHz y PLL
    SysCtlClockSet(SYSCTL_OSC_MAIN|SYSCTL_USE_PLL|SYSCTL_SYSDIV_5|SYSCTL_XTAL_16MHZ);

    // Configurar Puerto E - Sensores
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    GPIOPinTypeGPIOInput(SENSORES_BASE, SENSORES_PINS);

    // Configurar Puerto D - LEDS rojos
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    GPIOPinTypeGPIOOutput(LED_ROJO_BASE, LED_ROJO_MASK);

    // Configurar Puerto C - LEDS verdes
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    GPIOPinTypeGPIOOutput(LED_VERDE_BASE, LED_VERDE_MASK);

    //Habilitar módulo UART2 para la comunicación con ESP32
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART2);
    GPIOPinTypeUART(GPIO_PORTD_BASE, UART2_PINS);
    UARTConfigSetExpClk(UART2_BASE, SysCtlClockGet(), 115200,
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    //Habilitar módulo UART0 para la comunicación con computadora
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));



    //**********************************************************************************************************
    // Loop Principal
    //**********************************************************************************************************
    while (1)
    {
        sensorValues = GPIOPinRead(SENSORES_BASE, SENSORES_PINS);   //Lectura de sensores
        parqueosOcupados = sensorValues;            //Sensor tapado indica ocupado
        parqueosDisponibles = (~sensorValues) & 0x0F;
        GPIOPinWrite(LED_ROJO_BASE, LED_ROJO_MASK, parqueosOcupados);
        GPIOPinWrite(LED_VERDE_BASE, LED_VERDE_MASK, parqueosDisponibles<<4); //Corrimiento debido a que los pines de los leds verdes son [7:4]

        sendUART0();
        SysCtlDelay(300000);
    }
}

//**********************************************************************************************************
// Envio de datos a la computadora
//**********************************************************************************************************
void sendUART0(void){
    uint8_t disponibles = countSetBits(parqueosDisponibles) + '0';
    uint8_t ocupados = countSetBits(parqueosOcupados) + '0';
    UARTCharPut(UART0_BASE,'D');
    UARTCharPut(UART0_BASE,':');
    UARTCharPut(UART0_BASE,disponibles);
    UARTCharPut(UART0_BASE,' ');
    UARTCharPut(UART0_BASE,'-');
    UARTCharPut(UART0_BASE,' ');
    UARTCharPut(UART0_BASE,'O');
    UARTCharPut(UART0_BASE,':');
    UARTCharPut(UART0_BASE,ocupados);
    UARTCharPut(UART0_BASE,'\r');
}

//**********************************************************************************************************
// Envio de datos a ESP32
//**********************************************************************************************************
void sendUART2(void){
    uint8_t disponibles = countSetBits(parqueosDisponibles) + '0';
    UARTCharPut(UART2_BASE,disponibles);
    UARTCharPut(UART2_BASE,'\n');
}

uint8_t countSetBits(uint8_t bitwise){
    uint8_t count = 0;
    uint8_t i;
    for (i = 0; i < 4; i++){
        if ((bitwise % 2) == 1){
            count++;
        }
        bitwise = bitwise >> 1;
        bitwise &= 0x0F;
    }
    return count;
}
