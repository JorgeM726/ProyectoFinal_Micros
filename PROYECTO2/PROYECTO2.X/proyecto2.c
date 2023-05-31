#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)
// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits Write protection off)
       
#include <xc.h>
#include <stdint.h>
#include <string.h>
#define _XTAL_FREQ 500000
#define _tmr0_value 237

         

unsigned short CCPR = 0;        // Interpolacion lineal
unsigned short CCPR_2 = 0;       
unsigned int cont = 0;
int menu=0;
void setup(void);
int map(int input);
int map2(int input);
int map3(int input);
int map4(int input);
char input[2];
char input[2];
uint8_t address_CCPR1L =0x00; 
uint8_t address_CCP1CON =0x01; 
uint8_t address_CCPR2L =0x02; 
uint8_t address_CCP2CON =0x03; 
uint8_t addressC =0x06; 
uint8_t addressD =0x05; 

uint8_t EEPROM_read(uint8_t address); 
void EEPROM_write(uint8_t address, uint8_t data); 
char USART_read(void);

void __interrupt() isr (void){
    if(PIR1bits.ADIF){                     
        if(ADCON0bits.CHS == 0){   
            
           CCPR1L = map2(ADRESH);    
           CCP1CON = 0b00011100 ; 
            
        }
        else if(ADCON0bits.CHS == 1){  
            
             
            CCPR2L = map2(ADRESH);    
            CCP2CON = 0b00011100 ;  
           
        }
        else if(ADCON0bits.CHS == 2){   
           
            CCPR = map(ADRESH); 
            CCPR1L = (uint8_t)(CCPR>>2);    
            CCP1CONbits.DC1B = CCPR & 0b11; 
          
        }
        else if(ADCON0bits.CHS == 3){   
            
            CCPR_2 = map(ADRESH); 
            CCPR2L = (uint8_t)(CCPR_2>>2);    
            CCP2CONbits.DC2B0 = CCPR_2 & 0b11; 
            
        }
        
        PIR1bits.ADIF = 0;                  
    }
     if (INTCONbits.RBIF){
        
        if (PORTBbits.RB0==0){ 
            cont = 1;
        } 
        
        else if (PORTBbits.RB1 ==0){ 
            cont =2;
        } 
        else if (PORTBbits.RB2 ==0){ 
            cont = 3;
        } 
        else if (PORTBbits.RB3 ==0){ 
            cont =4;
        } 
        else if (PORTBbits.RB4 ==0){ 
            menu =1;//Manual
        }
        else if (PORTBbits.RB5 ==0){ 
            menu =2;//UART
        }
        else if (PORTBbits.RB6 ==0){ 
            menu =3;//EEPROM read
        }
        else if (PORTBbits.RB7 == 0){
            EEPROM_write(address_CCPR1L, CCPR1L);
            EEPROM_write(address_CCP1CON, CCP1CON);   
            EEPROM_write(address_CCPR2L, CCPR2L);
            EEPROM_write(address_CCP2CON, CCP2CON); 
            
            if (cont==1){
              EEPROM_write(addressC, 0xFF);
            }else if (cont==3){
                EEPROM_write(addressC, 0x00);
            }
            if (cont==2){
             EEPROM_write(addressD, 0xFF);    
            }else if (cont==4){
              EEPROM_write(addressD, 0x00);   
            }
            
        }
        
        
        INTCONbits.RBIF=0;
    }
    return;
}

void main(void) {
    setup();
    while(1){
        switch(menu){
        
            case 1:
                PORTE = 0b0001;
                if(ADCON0bits.GO == 0b00){   
            switch (cont){
                
                case 1: 
               PORTCbits.RC0 = 1;  
                ADCON0bits.CHS = 0;    // Cambio de canal
                __delay_us(40);                
                ADCON0bits.GO = 1;    
                break;
                
                case 2: 
                PORTDbits.RD1 = 1;
                ADCON0bits.CHS = 1;
                __delay_us(40);                
                ADCON0bits.GO = 1;    
                break;
                
                case 3:  
               PORTCbits.RC0 = 0;    
                ADCON0bits.CHS = 2;
                __delay_us(40);                
                ADCON0bits.GO = 1;    
                break;
                
                case 4:  
               PORTDbits.RD1 = 0;
                ADCON0bits.CHS = 3;
                __delay_us(40);                
                ADCON0bits.GO = 1;    
                
                break;
                case 0:
                    ADCON0bits.GO = 0;  
                    break;
            }
              
            
        } 
                break;
                
            case 2:
                
                PORTE = 0b0010;
                input[0] = USART_read();
                input[1] = '\0';
                if (strcmp(input, "1") == 0){
                   PORTCbits.RC0 = 1;    
                    CCPR1L = map3(USART_read());    
                    CCP1CON = 0b00011100 ; 
                 }
                else if (strcmp(input, "2") == 0){
                   PORTDbits.RD1 = 1; 
                    CCPR2L = map3(USART_read());    
                    CCP2CON = 0b00011100 ;
                }
                else if (strcmp(input, "3") == 0){
                   PORTCbits.RC0 = 0; 
                    CCPR = map4(USART_read()); 
                    CCPR1L = (uint8_t)(CCPR>>2);    
                    CCP1CONbits.DC1B = CCPR & 0b11; 
                }
                else if (strcmp(input, "4") == 0){
                   PORTDbits.RD1 = 0;
                    CCPR_2 = map4(USART_read()); 
                    CCPR2L = (uint8_t)(CCPR_2>>2);    
                    CCP2CONbits.DC2B0 = CCPR_2 & 0b11; 
                }
    
                break;
            case 3:
              PORTE = 0b0100;
              CCPR1L  = EEPROM_read(address_CCPR1L);
              CCP1CON = EEPROM_read(address_CCP1CON); 
              CCPR2L  = EEPROM_read(address_CCPR2L);
              CCP2CON = EEPROM_read(address_CCP2CON); 
              if (EEPROM_read(addressC)==0xFF){
                  PORTCbits.RC0 = 1;
              }else if (EEPROM_read(addressC)==0x00){
                  PORTCbits.RC0 = 0;
              }
              if (EEPROM_read(addressD) == 0xFF){
                  PORTDbits.RD1 = 1;
              }else if (EEPROM_read(addressD) == 0x00){
                  PORTDbits.RD1 = 0;
              }
              
                break;
        
        } 
        
    }
    return;
}

void setup(void){
    OPTION_REGbits.nRBPU=0; 
    ANSEL = 0b00001111; 
    ANSELH = 0; 
    TRISD = 0;
    PORTD = 0;
    TRISA = 0XFF;
    PORTA = 0;
    TRISB = 0xFF;
    PORTB = 0;
    WPUB = 0xFF;
    IOCB = 0xFF;
    TRISE = 0;
    PORTE = 0;
    
    TRISCbits.TRISC0 = 0;       
    
    // CONFIGURACION RELOJ INTERNO
    OSCCONbits.IRCF = 0b011;    // 500khz
    OSCCONbits.SCS = 1;         // Oscilador interno
    
     
    TXSTAbits.SYNC = 0;
    TXSTAbits.BRGH = 1;
    BAUDCTLbits.BRG16 = 1;
    SPBRG = 25;
    SPBRGH = 0;
    
    RCSTAbits.SPEN = 1;
    RCSTAbits.RX9 = 0;
    RCSTAbits.CREN = 1;
    TXSTAbits.TXEN = 1;
    
    // CONFIGURACION ADC
    ADCON0bits.ADCS = 0b01;             // Fosc/8
    ADCON1bits.VCFG0 = 0;               // VDD
    ADCON1bits.VCFG1 = 0;               // VSS
    ADCON0bits.CHS = 0;                // AN0   
    ADCON1bits.ADFM = 0;               
    ADCON0bits.ADON = 1;                // ADC
    __delay_us(40);                     
        
    // CONFIGURACION PWM
        
    TRISCbits.TRISC2 = 1;       
    TRISCbits.TRISC1 = 1;       
    PR2 = 0b10011011;                  
    
    // CONFIGURACION CCP
    CCP1CON = 0;                       
    CCP2CON = 0;                       
    CCP1CONbits.P1M = 0;               // Modo single output
    CCP2CONbits.CCP2M = 0b1100;        // Modo single output
    CCP1CONbits.CCP1M = 0b1100;        // PWM

    CCPR1L = 16>>2;
    CCPR2L = 16>>2;
    CCP1CONbits.DC1B0 = 48 & 0b11;    
    CCP2CONbits.DC2B0 = 48 & 0b11;    
    
    PIR1bits.TMR2IF = 0;       
    T2CONbits.T2CKPS = 0b11;    // prescaler 1:16
    T2CONbits.TMR2ON = 1;    
    T2CON = 0b00000111 ;
    
    while(!PIR1bits.TMR2IF);    
    PIR1bits.TMR2IF = 0;       
    
    TRISCbits.TRISC2 = 0;       // Salida de PWM
    TRISCbits.TRISC1 = 0;       // Salida de PWM

    // CONFIGURACION INTERRUPCIONES
    INTCONbits.RBIE=1; 
    INTCONbits.RBIF=0; 
    PIR1bits.ADIF = 0;  
    
    PIE1bits.ADIE = 1;          // Int. de ADC
    INTCONbits.PEIE = 1;        // Int. de perifericos
    INTCONbits.GIE = 1;         // Int. globales
    
    PIR1bits.RCIF = 0;
    PIE1bits.RCIE = 0;
    
}

int map(int input) {
    
    int inputMin = 0;
    int inputMax = 255;
    int outputMin = 9;
    int outputMax = 65;
    
    // Perform the mapping
    int output = ((input - inputMin) * (outputMax - outputMin)) / (inputMax - inputMin) + outputMin;
    
    return output;
}

int map2(int input) {
    
    int inputMin = 0;
    int inputMax = 255;
    int outputMin = 0;
    int outputMax = 140;
    
    // Perform the mapping
    int output = ((input - inputMin) * (outputMax - outputMin)) / (inputMax - inputMin) + outputMin;
    
    return output;
}

int map3(int input) {
    
    int inputMin = 0;
    int inputMax = 127;
    int outputMin = 0;
    int outputMax = 140;
    
    // Perform the mapping
    int output = ((input - inputMin) * (outputMax - outputMin)) / (inputMax - inputMin) + outputMin;
    
    return output;
}
int map4(int input) {
    
    int inputMin = 0;
    int inputMax = 127;
    int outputMin = 9;
    int outputMax = 65;
    
    // Perform the mapping
    int output = ((input - inputMin) * (outputMax - outputMin)) / (inputMax - inputMin) + outputMin;
    
    return output;
}



char USART_read(void)
{
    while(!RCIF);
    return RCREG;
}

//Lectura
uint8_t EEPROM_read(uint8_t address){ 
    
    EEADR = address; //Registro de dirección
    EECON1bits.EEPGD=0; //Acceso a memoria de datos
    EECON1bits.RD=1; //Read control activado
    __delay_us(1);
    return EEDAT; //Registro de memoria de datos
}

//Escritura
void EEPROM_write (uint8_t address, uint8_t data){ 
    EEADR = address;  //Registro de dirección
    EEDAT =data;     //Registro de datos
    EECON1bits.EEPGD=0; //Acceso a memoria de datos
    EECON1bits.WREN=1; //Write enable
    INTCONbits.GIE=0; //Apagar interrupciones globales
    EECON2 = 0x55;  //
    EECON2 = 0xAA; 
    EECON1bits.WR=1; //Empieza ciclo de escritura
    while(!PIR2bits.EEIF); //Mientras no se haya completado la escritura espera
    PIR2bits.EEIF = 0; //Limpieza de bandera de escritura
    EECON1bits.WREN = 0; // Se deshabilita escritura en la EEPROM
    INTCONbits.RBIF = 0; // Se limpian interrupciones del puerto B
    PIR1bits.ADIF = 0; // Limpieza de bandera del ADC
    INTCONbits.GIE = 1;  // Se habilitan interrupciones
}
