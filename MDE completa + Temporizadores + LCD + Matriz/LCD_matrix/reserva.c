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

/*############ ZONAS ############# */

struct zona{

	// Número da zona //
	int id;
	// flag de ativação da zona //
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
	// se está ativo //
	bool active;
	// se o sensor detectou algo //
	bool detect;
	
} sensor0, sensor1, sensor2, sensor3, sensor4, sensor5, sensor6, sensor7; 

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

// ASSOCIAÇÃO DE SENSOR COM ZONA //
void msg_h_ass();
void msg_d_ass();
void assSensorZona(int x, int y);

// AJUSTE TEMPORIZADOR DE ATIVAÇÃO //
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
	senhaAnova,  //qnd tem q salvar algo virá um estado
	ativado,
	panico,
	programacao,
	
	// só pra organizar //
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
	
	//DISPLAY PINOS
	DDRC = 0b0111111;
	
	//definções finais - depois dos setups menos o SPI
	DDRB = 0x1F;
	//DDRB =  (1 << CLK_IN_OUT)|(1 << CS)|(1 << PS_EXTIN)|(1 << PS_EXTOUT)|(1 << PS_EXTIN);
	DDRD =  (1 << EXT_OUT)|(1 << OUT_UART)|(1 << BUZZER);
	PORTD = (1 << MATRIZ1)|(1 << MATRIZ2)|(1 << MATRIZ3)|(1 << MATRIZ4);
	
	// INIALIZAÇÕES DO SISTEMA //
	SetupTimer1();
	SetupTimer0();
	init_int1();
	inicializa_display();
	
	sei();
	
    while(1)
    {	
		// SWITCH CASE PARA A SELEÇÃO DO MODO //
        switch(state){
			// ROTINAS PRINCIPAIS  //
			case recuperacao:
			StopTimer1(); // para o timer, caso a máquina estivesse em programação
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
			
			// ASSCIAÇÃO D SENSOR E ZONA //
			case AssSensorZona:
			
			break;
			
			// Tempo de ativação //
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

// INTERRUPÇÃO EXTERNA //

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
		// ESOLHER O SENSOR DA ASSOCIAÇÃO //
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
		// DETERMINAR T ATIVAÇÃO //
		if((state == AjusteTAtivacao) & (cont_t < 3)){
			get_timeA[cont_aa] = 1;
			cont_aa++;
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
		if((state == programacao) & ((flag_a_senha == 0) & (flag_timeout == 0))){
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
		// ESOLHER O SENSOR DA ASSOCIAÇÃO //
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
		// DETERMINAR T ATIVAÇÃO //
		if((state == AjusteTAtivacao) & (cont_t < 3)){
			get_timeA[cont_aa] = 2;
			cont_aa++;
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
		if((state == programacao) & ((flag_a_senha == 0) & (flag_timeout == 0))){
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
		// ESOLHER O SENSOR DA ASSOCIAÇÃO //
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
		// DETERMINAR T ATIVAÇÃO //
		if((state == AjusteTAtivacao) & (cont_t < 3)){
			get_timeA[cont_aa] = 3;
			cont_aa++;
		}
	}
	else if(valor_pinod == 0b00110000){
		// P
		// Inserir a senha Mestre para o modo de Programação //
		if(state == desativado){
			state = inserir_senha;
			msg_inserir_senha();
			StartTimer1();
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
		// FUNÇÃO DE ASSOCIAÇÃO DE SENSOR E ZONA //
		if((state == programacao) & ((flag_a_senha == 0) & (flag_timeout == 0))){
			state = AssSensorZona;
			if(flag_D_sensor == 0) msg_h_ass();
			if(flag_D_sensor == 1) msg_d_ass();
		}
		// ESOLHER O SENSOR DA ASSOCIAÇÃO //
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
		// DETERMINAR T ATIVAÇÃO //
		if((state == AjusteTAtivacao) & (cont_t < 3)){
			get_timeA[cont_aa] = 4;
			cont_aa++;
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
		if((state == programacao) & ((flag_a_senha == 0) & (flag_timeout == 0))){
			state = HabZona;
			if(flag_D_sensor == 0) msg_h_zona();
			if(flag_D_sensor == 1) msg_d_zona();
		}
		// ESOLHER O SENSOR DA ASSOCIAÇÃO //
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
		// DETERMINAR T ATIVAÇÃO //
		if((state == AjusteTAtivacao) & (cont_t < 3)){
			get_timeA[cont_aa] = 5;
			cont_aa++;
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
		// ESOLHER O SENSOR DA ASSOCIAÇÃO //
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
		// Temporizador de ativação //
		if((state == programacao) & ((flag_a_senha == 0) & (flag_timeout == 0))){
			state = AjusteTAtivacao;
			flag_tat = 1;
			msg_temp_ativ();
		}
		// DETERMINAR T ATIVAÇÃO //
		if((state == AjusteTAtivacao) & (cont_t < 3)){
			get_timeA[cont_aa] = 6;
			cont_aa++;
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
		// ESOLHER O SENSOR DA ASSOCIAÇÃO //
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
		// DETERMINAR T ATIVAÇÃO //
		if((state == AjusteTAtivacao) & (cont_t < 3)){
			get_timeA[cont_aa] = 7;
			cont_aa++;
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
		// DETERMINAR T ATIVAÇÃO //
		if((state == AjusteTAtivacao) & (cont_t < 3)){
			get_timeA[cont_aa] = 8;
			cont_aa++;
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
		// DETERMINAR T ATIVAÇÃO //
		if((state == AjusteTAtivacao) & (cont_t < 3)){
			get_timeA[cont_aa] = 9;
			cont_aa++;
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
		// ESOLHER O SENSOR DA ASSOCIAÇÃO //
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
		// DETERMINAR T ATIVAÇÃO //
		if((state == AjusteTAtivacao) & (cont_t < 3)){
			get_timeA[cont_aa] = 0;
			cont_aa++;
		}
	}
	else if(valor_pinod == 0b10000000){
		// S
		// Pânico //
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
		// ass de zona e sensor //
		if((state == AssSensorZona) & ((flag_z1 == 1) & (flag_z == 1))){
			state = desativado;
			assSensorZona(idsensor, idzona);
			flag_z = 0;
			flag_z1 = 0;
		}
		if((state == AjusteTout) & ((cont_t >= 2) & (flag_timeout == 1))){
			state = desativado;
			set_timeout();
			cont_t = 0;
		}
		if((state == AjusteTSirene) & (cont_s >= 3)){
			state = desativado;
			set_timeS();
			cont_s = 0;
		}
		if((state == AjusteTAtivacao) & (cont_aa >= 3)){
			state = desativado;
			set_timeA();
			cont_aa = 0;
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

*/
void clear_Display(){
	
	send_instruction_lcd(0x01);
	delay_ms(15);
	
}

// DISPLAY //

void send_instruction_lcd(uint8_t data){
	PORTC &= 0xC0;
	PORTC &= ~(1 << RS_LCD);  // RS = 0 => intruções
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
	PORTC &= ~(1 << RS_LCD);  // RS = 0 => intruções
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
	PORTC &= ~(1 << RS_LCD);  // RS = 0 => intruções
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
		clear_Display();
		StartTimer1();
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
		// Desabilitação do sensor //
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
	// Colocar as funções //
	if(flag_D_sensor == 0){
		// Habilitação da Zona //
		if(n == 1){
			zona1.active = 1;
		}else if(n == 2){
			zona2.active = 1;
		}else if(n == 3){
			zona3.active = 1;
		}
		
	}else if(flag_D_sensor == 1){
		// Desabilitação da Zona //
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

// SET TIME ATIVAÇÃO //

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
	TCCR1B |= (1<<WGM12); //WGM12- modo CTC comparação com A
	TIMSK1 |= (1<<OCIE1A); // interrupção quanto der match com A ligada
	OCR1A = 31250; // conta um segundo
	
}

void StartTimer1()
{
	// para contar 1 segundo o OCR1A deve ter o valor de 31250.. e o prescale 256
	TCCR1B |= (1 << CS12) + (0 << CS11) + (0 << CS10); //setando o prescaler = 256, a contagem começa
	
}

void StopTimer1()
{
	TCCR1B &= ~((1 << CS12) + (1 << CS11) + (1 << CS10)); // sem prescaler, o timer para
	TCNT1 = 0; // zerado para não atrapalhar a próxima contagem
	
}

/////////////////////////// TIMER 0 \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\/
void StartTimer0()
{
	TCCR0B |= (0 << CS02) | (0 << CS01) | (1 << CS00); //setando o prescaler, a contagem começa
	
}

void StopTimer0()
{
	TCCR0B &= ~((1 << CS02) | (1 << CS01) | (1 << CS00)); // sem prescaler, o timer para
	TCNT0 = 0; // zerado para não atrapalhar a próxima contagem
	
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

//////////////////////////////   INTERRUPÇÕES  \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\/
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
// NÃO PRECISA PARAR A SIRENE NA ROTINA ATIVADO, APENAS ATIVÁ-LA - A CONTAGEM JÁ FAZ A PARADA, MAS ISSO PODE SER MUDADO SE DESEJADO
// NÃO PRECISA MUDAR DE ESTADO FORA DO TIMER - NO FINAL DA CONTAGEM, JÁ É FEITA A TRANSFERÊNCIA DE ESTADO, MAS ISSO PODE SER MUDADO SE DESEJADO
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