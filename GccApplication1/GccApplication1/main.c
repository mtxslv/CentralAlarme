/*
 * GccApplication1.c
 *
 * Created: 15/04/2021 11:36:54
 * Author : lokas
 */ 
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

void SPI_Master_Setup(); // Função para fazer o setup da comunicação SPI.
void SPI_Master_Transmit(uint8_t dado); // Função para o envio de dados.
uint8_t SPI_Read(uint8_t code); // Função que faz leitura da informação enviada pelo o escravo.
uint8_t RTC_Read_Hours(); // Função que faz a solicitação e leitura da hora vinda do RTC.
uint8_t RTC_Read_Minu(); // Função que faz a solicitação e leitura dos minutos que vem do RTC.
uint8_t RTC_Read_Day(); // Função que faz a solicitação e leitura da data que vem do RTC.

int main(void)
{
    /* Replace with your application code */
	SPI_Master_Setup();
	DDRD = 0b11111111;
	int i = 0;
    while (1) 
    {
		if(i%10 == 0){
			PORTD = RTC_Read_Hours();
		}
		i++;
    }
}
void SPI_Master_Setup(){
	DDRB |= (1<<PORTB5) | (1<<PORTB3) | (1<<PORTB2); // Configura SCK, MOSI e SS como saaídas, demais pinos como entradas.
	SPCR |= (1<<SPE) | (1<<MSTR) | (1<<SPR0); // Habilita SPI mestre. 
	PORTB |= (1<<PORTB2); // Desativa o escravo.
}
void SPI_Master_Transmit(uint8_t dado){
	SPDR = dado;
}
uint8_t SPI_Read(uint8_t code){
	SPCR |= (1<<SPE)|(1<<MSTR)|(1<<SPR0)|(1<<CPHA);
	PORTB &= ~(1 << (PINB2));
	SPI_Master_Transmit(code);
	while(!(SPSR & (1<<SPIF)));
	SPI_Master_Transmit(0xFF);
	while(!(SPSR & (1<<SPIF)));
	PORTB |= (1<<PORTB2);
	return SPDR;
}
uint8_t RTC_Read_Hours(){
	return SPI_Read(0x00);
}
uint8_t RTC_Read_Minu(){
	return SPI_Read(0x01);
}
uint8_t RTC_Read_Day(){
	return SPI_Read(0x03);
}