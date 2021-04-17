/*
 * CentralAlarme.c
 *
 * Created: 14/04/2021 10:15:39
 * Author : Leandro Rodrigues
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



/*############ variáveis #############*/

// flags - funciona como um vetor de 8 espaços
 
/* 
 alocacões:
    flag_toprog 0  primeiro ponto - se tiver apenas ela acionada o valor da variavel será 1
    flag_toativo 1  segundo ponto - se tiver, ela apenas, ativada o valor da variavel será 2
    flag_todesativo 2
    flag_senhacorreta 3 - esta sempre está em combo com outra.. exemplo com toprog - valor = 9
    
*/
uint8_t flags = 0;

uint8_t  cont_timeout = 0, cont_R = 0;
uint16_t cont_sirene = 0, cont_ativacao = 0, timeout, t_ativacao, t_sirene;

char auxChar[15];
long int x = 0;
int cont_1 = 0;

int valor_pinod;

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


// INTERRUPÇÃO
void init_int1();


// TEMPORIZADORES //
void SetupTimer1();
void StartTimer1();
void StopTimer1();

void SetupTimer0();
void StartTimer0();
void StopTimer0();

void ZerarTemps();

uint8_t senhas[4][3];
/// /// /// /// /// /// /// /// /// 
enum states{
	recuperacao,
	desativado,
	senha,
	novasenha,
	HabSensor,
	AssSensorZona,
	HabZona,  //qnd tem q salvar algo virá um estado
	ativado,
	panico,
	programacao,
	AjusteTAtivacao,
	AjusteTout,
	AjusteTSirene,
};

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

enum states state = recuperacao;

int main(void)
{
    //DISPLAY PINOS
    DDRC = 0b0111111;
    
    //definções finais - depois dos setups menos o SPI
    DDRB = 0x1F;
    //DDRB =  (1 << CLK_IN_OUT)|(1 << CS)|(1 << PS_EXTIN)|(1 << PS_EXTOUT)|(1 << PS_EXTIN);
    DDRD =  (1 << EXT_OUT)|(1 << OUT_UART)|(1 << BUZZER);
    PORTD = (1 << MATRIZ1)|(1 << MATRIZ2)|(1 << MATRIZ3)|(1 << MATRIZ4);
    
    inicializa_display();
    char frase_1[] = "    ELE-1313    ";
    envia_frase(frase_1,l1C1);
    char frase_2[] = "CENTRAL - ALARME";
    envia_frase(frase_2,l2C1);
     
  
    
    SetupTimer1();
    SetupTimer0();
    init_int1();
    sei();

    while (1) 
    {
		switch(state)
		{
			case recuperacao:
                StopTimer1(); // para o timer, caso a máquina estivesse em programação
                // aqui pararia a sirene
                timeout = 12;
                t_sirene = 5;
                t_ativacao = 5;
                state = desativado;
                break;
			
			case desativado:
                //nada
                break;
			
			case novasenha:
                //anda
                break;
			
			case ativado:
                
                break;
			
			case panico:
                
                break;
			
			case senha:
                
                break;			
			
			case programacao:
                break;
			
			case HabSensor:
                
                break;
			
			case AssSensorZona:
                
                break;
			
			case HabZona:
                
                break;
			
			case AjusteTAtivacao:
                
                break;
			
			case AjusteTout:
                
                break;
			
			case AjusteTSirene:
            	break;
		}
     
    }
}

void init_int1(){
	
	EICRA = (1<<ISC11) | (1<<ISC10);
	EIMSK = (1<<INT1);
	
}


/////////////////////////// LCD \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\/
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
ISR(INT1_vect){
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
}

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
    
    else if(state == desativado)
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