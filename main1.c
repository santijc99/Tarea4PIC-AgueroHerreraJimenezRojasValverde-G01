/*
 * File:   Main.c
 * Author: Santiago
 *
 * Created on 11 de octubre de 2021, 10:03 PM
 */

#include <xc.h>
#include <pic18f4550.h>
 
void main(void) {
    //setup de los puertos AN12, AN10 y AN8
    
    //se asignan como digitales todos los puetos AN
    
    ADCON1 = 0b00001111;
    
    //selección de la dirección de información,
    //los puertos RA0 a RA6 son outputs
    
    TRISA = 0b00100000;
    
    //los puertos RB0 al RB2, son inputs
    // los puertos RB3 al RB7 con outputs
    
    TRISB = 0b00000000;
    
    //los puertos RC0 al RC7 son outputs
    TRISC = 0b00000001;
    // estos bits se utilizarán para encender los LEDS para indicar cuantas 
    // cajas hay
    
    //Se asigna valores iniciales a estas salidas de 0
    LATB = 0b00000000;
    
    //Setup de las interrupciones
    
    RCONbits.IPEN = 1;         //  Se habilita las prioridades
    INTCONbits.GIEH = 1;       // Enable de Interrupciones no enmascaradas
    INTCONbits.GIEL = 0;      // Se habilita Interrupciones de Perifericos
    
    //setup de la interrupción INT0
    // se usa esta interrupción para el sensor de entrada
    
    INTCONbits.INT0IE = 1;  // se habilita la interrupción INT0
    INTCONbits.INT0IF = 0; // se limpia la bandera
    INTCON2bits.INTEDG0 = 1;  // INT0 en flanco positivo
    

    //setup de le interrupción INT1
    //se utiliza para el sensor de salida
    INTCON3bits.INT1IF  = 0;   // Se limpia la bandera de INT1
    INTCON3bits.INT1IP  = 0;   // Baja prioridad a la INT1
    INTCON3bits.INT1IE  = 1;   // Se habilita INT1
    INTCON2bits.INTEDG1 = 1;   // INT1 con flanco positivo
    
    //Setup de la interrupcion 2
    
    INTCON3bits.INT2IF  = 0;   // Se limpia la bandera de INT2
    //INTCON3bits.INT1IP  = 0;   // Baja prioridad a la INT1
    INTCON3bits.INT2IE  = 1;   // Se habilita INT1
    INTCON2bits.INTEDG2 = 1;   // INT1 con flanco positivo
    
   
    //Setup del contador
    
    
    T0CONbits.T08BIT = 1; // Timer 0 a 8 bit timer
    T0CONbits.T0CS = 1; // transición externa
    T0CONbits.T0SE = 0; // Flanco positivo para la patilla T0CKl
    T0CONbits.PSA = 1; // No se usa pre escala
    
    // T0CONbits.T0PS2 = 0;
    // T0CONbits.T0PS1 = 0;  
    // T0CONbits.T0PS0 = 1;
    
    T0CONbits.TMR0ON = 1; // Se habilita el timer
    
    //configuracion de timer 3
    // se usa este timer para contar los 5 segundos y actualizar los valores
    //se usa una prescala de 8, y un valor inicial de TMR3 de 65286 el cual
    // permite contar 1 ms
    
    T3CONbits.RD16 = 1; //modo de operacion de 16 bits
    /*T3CONbits.T3CCP1 = 0; // ???
    T3CONbits.T3CCP2 = 0; // ???*/
    T3CONbits.T3CKPS0 = 1; //preescala 1:8
    T3CONbits.T3CKPS1 = 1; // prescala de 1:8
    T3CONbits.T3SYNC = 0; // internal clock 
    T3CONbits.TMR3CS = 0; // se usa el reloj interno
    T3CONbits.TMR3ON = 1; // se enciende el timer3
    TMR3 = 65286;
    
    // INTERRUPCION TIMER 3
    PIE2bits.TMR3IE = 1; // Activar overflow Timer3
    PIR2bits.TMR3IF = 0; //se limpia la bandera de overflow
    TMR3 = 0x00FA; // poniendo el timer 5 digitos antes del overflow
    
    
    
    // Se inicia el Registro del Timer en el valor Máximo 0xFF
    // Esto permite ver la interrupción por timer sin tener que
    // pasar por tantas señales a la patilla T0CKl
    TMR0L = 0xA;
    
    //se declara una variable que sirve para contar los milisegunos que pasen
    int contador = 0;
    //ciclo principal 
    while(1)
    {
        //se puede hacer poner los valores con un delay
    }
    return;
    
    //se apagan los leds
    LATBbits.LB3 = 0;
    LATBbits.LB4 = 0;
    LATBbits.LB5 = 0;
    LATBbits.LB6 = 0;
    
   
}
void __interrupt() isr(void){
    
    //handler para INT0 es decir,para el sensor de entrada
    
if (INTCON3bits.INT1IF == 1) //Sensor de Salida
{     // Disminución del contador, además de alarma de inconsistencias

        TMR0L=TMR0L-1;

       /* if (TMR0L<10) //Si hay mas salidas que entrada envia una señal
            
        {
           LATBbits.LB7 = 1; //Encender una salida, alarma de irregularidades;
        }
    
        else 
            
        {
            LATBbits.LB7 = 0;
        }*/

        INTCON3bits.INT1IF = 0;
}

if (INTCON3bits.INT2IF == 1) 
    
{    //Paro de emergencia
    
        INTCON3bits.INT1IE = ~INTCON3bits.INT1IE;
        T0CONbits.TMR0ON = ~T0CONbits.TMR0ON;
        T3CONbits.TMR3ON = ~T3CONbits.TMR3ON;
        LATBbits.LB3 = 0;
        LATBbits.LB4= 0;
        LATBbits.LB5= 0;
        LATBbits.LB6= 0;
        LATBbits.LB7= 0;

        INTCON3bits.INT2IF = 0;
}

if (PIR2bits.TMR3IF == 1) //se llegó al overflow, es decir, contó un milisegundo
{
    int contador = contador + 1;
    
    if (contador == 5000) //ya pasaron cinco segundos
    {
  
        contador = contador - 5000;
        
        /*
        LATBbits.LB3 = ~LATBbits.LB3;
        LATBbits.LB4 = ~LATBbits.LB4;
        LATBbits.LB5 = ~LATBbits.LB5;
        LATBbits.LB6 = ~LATBbits.LB6;
         */
        
        //ifs para los distintos casos de valores del contador
        //de cajas

            if (TMR0L == 10)
             {
             LATBbits.LB3 = 0;
             LATBbits.LB4= 0;
             LATBbits.LB5= 0;
             LATBbits.LB6= 0;
             LATBbits.LB7=0;
             }
            if (TMR0L < 10)
             {
             LATBbits.LB3 = 0;
             LATBbits.LB4= 0;
             LATBbits.LB5= 0;
             LATBbits.LB6= 0;
             LATBbits.LB7=1;
             }
             if (TMR0L == 11)
             {
             LATBbits.LB3 = 1;
             LATBbits.LB4= 0;
             LATBbits.LB5= 0;
             LATBbits.LB6= 0;
             LATBbits.LB7=0;
             }

             if (TMR0L == 12)
             {
             LATBbits.LB3 = 0;
             LATBbits.LB4= 1;
             LATBbits.LB5= 0;
             LATBbits.LB6= 0;
             LATBbits.LB7=0;
             }
             if (TMR0L==13)
             {
             LATBbits.LB3 = 0;
             LATBbits.LB4= 0;
             LATBbits.LB5= 1;
             LATBbits.LB6= 0;
             LATBbits.LB7=0;
             }
             if (TMR0L == 14)
             {
             LATBbits.LB3 = 0;
             LATBbits.LB4= 0;
             LATBbits.LB5= 0;
             LATBbits.LB6= 1;
             LATBbits.LB7=0;
             }
             if (TMR0L>14)
             {
             LATBbits.LB3 = 1;
             LATBbits.LB4= 1;
             LATBbits.LB5= 1;
             LATBbits.LB6= 1;
             LATBbits.LB7=0;
             }
        
        
          }
    
    
    PIR2bits.TMR3IF = 0; //se limpia la bandera
    TMR3 = 65286; //valor inicial que debe tener el contador TMR3 para poder 
    // contar un milisegundo
    
        }
} 
 