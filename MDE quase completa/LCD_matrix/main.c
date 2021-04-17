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
char senha1[] = "1234";
char senha2[] = "1234";
char senha3[] = "1234";

char senha_inserida[5];
bool flag_a_senha = 0;
int addr_senha;

int count_senha = 0;
int cont_a = 0;
int cont_b = 0;

// sensor //
bool flag_sensor = 0;
bool flag_D_sensor = 0;

// Pânico //
bool flag_p = 0;

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
void senha_arm(int n, const char *digito);
void verify_senhaM();
void verify_senhaA();
void verify_senhaD();
void alterar_senha();
void msg_inserir_senha();

// DESATIVADO //
void msg_desativado();

// PROGRAMA //
void msg_programacao();

// ALTERA SENHA //
void msg_altera_senha();

// HABILITAÇÃO E DESABILITAÇÃO DE SENSOR //
void hab_sensor(int n);
void msg_h_sensores();
void msg_d_sensores();

// ZONA //
void msg_h_zona();
void msg_d_zona();
void hab_zonas(int n);

// AJUSTE TEMPORIZADOR DE ATIVAÇÃO //
void msg_temp_ativ();

// AJUSTE TIMEOUT //
void msg_timeout();

// TEMPO SIRENE //
void msg_time_sirene();

// ATIVADO //
void msg_ativado();

// Panico //
void msg_panico();

// Restauracao //
void msg_restauracao();

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
	AssSensorZona,  //qnd tem q salvar algo virá um estado
	ativado,
	panico,
	programacao,
	AjusteTAtivacao,
	AjusteTout,
	AjusteTSirene,
	
	// novos estados //
	inserir_senha,
	inserir_senhaN,
	inserir_senhaA,
	inserir_senhaD,
	verificar_senhaM,
	verificar_senhaA,
	verificar_senhaD,
	HabSensor,
	HabZona, 
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
			msg_ativado();
			break;
			case programacao:
			msg_programacao();
			// inserir senha //
			// ver se a senha está correta //
			// init timer do timeout //
			// fazer a configurações restantes //
			break;
			case panico:
			msg_panico();
			break;
			
			// ROTINAS SECUNDARIAS //
			
			case inserir_senha:
			// Inserndo a senha Mestre //
			//msg_inserir_senha();
			break;
			
			case inserir_senhaA:
			// Inserindo uma das senhas qualquer //
			break;
			
			case inserir_senhaD:
			// Inserir senha Nova //
			break;
			
			case inserir_senhaN:
			// Inserir senha Nova //
			break;
			
			case verificar_senhaM:
			// verificando a senha Mestre//
			verify_senhaM();
			break;
			case verificar_senhaA:
			// verificando a senha A //
			verify_senhaA();
			break;
			
			case verificar_senhaD:
			// verificando a senha D //
			verify_senhaD();
			break;
			
			// Habilita sensores //
			case HabSensor:
			
			break;
			
			// HAB ZONA //
			case HabZona:
			
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
		// Inserir a senha //
		if((state == inserir_senha) & (count_senha < 4)){
			send_word_lcd(0b00110001);
			insert_senha(count_senha, "1");
			count_senha++;
		}
		// Senha Nova //
		if((state == inserir_senhaN) & (count_senha < 4)){
			send_word_lcd(0b00110001);
			senha_arm(count_senha, "1");
			count_senha++;
		}
		// Senha de Ativação //
		if((state == inserir_senhaA) & (count_senha < 4)){
			send_word_lcd(0b00110001);
			insert_senha(count_senha, "1");
			count_senha++;
		}
		// Senha de desativação //
		if((state == inserir_senhaD) & (count_senha < 4)){
			send_word_lcd(0b00110001);
			insert_senha(count_senha, "1");
			count_senha++;
		}
		// Escolhe o endereço da senha a ser alterada //
		if((state == programacao) & (flag_a_senha == 1)){
			state = inserir_senhaN;
			msg_altera_senha();
			addr_senha = 1;
			flag_a_senha = 0;
		}
		// Escolher o sensor a ser habilitado //
		if((state == HabSensor) & (flag_sensor == 0)){
			hab_sensor(1);
		}
		// escolher ZONA p habilitar//
		if((state == HabZona) & (flag_sensor == 0)){
			hab_zonas(1);
		}
	}
	else if(valor_pinod == 0b01110000){
		// 2
		// Inserir a senha Mestre //
		if((state == inserir_senha) & (count_senha < 4)){
			send_word_lcd(0b00110010);
			insert_senha(count_senha, "2");
			count_senha++;
		}
		// Inserir senha Nova //
		if((state == inserir_senhaN) & (count_senha < 4)){
			send_word_lcd(0b00110010);
			senha_arm(count_senha, "2");
			count_senha++;
		}
		// Senha de Ativação //
		if((state == inserir_senhaA) & (count_senha < 4)){
			send_word_lcd(0b00110010);
			insert_senha(count_senha, "2");
			count_senha++;
		}
		// Senha de desativação //
		if((state == inserir_senhaD) & (count_senha < 4)){
			send_word_lcd(0b00110010);
			insert_senha(count_senha, "2");
			count_senha++;
		}
		// Alterar a senha //
		if((state == programacao) & (flag_a_senha == 0)){
			// flag para escolher o endereço da senha //
			flag_a_senha = 1;
		}
		else if((state == programacao) & (flag_a_senha == 1)){
			// Escolhe o endereço da senha a ser alterada //
			state = inserir_senhaN;
			msg_altera_senha();
			addr_senha = 2;
			flag_a_senha = 0;
		}
		// Escolher o sensor a ser habilitado //
		if((state == HabSensor) & (flag_sensor == 0)){
			hab_sensor(2);
		}
		// escolher ZONA p habilitar//
		if((state == HabZona) & (flag_sensor == 0)){
			hab_zonas(2);
		}
	}
	else if(valor_pinod == 0b10110000){
		// 3
		// Inserir a senha //
		if((state == inserir_senha) & (count_senha < 4)){
			send_word_lcd(0b00110011);
			insert_senha(count_senha, "3");
			count_senha++;
		}
		// Inserir senha Nova //
		if((state == inserir_senhaN) & (count_senha < 4)){
			send_word_lcd(0b00110011);
			senha_arm(count_senha, "3");
			count_senha++;
		}
		// Senha de Ativação //
		if((state == inserir_senhaA) & (count_senha < 4)){
			send_word_lcd(0b00110011);
			insert_senha(count_senha, "3");
			count_senha++;
		}
		// Senha de desativação //
		if((state == inserir_senhaD) & (count_senha < 4)){
			send_word_lcd(0b00110011);
			insert_senha(count_senha, "3");
			count_senha++;
		}
		// FUNÇÃO DE HABILITAR SENSOR //
		if((state == programacao) & (flag_a_senha == 0)){
			state = HabSensor;
			if(flag_D_sensor == 0) msg_h_sensores();
			if(flag_D_sensor == 1) msg_d_sensores();
		}
		// Escolhe o endereço da senha a ser alterada //
		if((state == programacao) & (flag_a_senha == 1)){
			state = inserir_senhaN;
			msg_altera_senha();
			addr_senha = 3;
			flag_a_senha = 0;
		}
		// Escolher o sensor a ser habilitado //
		if((state == HabSensor) & (flag_sensor == 0)){
			hab_sensor(3);
		}
		// escolher ZONA p habilitar//
		if((state == HabZona) & (flag_sensor == 0)){
			hab_zonas(3);
		}
	}
	else if(valor_pinod == 0b00110000){
		// P
		// Inserir a senha Mestre para o modo de Programação //
		if(state == desativado){
			state = inserir_senha;
			msg_inserir_senha();
		}
	}
	else if(valor_pinod == 0b11010000){
		// 4
		// Inserir a senha //
		if((state == inserir_senha) & (count_senha < 4)){
			send_word_lcd(0b00110100);
			insert_senha(count_senha, "4");
			count_senha++;
		}
		// Inserir senha Nova //
		if((state == inserir_senhaN) & (count_senha < 4)){
			send_word_lcd(0b00110100);
			senha_arm(count_senha, "4");
			count_senha++;
		}
		// SENHA DE ATIVAÇÃO //
		if((state == inserir_senhaA) & (count_senha < 4)){
			send_word_lcd(0b00110100);
			insert_senha(count_senha, "4");
			count_senha++;
		}
		// SENHA DE DESATIVAÇÃO //
		if((state == inserir_senhaD) & (count_senha < 4)){
			send_word_lcd(0b00110100);
			insert_senha(count_senha, "4");
			count_senha++;
		}
		// Escolher o sensor a ser habilitado //
		if((state == HabSensor) & (flag_sensor == 0)){
			hab_sensor(4);
		}
	}
	else if(valor_pinod == 0b01010000){
		// 5
		
		// Inserir a senha //
		if((state == inserir_senha) & (count_senha < 4)){
			send_word_lcd(0b00110101);
			insert_senha(count_senha, "5");
			count_senha++;
		}
		// Inserir senha Nova //
		if((state == inserir_senhaN) & (count_senha < 4)){
			send_word_lcd(0b00110101);
			senha_arm(count_senha, "5");
			count_senha++;
		}
		// senha de ativação //
		if((state == inserir_senhaA) & (count_senha < 4)){
			send_word_lcd(0b00110101);
			insert_senha(count_senha, "5");
			count_senha++;
		}
		// Senha de desativação //
		if((state == inserir_senhaD) & (count_senha < 4)){
			send_word_lcd(0b00110101);
			insert_senha(count_senha, "5");
			count_senha++;
		}
		// Escolher o sensor a ser habilitado //
		if((state == HabSensor) & (flag_sensor == 0)){
			hab_sensor(5);
		}
		// FUNÇÃO DE HAB ZONA //
		if((state == programacao) & (flag_a_senha == 0)){
			state = HabZona;
			if(flag_D_sensor == 0) msg_h_zona();
			if(flag_D_sensor == 1) msg_d_zona();
		}
	}
	else if(valor_pinod == 0b10010000){
		// 6
		
		// Inserir a senha //
		if((state == inserir_senha) & (count_senha < 4)){
			send_word_lcd(0b00110110);
			insert_senha(count_senha, "6");
			count_senha++;
		}
		// Inserir senha Nova //
		if((state == inserir_senhaN) & (count_senha < 4)){
			send_word_lcd(0b00110110);
			senha_arm(count_senha, "6");
			count_senha++;
		}
		// senha de ativação //
		if((state == inserir_senhaA) & (count_senha < 4)){
			send_word_lcd(0b00110110);
			insert_senha(count_senha, "6");
			count_senha++;
		}
		// Senha de desativação //
		if((state == inserir_senhaD) & (count_senha < 4)){
			send_word_lcd(0b00110110);
			insert_senha(count_senha, "6");
			count_senha++;
		}
		// Escolher o sensor a ser habilitado //
		if((state == HabSensor) & (flag_sensor == 0)){
			hab_sensor(6);
		}
	}
	else if(valor_pinod == 0b00010000){
		// A
		
		// Confirmar senha mestre //
		if((state == inserir_senha) & (count_senha == 4)){
			state = verificar_senhaM;
			count_senha = 0;
		}
		// Ativar o sistema //
		if(state == desativado){
			state = inserir_senhaA;
			msg_inserir_senha();
		}
	}
	else if(valor_pinod == 0b11100000){
		// 7
		// Inserir a senha //
		if((state == inserir_senha) & (count_senha < 4)){
			send_word_lcd(0b00110111);
			insert_senha(count_senha, "7");
			count_senha++;
		}
		// Inserir senha Nova //
		if((state == inserir_senhaN) & (count_senha < 4)){
			send_word_lcd(0b00110111);
			senha_arm(count_senha, "7");
			count_senha++;
		}
		// senha de ativação //
		if((state == inserir_senhaA) & (count_senha < 4)){
			send_word_lcd(0b00110111);
			insert_senha(count_senha, "7");
			count_senha++;
		}
		// Senha de desativação //
		if((state == inserir_senhaD) & (count_senha < 4)){
			send_word_lcd(0b00110111);
			insert_senha(count_senha, "6");
			count_senha++;
		}
		// Escolher o sensor a ser habilitado //
		if((state == HabSensor) & (flag_sensor == 0)){
			hab_sensor(7);
		}
	}
	else if(valor_pinod == 0b01100000){
		// 8
		// Inserir a senha //
		if((state == inserir_senha) & (count_senha < 4)){
			send_word_lcd(0b00111000);
			insert_senha(count_senha, "8");
			count_senha++;
		}
		// Inserir senha Nova //
		if((state == inserir_senhaN) & (count_senha < 4)){
			send_word_lcd(0b00111000);
			senha_arm(count_senha, "8");
			count_senha++;
		}
		if((state == inserir_senhaA) & (count_senha < 4)){
			send_word_lcd(0b00111000);
			insert_senha(count_senha, "8");
			count_senha++;
		}
		if((state == inserir_senhaD) & (count_senha < 4)){
			send_word_lcd(0b00111000);
			insert_senha(count_senha, "6");
			count_senha++;
		}
	}
	else if(valor_pinod == 0b10100000){
		// 9
		// Inserir a senha //
		if((state == inserir_senha) & (count_senha < 4)){
			send_word_lcd(0b00111001);
			insert_senha(count_senha, "9");
			count_senha++;
		}
		// Inserir senha Nova //
		if((state == inserir_senhaN) & (count_senha < 4)){
			send_word_lcd(0b00111001);
			senha_arm(count_senha, "9");
			count_senha++;
		}
		if((state == inserir_senhaA) & (count_senha < 4)){
			send_word_lcd(0b00111001);
			insert_senha(count_senha, "9");
			count_senha++;
		}
		if((state == inserir_senhaD) & (count_senha < 4)){
			send_word_lcd(0b00111001);
			insert_senha(count_senha, "9");
			count_senha++;
		}
	}
	else if(valor_pinod == 0b00100000){
		// D
		if(state == ativado){
			state = inserir_senhaD;
			msg_inserir_senha();
		}
		// Confirmar senha mestre //
		if((state == inserir_senha) & (count_senha == 4)){
			state = verificar_senhaM;
			flag_D_sensor = 1;
			count_senha = 0;
		}
	}
	else if(valor_pinod == 0b11000000){
		// R
		// Recuperãção //
		state = recuperacao;
		clear_Display();
		msg_restauracao();
		
		// REINICIALIZANDO TODO OS DADOS //
		flag_p = 0;
		flag_sensor = 0;
		flag_D_sensor = 0;
		flag_a_senha = 0;
		// desativando a interrupão externa //
		EICRA = (0<<ISC11) | (0<<ISC10);
		EIMSK = (0<<INT1);
		
		delay_ms(50);
		
		// Inicializando algumas coisas //
		init_int1();
	}
	else if(valor_pinod == 0b01000000){
		// 0
		
		// Inserir a senha //
		if((state == inserir_senha) & (count_senha < 4)){
			send_word_lcd(0b00110000);
			insert_senha(count_senha, "0");
			count_senha++;
		}
		// Inserir senha Nova //
		if((state == inserir_senhaN) & (count_senha < 4)){
			send_word_lcd(0b00110000);
			senha_arm(count_senha, "0");
			count_senha++;
		}
		// Senha de Ativação //
		if((state == inserir_senhaA) & (count_senha < 4)){
			send_word_lcd(0b00110000);
			insert_senha(count_senha, "0");
			count_senha++;
		}
		// Senha de Desativação //
		if((state == inserir_senhaD) & (count_senha < 4)){
			send_word_lcd(0b00110000);
			insert_senha(count_senha, "0");
			count_senha++;
		}
		// Escolher o sensor a ser habilitado //
		if((state == HabSensor) & (flag_sensor == 0)){
			hab_sensor(0);
		}
	}
	else if(valor_pinod == 0b10000000){
		// S
		// Pânico //
		if(flag_p == 0){
			state = panico;
			flag_p = 1;
		}else if(flag_p == 1){
			state = desativado;
		}		
	}
	else if(valor_pinod == 0b000000000){
		// E
		if((state == inserir_senhaN) & (count_senha == 4)){
			state = desativado;
			clear_Display();
			count_senha = 0;
		}
		if((state == inserir_senhaA) & (count_senha == 4)){
			state = verificar_senhaA;
			clear_Display();
			count_senha = 0;
		}
		if((state == inserir_senhaD) & (count_senha == 4)){
			state = verificar_senhaD;
			clear_Display();
			count_senha = 0;
		}
		if((state == HabSensor) & (flag_sensor == 1)){
			//
			state = desativado;
			flag_sensor = 0;
		}
		if((state == HabZona) & (flag_sensor == 1)){
			//
			state = desativado;
			flag_sensor = 0;
		}
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

void clear_Display(){
	
	send_instruction_lcd(0x01);
	delay_ms(15);
	
}

// MSGS //

void msg_inserir_senha(){
	
	clear_Display();
	
	char frase_1[] = "Insira a senha..";
	envia_frase(frase_1,l1C1);
	send_instruction_lcd(0xC0);
	
}

void msg_desativado(){
	
	char frase_1[] = "   Desativado   ";
	envia_frase(frase_1,l1C1);
	char frase_2[] = "";
	envia_frase(frase_2,l2C1);
	
	delay_ms(3);
}

void msg_ativado(){
	
	char frase_1[] = "    Ativado  ";
	envia_frase(frase_1,l1C1);
	char frase_2[] = "";
	envia_frase(frase_2,l2C1);
	
	delay_ms(3);
	
}

void msg_programacao(){
	
	char frase_1[] = "   Programacao  ";
	envia_frase(frase_1,l1C1);
	char frase_2[] = "";
	envia_frase(frase_2,l2C1);
	
	delay_ms(3);

}

void msg_altera_senha(){
	
	clear_Display();
	
	char frase_1[] = " Senha desejada";
	envia_frase(frase_1,l1C1);
	send_instruction_lcd(0xC0);
}

void msg_h_sensores(){
	
	clear_Display();
	
	char frase_1[] = "  Sensor a ser";
	envia_frase(frase_1,l1C1);
	char frase_2[] = "   habilitado";
	envia_frase(frase_2,l2C1);
	
}

void msg_d_sensores(){
	
	clear_Display();
	
	char frase_1[] = "  Sensor a ser";
	envia_frase(frase_1,l1C1);
	char frase_2[] = "  desabilitado";
	envia_frase(frase_2,l2C1);
	
}

void msg_panico(){
	
	char frase_1[] = "   Mode Panic";
	envia_frase(frase_1,l1C1);
	char frase_2[] = "  Police coming";
	envia_frase(frase_2,l2C1);
	
}

void msg_restauracao(){
	
	char frase_1[] = "Erasing.....";
	envia_frase(frase_1,l1C1);
	
}

void msg_h_zona(){
	
	clear_Display();
	
	char frase_1[] = "  Zona a ser";
	envia_frase(frase_1,l1C1);
	char frase_2[] = "  habilitada";
	envia_frase(frase_2,l2C1);
	
}

void msg_d_zona(){
	
	clear_Display();
	
	char frase_1[] = "  Zona a ser";
	envia_frase(frase_1,l1C1);
	char frase_2[] = " desabilitada";
	envia_frase(frase_2,l2C1);
	
}

// INSERIR SENHA //

void insert_senha(int n,const char *digito){
	
	senha_inserida[n] = digito[0];
	
}

void senha_arm(int n,const char *digito){
	
	//senha1[n] = digito[0];
	
	if(addr_senha == 1) senha1[n] = digito[0];
	if(addr_senha == 2) senha2[n] = digito[0];
	if(addr_senha == 3) senha3[n] = digito[0];
	
}

void verify_senhaM(){
	
	cont_a = 0;
	cont_b = 0;
	
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
	
}

void verify_senhaA(){
	
	cont_a = 0;
	cont_b = 0;
	
	// Verifica a senha 0 //
	while(mestre[cont_a] != '\0'){
		
		if(mestre[cont_a] == senha_inserida[cont_a]){
			cont_b++;
		}
		cont_a++;
	}
	if(cont_b == 4){
		
		state = ativado;
		clear_Display();
		return;
		
	}
	
	// Verifica a senha 1 //
	
	cont_a = 0;
	cont_b = 0;
	
	while(senha1[cont_a] != '\0'){
		
		if(senha1[cont_a] == senha_inserida[cont_a]){
			cont_b++;
		}
		cont_a++;
	}
	if(cont_b == 4){
		
		state = ativado;
		clear_Display();
		return;
		
	}
	
	// Verifica a senha 2 //
	
	cont_a = 0;
	cont_b = 0;
	
	while(senha2[cont_a] != '\0'){
		
		if(senha2[cont_a] == senha_inserida[cont_a]){
			cont_b++;
		}
		cont_a++;
	}
	if(cont_b == 4){
		
		state = ativado;
		clear_Display();
		return;
		
	}
	
	// Verifica a senha 3 //
	
	cont_a = 0;
	cont_b = 0;
	
	while(senha3[cont_a] != '\0'){
		
		if(senha3[cont_a] == senha_inserida[cont_a]){
			cont_b++;
		}
		cont_a++;
	}
	if(cont_b == 4){
		
		state = ativado;
		clear_Display();
		return;
		
	}else{
		
		state = desativado;
		clear_Display();
		return;
	}
}

void verify_senhaD(){
	
	cont_a = 0;
	cont_b = 0;
	
	// Verifica a senha 0 //
	while(mestre[cont_a] != '\0'){
		
		if(mestre[cont_a] == senha_inserida[cont_a]){
			cont_b++;
		}
		cont_a++;
	}
	if(cont_b == 4){
		
		state = desativado;
		clear_Display();
		return;
		
	}
	
	// Verifica a senha 1 //
	
	cont_a = 0;
	cont_b = 0;
	
	while(senha1[cont_a] != '\0'){
		
		if(senha1[cont_a] == senha_inserida[cont_a]){
			cont_b++;
		}
		cont_a++;
	}
	if(cont_b == 4){
		
		state = desativado;
		clear_Display();
		return;
		
	}
	
	// Verifica a senha 2 //
	
	cont_a = 0;
	cont_b = 0;
	
	while(senha2[cont_a] != '\0'){
		
		if(senha2[cont_a] == senha_inserida[cont_a]){
			cont_b++;
		}
		cont_a++;
	}
	if(cont_b == 4){
		
		state = desativado;
		clear_Display();
		return;
		
	}
	
	// Verifica a senha 3 //
	
	cont_a = 0;
	cont_b = 0;
	
	while(senha3[cont_a] != '\0'){
		
		if(senha3[cont_a] == senha_inserida[cont_a]){
			cont_b++;
		}
		cont_a++;
	}
	if(cont_b == 4){
		
		state = desativado;
		clear_Display();
		return;
		
		}else{
		
		state = ativado;
		clear_Display();
		return;
	}
}

// HABILITAR SENSOR //

void hab_sensor(int n){
	
	flag_sensor = 1; // Essa flag serve para dizer se ja foi 
					 //	selecionado o sensor/Zona a ser habilitado ou desabilitado //
	// Colocar as funções //
	if(flag_D_sensor == 0){
		// Habilitação do sensor //
		
	}else if(flag_D_sensor == 1){
		// Desabilitação do sensor //
		
		flag_D_sensor = 0;
	}
}

// HABILITA ZONA //

void hab_zonas(int n){
	
	flag_sensor = 1; // Essa flag serve para dizer se ja foi
	//	selecionado o sensor/Zona a ser habilitado ou desabilitado //
	// Colocar as funções //
	if(flag_D_sensor == 0){
		// Habilitação da Zona //
		
	}else if(flag_D_sensor == 1){
		// Desabilitação da Zona //
		
		flag_D_sensor = 0;
	}
}