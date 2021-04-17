/*
 * GccApplication1.c
 *
 * Created: 09/04/2021 10:53:00
 * Author : Leandro Rodrigues
 */ 

#include <avr/io.h>
#define FREQCPU 8000000UL
#define TAXA 9600

void IniciarUART (uint32_t taxa, uint32_t freqCpu);
uint8_t ProntoTXUART (void);
uint8_t ProntoRXUART (void);
uint8_t LerRXUART ();
void EnviarTXUART (uint8_t dado);
void EnviarStringUART (char *c);

int main(void)
{
    unsigned dia, hora;
	dia = 5;
	hora = 10;

    while (1) 
    {
		IniciarUART(TAXA, FREQCPU);
		EnviarStringUART("A central foi ativada no dia  ");
		while(!ProntoTXUART()); //aguarda a string ser enviada
		EnviarDec1Byte(dia); 
		EnviarStringUART(" e hora  ");
		while(!ProntoTXUART()); //aguarda a string ser enviada
		EnviarDec1Byte(hora);
		EnviarStringUART("\r\n");
    }
}

//uartDec2B(valorADC);

void IniciarUART (uint32_t taxa, uint32_t freqCpu){
	uint16_t valorReg = freqCpu/16/taxa-1;
	UBRR0H = (uint8_t) (valorReg>>8); // ajusta velocidade de transm
	UBRR0L = (uint8_t)valorReg;
	UCSR0A = 0;               //desab velocid dupla
	UCSR0B = (1<<RXEN0)|(1<<TXEN0); // hab trans e recepção SEM interrupção
	UCSR0C = (1<<UCSZ01)|(1<<UCSZ00); // assincrono, com 1 byte, 1 bit de parada e sem paridade. 
}

uint8_t ProntoTXUART (void){
return (UCSR0A & (1<<UDRE0)); // retorna 1 qnd pode ser enviado
}

uint8_t ProntoRXUART (void){
	return (UCSR0A & (1<<RXC0)); // retorna 1 qnd pode ser enviado
}

uint8_t LerRXUART (){
	return UDR0;
}

void EnviarTXUART (uint8_t dado){
	UDR0 = dado;
}


void EnviarStringUART (char *c){
	for( ; *c != 0; c++){ // a string tem como ultimo valor do vetor =  0
		while (!ProntoTXUART());
		EnviarTXUART(*c);
	}
	
}

//mudar função
void EnviarDec1Byte(uint8_t valor){
  int8_t disp;
  char digitos[3]; //se quiser enviar mais byte muda o paramentro 
  int8_t conta = 0;
  do //converte o valor armazenando os algarismos no vetor digitos
  { disp = (valor%10) + 48;//armazena o resto da divisao por 10 e soma com 48
    valor /= 10;
    digitos[conta]=disp;
    conta++;
  } while (valor!=0);
  for (disp=conta-1; disp>=0; disp-- )//envia valores do vetor digitos
  { while (!ProntoTXUART());  //aguarda último dado ser enviado
    EnviarTXUART(digitos[disp]);//envia algarismo
  }
}

