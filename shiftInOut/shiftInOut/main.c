/*
 * shiftInOut.c
 *
 * Created: 09/04/2021 13:02:25
 * Author : mateu
 * Links: 
 *			https://www.programiz.com/c-programming/c-operators
 */ 

#include <avr/io.h>

/* 
Pinagem:
	O pino Sin pode ou ficar aterrado ou em n?vel l?gico alto. 
	D0 a D8 recebem os sensores.
	Leitura Serial realizada em Q7							  : conectada a entrada PB0
	P/S' recebe o controle (leitura serial ou carga paralela) : conectado a sa?da PB1
	CLK recebe os pulsos de clock do sistema				  : conectado a sa?da PB2
	
Vari?veis:
	A vari?vel que l? os sensores deve ser global.	
	
Explica??o do c?digo de shiftIn, utilizando o registrador CD4014:
	1. a 3. : os dados da entrada paralela s?o salvas no registrador
	4. a 6. : os dados s?o lidos de maneira serial
	
	1.	P/S' vai pra 1
	2.	CLK vai pra 1
	3.	CLK vai pra 0
	4.	P/S' vai pra 0
	5.	LOOP 8x: 
	6.		CLK vai pra 1
	7.		l? o bit
	8.		CLK vai pra 0

Note que o controle de leitura ? autom?tico. Isto ?, precisamos
impor ao registrador que ele realize a leitura paralela dos sensores	
Recomendo que se use uma interrup??o (como? Ainda n?o sei)	para
ler periodicamente o registrador.
*/


#define clock_shftin PINB2
#define P_Shftin PINB1
// talvez possa ser o mesmo clock do extensor entrada
#define clock_shftout PINB3 
#define Pinout_serial PINB4


uint8_t varLeituraSerial = 0;
uint8_t varEscritaSerial = 0xAA;
uint8_t auxRaiva = 0;

uint8_t SetBit(uint8_t valor, uint8_t posicao)
{
    valor |= (1 << posicao);
    return valor;
}

uint8_t ClearBit(uint8_t valor, uint8_t posicao){
	valor &= ~(1<<posicao);
	return valor;
}

void OutSerial()
{
    PINB |= (1 << Pinout_serial);
}

void shiftIn()
{

	// Carga paralela do registrador
	PORTB |= (1 << P_Shftin); // P/S' vai pra 1
	PORTB |= (1 << clock_shftin); // CLK  vai pra 1
	PORTB &= ~(1 << clock_shftin); // CLK  vai pra 0
	PORTB &= ~(1 << P_Shftin); // P/S' vai pra 0
	
	if(PINB & (1<<PINB0)) {
		varLeituraSerial = SetBit(varLeituraSerial, 0); // QUANDO FOR 1, IRÁ SETAR O VALOR NO REGISTRADOR
	}
	
	
	for (uint8_t iter = 1; iter<8 ; iter++){
		PORTB |= (1 << clock_shftin); // CLK vai pra 1
        auxRaiva = (PINB & (1<<PINB0));
       if(PINB & (1 << PINB0)){
	       if(varLeituraSerial & ~(1<<iter)) 
				varLeituraSerial = SetBit(varLeituraSerial,iter); // QUANDO FOR 0, COLOCA A SAÍDA PRA 1
       }
       else{
	       if(varLeituraSerial & (1<<iter)) 
				varLeituraSerial = ClearBit(varLeituraSerial,iter); // QUANDO FOR 1, IRÁ SETAR O VALOR NO REGISTRADOR
       }
		PORTB &= ~(1 << clock_shftin); // CLK vai pra 0
	}
}

void shiftOut() // ainda não testei mas fiz a parte de Setar e dar Clear no pino
{
    // pinos de controle do CI
    
    
   for (uint8_t iter = 0; iter<8 ; iter++)
   {
       if(PINB & (1 << Pinout_serial))
       {
           if(varEscritaSerial & ~(1<<iter)) OutSerial(); // QUANDO FOR 0, COLOCA A SAÍDA PRA 1
       }
       else
       {
           if(varEscritaSerial & (1<<iter)) OutSerial(); // QUANDO FOR 1, IRÁ SETAR O VALOR NO REGISTRADOR
       }
       
       PORTB |= (1 << clock_shftout); // CLK vai pra 1
       PORTB &= ~(1 << clock_shftout); // CLK vai pra 0
	}
    
}

int main(void)
{
	DDRB &= 0b11111110; // o pino PB0 ser? usado como leitura do serial, 
	DDRB |= 0b00000110; // os pinos PB1 e PB2 ser? a sa?da de clock (CLK) e o controle paralelo/serial (P/S')
	DDRD = 0xFF; // PORTD ? sa?da (s? pra teste)
    /* Replace with your application code */
    while (1) 
    {
        //shiftOut();
		shiftIn();
		PORTD = varLeituraSerial;

        //varLeituraSerial = 0;
    }
}