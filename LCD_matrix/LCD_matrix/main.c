/*
 * main.c
 *
 * Created: 4/8/2021 2:55:04 PM
 *  Author: erika
 */ 

#define F_CPU 8000000UL
#include <avr/io.h>
#include <avr/interrupt.h>

#define pino_key PIND



/*############ VARIAVEIS #############*/
typedef int bool;

char auxChar[15];
long int x = 0;
int cont_1 = 0;

int valor_pinod;

// SENHA //
char mestre[] = "1234";
char senha1[] = "2222";
char senha_inserida[5];

int count_senha = 0;
bool flag_senha_correta = 0;

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

void clear_Display();

// BOTOES //
void init_int1();

// ROTINA DE SENHA //
void insert_senha(int n, const char *digito);
void verify_senha();
void msg_inserir_senha();

// DESATIVADO //
void msg_desativado();

// PROGRAMA //
void msg_programacao();


/* ########## PINOS e ESTADOS ###########*/

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

enum states{
	recuperacao,
	desativado,
	senhaMcorreta,
	senhaAcorreta,
	senhaAnova,
	HabSensor,
	AssSensorZona,
	HabZona,  //qnd tem q salvar algo virá um estado
	ativado,
	panico,
	programacao,
	AjusteTAtivacao,
	AjusteTout,
	AjusteTSirene,
	
	// novos estados //
	inserir_senha,
	verificar_senha,
};

// ESTADO INICIAL //
enum states state=recuperacao;

int main(void)
{
	// AJUSTE DOS PINOS DO ATMEGA //
	// display pinos //
	DDRB = 0X3F;
	// matrix pinos //
	DDRD = 0x00;
	PORTD = 0xF0;
	
	// INIALIZAÇÕES DO SISTEMA //
	init_int1();
	inicializa_display();
	
	sei();
	
    while(1)
    {	
		// SWITCH CASE PARA A SELEÇÃO DO MODO //
        switch(state){
			// ROTINAS PRINCIPAIS  //
			case recuperacao:
			// vai direto pro desativado //
			state = desativado;
			break;
			case desativado:
			msg_desativado();
			break;
			case ativado:
			break;
			case programacao:
			msg_programacao();
			// inserir senha //
			// ver se a senha está correta //
			// init timer do timeout //
			// fazer a configurações restantes //
			break;
			case panico:
			break;
			
			// ROTINAS SECUNDARIAS //
			
			case inserir_senha:
			// Inserndo a senha //
			//msg_inserir_senha();
			if(count_senha >= 4){
				state = verificar_senha;
				count_senha = 0;
			}
			break;
			
			case verificar_senha:
			// verificando a senha //
			verify_senha();
			break;
		}
    }
}

// INTERRUPÇÃO EXTERNA //

void init_int1(){
	
	EICRA = (1<<ISC11) | (1<<ISC10);
	EIMSK = (1<<INT1);
	
}

ISR(INT1_vect){
	// Valor do pino de entrada //
	valor_pinod = (PIND & 0xF0);
	
	
	if(valor_pinod == 0xF0){
		// 1
		send_word_lcd(0b00110001);
		
		// Inserir a senha //
		if(state == inserir_senha){
			insert_senha(count_senha, "1");
			count_senha++;
		}
	}
	else if(valor_pinod == 0b01110000){
		// 2
		send_word_lcd(0b00110010);
		
		// Inserir a senha //
		if(state == inserir_senha){
			insert_senha(count_senha, "2");
			count_senha++;
		}
	}
	else if(valor_pinod == 0b10110000){
		// 3
		send_word_lcd(0b00110011);
		
		// Inserir a senha //
		if(state == inserir_senha){
			insert_senha(count_senha, "3");
			count_senha++;
		}
	}
	else if(valor_pinod == 0b00110000){
		// P
		send_word_lcd(0b01010000);
		state = inserir_senha;
		msg_inserir_senha();
	}
	else if(valor_pinod == 0b11010000){
		// 4
		send_word_lcd(0b00110100);
		
		// Inserir a senha //
		if(state == inserir_senha){
			insert_senha(count_senha, "4");
			count_senha++;
		}
	}
	else if(valor_pinod == 0b01010000){
		// 5
		send_word_lcd(0b00110101);
		
		// Inserir a senha //
		if(state == inserir_senha){
			insert_senha(count_senha, "5");
			count_senha++;
		}
	}
	else if(valor_pinod == 0b10010000){
		// 6
		send_word_lcd(0b00110110);
		
		// Inserir a senha //
		if(state == inserir_senha){
			insert_senha(count_senha, "6");
			count_senha++;
		}
	}
	else if(valor_pinod == 0b00010000){
		// A
		send_word_lcd(0b01000001);
	}
	else if(valor_pinod == 0b11100000){
		// 7
		send_word_lcd(0b00110111);
		
		// Inserir a senha //
		if(state == inserir_senha){
			insert_senha(count_senha, "7");
			count_senha++;
		}
	}
	else if(valor_pinod == 0b01100000){
		// 8
		send_word_lcd(0b00111000);
		
		// Inserir a senha //
		if(state == inserir_senha){
			insert_senha(count_senha, "8");
			count_senha++;
		}
	}
	else if(valor_pinod == 0b10100000){
		// 9
		send_word_lcd(0b00111001);
		
		// Inserir a senha //
		if(state == inserir_senha){
			insert_senha(count_senha, "9");
			count_senha++;
		}
	}
	else if(valor_pinod == 0b00100000){
		// D
		send_word_lcd(0b01000100);
	}
	else if(valor_pinod == 0b11000000){
		// R
		send_word_lcd(0b01010010);
	}
	else if(valor_pinod == 0b01000000){
		// 0
		send_word_lcd(0b00110000);
		
		// Inserir a senha //
		if(state == inserir_senha){
			insert_senha(count_senha, "0");
			count_senha++;
		}
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


// DELAY //

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

// DISPLAY //

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

// MSGS //

void msg_inserir_senha(){
	
	clear_Display();
	
	char frase_1[] = "Insira a senha  ";
	envia_frase(frase_1,l1C1);
	//char frase_2[] = "                ";
	//envia_frase(frase_2,l2C1);
	
}

void msg_desativado(){
	
	char frase_1[] = "Desativado      ";
	envia_frase(frase_1,l1C1);
	char frase_2[] = "";
	envia_frase(frase_2,l2C1);
}

void msg_programacao(){
	
	char frase_1[] = "Programacao     ";
	envia_frase(frase_1,l1C1);
	char frase_2[] = "";
	envia_frase(frase_2,l2C1);

}

// INSERIR SENHA //

void insert_senha(int n,const char *digito){
	
	senha_inserida[n] = digito[0];
	
}

void verify_senha(){
	
	int cont_a = 0;
	int cont_b = 0;
	
	while(mestre[cont_a] != '\0'){
		
		if(mestre[cont_a] == senha_inserida[cont_a]){
			cont_b++;
		}
		cont_a++;
	}
	
	if(cont_b == 4){ 
		
		state = programacao;
		clear_Display();
		
	}else{
		
		state = desativado;
		clear_Display();
	}
	
	delay_ms(20);
	
}

void clear_Display(){
	
	send_instruction_lcd(0x01);
	delay_ms(5);
	
}