/**
 * PROYECTO 3 - ELECTRONICA DIGITAL 2
 * PARQUEO INTELIGENTE
 *
 * DESARROLLADO POR:
 *      JUDAH PÉREZ - 21536
 *      DANIEL VALDEZ -
 *
 * HARDWARE:
 *      SENSORES (LDR)  -> PORTB[0:1] + PORTE[4:5] + PORTB[4] + PORTA[5:7]
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
#define SENSORES_PORTA_PINS  GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7
#define SENSORES_PORTB_PINS  GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4
#define SENSORES_PORTE_PINS  GPIO_PIN_4|GPIO_PIN_5

#define RED_PORTD_MASK  GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
#define RED_PORTE_MASK  GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
#define RED_PORTF_MASK  GPIO_PIN_1

#define GREEN_PORTA_MASK  GPIO_PIN_2
#define GREEN_PORTB_MASK  GPIO_PIN_3
#define GREEN_PORTC_MASK  GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7
#define GREEN_PORTF_MASK  GPIO_PIN_2|GPIO_PIN_3

#define UART2_PINS     GPIO_PIN_7|GPIO_PIN_6

uint8_t sensorValues;
uint8_t parqueosDisponibles;
uint8_t parqueosOcupados;

//**************************************************************************************************************
// Prototipos de Función
//**************************************************************************************************************
uint8_t readSensors(void);
void writeRed(uint8_t redVal);
void writeGreen(uint8_t greenVal);
void sendUART0(void);
void sendUART2(void);
uint8_t countSetBits(uint8_t bitwise);
void binaryToASCII(uint8_t bitwise);

//**************************************************************************************************************
// Código Principal
//**************************************************************************************************************
int main(void)
{
    // configuración del reloj a 40MHz con el oscilador externo de 16MHz y PLL
    SysCtlClockSet(SYSCTL_OSC_MAIN|SYSCTL_USE_PLL|SYSCTL_SYSDIV_5|SYSCTL_XTAL_16MHZ);

    // Habilitar Puertos
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    // Configurar Pines de sensores
    GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, SENSORES_PORTA_PINS);
    GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, SENSORES_PORTB_PINS);
    GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, SENSORES_PORTE_PINS);

    // Configurar Pines LEDs
    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GREEN_PORTA_MASK);
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GREEN_PORTB_MASK);
    GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GREEN_PORTC_MASK);
    GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, RED_PORTD_MASK);
    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, RED_PORTE_MASK);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, RED_PORTF_MASK|GREEN_PORTF_MASK);

    //Habilitar módulo UART2 para la comunicación con ESP32
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART2);
    GPIOPinTypeUART(GPIO_PORTD_BASE, UART2_PINS);
    UARTConfigSetExpClk(UART2_BASE, SysCtlClockGet(), 115200,
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    //Habilitar módulo UART0 para la comunicación con computadora
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));



    //**********************************************************************************************************
    // Loop Principal
    //**********************************************************************************************************
    while (1)
    {
        sensorValues = readSensors();   //Lectura de sensores
        parqueosOcupados = sensorValues;            //Sensor tapado indica ocupado
        parqueosDisponibles = (~sensorValues);
        writeRed(parqueosOcupados);
        writeGreen(parqueosDisponibles);


        sendUART0();
        sendUART2();
        SysCtlDelay(300000);
    }
}

//**********************************************************************************************************
// Lectura de sensores LDR
//**********************************************************************************************************
uint8_t readSensors(void){
    return GPIOPinRead(GPIO_PORTA_BASE, SENSORES_PORTA_PINS)|
           GPIOPinRead(GPIO_PORTB_BASE, SENSORES_PORTB_PINS)|
           GPIOPinRead(GPIO_PORTE_BASE, SENSORES_PORTE_PINS) >> 2;
}

//**********************************************************************************************************
// Escrituda de LEDS rojos
//**********************************************************************************************************
void writeRed(uint8_t redVal){
    GPIOPinWrite(GPIO_PORTD_BASE, RED_PORTD_MASK, redVal);
    GPIOPinWrite(GPIO_PORTE_BASE, RED_PORTE_MASK, redVal >> 3);
    GPIOPinWrite(GPIO_PORTF_BASE, RED_PORTF_MASK, redVal >> 6);
}

//**********************************************************************************************************
// Escrituda de LEDS verdes
//**********************************************************************************************************
void writeGreen(uint8_t greenVal){
    GPIOPinWrite(GPIO_PORTA_BASE, GREEN_PORTA_MASK, greenVal >> 5);
    GPIOPinWrite(GPIO_PORTB_BASE, GREEN_PORTB_MASK, greenVal << 1);
    GPIOPinWrite(GPIO_PORTC_BASE, GREEN_PORTC_MASK, greenVal << 1);
    GPIOPinWrite(GPIO_PORTF_BASE, GREEN_PORTF_MASK, greenVal << 2);
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
    UARTCharPut(UART0_BASE,' ');
    binaryToASCII(parqueosOcupados);
    UARTCharPut(UART0_BASE,' ');
    UARTCharPut(UART0_BASE,parqueosOcupados);
    UARTCharPut(UART0_BASE,'\r');
}

//**********************************************************************************************************
// Envio de datos a ESP32
//**********************************************************************************************************
void sendUART2(void){
    //uint8_t disponibles = countSetBits(parqueosDisponibles);
    //UARTCharPut(UART2_BASE, parqueosOcupados);
    UARTCharPut(UART2_BASE, parqueosOcupados);
}

uint8_t countSetBits(uint8_t bitwise){
    uint8_t count = 0;
    uint8_t i;
    for (i = 0; i < 8; i++){
        if ((bitwise % 2) == 1){
            count++;
        }
        bitwise = bitwise >> 1;
        bitwise &= 0x7F;
    }
    return count;
}

void binaryToASCII(uint8_t bitwise){
    uint8_t i;
    for (i = 0; i < 8; i++){
        if (((bitwise & 0x01) % 2) == 0){ //último bit es 0
            UARTCharPut(UART0_BASE,'0');
        }
        else {
            UARTCharPut(UART0_BASE,'1');
        }
        bitwise = bitwise >> 1;
        }
}
