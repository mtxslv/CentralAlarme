/*
 * main.c
 *
 * Created: 4/8/2021 2:55:04 PM
 *  Author: erika
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU 8000000UL
#define FREQCPU 8000000UL
#define TAXA 9600
#define BUZZER     PD0
#define OUT_UART   PD1
#define EXT_OUT    PD2
#define INT_MATRIZ PD3
#define MATRIZ1    PD7
#define MATRIZ2    PD6
#define MATRIZ3    PD5
#define MATRIZ4    PD4
#define EXT_IN     PB0
#define CLK_IN_OUT PB1
#define CS         PB2
#define MOSI       PB3
#define MISO       PB4
#define SCLK       PB5
#define PS_EXTIN   PB6
#define PS_EXTOUT  PB7
#define D4         PC0
#define D5         PC1
#define D6         PC2
#define D7         PC3
#define RS_LCD     PC4
#define EN_LCD     PC5

#define clock_shftin PINB2
#define P_Shftin PINB1
// talvez possa ser o mesmo clock do extensor entrada

#define Pinout_serial PINC0 // sa?da de dados serial
#define pLoad_pin PINB7 // controle carga paralela

// TUDO DE SERIAL SHIFT IN E OUT ? AQUI PEGUI?A DE ORGANIZAR J? //

uint8_t varLeituraSerial = 0;
uint8_t varEscritaSerial = 0xFF;

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
    PINC |= (1 << Pinout_serial);
}

void shiftIn()
{

	// Carga paralela do registrador
	PORTB |= (1 << P_Shftin); // P/S' vai pra 1
	PORTB |= (1 << clock_shftin); // CLK  vai pra 1
	PORTB &= ~(1 << clock_shftin); // CLK  vai pra 0
	PORTB &= ~(1 << P_Shftin); // P/S' vai pra 0
		
	for (uint8_t iter = 0; iter<8 ; iter++){

 
       if(PINB & (1 << PINB0)){
	       if(varLeituraSerial | ~(1<<iter)) // 0b00000000 | 0b11111111 = 0b11111111
				varLeituraSerial = SetBit(varLeituraSerial,iter); // QUANDO FOR 0, COLOCA A SA?DA PRA 1
       }
       else{
	       if(varLeituraSerial & (1<<iter)) 
				varLeituraSerial = ClearBit(varLeituraSerial,iter); // QUANDO FOR 1, IR? SETAR O VALOR NO REGISTRADOR
       }
	   	PORTB |= (1 << clock_shftin); // CLK vai pra 1
		PORTB &= ~(1 << clock_shftin); // CLK vai pra 0
	}
}

/*
Como ShiftOut funciona?
1. ST_CP <- LOW
2. loop 8x
3.		dataPin <- bit a ser enviado
4.		SH_CP <- 1
5.		SH_CP <- 0 
6. ST_CP <- HIGH 
DS == serial data transfer pin
ST_CP == parallel load pin
SH_CP == clock pin
*/

void shiftOut() // ainda n?o testei mas fiz a parte de Setar e dar Clear no pino
{
    // pinos de controle do CI
//    PINB &= ~(1<<pLoad_pin);// PINB7
		for (uint8_t iter = 0; iter<8; iter++){
			if( ((varEscritaSerial>>iter)& 0x1) ) // aqui deve vir a resposta a "qual o bit que est? guardado na posi??o iter do byte a ser enviado?"
			PORTC |= 0x01; // 0b00000001 -> ligar PORTC0. (Pinout_serial)
			else
			PORTC &= 0xFE;// 0b11111110 -> desligar PORTC0. (Pinout_serial)
			
			PORTB |= (1 << clock_shftin); // PINB3,CLK vai pra 1
			PORTB &= ~(1 << clock_shftin); // PINB3,CLK vai pra 0
		}
		PINB &= ~(1<<pLoad_pin);
		PINB |= (1<<pLoad_pin); // PINB7
/*	    
   for (uint8_t iter = 0; iter<8 ; iter++){
       if(PINB & (1 << Pinout_serial))
       {
           if(varEscritaSerial & ~(1<<iter)) OutSerial(); // QUANDO FOR 0, COLOCA A SA?DA PRA 1
       }
       else
       {
           if(varEscritaSerial & (1<<iter)) OutSerial(); // QUANDO FOR 1, IR? SETAR O VALOR NO REGISTRADOR
       }           
	   
	   
	   PORTB |= (1 << clock_shftout); // PINB3,CLK vai pra 1 
       PORTB &= ~(1 << clock_shftout); // PINB3,CLK vai pra 0 
	}
	// parallel load
	PINB |= (1<<pLoad_pin); // PINB7
    PINB &= ~(1<<pLoad_pin);
*/	
}


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

// P?nico //
bool flag_p = 0;

// zona + sensor //
int idzona;
int idsensor;
bool flag_z = 0;
bool flag_z1 = 0;

// timer //

uint8_t flags = 0;

uint8_t  cont_timeout = 0, cont_R = 0;
uint16_t cont_sirene = 0, cont_ativacao = 0, timeout, t_ativacao, t_sirene;

int get_timeA[3];
int get_timeout[2];
int get_timeS[3];
bool flag_timeout = 0;
bool flag_tsirene = 0;
bool flag_tat = 0;
int cont_t = 0;
int cont_aa = 0;
int cont_s = 0;
int User;

/*############ ZONAS ############# */

struct zona{

	// N?mero da zona //
	int id;
	// flag de ativa??o da zona //
	bool active;
	// Sensores //
	bool sensor0;
	bool sensor1;
	bool sensor2;
	bool sensor3;
	bool sensor4;
	bool sensor5;
	bool sensor6;
	bool sensor7;
	
} zona1, zona2, zona3;

struct sensor{

	//id do sensor //
	int id;
	// se est? ativo //
	bool active;
	// se o sensor detectou algo //
	bool detect;
	
} sensor0, sensor1, sensor2, sensor3, sensor4, sensor5, sensor6, sensor7; 

/*############ FUN??ES #############*/
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

// HABILITA??O E DESABILITA??O DE SENSOR //
void hab_sensor(int n);
void msg_h_sensores();
void msg_d_sensores();

// ZONA //
void msg_h_zona();
void msg_d_zona();
void hab_zonas(int n);

// ASSOCIA??O DE SENSOR COM ZONA //
void msg_h_ass();
void msg_d_ass();
void assSensorZona(int x, int y);

// AJUSTE TEMPORIZADOR DE ATIVA??O //
void set_timeA();
void msg_temp_ativ();

// AJUSTE TIMEOUT //
void set_timeout();
void msg_ajusteTout();

// TEMPO SIRENE //
void set_timeS();
void msg_time_sirene();

// ATIVADO //
void msg_ativado();

// Panico //
void msg_panico();

// Restauracao //
void msg_restauracao();

// TEMPORIZADORES //
void SetupTimer1();
void StartTimer1();
void StopTimer1();

void SetupTimer0();
void StartTimer0();
void StopTimer0();

void ZerarTemps();

void StartSirene();
void StopSirene();

// DECLARA??O DE FUN??ES REL?GIO
void SPI_Master_Setup(); // Fun??o para fazer o setup da comunica??o SPI.
void SPI_Master_Transmit(uint8_t dado); // Fun??o para o envio de dados.
uint8_t SPI_Read(uint8_t code); // Fun??o que faz leitura da informa??o enviada pelo o escravo.
uint8_t RTC_Read_Hours(); // Fun??o que faz a solicita??o e leitura da hora vinda do RTC.
uint8_t RTC_Read_Minu(); // Fun??o que faz a solicita??o e leitura dos minutos que vem do RTC.
uint8_t RTC_Read_Day(); // Fun??o que faz a solicita??o e leitura da data que vem do RTC.

// DECLARA??O DE FUN??ES UART
void IniciarUART (uint32_t taxa, uint32_t freqCpu);
uint8_t ProntoTXUART (void);
void EnviarTXUART (uint8_t dado);
void EnviarStringUART (const char *c);
void UARTA(uint8_t dia, uint8_t hora);
void UARTP(uint8_t dia, uint8_t hora);
void UARTD(uint8_t dia, uint8_t hora);

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
	senhaAnova,  //qnd tem q salvar algo vir? um estado
	ativado,
	panico,
	programacao,
	
	// s? pra organizar //
	inserir_senha,
	inserir_senhaN,
	inserir_senhaA,
	inserir_senhaD,
	verificar_senhaM,
	verificar_senhaA,
	verificar_senhaD,
	HabSensor,
	HabZona,
	AssSensorZona,
	AjusteTAtivacao,
	AjusteTout,
	AjusteTSirene,
};

// ESTADO INICIAL //
enum states state=recuperacao;

// ZONAS CRIADAS //
struct zona zona1 = {.id = 1, .active = 0};

struct zona zona2 = {.id = 2, .active = 0};

struct zona zona3 = {.id = 3, .active = 0};

// SENSORES //
struct sensor sensor0 = {.id = 0, .active = 0};
struct sensor sensor1 = {.id = 1, .active = 0};
struct sensor sensor2 = {.id = 2, .active = 0};
struct sensor sensor3 = {.id = 3, .active = 0};
struct sensor sensor4 = {.id = 4, .active = 0};
struct sensor sensor5 = {.id = 5, .active = 0};
struct sensor sensor6 = {.id = 6, .active = 0};
struct sensor sensor7 = {.id = 7, .active = 0};


int main(void)
{
	/*// AJUSTE DOS PINOS DO ATMEGA //
	// display pinos //
	DDRB = 0X3F;
	// matrix pinos //
	DDRD = 0x00;
	PORTD = 0xF0;*/
	
	DDRB &= 0b11111110; // o pino PB0 ser? usado como leitura do serial,
	DDRB |= 0b10001110; // os pinos PB1 e PB2 ser? a sa?da de clock (CLK) e o controle paralelo/serial (P/S')
	DDRD = 0xFF; // PORTD ? sa?da (s? pra teste)
	/* Replace with your application code */
	DDRC |= 0b00000001; // sa?da dados serial
	
	//DISPLAY PINOS
	DDRC = 0b01111110;
	
	//defin??es finais - depois dos setups menos o SPI
	DDRB = 0x1F;
	//DDRB =  (1 << CLK_IN_OUT)|(1 << CS)|(1 << PS_EXTIN)|(1 << PS_EXTOUT)|(1 << PS_EXTIN);
	DDRD =  (1 << EXT_OUT)|(1 << OUT_UART)|(1 << BUZZER);
	PORTD = (1 << MATRIZ1)|(1 << MATRIZ2)|(1 << MATRIZ3)|(1 << MATRIZ4);
	
	// INIALIZA??ES DO SISTEMA //
	SetupTimer1();
	SetupTimer0();
	init_int1();
	inicializa_display();
	SPI_Master_Setup();
	IniciarUART(TAXA, FREQCPU);
	sei();
	
    while(1)
    {	
		// SENSORES //
		shiftIn();
		varEscritaSerial = varLeituraSerial;
		shiftOut();
		// SWITCH CASE PARA A SELE??O DO MODO //
        switch(state){
			// ROTINAS PRINCIPAIS  //
			case recuperacao:
			StopTimer1(); // para o timer, caso a m?quina estivesse em programa??o
			// aqui pararia a sirene
			timeout = 12;
			t_sirene = 5;
			t_ativacao = 5;
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
			// ver se a senha est? correta //
			// init timer do timeout //
			// fazer a configura??es restantes //
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
			
			// ASSCIA??O D SENSOR E ZONA //
			case AssSensorZona:
			
			break;
			
			// Tempo de ativa??o //
			case AjusteTAtivacao:
			
			break;
			
			// Timeout //
			case AjusteTout:
			
			break;
			
			// tempo sinere //
			case AjusteTSirene:
			
			break;
			
		}
    }
}

// INTERRUP??O EXTERNA //

void init_int1(){
	
	EICRA = (1<<ISC11) | (1<<ISC10);
	EIMSK = (1<<INT1);
	
}

ISR(INT1_vect){
	// Valor do pino de entrada //
	/*valor_pinod = (PIND & 0xF0);*/
	
	valor_pinod = (PIND & ((1 << MATRIZ1)|(1 << MATRIZ2)|(1 << MATRIZ3)|(1 << MATRIZ4)));
	
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
		// Senha de Ativa??o //
		if((state == inserir_senhaA) & (count_senha < 4)){
			send_word_lcd(0b00110001);
			insert_senha(count_senha, "1");
			count_senha++;
		}
		// Senha de desativa??o //
		if((state == inserir_senhaD) & (count_senha < 4)){
			send_word_lcd(0b00110001);
			insert_senha(count_senha, "1");
			count_senha++;
		}
		// Escolhe o endere?o da senha a ser alterada //
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
		// ESOLHER O SENSOR DA ASSOCIA??O //
		if((state == AssSensorZona) & (flag_z1 == 0)){
			if(flag_z == 0){
				idsensor = 1;
				flag_z = 1;
			}else if(flag_z == 1){
				idzona = 1;
				flag_z1 = 1;
			}
		}
		// DETERMINAR O TIMEOUT //
		if((state == AjusteTout) & (cont_t < 2)){
			get_timeout[cont_t] = 1;
			cont_t++;
		}
		// DETERMINAR T SIRENE //
		if((state == AjusteTSirene) & (cont_t < 3)){
			get_timeS[cont_s] = 1;
			cont_s++;
		}
		// DETERMINAR T ATIVA??O //
		if((state == AjusteTAtivacao) & (cont_t < 3)){
			get_timeA[cont_aa] = 1;
			cont_aa++;
		}
	}
	else if(valor_pinod == 0xE0 ){
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
		// Senha de Ativa??o //
		if((state == inserir_senhaA) & (count_senha < 4)){
			send_word_lcd(0b00110010);
			insert_senha(count_senha, "2");
			count_senha++;
		}
		// Senha de desativa??o //
		if((state == inserir_senhaD) & (count_senha < 4)){
			send_word_lcd(0b00110010);
			insert_senha(count_senha, "2");
			count_senha++;
		}
		// Alterar a senha //
		if((state == programacao) & ((flag_a_senha == 0) & (flag_timeout == 0))){
			// flag para escolher o endere?o da senha //
			flag_a_senha = 1;
		}
		else if((state == programacao) & (flag_a_senha == 1)){
			// Escolhe o endere?o da senha a ser alterada //
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
		// ESOLHER O SENSOR DA ASSOCIA??O //
		if((state == AssSensorZona) & (flag_z1 == 0)){
			if(flag_z == 0){
				idsensor = 2;
				flag_z = 1;
			}else if(flag_z == 1){
				idzona = 2;
				flag_z1 = 1;
			}
		}
		// DETERMINAR O TIMEOUT //
		if((state == AjusteTout) & (cont_t < 2)){
			get_timeout[cont_t] = 2;
			cont_t++;
		}
		// DETERMINAR T SIRENE //
		if((state == AjusteTSirene) & (cont_t < 3)){
			get_timeS[cont_s] = 2;
			cont_s++;
		}
		// DETERMINAR T ATIVA??O //
		if((state == AjusteTAtivacao) & (cont_t < 3)){
			get_timeA[cont_aa] = 2;
			cont_aa++;
		}
	}
	else if(valor_pinod == 0xD0){
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
		// Senha de Ativa??o //
		if((state == inserir_senhaA) & (count_senha < 4)){
			send_word_lcd(0b00110011);
			insert_senha(count_senha, "3");
			count_senha++;
		}
		// Senha de desativa??o //
		if((state == inserir_senhaD) & (count_senha < 4)){
			send_word_lcd(0b00110011);
			insert_senha(count_senha, "3");
			count_senha++;
		}
		// FUN??O DE HABILITAR SENSOR //
		if((state == programacao) & ((flag_a_senha == 0) & (flag_timeout == 0))){
			state = HabSensor;
			if(flag_D_sensor == 0) msg_h_sensores();
			if(flag_D_sensor == 1) msg_d_sensores();
		}
		// Escolhe o endere?o da senha a ser alterada //
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
		// ESOLHER O SENSOR DA ASSOCIA??O //
		if((state == AssSensorZona) & (flag_z1 == 0)){
			if(flag_z == 0){
				idsensor = 3;
				flag_z = 1;
			}else if(flag_z == 1){
				idzona = 3;
				flag_z1 = 1;
			}
		}
		// DETERMINAR O TIMEOUT //
		if((state == AjusteTout) & (cont_t < 2)){
			get_timeout[cont_t] = 3;
			cont_t++;
		}
		// DETERMINAR T SIRENE //
		if((state == AjusteTSirene) & (cont_t < 3)){
			get_timeS[cont_s] = 3;
			cont_s++;
		}
		// DETERMINAR T ATIVA??O //
		if((state == AjusteTAtivacao) & (cont_t < 3)){
			get_timeA[cont_aa] = 3;
			cont_aa++;
		}
	}
	else if(valor_pinod == 0xC0){
		// P
		// Inserir a senha Mestre para o modo de Programa??o //
		if(state == desativado){
			state = inserir_senha;
			msg_inserir_senha();
			StartTimer1();
		}
	}
	else if(valor_pinod == 0xB0){
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
		// SENHA DE ATIVA??O //
		if((state == inserir_senhaA) & (count_senha < 4)){
			send_word_lcd(0b00110100);
			insert_senha(count_senha, "4");
			count_senha++;
		}
		// SENHA DE DESATIVA??O //
		if((state == inserir_senhaD) & (count_senha < 4)){
			send_word_lcd(0b00110100);
			insert_senha(count_senha, "4");
			count_senha++;
		}
		// Escolher o sensor a ser habilitado //
		if((state == HabSensor) & (flag_sensor == 0)){
			hab_sensor(4);
		}
		// FUN??O DE ASSOCIA??O DE SENSOR E ZONA //
		if((state == programacao) & ((flag_a_senha == 0) & (flag_timeout == 0))){
			state = AssSensorZona;
			if(flag_D_sensor == 0) msg_h_ass();
			if(flag_D_sensor == 1) msg_d_ass();
		}
		// ESOLHER O SENSOR DA ASSOCIA??O //
		if((state == AssSensorZona) & ((flag_z1 == 0) & (flag_z == 0))){
				idsensor = 4;
				flag_z1 = 1;
		}
		// DETERMINAR O TIMEOUT //
		if((state == AjusteTout) & (cont_t < 2)){
			get_timeout[cont_t] = 4;
			cont_t++;
		}
		// DETERMINAR T SIRENE //
		if((state == AjusteTSirene) & (cont_t < 3)){
			get_timeS[cont_s] = 4;
			cont_s++;
		}
		// DETERMINAR T ATIVA??O //
		if((state == AjusteTAtivacao) & (cont_t < 3)){
			get_timeA[cont_aa] = 4;
			cont_aa++;
		}
	}
	else if(valor_pinod == 0xA0){
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
		// senha de ativa??o //
		if((state == inserir_senhaA) & (count_senha < 4)){
			send_word_lcd(0b00110101);
			insert_senha(count_senha, "5");
			count_senha++;
		}
		// Senha de desativa??o //
		if((state == inserir_senhaD) & (count_senha < 4)){
			send_word_lcd(0b00110101);
			insert_senha(count_senha, "5");
			count_senha++;
		}
		// Escolher o sensor a ser habilitado //
		if((state == HabSensor) & (flag_sensor == 0)){
			hab_sensor(5);
		}
		// FUN??O DE HAB ZONA //
		if((state == programacao) & ((flag_a_senha == 0) & (flag_timeout == 0))){
			state = HabZona;
			if(flag_D_sensor == 0) msg_h_zona();
			if(flag_D_sensor == 1) msg_d_zona();
		}
		// ESOLHER O SENSOR DA ASSOCIA??O //
		if((state == AssSensorZona) & ((flag_z1 == 0) & (flag_z == 0))){
			idsensor = 5;
			flag_z1 = 1;
		}
		// DETERMINAR O TIMEOUT //
		if((state == AjusteTout) & (cont_t < 2)){
			get_timeout[cont_t] = 5;
			cont_t++;
		}
		// DETERMINAR T SIRENE //
		if((state == AjusteTSirene) & (cont_t < 3)){
			get_timeS[cont_s] = 5;
			cont_s++;
		}
		// DETERMINAR T ATIVA??O //
		if((state == AjusteTAtivacao) & (cont_t < 3)){
			get_timeA[cont_aa] = 5;
			cont_aa++;
		}
	}
	else if(valor_pinod == 0x90){
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
		// senha de ativa??o //
		if((state == inserir_senhaA) & (count_senha < 4)){
			send_word_lcd(0b00110110);
			insert_senha(count_senha, "6");
			count_senha++;
		}
		// Senha de desativa??o //
		if((state == inserir_senhaD) & (count_senha < 4)){
			send_word_lcd(0b00110110);
			insert_senha(count_senha, "6");
			count_senha++;
		}
		// Escolher o sensor a ser habilitado //
		if((state == HabSensor) & (flag_sensor == 0)){
			hab_sensor(6);
		}
		// ESOLHER O SENSOR DA ASSOCIA??O //
		if((state == AssSensorZona) & ((flag_z1 == 0) & (flag_z == 0))){
			idsensor = 6;
			flag_z1 = 1;
		}
		// DETERMINAR O TIMEOUT //
		if((state == AjusteTout) & (cont_t < 2)){
			get_timeout[cont_t] = 6;
			cont_t++;
		}
		// DETERMINAR T SIRENE //
		if((state == AjusteTSirene) & (cont_t < 3)){
			get_timeS[cont_s] = 6;
			cont_s++;
		}
		// Temporizador de ativa??o //
		if((state == programacao) & ((flag_a_senha == 0) & (flag_timeout == 0))){
			state = AjusteTAtivacao;
			flag_tat = 1;
			msg_temp_ativ();
		}
		// DETERMINAR T ATIVA??O //
		if((state == AjusteTAtivacao) & (cont_t < 3)){
			get_timeA[cont_aa] = 6;
			cont_aa++;
		}
		
	}
	else if(valor_pinod == 0x80){
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
	else if(valor_pinod == 0x70){
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
		// senha de ativa??o //
		if((state == inserir_senhaA) & (count_senha < 4)){
			send_word_lcd(0b00110111);
			insert_senha(count_senha, "7");
			count_senha++;
		}
		// Senha de desativa??o //
		if((state == inserir_senhaD) & (count_senha < 4)){
			send_word_lcd(0b00110111);
			insert_senha(count_senha, "6");
			count_senha++;
		}
		// Escolher o sensor a ser habilitado //
		if((state == HabSensor) & (flag_sensor == 0)){
			hab_sensor(7);
		}
		// ESOLHER O SENSOR DA ASSOCIA??O //
		if((state == AssSensorZona) & ((flag_z1 == 0) & (flag_z == 0))){
			idsensor = 7;
			flag_z1 = 1;
		}
		// DETERMINAR O TIMEOUT //
		if((state == AjusteTout) & (cont_t < 2)){
			get_timeout[cont_t] = 7;
			cont_t++;
		}
		// Temporizador de timeout //
		if((state == programacao) & ((flag_a_senha == 0) & (flag_timeout == 0))){
			state = AjusteTout;
			flag_timeout = 1;
			msg_ajusteTout();
		}
		// DETERMINAR T SIRENE //
		if((state == AjusteTSirene) & (cont_t < 3)){
			get_timeS[cont_s] = 7;
			cont_s++;
		}
		// DETERMINAR T ATIVA??O //
		if((state == AjusteTAtivacao) & (cont_t < 3)){
			get_timeA[cont_aa] = 7;
			cont_aa++;
		}
	}
	else if(valor_pinod == 0x60){
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
		// DETERMINAR O TIMEOUT //
		if((state == AjusteTout) & (cont_t < 2)){
			get_timeout[cont_t] = 8;
			cont_t++;
		}
		// DETERMINAR T SIRENE //
		if((state == AjusteTSirene) & (cont_t < 3)){
			get_timeS[cont_s] = 8;
			cont_s++;
		}
		// Temporizador de sirene //
		if((state == programacao) & ((flag_a_senha == 0) & (flag_timeout == 0))){
			state = AjusteTSirene;
			flag_tsirene = 1;
			msg_time_sirene();
		}
		// DETERMINAR T ATIVA??O //
		if((state == AjusteTAtivacao) & (cont_t < 3)){
			get_timeA[cont_aa] = 8;
			cont_aa++;
		}
	}
	else if(valor_pinod == 0x50){
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
		// DETERMINAR O TIMEOUT //
		if((state == AjusteTout) & (cont_t < 2)){
			get_timeout[cont_t] = 9;
			cont_t++;
		}
		// DETERMINAR T SIRENE //
		if((state == AjusteTSirene) & (cont_t < 3)){
			get_timeS[cont_s] = 9;
			cont_s++;
		}
		// DETERMINAR T ATIVA??O //
		if((state == AjusteTAtivacao) & (cont_t < 3)){
			get_timeA[cont_aa] = 9;
			cont_aa++;
		}
	}
	else if(valor_pinod == 0x40){
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
	else if(valor_pinod == 0x30){
		// R
		// Recuper???o //
		state = recuperacao;
		clear_Display();
		msg_restauracao();
		
		// REINICIALIZANDO TODO OS DADOS //
		flag_p = 0;
		flag_sensor = 0;
		flag_D_sensor = 0;
		flag_a_senha = 0;
		// desativando a interrup?o externa //
		EICRA = (0<<ISC11) | (0<<ISC10);
		EIMSK = (0<<INT1);
		
		delay_ms(50);
		
		// Inicializando algumas coisas //
		init_int1();
	}
	else if(valor_pinod == 0x20){
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
		// Senha de Ativa??o //
		if((state == inserir_senhaA) & (count_senha < 4)){
			send_word_lcd(0b00110000);
			insert_senha(count_senha, "0");
			count_senha++;
		}
		// Senha de Desativa??o //
		if((state == inserir_senhaD) & (count_senha < 4)){
			send_word_lcd(0b00110000);
			insert_senha(count_senha, "0");
			count_senha++;
		}
		// Escolher o sensor a ser habilitado //
		if((state == HabSensor) & (flag_sensor == 0)){
			hab_sensor(0);
		}
		// ESOLHER O SENSOR DA ASSOCIA??O //
		if((state == AssSensorZona) & ((flag_z1 == 0) & (flag_z == 0))){
			idsensor = 0;
			flag_z1 = 1;
		}
		// DETERMINAR O TIMEOUT //
		if((state == AjusteTout) & (cont_t < 2)){
			get_timeout[cont_t] = 0;
			cont_t++;
		}
		// DETERMINAR T SIRENE //
		if((state == AjusteTSirene) & (cont_t < 3)){
			get_timeS[cont_s] = 0;
			cont_s++;
		}
		// DETERMINAR T ATIVA??O //
		if((state == AjusteTAtivacao) & (cont_t < 3)){
			get_timeA[cont_aa] = 0;
			cont_aa++;
		}
	}
	else if(valor_pinod == 0x10){
		// S
		// P?nico //
		if(flag_p == 0){
			if(state == programacao || state == ativado)
			{
				StopTimer1(); // para o Timer 1, caso ele estivesse
				ZerarTemps(); // zera os contadores que estavam ligados
			}
			StartSirene();
			state = panico;
			flag_p = 1;
		}else if(flag_p == 1){
			StopSirene();
			state = desativado;
		}		
	}
	else if(valor_pinod == 0x00){
		// E
		if((state == inserir_senhaN) & (count_senha == 4)){
			state = desativado;
			clear_Display();
			count_senha = 0;
			UARTP(RTC_Read_Day(), RTC_Read_Hours());
		}
		if((state == inserir_senhaA) & (count_senha == 4)){
			state = verificar_senhaA;
			clear_Display();
			count_senha = 0;
			UARTA(RTC_Read_Day(), RTC_Read_Hours());
		}
		if((state == inserir_senhaD) & (count_senha == 4)){
			state = verificar_senhaD;
			clear_Display();
			count_senha = 0;
			UARTD(RTC_Read_Day(), RTC_Read_Hours());
		}
		if((state == HabSensor) & (flag_sensor == 1)){
			//
			state = desativado;
			flag_sensor = 0;
			UARTP(RTC_Read_Day(), RTC_Read_Hours());
		}
		if((state == HabZona) & (flag_sensor == 1)){
			//
			state = desativado;
			flag_sensor = 0;
			UARTP(RTC_Read_Day(), RTC_Read_Hours());
		}
		// ass de zona e sensor //
		if((state == AssSensorZona) & ((flag_z1 == 1) & (flag_z == 1))){
			state = desativado;
			assSensorZona(idsensor, idzona);
			flag_z = 0;
			flag_z1 = 0;
			UARTP(RTC_Read_Day(), RTC_Read_Hours());
		}
		if((state == AjusteTout) & ((cont_t >= 2) & (flag_timeout == 1))){
			state = desativado;
			set_timeout();
			cont_t = 0;
			UARTP(RTC_Read_Day(), RTC_Read_Hours());
		}
		if((state == AjusteTSirene) & (cont_s >= 3)){
			state = desativado;
			set_timeS();
			cont_s = 0;
			UARTP(RTC_Read_Day(), RTC_Read_Hours());
		}
		if((state == AjusteTAtivacao) & (cont_aa >= 3)){
			state = desativado;
			set_timeA();
			cont_aa = 0;
			UARTP(RTC_Read_Day(), RTC_Read_Hours());
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
/*
void send_instruction_lcd(uint8_t data){
	PORTB &= 0xC0;
	PORTB &= ~(1<<RS);  // RS = 0 => intru??es
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
	PORTB &= ~(1<<RS);  // RS = 0 => intru??es
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

	send_instruction_lcd(0x28); // Determinar o modo de opera??o
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
	PORTB &= ~(1<<RS);  // RS = 0 => intru??es
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

*/
void clear_Display(){
	
	send_instruction_lcd(0x01);
	delay_ms(15);
	
}

// DISPLAY //

void send_instruction_lcd(uint8_t data){
	PORTC &= 0xC0;
	PORTC &= ~(1 << RS_LCD);  // RS = 0 => intru??es
	PORTC |= (1 << EN_LCD);  // Seta o enable
	PORTC |= (data >> 4); // seta barramento de dados com nibble mais significativo
	PORTC &= ~(1 << EN_LCD);  // EN = 0
	
	PORTC &= 0xC0;
	PORTC |= (1 << EN_LCD);  // Seta o enable
	PORTC |= data & 0x0F; // seta barramento de dados com nibble mais significativo
	PORTC &= ~(1 << EN_LCD);  // EN = 0
}

void send_instruction_lcd_nibble(uint8_t data){
	PORTC &= 0xC0;
	PORTC &= ~(1 << RS_LCD);  // RS = 0 => intru??es
	PORTC |= (1 << EN_LCD);  // Seta o enable
	PORTC |= data; // seta barramento de dados com nibble mais significativo
	PORTC &= ~(1 << EN_LCD);  // EN = 0
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

	send_instruction_lcd(0x28); // Determinar o modo de opera??o
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
	PORTC &= 0xF0;
	PORTC |= (1 << RS_LCD);  // RS = 1 => dados
	PORTC |= (1 << EN_LCD);  // Seta o enable
	PORTC |= (data >> 4);// seta barramento de dados com nibble mais significativo
	PORTC &= ~(1 << EN_LCD);  // EN = 0
	
	PORTC &= 0xF0;
	PORTC |= (1 << EN_LCD);  // Seta o enable
	PORTC |= data & 0x0F; // seta barramento de dados com nibble menos significativo
	PORTC &= ~(1 << EN_LCD);  // EN = 0

	delay_ns(43);
}

void set_DDRAM_ADDR(uint8_t pos){
	uint8_t data = 0x80 | pos;
	PORTC &= 0xC0;
	PORTC &= ~(1 << RS_LCD);  // RS = 0 => intru??es
	PORTC |= (1 << EN_LCD);  // Seta o enable
	PORTC |= (data >> 4); // seta barramento de dados com nibble mais significativo
	PORTC &= ~(1 << EN_LCD);  // EN = 0
	
	PORTC &= 0xC0;
	PORTC |= (1 << EN_LCD);  // Seta o enable
	PORTC |= data & 0x0F; // seta barramento de dados com nibble menos significativo
	PORTC &= ~(1 << EN_LCD);  // EN = 0
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

void msg_h_ass(){
	
	clear_Display();
	
	char frase_1[] = " Associacao ";
	envia_frase(frase_1,l1C1);
	char frase_2[] = "Sensor e Zona";
	envia_frase(frase_2,l2C1);
	
}

void msg_d_ass(){
	
	clear_Display();
	
	char frase_1[] = "Desassociacao";
	envia_frase(frase_1,l1C1);
	char frase_2[] = "Sensor e Zona";
	envia_frase(frase_2,l2C1);
	
}

void msg_ajusteTout(){
	
	clear_Display();
	
	char frase_1[] = "  Ajuste do  ";
	envia_frase(frase_1,l1C1);
	char frase_2[] = "   Timeout  ";
	envia_frase(frase_2,l2C1);
	
}

void msg_time_sirene(){
	
	clear_Display();
	
	char frase_1[] = "  Ajuste do  ";
	envia_frase(frase_1,l1C1);
	char frase_2[] = "Tempo da Sirene";
	envia_frase(frase_2,l2C1);
	
}

void msg_temp_ativ(){
	
	clear_Display();
	
	char frase_1[] = "  Ajuste do  ";
	envia_frase(frase_1,l1C1);
	char frase_2[] = "Tempo de Ativ";
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
		User = 0;
		
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
		
		//state = ativado;
		User = 0;
		clear_Display();
		StartTimer1();
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
		
		//state = ativado;
		User = 1;
		clear_Display();
		StartTimer1();
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
		
		//state = ativado;
		User = 2;
		clear_Display();
		StartTimer1();
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
		
		//state = ativado;
		User = 3;
		clear_Display();
		StartTimer1();
		return;
		
	}else{
		//User = 0;
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
		User = 0;
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
		User = 1;
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
		User = 2;
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
		User = 3;
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
	// Colocar as fun??es //
	if(flag_D_sensor == 0){
		// Habilita??o do sensor //
		if(n == 0){
			sensor0.active = 1;
		}else if(n == 1){
			sensor1.active = 1;
		}else if(n == 2){
			sensor2.active = 1;
		}else if(n == 3){
			sensor3.active = 1;
		}else if(n == 4){
			sensor4.active = 1;
		}else if(n == 5){
			sensor5.active = 1;
		}else if(n == 6){
			sensor6.active = 1;
		}else if(n == 7){
			sensor7.active = 1;
		}
		
	}else if(flag_D_sensor == 1){
		// Desabilita??o do sensor //
		if(n == 0){
		sensor0.active = 0;
		}else if(n == 1){
		sensor1.active = 0;
		}else if(n == 2){
		sensor2.active = 0;
		}else if(n == 3){
		sensor3.active = 0;
		}else if(n == 4){
		sensor4.active = 0;
		}else if(n == 5){
		sensor5.active = 0;
		}else if(n == 6){
		sensor6.active = 0;
		}else if(n == 7){
		sensor7.active = 0;
		}
		
		flag_D_sensor = 0;
	}
}

// HABILITA ZONA //

void hab_zonas(int n){
	
	flag_sensor = 1; // Essa flag serve para dizer se ja foi
	//	selecionado o sensor/Zona a ser habilitado ou desabilitado //
	// Colocar as fun??es //
	if(flag_D_sensor == 0){
		// Habilita??o da Zona //
		if(n == 1){
			zona1.active = 1;
		}else if(n == 2){
			zona2.active = 1;
		}else if(n == 3){
			zona3.active = 1;
		}
		
	}else if(flag_D_sensor == 1){
		// Desabilita??o da Zona //
		if(n == 1){
			zona1.active = 0;
		}else if(n == 2){
			zona2.active = 0;
		}else if(n == 3){
			zona3.active = 0;
		}
		flag_D_sensor = 0;
	}
}

// ASSOCIA SENSOR A ZONA //

void assSensorZona(int x, int y){
	
	
	if(flag_D_sensor == 0){
		
		if(y == 1){
			if(x == 0) zona1.sensor0 = 1;
			if(x == 1) zona1.sensor1 = 1;
			if(x == 2) zona1.sensor2 = 1;
			if(x == 3) zona1.sensor3 = 1;
			if(x == 4) zona1.sensor4 = 1;
			if(x == 5) zona1.sensor5 = 1;
			if(x == 6) zona1.sensor6 = 1;
			if(x == 7) zona1.sensor7 = 1;
		}else if(y == 2){
			if(x == 0) zona1.sensor0 = 1;
			if(x == 1) zona1.sensor1 = 1;
			if(x == 2) zona1.sensor2 = 1;
			if(x == 3) zona1.sensor3 = 1;
			if(x == 4) zona1.sensor4 = 1;
			if(x == 5) zona1.sensor5 = 1;
			if(x == 6) zona1.sensor6 = 1;
			if(x == 7) zona1.sensor7 = 1;
		}else if(y == 3){
			if(x == 0) zona1.sensor0 = 1;
			if(x == 1) zona1.sensor1 = 1;
			if(x == 2) zona1.sensor2 = 1;
			if(x == 3) zona1.sensor3 = 1;
			if(x == 4) zona1.sensor4 = 1;
			if(x == 5) zona1.sensor5 = 1;
			if(x == 6) zona1.sensor6 = 1;
			if(x == 7) zona1.sensor7 = 1;
		}
	}else if(flag_D_sensor == 1){
		
		if(y == 1){
			if(x == 0) zona1.sensor0 = 0;
			if(x == 1) zona1.sensor1 = 0;
			if(x == 2) zona1.sensor2 = 0;
			if(x == 3) zona1.sensor3 = 0;
			if(x == 4) zona1.sensor4 = 0;
			if(x == 5) zona1.sensor5 = 0;
			if(x == 6) zona1.sensor6 = 0;
			if(x == 7) zona1.sensor7 = 0;
		}else if(y == 2){
			if(x == 0) zona1.sensor0 = 0;
			if(x == 1) zona1.sensor1 = 0;
			if(x == 2) zona1.sensor2 = 0;
			if(x == 3) zona1.sensor3 = 0;
			if(x == 4) zona1.sensor4 = 0;
			if(x == 5) zona1.sensor5 = 0;
			if(x == 6) zona1.sensor6 = 0;
			if(x == 7) zona1.sensor7 = 0;
		}else if(y == 3){
			if(x == 0) zona1.sensor0 = 0;
			if(x == 1) zona1.sensor1 = 0;
			if(x == 2) zona1.sensor2 = 0;
			if(x == 3) zona1.sensor3 = 0;
			if(x == 4) zona1.sensor4 = 0;
			if(x == 5) zona1.sensor5 = 0;
			if(x == 6) zona1.sensor6 = 0;
			if(x == 7) zona1.sensor7 = 0;
		}
	}
}

// SET TIMEOUT //

void set_timeout(){
	
	timeout = (get_timeout[0] * 10) + (get_timeout[1]);
	
}

// SET TIME ATIVA??O //

void set_timeA(){
	
	t_ativacao = (get_timeA[0] * 100) + (get_timeA[1] * 10) + (get_timeA[2]);
	
}

// SET TIME SINERE //

void set_timeS(){
	
	t_sirene = (get_timeS[0] * 100) + (get_timeS[1] * 10) + (get_timeS[2]);
	
}

//////////////////////////////////////////////  PARTE DE ALISSON \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\/


/////////////////////////// TIMER 1 \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\/
void SetupTimer1()
{
	TCCR1B |= (1<<WGM12); //WGM12- modo CTC compara??o com A
	TIMSK1 |= (1<<OCIE1A); // interrup??o quanto der match com A ligada
	OCR1A = 31250; // conta um segundo
	
}

void StartTimer1()
{
	// para contar 1 segundo o OCR1A deve ter o valor de 31250.. e o prescale 256
	TCCR1B |= (1 << CS12) + (0 << CS11) + (0 << CS10); //setando o prescaler = 256, a contagem come?a
	
}

void StopTimer1()
{
	TCCR1B &= ~((1 << CS12) + (1 << CS11) + (1 << CS10)); // sem prescaler, o timer para
	TCNT1 = 0; // zerado para n?o atrapalhar a pr?xima contagem
	
}

/////////////////////////// TIMER 0 \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\/
void StartTimer0()
{
	TCCR0B |= (0 << CS02) | (0 << CS01) | (1 << CS00); //setando o prescaler, a contagem come?a
	
}

void StopTimer0()
{
	TCCR0B &= ~((1 << CS02) | (1 << CS01) | (1 << CS00)); // sem prescaler, o timer para
	TCNT0 = 0; // zerado para n?o atrapalhar a pr?xima contagem
	
}

void SetupTimer0()
{
	TCCR0A |= (1<<WGM01); //WGM12-CTC mode | clkIO/1 (no prescaling)
	TIMSK0 |= (1<<OCIE0A); // Output Compare A Match Interrupt Enable
	
}

void ZerarTemps()
{
	cont_timeout = 0;
	cont_ativacao = 0;
	cont_sirene = 0;
}

/////////////////////////// SIRENE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\/
void StartSirene()
{
	PORTD |= (1 << BUZZER); // sirene
}

void StopSirene()
{
	PORTD &= (0 << BUZZER); // sirene
}

//////////////////////////////   INTERRUP??ES  \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\/
/*ISR(INT1_vect){
	valor_pinod = (PIND & ((1 << MATRIZ1)|(1 << MATRIZ2)|(1 << MATRIZ3)|(1 << MATRIZ4)));
	
	if(valor_pinod == 0xF0){
		// 1
		if(state == senha) send_word_lcd(0b00110001);
	}
	else if(valor_pinod == 0xE0){
		// 2
		if(state == senha) send_word_lcd(0b00110010);
	}
	else if(valor_pinod == 0xD0){
		// 3
		if(state == senha) send_word_lcd(0b00110011);
	}
	else if(valor_pinod == 0xC0){
		// P
		if(state == desativado)
		{
			state = programacao;
			StartTimer1();
		}
		
	}
	else if(valor_pinod == 0xB0){
		// 4
		if(state == senha) send_word_lcd(0b00110100);
	}
	else if(valor_pinod == 0xA0){
		// 5
		if(state == senha) send_word_lcd(0b00110101);
	}
	else if(valor_pinod == 0x90){
		// 6
		if(state == senha) send_word_lcd(0b00110110);
	}
	else if(valor_pinod == 0x80){
		// A
		if(state == desativado)
		{
			StartTimer1();
		}
	}
	else if(valor_pinod == 0x70){
		// 7
		if(state == senha) send_word_lcd(0b00110111);
	}
	else if(valor_pinod == 0x60){
		// 8
		if(state == senha) send_word_lcd(0b00111000);
	}
	else if(valor_pinod == 0x50){
		// 9
		if(state == senha) send_word_lcd(0b00111001);
	}
	else if(valor_pinod == 0x40){
		// D
	}
	else if(valor_pinod == 0x30){
		// R
		state = recuperacao;
	}
	else if(valor_pinod == 0x20){
		// 0
		if(state == senha) send_word_lcd(0b00110000);
	}
	else if(valor_pinod == 0x10){
		// S
		if(state == panico)
		{
			StopSirene();
			state = desativado;
		}
		else
		{
			if(state == programacao || state == ativado)
			{
				StopTimer1(); // para o Timer 1, caso ele estivesse
				ZerarTemps(); // zera os contadores que estavam ligados
			}
			StartSirene();
			state = panico;
		}
	}
	else if(valor_pinod == 0x00){
		// E
		//if(state == senha && (flags == 5 || flags == 4)) ;
	}
}*/

ISR(TIMER1_COMPA_vect)
// N?O PRECISA PARAR A SIRENE NA ROTINA ATIVADO, APENAS ATIV?-LA - A CONTAGEM J? FAZ A PARADA, MAS ISSO PODE SER MUDADO SE DESEJADO
// N?O PRECISA MUDAR DE ESTADO FORA DO TIMER - NO FINAL DA CONTAGEM, J? ? FEITA A TRANSFER?NCIA DE ESTADO, MAS ISSO PODE SER MUDADO SE DESEJADO
{
	
	if(state == programacao)
	{
		if(cont_timeout == timeout)
		{
			state = desativado;
			cont_timeout = 0;
			StopTimer1();
		}
		else
		{
			cont_timeout++;
		}
	}
	
	else if(state == ativado)
	{
		
		if(cont_sirene == t_sirene)
		{
			StopSirene();
			cont_sirene = 0;
			StopTimer1();
			
		}
		else
		{
			cont_sirene++;
		}
	}
	
	else if(state == verificar_senhaA)
	{
		if(cont_ativacao == t_ativacao)
		{
			state = ativado;
			cont_ativacao = 0;
			
			StopTimer1();
		}
		else
		{
			cont_ativacao++;
		}
		
	}
}

void SPI_Master_Setup(){
	DDRB |= (1<<PORTB5) | (1<<PORTB3) | (1<<PORTB2); // Configura SCK, MOSI e SS como saa?das, demais pinos como entradas.
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
	return SPI_Read(0x02);
}
uint8_t RTC_Read_Minu(){
	return SPI_Read(0x01);
}
uint8_t RTC_Read_Day(){
	return SPI_Read(0x03);
}

void IniciarUART (uint32_t taxa, uint32_t freqCpu){
	uint16_t valorReg = freqCpu/16/taxa-1;
	UBRR0H = (uint8_t) (valorReg>>8); // ajusta velocidade de transm
	UBRR0L = (uint8_t)valorReg;
	UCSR0A = 0;               //desab velocid dupla
	UCSR0B = (1<<TXEN0); // hab trans  SEM interrup??o
	UCSR0C = (1<<UCSZ01)|(1<<UCSZ00); // assincrono, com 1 byte, 1 bit de parada e sem paridade.
}

uint8_t ProntoTXUART (void){
	return (UCSR0A & (1<<UDRE0)); // retorna 1 qnd pode ser enviado
}



void EnviarTXUART (uint8_t dado){
	UDR0 = dado;
}


void EnviarStringUART (const char *c){
	for( ; *c != 0; c++){ // a string tem como ultimo valor do vetor =  0
		while (!ProntoTXUART());
		EnviarTXUART(*c);
	}
	
}


void EnviarDec1Byte(uint8_t valor){
	int8_t disp;
	char digitos[3];
	int8_t conta = 0;
	do
	{ disp = (valor%10) + 48;//armazena o resto da divisao por 10 e soma com 48
		valor /= 10;
		digitos[conta]=disp;
		conta++;
	} while (valor!=0);
	for (disp=conta-1; disp>=0; disp-- )
	{ while (!ProntoTXUART());  //aguarda ?ltimo dado ser enviado
		EnviarTXUART(digitos[disp]);//envia algarismo
	}
}

void UARTA(uint8_t dia, uint8_t hora){
	EnviarStringUART("A central foi ativada pelo usuario  ");
	while(!ProntoTXUART()); //aguarda a string ser enviada
	EnviarDec1Byte(User);
	EnviarStringUART(" no dia  ");
	while(!ProntoTXUART()); //aguarda a string ser enviada
	EnviarDec1Byte(dia);
	EnviarStringUART(" e na hora  ");
	while(!ProntoTXUART()); //aguarda a string ser enviada
	EnviarDec1Byte(hora);
	EnviarStringUART("\r\n");
}
void UARTP(uint8_t dia, uint8_t hora){
	EnviarStringUART("A central foi programada pelo usuario  ");
	while(!ProntoTXUART()); //aguarda a string ser enviada
	EnviarDec1Byte(User);
	EnviarStringUART(" no dia  ");
	while(!ProntoTXUART()); //aguarda a string ser enviada
	EnviarDec1Byte(dia);
	EnviarStringUART(" e na hora  ");
	while(!ProntoTXUART()); //aguarda a string ser enviada
	EnviarDec1Byte(hora);
	EnviarStringUART("\r\n");
}
void UARTD(uint8_t dia, uint8_t hora){
	EnviarStringUART("A central foi desativada pelo usuario  ");
	while(!ProntoTXUART()); //aguarda a string ser enviada
	EnviarDec1Byte(User);
	EnviarStringUART(" no dia  ");
	while(!ProntoTXUART()); //aguarda a string ser enviada
	EnviarDec1Byte(dia);
	EnviarStringUART(" e na hora  ");
	while(!ProntoTXUART()); //aguarda a string ser enviada
	EnviarDec1Byte(hora);
	EnviarStringUART("\r\n");
}