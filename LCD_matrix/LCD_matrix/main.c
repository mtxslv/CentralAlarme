/*
 * main.c
 *
 * Created: 4/8/2021 2:55:04 PM
 *  Author: erika
 */ 

#define F_CPU	16000000
#include <avr/io.h>
#include <avr/interrupt.h>

#define pino_key PIND

/*############ VARIAVEIS #############*/

char auxChar[15];
long int x = 0;
int cont_1 = 0;

int valor_pinod;

int cont_r = 0;

/*############ FUNÇÕES #############*/
// DELAYS //
void delay_ms(long int n);
void delay_ns(long int n);

// DISPLAY //
void send_instruction_lcd(uint8_t data);	
void send_instruction_lcd_nibble(uint8_t data);
void inicializa_display();
void send_word_lcd(uint8_t data);
void set_DDRAM_ADDR(uint8_t pos);
void envia_frase(char *frase,uint8_t pos);
// BOTOES //
void init_int1();
void get_button();


/* ########## PINOS ###########*/

enum posicao_lcd{
	l1C1,
	l1C2,
	l1C3,
	l1C4,
	l1C5,
	l1C6,
	l1C7,
	l1C8,
	l1C9,
	l1C10,
	l1C11,
	l1C12,
	l1C13,
	l1C14,
	l1C15,
	l1C16,
	l2C1 = 0x40,
	l2C2,
	l2C3,
	l2C4,
	l2C5,
	l2C6,
	l2C7,
	l2C8,
	l2C9,
	l2C10,
	l2C11,
	l2C12,
	l2C13,
	l2C14,
	l2C15,
	l2C16
};

enum display_pinos{
	DB4,
	DB5,
	DB6,
	DB7,
	RS,
	EN
};

int main(void)
{
	// display pinos //
	DDRB = 0X3F;
	// matrix pinos //
	DDRD = 0x00;
	PORTD = 0xF0;
	
	init_int1();
	
	inicializa_display();
	char frase_1[] = "    BOM DIA";
	envia_frase(frase_1,l1C1);
	char frase_2[] = "";
	envia_frase(frase_2,l2C1);
	
	sei();
	
    while(1)
    {
        //get_button(); 
    }
}

void init_int1(){
	
	EICRA = (1<<ISC11) | (1<<ISC10);
	EIMSK = (1<<INT1);
	
}

ISR(INT1_vect){
	//send_word_lcd(0b00110001);	
	
	//valor_pinod = (PIND7) | (PIND6) | (PIND5) | (PIND4) ;
	valor_pinod = (PIND & 0xF0);
	
	if(valor_pinod == 0xF0){
		// 1
		send_word_lcd(0b00110001);
	}
	else if(valor_pinod == 0b01110000){
		// 2
		send_word_lcd(0b00110010);
	}
	else if(valor_pinod == 0b10110000){
		// 3
		send_word_lcd(0b00110011);
	}
	else if(valor_pinod == 0b00110000){
		// P
		send_word_lcd(0b01010000);
	}
	else if(valor_pinod == 0b11010000){
		// 4
		send_word_lcd(0b00110100);
	}
	else if(valor_pinod == 0b01010000){
		// 5
		send_word_lcd(0b00110101);
	}
	else if(valor_pinod == 0b10010000){
		// 6
		send_word_lcd(0b00110110);
	}
	else if(valor_pinod == 0b00010000){
		// A
		send_word_lcd(0b01000001);
	}
	else if(valor_pinod == 0b11100000){
		// 7
		send_word_lcd(0b00110111);
	}
	else if(valor_pinod == 0b01100000){
		// 8
		send_word_lcd(0b00111000);
	}
	else if(valor_pinod == 0b10100000){
		// 9
		send_word_lcd(0b00111001);
	}
	else if(valor_pinod == 0b00100000){
		// D
		send_word_lcd(0b01000100);
	}
	else if(valor_pinod == 0b11000000){
		// R
		cont_r++;
		send_word_lcd(0b01010010);
	}
	else if(valor_pinod == 0b01000000){
		// 0
		send_word_lcd(0b00110000);
	}
	else if(valor_pinod == 0b10000000){
		// S
		send_word_lcd(0b01010011);
	}
	else if(valor_pinod == 0b000000000){
		// E
		send_word_lcd(0b01000101);
	}
}

void delay_ms(long int n){
	
	x = 0;
	
	long int m = 1000 * n;
	
	while(x <= m){
		x++;
	}
}

void delay_ns(long int n){

	x = 0;
	
	long int m = 1 * n;
	
	while(x <= m){
		x++;
	}
}

void send_instruction_lcd(uint8_t data){
	PORTB &= 0xC0;
	PORTB &= ~(1<<RS);  // RS = 0 => intruções
	PORTB |= (1 << EN);  // Seta o enable
	PORTB |= (data >> 4); // seta barramento de dados com nibble mais significativo
	PORTB &= ~(1<<EN);  // EN = 0
	
	PORTB &= 0xC0;
	PORTB |= (1 << EN);  // Seta o enable
	PORTB |= data & 0x0F; // seta barramento de dados com nibble mais significativo
	PORTB &= ~(1<<EN);  // EN = 0
}

void send_instruction_lcd_nibble(uint8_t data){
	PORTB &= 0xC0;
	PORTB &= ~(1<<RS);  // RS = 0 => intruções
	PORTB |= (1 << EN);  // Seta o enable
	PORTB |= data; // seta barramento de dados com nibble mais significativo
	PORTB &= ~(1<<EN);  // EN = 0
}

void inicializa_display(){
	
	delay_ms(15);

	send_instruction_lcd_nibble(0x03);
	delay_ms(5);

	send_instruction_lcd_nibble(0x03);
	delay_ns(100);

	send_instruction_lcd_nibble(0x03);//
	delay_ns(40);
	
	send_instruction_lcd_nibble(0x02); // Return Home
	delay_ns(40);

	send_instruction_lcd(0x28); // Determinar o modo de operação
	// 4 bit, matriz 5x7 e 2 linhas
	delay_ns(40);

	send_instruction_lcd(0x01); // clear display
	delay_ms(2);
	send_instruction_lcd(0x06); //
	delay_ns(40);

	send_instruction_lcd(0x0C); // Display On, cursosr OFF
	delay_ns(40);
	
}

void send_word_lcd(uint8_t data){
	PORTB &= 0xF0;
	PORTB |= (1<<RS);  // RS = 1 => dados
	PORTB |= (1 << EN);  // Seta o enable
	PORTB |= (data >> 4);// seta barramento de dados com nibble mais significativo
	PORTB &= ~(1<<EN);  // EN = 0
	
	PORTB &= 0xF0;
	PORTB |= (1 << EN);  // Seta o enable
	PORTB |= data & 0x0F; // seta barramento de dados com nibble menos significativo
	PORTB &= ~(1<<EN);  // EN = 0

	delay_ns(43);
}

void set_DDRAM_ADDR(uint8_t pos){
	uint8_t data = 0x80 | pos;
	PORTB &= 0xC0;
	PORTB &= ~(1<<RS);  // RS = 0 => intruções
	PORTB |= (1 << EN);  // Seta o enable
	PORTB |= (data >> 4); // seta barramento de dados com nibble mais significativo
	PORTB &= ~(1<<EN);  // EN = 0
	
	PORTB &= 0xC0;
	PORTB |= (1 << EN);  // Seta o enable
	PORTB |= data & 0x0F; // seta barramento de dados com nibble menos significativo
	PORTB &= ~(1<<EN);  // EN = 0
	delay_ns(43);
}

void envia_frase(char *frase,uint8_t pos){
	set_DDRAM_ADDR(pos);
	uint8_t cont = 0;
	while(frase[cont] != '\0'){
		send_word_lcd(frase[cont]);
		cont++;
	}
}

void passarValor(const char *data){
	uint8_t cont = 0;
	while(data[cont] != '\0'){
		auxChar[cont] = data[cont];
		cont++;
	}
	auxChar[cont] = '\0';
}



