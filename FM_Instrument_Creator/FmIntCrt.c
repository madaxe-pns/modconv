/***********************************************/
/*									           */
/*  		FM Instrument Creator 1.7  		   */
/*										       */
/* ADLIB / SOUNDBLASTER - Yamaha YM3812 - OPL2 */
/*										       */
/* 		   (C) 2020 Penisoft / MadAxe		   */
/*									           */		
/***********************************************/

#include <stdio.h>
#include <dos.h>
#include <conio.h>
#include <string.h>

/* Códigos de Teclado */
#define KB_0 48
#define KB_1 49
#define KB_2 50
#define KB_3 51
#define KB_4 52
#define KB_5 53
#define KB_6 54
#define KB_7 55
#define KB_8 56
#define KB_9 57

#define KB_Z 122
#define KB_X 120
#define KB_C 99
#define KB_V 118
#define KB_B 98
#define KB_N 110
#define KB_M 109
#define KB_VIRGULA 44

#define KB_S 115
#define KB_D 100
#define KB_G 103
#define KB_H 104
#define KB_J 106
#define KB_L 108

#define KB_O 111

#define KB_F1 59
#define KB_F2 60
#define KB_F3 61

#define KB_RETURN 13
#define KB_ESCAPE 27
#define KB_BACKSPACE 8
#define KB_TAB 9
#define KB_CTRL_TAB 148

#define KB_INS 82
#define KB_DEL 83

#define KB_CTRL_N 14

#define KB_UP 72
#define KB_DOWN 80
#define KB_LEFT 75
#define KB_RIGHT 77

#define KB_PAGE_UP 73
#define KB_PAGE_DOWN 81
#define KB_HOME 71
#define KB_END 79

#define KB_MAIS 43
#define KB_MENOS 45

/* Pode retirar-se */
#define KB_CTRL_LEFT 115
#define KB_CTRL_RIGHT 116
#define KB_ALT_LEFT 155
#define KB_ALT_RIGHT 157

/* Portos da Placa de Som FM */
#define ADL_REG		0x388	/* Porto dos Registos/Status na Adlib */
#define ADL_DAT		0x389 	/* Porto dos Dados na Adlib           */

/* Registos da Placa de Som FM */
#define MIN_REGISTER 0x01	/* Primeiro Registo */
#define MAX_REGISTER 0xF5	/* Último Registo   */

/* Frequência das Notas FM */
#define C	262	/* Dó */
#define Cs	278	/* Dó Sustenido */
#define D	294	/* Ré */
#define Ds	312	/* Ré Sustenido */
#define E	330	/* Mi */
#define F	350	/* Fá */
#define Fs	370	/* Fá Sustenido */
#define G	392	/* Sol */
#define Gs	416	/* Sol Sustenido */
#define A	441	/* Lá */
#define As	467	/* Lá Sustenido */
#define B	494	/* Si */
#define Cm	524	/* Dó Maior */
#define Cms	555	/* Dó Maior Sustenido */

/* Prototypes */
void screenopen(void);
void screenclose(void);

int inicializa(void);
void mainmenu(void);

void resetfm(void);
void initfm(void);
void writefm(int reg, int value);
int readfm(void);
void criasomfm(int freqindex);
void stopsomfm();

int leinstfm(void);
int gravainstfm(void);

void printdata(void);
void menuval(void);
void menureg(void);

void printcursor(short onx, short ony, short nx, short ny, short nc);
void printcursornome(short onx, short ony, short nx, short ny, short nc);
void printestado(void);
void printoct(void);
void printinstfm(void);
void printval(void);
void printreg(void);
void printhelp(void);

void alteracab(void);
void navega(void);
void navegainstfm(void);
void navegaval(void);
void navegareg(void);
void navegahelp(void);

void inputval(void);
void updateval(void);
void inputreg(void);
void updatereg(void);

void inputnome(void);
void updatenome(void);

void insereinstfm(int inst);
void apagainstfm(int inst);

char *hexa(int deca);

void criainst();

/* Teclado */
int KB_code=0;
int estado=0;
int menu,menuv;

/* Navegação nos Instrumentos FM */
int sy,py,instcolor;
int valcolor;

/* Oitava */
int oitava=4;

/* Valores */
unsigned short o1r2t,o1r2v,o1r2s,o1r2k,o1r2h;
unsigned short o1r4k,o1r4v;
unsigned short o1r6a,o1r6d;
unsigned short o1r8s,o1r8r;
unsigned short o1ree;
unsigned short o2r2t,o2r2v,o2r2s,o2r2k,o2r2h;
unsigned short o2r4k,o2r4v;
unsigned short o2r6a,o2r6d;
unsigned short o2r8s,o2r8r;
unsigned short o2ree;
unsigned short rcf,rca;

/* Navegação nos Valores dos Operadores */
unsigned short vox,voy,ovox,ovoy;
unsigned short voed,voc,voi;
unsigned short tvoy[13]={7,8,9,10,11,13,14,16,17,18,19,21,23};
unsigned short tvox[2]={42,70};

/* Navegação nos Registos dos Operadores */
unsigned short rox,roy,orox,oroy;
unsigned short roed,roc,roi;
unsigned short troy[6]={9,11,13,15,17,19};
unsigned short trox[2]={33,63};

unsigned short noed,noc,noi;

/* Introdução de Dados Valores / Registos dos Operadores */
char vro[4],vron[20];
char tccc[3];
int cc=0,dd=0,uu=0;
int keypressed=0;

/* Help */
int help;

/* Definição dos Registos para cada Instrumento FM */
struct instregs
{
	char nome[20];
	unsigned char num;
	unsigned char regc0;
	unsigned char op1reg2;
	unsigned char op1reg4;
	unsigned char op1reg6;
	unsigned char op1reg8;
	unsigned char op1rege;
	unsigned char op2reg2;
	unsigned char op2reg4;
	unsigned char op2reg6;
	unsigned char op2reg8;
	unsigned char op2rege;
} instreg[128];

short notas[14]={C,Cs,D,Ds,E,F,Fs,G,Gs,A,As,B,Cm,Cms};

char nnotas[14][4]={"C  \0","C# \0","D  \0","D# \0","E  \0",
					"F  \0","F# \0","G  \0","G# \0","A  \0",
					"A# \0","B  \0","Cm \0","Cm#\0"};

char txtsts[50];

int main (void)
{

	if (inicializa()!=0) 
		return(1);
	
	screenopen();
	
	mainmenu();
	
	screenclose();
	
	return(0);
	
}

/* Inicializa o Écran */
void screenopen(void)
{
	
	clrscr();	
	_setcursortype(_NOCURSOR);
	
}

/* Volta para o MS-Dos */
void screenclose(void)
{

	resetfm();

	textbackground(0);
	textcolor(7);
	clrscr();
	
	textcolor(11);
	cprintf("Instrument Creator 1.7\r\n");
	textcolor(10);
	cprintf("(C) 2020 Penisoft / MadAxe\r\n");
	
	_setcursortype(_NORMALCURSOR);
	
}

/* Inicializa o Programa */
int inicializa(void)
{

	int f;

	clrscr();
	
	textcolor(11);
	cprintf("Instrument Creator 1.7\r\n");
	textcolor(10);
	cprintf("(C) 2020 Penisoft / MadAxe\r\n\r\n");
	
	textcolor(15);

//	criainst();

	f=leinstfm();
	if (f!=0)
	{
		cprintf("%s\r\n",txtsts);
		return(f);
	}
	
	resetfm();
	initfm();

	/* Menu */
	menu=0;
	menuv=0;
	estado=0;
	
	/* Help */
	help=0;
		
	/* Instrumentos FM */
	py=0;
	sy=0;
	instcolor=14;
	valcolor=15;
	
	/* Navegação nos Valores dos Operadores */
	vox=0;
	voy=0;
	ovox=vox;
	ovoy=voy;
	voed=0;
	voc=3;
	
	/* Navegação nos Registos dos Operadores */
	rox=0;
	roy=0;
	orox=rox;
	oroy=roy;
	roed=0;
	roc=3;
	
	/* Alteração do Nome do Instrumento FM */
	noed=0;
	noc=0;
	
	return(0);
	
}

/* Main */
void mainmenu(void)
{

	do
	{
		switch (estado)
		{
			case 0:
			{
				printdata();
				navega();
				break;
			}
			case 1:
			{
				printhelp();
				navegahelp();
				break;	
			}
		}
	}
	while (estado!=10);
	
}

/* Faz o Reset à Placa de Som FM */
void resetfm(void)
{
	int i;
	
	for (i=MIN_REGISTER; i<MAX_REGISTER; i++) writefm(i,0);
	
}

/* Inicializa a Placa de Som FM */
void initfm(void)
{

	/* Waveform Enable para todos os Canais */
	writefm(0x01,0x20);
	
	/* Tremolo e Vibrato Enable para todos os Canais */
	writefm(0xBD,0xC0);
	
}

/* Escreve um Valor num Registo FM */
void writefm(int reg, int value)
{
	
	int i,val;
	
	val=outp(ADL_REG,reg);					/* Escreve o Registo no Porto Register/Status */
	for (i = 0; i <6; i++) inp(ADL_REG);	/* Espera 12 ciclos */
	
	val=outp(ADL_DAT,value);				/* Escreve o Valor no Porto Data */
	for (i = 0; i <35; i++) inp(ADL_REG);   /* Espera 12 ciclos */
	
	val++;
	
}

/* Lê um Valor do Porto Status/Register FM */
int readfm(void)
{
	return (inp(ADL_REG));
}

/* Produz um Som FM */
void criasomfm(int freqindex)
{
	
	int freq,freql,freqh;
	int regb0;
		
	freq=notas[freqindex];
		
	freqh=(int)(freq/256);
	freql=(int)(freq-(freqh*256));
		
	sprintf(txtsts,"Playing %s, Frequencia: %d\0",nnotas[freqindex],freq);
	printestado();
	
	/* Desliga o Canal */
	regb0=0*1+4*oitava+freqh;
	writefm(0xB0,regb0);

	/* Toca o Instrumento */
	regb0=32*1+4*oitava+freqh;
	
	/* Operador 1 */
	writefm(0x20,instreg[sy].op1reg2); /* Tremolo, Vibrato, Hold Note, Keyboard Scaling Rate, Harmonic */
	writefm(0x40,instreg[sy].op1reg4); /* Scale Level, Volume Level */
	writefm(0x60,instreg[sy].op1reg6); /* Attack, Decay */
	writefm(0x80,instreg[sy].op1reg8); /* Sustain, Release */
	writefm(0xE0,instreg[sy].op1rege); /* Envelope Type */
	
		/* Operador 2 */
	writefm(0x23,instreg[sy].op2reg2); /* Tremolo, Vibrato, Hold Note, Keyboard Scaling Rate, Harmonic */
	writefm(0x43,instreg[sy].op2reg4); /* Scale Level, Volume Level */
	writefm(0x63,instreg[sy].op2reg6); /* Attack, Decay */
	writefm(0x83,instreg[sy].op2reg8); /* Sustain, Release */
	writefm(0xE3,instreg[sy].op2rege); /* Envelope Type */
	
	writefm(0xC0,instreg[sy].regc0);  	/* Feedback Modulation, FM Synthesis ON/OFF */
	writefm(0xA0,freql); 				/* Frequência Low Byte */
	writefm(0xB0,regb0);  				/* Play Note, Oitava, Frequência Hi Byte */
			
}

/* Desliga o Canal FM */
void stopsomfm()
{
	
	sprintf(txtsts,"Som Desligado\0");
	printestado();
	
	/* Desliga o Canal */
	writefm(0xB0,0x00);
	
}

/* Imprime os Instrumentos FM e os Valores/Registos dos Operadores */
void printdata(void)
{
	
	int i;
	
	textbackground(0);
	clrscr();
	
	/* Menu */
	textbackground(1);
	textcolor(15);
	
	gotoxy(1,1);
	cprintf("  F1-Help  F2-Load  F3-Save                                           ESC-Sair  ");
	
	/* Teclado */
	textbackground(15);
	textcolor(0);
	gotoxy(3,23);cprintf(" ");
	gotoxy(3,24);cprintf("Z ");
	gotoxy(6,23);cprintf(" ");
	gotoxy(6,24);cprintf("X ");
	gotoxy(9,23);cprintf("  ");
	gotoxy(9,24);cprintf("C ");
	gotoxy(12,23);cprintf(" ");
	gotoxy(12,24);cprintf("V ");
	gotoxy(15,23);cprintf(" ");
	gotoxy(15,24);cprintf("B ");
	gotoxy(18,23);cprintf(" ");
	gotoxy(18,24);cprintf("N ");
	gotoxy(21,23);cprintf("  ");
	gotoxy(21,24);cprintf("M ");
	gotoxy(24,23);cprintf(" ");
	gotoxy(24,24);cprintf(", ");
	
	textbackground(0);
	textcolor(7);
	gotoxy(4,23);cprintf("S");
	gotoxy(7,23);cprintf("D");
	gotoxy(13,23);cprintf("G");
	gotoxy(16,23);cprintf("H");
	gotoxy(19,23);cprintf("J");
	gotoxy(25,23);cprintf("L");
	
	/* Oitava */
	textcolor(12);
	gotoxy(3,22);cprintf("Oitava - ");
	printoct();
	
	/* Estado */
	printestado();
	
	/* Instrumentos FM */
	printinstfm();
	
	/* Operadores */
	valcolor=15;
	if (menuv==0)
	{
		menuval();
		printval();
	}
	else
	{
		menureg();
		printreg();
	}
	
	alteracab();
	
}

/* Menu dos Valores dos Operadores */
void menuval(void)
{
	
	int i;
	
	textbackground(0);
	textcolor(11);
	
	for (i=5; i<24; i++)
	{
		gotoxy(28,i);
		cprintf("                                                    ");
	}
	
	gotoxy(28,5);cprintf("Nome");
	gotoxy(56,5);cprintf("Posicao");
	
	gotoxy(28,7);cprintf("Tremolo");gotoxy(56,7);cprintf("Tremolo");
	gotoxy(28,8);cprintf("Vibrato");gotoxy(56,8);cprintf("Vibrato");
	gotoxy(28,9);cprintf("Sustain");gotoxy(56,9);cprintf("Sustain");
	gotoxy(28,10);cprintf("Scaling Rate");gotoxy(56,10);cprintf("Scaling Rate");
	gotoxy(28,11);cprintf("Harmonic");gotoxy(56,11);cprintf("Harmonic");
	
	gotoxy(28,13);cprintf("Scale Level");gotoxy(56,13);cprintf("Scale Level");
	gotoxy(28,14);cprintf("Volume Level");gotoxy(56,14);cprintf("Volume Level");
	
	gotoxy(28,16);cprintf("Attack");gotoxy(56,16);cprintf("Attack");
	gotoxy(28,17);cprintf("Decay");gotoxy(56,17);cprintf("Decay");
	gotoxy(28,18);cprintf("Sustain");gotoxy(56,18);cprintf("Sustain");
	gotoxy(28,19);cprintf("Release");gotoxy(56,19);cprintf("Release");
	
	gotoxy(28,21);cprintf("Envelope Type");gotoxy(56,21);cprintf("Envelope Type");
	
	gotoxy(28,23);cprintf("Mod Feedback");gotoxy(56,23);cprintf("FM Synthesis");
	
	textcolor(12);
	gotoxy(44,6);cprintf("VAL");gotoxy(72,6);cprintf("VAL");
	gotoxy(49,6);cprintf("REG");gotoxy(77,6);cprintf("REG");
	
	printcursor(tvox[ovox],tvoy[ovoy],tvox[vox],tvoy[voy],2);
	
}

/* Menu dos Registos dos Operadores */
void menureg(void)
{
	
	int i;
	
	textbackground(0);
	textcolor(11);
	
	for (i=5; i<24; i++)
	{
		gotoxy(28,i);
		cprintf("                                                    ");
	}
	
	gotoxy(28,5);cprintf("Nome");
	gotoxy(56,5);cprintf("Posicao");
	
	gotoxy(28,9);cprintf("20");gotoxy(58,9);cprintf("23");
	gotoxy(28,11);cprintf("40");gotoxy(58,11);cprintf("43");
	gotoxy(28,13);cprintf("60");gotoxy(58,13);cprintf("63");
	gotoxy(28,15);cprintf("80");gotoxy(58,15);cprintf("83");
	gotoxy(28,17);cprintf("E0");gotoxy(58,17);cprintf("E3");
	gotoxy(28,19);cprintf("C0");
	
	textcolor(12);
	gotoxy(35,7);cprintf("DEC");gotoxy(65,7);cprintf("DEC");
	gotoxy(45,7);cprintf("HEX");gotoxy(75,7);cprintf("HEX");
	
	printcursor(trox[orox],troy[oroy],trox[rox],troy[roy],2);
	
}

/* Altera as Cores dos Instrumentos / Operadores */
void alteracab(void)
{
	
	textbackground(0);
	
	if (menu==0)
	{
		instcolor=14;
		valcolor=15;
	}
	else
	{
		instcolor=15;
		valcolor=14;	
	}
	
	textcolor(instcolor);
	gotoxy(5,3);
	cprintf("Instrumentos FM");
	
	textcolor(valcolor);
	gotoxy(28,3);cprintf("Operador 1 - Modelator");
	gotoxy(56,3);cprintf("Operador 2 - Carrier");
	
	textcolor(7);
	
}

/* Imprime o Cursor da Navegação dos Operadores */
void printcursor(short onx, short ony, short nx, short ny, short nc)
{
	
	/* Apaga o Cursor na Posição Antiga */
	textbackground(0);
	gotoxy(onx,ony);
	cprintf(" ");
	
	/* Imprime o Cursor na Posição Nova */
	textbackground(nc);
	gotoxy(nx,ny);
	cprintf(" ");
	
	textbackground(0);
	
}

/* Imprime o Cursor da Alteração do Nome */
void printcursornome(short onx, short ony, short nx, short ny, short nc)
{
	
	/* Apaga o Cursor na Posição Antiga */
	textbackground(4);
	gotoxy(onx,ony);
	cprintf(" ");
	
	/* Imprime o Cursor na Posição Nova */
	textbackground(nc);
	gotoxy(nx,ny);
	cprintf(" ");
	
	textbackground(0);
	
}

/* Imprime o Estado */
void printestado(void)
{
	
	textbackground(1);
	textcolor(15);
	gotoxy(28,25);cprintf("%-52s",txtsts);
	
	textbackground(0);
		
}

/* Imprime a Oitava */
void printoct(void)
{

	textbackground(0);
	textcolor(13);
	gotoxy(12,22);cprintf("%d",oitava);
	
}

/* Imprime a Informação dos Instrumentos FM */
void printinstfm(void)
{
	
	int i;
		
	for (i=py; i<py+16; i++)
	{
		
		gotoxy(5,i-py+5);
		textbackground(2);
		textcolor(15);	
				
		if (i==sy) textbackground(1);
			
		cprintf("%-20s",instreg[i].nome);
		
	}
	
}

/* Imprime a Informação sobre os Valores dos Operadores */
void printval(void)
{
	
	int h;
	
	/* Converte os Registos dos Operadores para os Respetivos Valores */
	
	/* Operador 1 */
	if (instreg[sy].op1reg2&128) o1r2t=1;
	else o1r2t=0;
	
	if (instreg[sy].op1reg2&64) o1r2v=1;
	else o1r2v=0;
	
	if (instreg[sy].op1reg2&32) o1r2s=1;
	else o1r2s=0;
		
	if (instreg[sy].op1reg2&16) o1r2k=1;
	else o1r2k=0;
	
	h=(short)(instreg[sy].op1reg2/16);
	o1r2h=(short)(instreg[sy].op1reg2-h*16);
	
	o1r4k=(short)(instreg[sy].op1reg4/64);
	o1r4v=(short)(instreg[sy].op1reg4-o1r4k*64);
	
	o1r6a=(short)(instreg[sy].op1reg6/16);
	o1r6d=(short)(instreg[sy].op1reg6-o1r6a*16);
	o1r8s=(short)(instreg[sy].op1reg8/16);
	o1r8r=(short)(instreg[sy].op1reg8-o1r8s*16);
	
	h=(short)(instreg[sy].op1rege/4);
	o1ree=(short)(instreg[sy].op1rege-h*4);

	/* Operador 2 */
	if (instreg[sy].op2reg2&128) o2r2t=1;
	else o2r2t=0;
	
	if (instreg[sy].op2reg2&64) o2r2v=1;
	else o2r2v=0;
	
	if (instreg[sy].op2reg2&32) o2r2s=1;
	else o2r2s=0;
		
	if (instreg[sy].op2reg2&16) o2r2k=1;
	else o2r2k=0;
	
	h=(short)(instreg[sy].op2reg2/16);
	o2r2h=(short)(instreg[sy].op2reg2-h*16);
	
	o2r4k=(short)(instreg[sy].op2reg4/64);
	o2r4v=(short)(instreg[sy].op2reg4-o2r4k*64);
	
	o2r6a=(short)(instreg[sy].op2reg6/16);
	o2r6d=(short)(instreg[sy].op2reg6-o2r6a*16);
	o2r8s=(short)(instreg[sy].op2reg8/16);
	o2r8r=(short)(instreg[sy].op2reg8-o2r8s*16);
	
	h=(short)(instreg[sy].op2rege/4);
	o2ree=(short)(instreg[sy].op2rege-h*4);

	/* Registo C0 */
	rcf=(short)(instreg[sy].regc0/2);
	rca=(short)(instreg[sy].regc0-rcf*2);

	/* Nome do Instrumento */	
	textbackground(10);
	textcolor(15);
	gotoxy(35,5);cprintf("%-20s",instreg[sy].nome);

	textbackground(0);
	textcolor(7);
	
	/* Posição do Instrumento no Bank */
	gotoxy(64,5);cprintf("%3d",sy);
	
	/* Valores dos Operadores */
	gotoxy(44,7);cprintf("%3d",o1r2t);gotoxy(72,7);cprintf("%3d",o2r2t);
	gotoxy(44,8);cprintf("%3d",o1r2v);gotoxy(72,8);cprintf("%3d",o2r2v);
	gotoxy(44,9);cprintf("%3d",o1r2s);gotoxy(72,9);cprintf("%3d",o2r2s);
	gotoxy(44,10);cprintf("%3d",o1r2k);gotoxy(72,10);cprintf("%3d",o2r2k);
	gotoxy(44,11);cprintf("%3d",o1r2h);gotoxy(72,11);cprintf("%3d",o2r2h);
	
	gotoxy(44,13);cprintf("%3d",o1r4k);gotoxy(72,13);cprintf("%3d",o2r4k);
	gotoxy(44,14);cprintf("%3d",o1r4v);gotoxy(72,14);cprintf("%3d",o2r4v);
	
	gotoxy(44,16);cprintf("%3d",o1r6a);gotoxy(72,16);cprintf("%3d",o2r6a);
	gotoxy(44,17);cprintf("%3d",o1r6d);gotoxy(72,17);cprintf("%3d",o2r6d);
	gotoxy(44,18);cprintf("%3d",o1r8s);gotoxy(72,18);cprintf("%3d",o2r8s);
	gotoxy(44,19);cprintf("%3d",o1r8r);gotoxy(72,19);cprintf("%3d",o2r8r);
	
	gotoxy(44,21);cprintf("%3d",o1ree);gotoxy(72,21);cprintf("%3d",o2ree);
	
	gotoxy(44,23);cprintf("%3d",rcf);gotoxy(72,23);cprintf("%3d",rca);
	
	/* Registos dos Operadores */
	gotoxy(49,7);cprintf("%3s",hexa(instreg[sy].op1reg2));
	gotoxy(49,13);cprintf("%3s",hexa(instreg[sy].op1reg4));
	gotoxy(49,16);cprintf("%3s",hexa(instreg[sy].op1reg6));
	gotoxy(49,18);cprintf("%3s",hexa(instreg[sy].op1reg8));
	gotoxy(49,21);cprintf("%3s",hexa(instreg[sy].op1rege));
	gotoxy(77,23);cprintf("%3s",hexa(instreg[sy].regc0));
	
	gotoxy(77,7);cprintf("%3s",hexa(instreg[sy].op2reg2));
	gotoxy(77,13);cprintf("%3s",hexa(instreg[sy].op2reg4));
	gotoxy(77,16);cprintf("%3s",hexa(instreg[sy].op2reg6));
	gotoxy(77,18);cprintf("%3s",hexa(instreg[sy].op2reg8));
	gotoxy(77,21);cprintf("%3s",hexa(instreg[sy].op2rege));
		
}

/* Imprime a Informação sobre os Registos dos Operadores */
void printreg(void)
{
	
	/* Nome do Instrumento */	
	textbackground(10);
	textcolor(15);
	gotoxy(35,5);cprintf("%-20s",instreg[sy].nome);
	
	textbackground(0);
	textcolor(7);
	
	/* Posição do Instrumento no Bank */
	gotoxy(64,5);cprintf("%3d",sy);
	
	/* Registos dos Operadores */
	/* Operador 1 */
	gotoxy(35,9);cprintf("%3d",instreg[sy].op1reg2);gotoxy(45,9);cprintf("%3s",hexa(instreg[sy].op1reg2));
	gotoxy(35,11);cprintf("%3d",instreg[sy].op1reg4);gotoxy(45,11);cprintf("%3s",hexa(instreg[sy].op1reg4));
	gotoxy(35,13);cprintf("%3d",instreg[sy].op1reg6);gotoxy(45,13);cprintf("%3s",hexa(instreg[sy].op1reg6));
	gotoxy(35,15);cprintf("%3d",instreg[sy].op1reg8);gotoxy(45,15);cprintf("%3s",hexa(instreg[sy].op1reg8));
	gotoxy(35,17);cprintf("%3d",instreg[sy].op1rege);gotoxy(45,17);cprintf("%3s",hexa(instreg[sy].op1rege));
		
	/* Operador 2 */
	gotoxy(65,9);cprintf("%3d",instreg[sy].op2reg2);gotoxy(75,9);cprintf("%3s",hexa(instreg[sy].op2reg2));
	gotoxy(65,11);cprintf("%3d",instreg[sy].op2reg4);gotoxy(75,11);cprintf("%3s",hexa(instreg[sy].op2reg4));
	gotoxy(65,13);cprintf("%3d",instreg[sy].op2reg6);gotoxy(75,13);cprintf("%3s",hexa(instreg[sy].op2reg6));
	gotoxy(65,15);cprintf("%3d",instreg[sy].op2reg8);gotoxy(75,15);cprintf("%3s",hexa(instreg[sy].op2reg8));
	gotoxy(65,17);cprintf("%3d",instreg[sy].op2rege);gotoxy(75,17);cprintf("%3s",hexa(instreg[sy].op2rege));
	
	/* Registo C0 */
	gotoxy(35,19);cprintf("%3d",instreg[sy].regc0);gotoxy(45,19);cprintf("%3s",hexa(instreg[sy].regc0));
		
}

/* Imprime o Help */
void printhelp(void)
{
	
	int i;
	
	textbackground(0);
	textcolor(7);
	clrscr();
		
	textbackground(1);
	textcolor(15);
	gotoxy(1,1);
	cprintf("  F1-Help  F2-Load  F3-Save                                           ESC-Sair  ");
	
	textbackground(1);
	textcolor(15);
	for (i=3; i<25; i++)
	{
		gotoxy(3,i);
		cprintf("                                                                            ");
	}
	
	gotoxy(5,4);cprintf("Help:");
	
	if (help==0)
	{
		gotoxy(11,4);cprintf("Pagina 1/2");
		
		gotoxy(5,6);cprintf("ALT - Alterna entre os Instrumentos FM e os Operadores.");
	
		gotoxy(5,8);cprintf("CTRL+ALT - Alterna entre os Valores e os Registos dos Operadores.");
	
		gotoxy(5,10);cprintf("Instrumentos FM");gotoxy(5,11);cprintf("---------------");
		gotoxy(5,12);cprintf("Cursor Cima/Cursor Baixo seleciona o Instrumento FM.");
	
		gotoxy(5,14);cprintf("Operadores");gotoxy(5,15);cprintf("----------");
		gotoxy(5,16);cprintf("Cursores navegam nos Valores / Registos dos Operadores.");
		gotoxy(5,17);cprintf("CTRL+N para alterar o Nome do Instrumento FM.");
		gotoxy(5,18);cprintf("Enter para terminar alteracao do nome.");
		gotoxy(5,19);cprintf("0-9 para novos valores dos Valores / Registos dos Operadores.");
		gotoxy(5,20);cprintf("Enter para terminar alteracao dos Valores / Registos dos Operadores.");
	
		gotoxy(5,23);cprintf("Cursor Direita: Pagina Seguinte.                   Return: Volta ao Menu.");
	}
	else
	{
		gotoxy(11,4);cprintf("Pagina 2/2");
		
		gotoxy(5,6);cprintf("Teclado Musical");gotoxy(5,7);cprintf("---------------");
		
		gotoxy(5,8);cprintf("Z - Do.");gotoxy(20,8);cprintf("S - Do Sustenido.");
		gotoxy(5,9);cprintf("X - Re.");gotoxy(20,9);cprintf("D - Re Sustenido.");
		gotoxy(5,10);cprintf("C - Mi.");
		gotoxy(5,11);cprintf("V - Fa.");gotoxy(20,11);cprintf("G - Fa Sustenido.");
		gotoxy(5,12);cprintf("B - Sol.");gotoxy(20,12);cprintf("H - Sol Sustenido.");
		gotoxy(5,13);cprintf("N - La.");gotoxy(20,13);cprintf("J - La Sustenido.");
		gotoxy(5,14);cprintf("M - Si.");
		gotoxy(5,15);cprintf(", - Do Maior.");gotoxy(20,15);cprintf("L - Do Maior Sustenido.");
		
		gotoxy(5,17);cprintf("+ / - : Aumenta / Diminui a Oitava.");
		
		gotoxy(5,19);cprintf("F2 - Le os Instrumentos FM a partir do Ficheiro FmIntCrt.dat.");
		gotoxy(5,20);cprintf("F3 - Grava os Instrumentos FM para o Ficheiro FmIntCrt.dat.");
				
		gotoxy(5,23);cprintf("Cursor Esquerda: Pagina Anterior.                  Return: Volta ao Menu.");
	}
	
}

/* Navega nos Instrumentos / Operadores / Teclado */
void navega(void)
{
	
	int i;
	
	do
	{
		KB_code=getch();
		
		if (KB_code==KB_F1)	/* F1 - Help */
		{
			estado=1;
			help=0;
		}
		
		if (KB_code==KB_F2)	/* F2 - Load */
		{
			i=leinstfm();
			printestado();
			i++;
		}
		
		if (KB_code==KB_F3)	/* F3 - Save */
		{
			i=gravainstfm();
			printestado();
			i++;
		}
		
		if (KB_code==KB_ESCAPE)	estado=10; /* Escape - Sair */

		/* Alterna entre Instrumentos e Operadores */
		if (KB_code==KB_TAB) menu=1-menu; /* TAB */
		
		/* Alterna entre Valores e Registos dos Operadores */
		if (KB_code==KB_CTRL_TAB)	 /* CTRL + TAB */
		{
			menuv=1-menuv;
										
			if (menuv==0)
			{
				menuval();
				printval();
			}
			else
			{
				menureg();
				printreg();
			}
		}

		if (menu==0)
		{
			alteracab();
			navegainstfm();
		}
		else
		{
			alteracab();
			if (menuv==0) navegaval();
			else navegareg();
		}
						
		/* Oitava */
		if (KB_code==KB_MENOS && oitava>0)	/* Tecla - */
		{
			oitava--;
			printoct();
		}
		
		if (KB_code==KB_MAIS && oitava<7)	/* Tecla + */
		{
			oitava++;
			printoct();
		}
		
		if (KB_code==KB_INS)	/* INSERT - Insere Instrumento FM */
			insereinstfm(sy);
			
		if (KB_code==KB_DEL)	/* DELETE - Apaga Instrumento FM */
			apagainstfm(sy);
		
		/* Teclado Musical */
		if (noed==0)
		{
			switch(KB_code)
			{
				case KB_Z:criasomfm(0);break;	/* Tecla Z - Dó */
				case KB_S:criasomfm(1);break;	/* Tecla S - Dó Sustenido */
				case KB_X:criasomfm(2);break;	/* Tecla X - Ré */
				case KB_D:criasomfm(3);break;	/* Tecla D - Ré Sustenido */
				case KB_C:criasomfm(4);break;	/* Tecla C - Mi */
				case KB_V:criasomfm(5);break;	/* Tecla V - Fá */
				case KB_G:criasomfm(6);break;	/* Tecla G - Fá Sustenido */
				case KB_B:criasomfm(7);break;	/* Tecla B - Sol */
				case KB_H:criasomfm(8);break;	/* Tecla H - Sol Sustenido */
				case KB_N:criasomfm(9);break;	/* Tecla N - Lá */
				case KB_J:criasomfm(10);break;	/* Tecla J - Lá Sustenido */
				case KB_M:criasomfm(11);break;	/* Tecla M - Si */
				case KB_VIRGULA:criasomfm(12);break;	/* Tecla , - Dó Maior */
				case KB_L:criasomfm(13);break;	/* Tecla L - Dó Maior Sustenido */
				case KB_O:stopsomfm();break;	/* Desliga o Canal */
			}
		}
		
//		gotoxy(1,2);
//		printf("%d ",KB_code);
			
	}
	while (estado==0);
	
}

/* Navega nos Instrumentos FM */
void navegainstfm(void)
{

	if (KB_code==KB_UP && sy>0)		/* Cima */
	{
		sy--;
		if (sy<112 && py>0) py--;
	}
			
	if (KB_code==KB_DOWN && sy<127) /* Baixo */
	{
		sy++;
		if (sy>15 && py<112) py++;
	}
	
	
	if (KB_code==KB_HOME)	/* Home */
	{
		py=0;
		sy=0;
	}
						
	if (KB_code==KB_END)	/* End */
	{
		py=112;
		sy=127;
	}

						
	if (KB_code==KB_PAGE_UP)	/* Page UP */
	{
		py-=16;
		if (py<0) py=0;
		sy=py;
	}
	
	if (KB_code==KB_PAGE_DOWN)	/* Page DOWN */
	{
		py+=16;
		if (py>112) py=112;
		sy=py;
	}
	
	printinstfm();
	
	if (menuv==0) printval();
	else printreg();
		
}

/* Navega nos Valores dos Operadores */
void navegaval(void)
{
	
	if (KB_code==KB_LEFT && vox>0)	/* Esquerda */
	{
		if (voed==1) updateval();
		if (noed==1) updatenome();
		vox--;
		printcursor(tvox[ovox],tvoy[ovoy],tvox[vox],tvoy[voy],2);
		ovox=vox;
	};
		
	if (KB_code==KB_RIGHT && vox<1) /* Direita */
	{
		if (voed==1) updateval();
		if (noed==1) updatenome();
		vox++;
		printcursor(tvox[ovox],tvoy[ovoy],tvox[vox],tvoy[voy],2);
		ovox=vox;
	};
			
	if (KB_code==KB_UP && voy>0)	/* Cima */
	{
		if (voed==1) updateval();
		if (noed==1) updatenome();
		voy--;
		printcursor(tvox[ovox],tvoy[ovoy],tvox[vox],tvoy[voy],2);
		ovoy=voy;
	};
		
	if (KB_code==KB_DOWN && voy<12)	/* Baixo */
	{
		if (voed==1) updateval();
		if (noed==1) updatenome();
		voy++;
		printcursor(tvox[ovox],tvoy[ovoy],tvox[vox],tvoy[voy],2);
		ovoy=voy;
	};
	
	if (KB_code==KB_CTRL_N && noed==0) /* Altera o Nome do Instrumento */
	{
		noed=1; 
		
		textbackground(4);
		textcolor(14);
		
		sprintf(vron,instreg[sy].nome);
		gotoxy(35,5);cprintf("%-20s",vron);
		
		noc=0;
		while(vron[noc]!=0) noc++;
		
		printcursornome(35+noc,5,35+noc,5,2);
	}

	if (noed==1) inputnome();
	else inputval();

}

/* Navega nos Registos dos Operadores */
void navegareg(void)
{
	
	if (KB_code==KB_LEFT && rox>0)	/* Esquerda */
	{
		if (roed==1) updatereg();
		if (noed==1) updatenome();
		rox--;
		printcursor(trox[orox],troy[oroy],trox[rox],troy[roy],2);
		orox=rox;
	};
		
	if (KB_code==KB_RIGHT && rox<1) /* Direita */
	{
		if (roed==1) updatereg();
		if (noed==1) updatenome();
		if (roy!=5)
		{
			rox++;
			printcursor(trox[orox],troy[oroy],trox[rox],troy[roy],2);
			orox=rox;
		}
	};
			
	if (KB_code==KB_UP && roy>0)	/* Cima */
	{
		if (roed==1) updatereg();
		if (noed==1) updatenome();
		roy--;
		printcursor(trox[orox],troy[oroy],trox[rox],troy[roy],2);
		oroy=roy;
	};
		
	if (KB_code==KB_DOWN && roy<5)	/* Baixo */
	{
		if (roed==1) updatereg();
		if (noed==1) updatenome();
		if (rox==1 && roy==4){}
		else
		{
			roy++;
			printcursor(trox[orox],troy[oroy],trox[rox],troy[roy],2);
			oroy=roy;
		}
	};
	
	if (KB_code==KB_CTRL_N && noed==0) /* Altera o Nome do Instrumento */
	{
		noed=1; 
		
		textbackground(4);
		textcolor(14);
		
		sprintf(vron,instreg[sy].nome);
		gotoxy(35,5);cprintf("%-20s",vron);
		
		noc=0;
		while(vron[noc]!=0) noc++;
		
		printcursornome(35+noc,5,35+noc,5,2);
	}
		
	if (noed==1) inputnome();
	else inputreg();
	
}

/* Navega no Help */
void navegahelp(void)
{
	
	do
	{
		KB_code=getch();
		
		if (KB_code==KB_RETURN) estado=0;
		
		if (KB_code==KB_ESCAPE)	estado=10; /* Escape - Sair */
		
		if (KB_code==KB_LEFT && help==1)
		{
			help=0;
			printhelp();
		}
	
		if (KB_code==KB_RIGHT && help==0)
		{
			help=1;
			printhelp();
		}
	}
	while(estado==1);
	
}

/* Edita os Valores dos Operadores */
void inputval(void)
{
	
	cc=0;
	dd=0;
	uu=0;
	keypressed=0;
	
	/* Valores dos Operadores */
	if (KB_code>=KB_0 && KB_code<=KB_9 && voc>0)
	{
		keypressed=1;
		uu=KB_code-48;
		voed=1;
		printcursor(tvox[ovox],tvoy[ovoy],tvox[vox],tvoy[voy],4);
	};
	
	if (KB_code==KB_BACKSPACE && voc<3 && voed==1)
	{
		voc++;
	
		if (voc==1)
		{
			vro[2]=vro[1];
			vro[1]=vro[0];
			vro[0]=' ';
		}
		
		if (voc==2)
		{
			vro[2]=vro[1];
			vro[1]=' ';
			vro[0]=' ';
		}
		
		if (voc==3)
		{
			vro[2]=' ';
			vro[1]=' ';
			vro[0]=' ';
		}
		
		vro[3]='\0';
		
		gotoxy(tvox[vox]+2,tvoy[voy]);
		cprintf("%s",vro);
	};
	
	if (keypressed==1 && voed==1)
	{
		voc--;
	
		if (voc==0)
		{
			vro[0]=vro[1];
			vro[1]=vro[2];
		}
		
		if (voc==1)
		{
			vro[0]=' ';
			vro[1]=vro[2];
		}
		
		if (voc==2)
		{
			vro[0]=' ';
			vro[1]=' ';
		}
		
		vro[2]=uu+48;
		vro[3]='\0';
		
		gotoxy(tvox[vox]+2,tvoy[voy]);
		cprintf("%s",vro);
	}
	
	if (KB_code==KB_RETURN && voed==1)
		updateval();
	
}

/* Coloca os Novos Valores nos Operadores */
void updateval(void)
{

	unsigned short treg;
	
	voc=3;
		
	if (vro[0]==' ') cc=0;
	else cc=(int)((vro[0]-48)*100);
		
	if (vro[1]==' ') dd=0;
	else dd=(int)((vro[1]-48)*10);
		
	if (vro[2]==' ') uu=0;
	else uu=(int)(vro[2]-48);
	
	voi=cc+uu+dd;
		
	if (voy==0 && voi>1) voi=1;
	if (voy==1 && voi>1) voi=1;
	if (voy==2 && voi>1) voi=1;
	if (voy==3 && voi>1) voi=1;
	if (voy==4 && voi>15) voi=15;
	
	if (voy==5 && voi>3) voi=3;
	if (voy==6 && voi>63) voi=63;
	
	if (voy==7 && voi>15) voi=15;
	if (voy==8 && voi>15) voi=15;
	if (voy==9 && voi>15) voi=15;
	if (voy==10 && voi>15) voi=15;
	
	if (voy==11 && voi>3) voi=3;
	
	if (voy==12 && vox==0 && voi>7) voi=7;
	if (voy==12 && vox==1 && voi>1) voi=1;
				
	gotoxy(tvox[vox]+2,tvoy[voy]);cprintf("%3d",voi);
		
	printcursor(tvox[ovox],tvoy[ovoy],tvox[vox],tvoy[voy],2);
	voed=0;

	if (vox==0)	/* Operador 1 */
	{
		/* Registo 20 */
		if (voy==0) o1r2t=voi;
		if (voy==1) o1r2v=voi;
		if (voy==2) o1r2s=voi;
		if (voy==3) o1r2k=voi;
		if (voy==4) o1r2h=voi;
		
		if (voy==0 || voy==1 || voy==2 || voy==3 || voy==4)
		{
			treg=128*o1r2t+64*o1r2v+32*o1r2s+16*o1r2k+o1r2h;
			instreg[sy].op1reg2=treg;
			gotoxy(49,7);cprintf("%3s",hexa(treg));
		}
	
		/* Registo 40 */
		if (voy==5) o1r4k=voi;
		if (voy==6) o1r4v=voi;
		
		if (voy==5 || voy==6)
		{
			treg=64*o1r4k+o1r4v;
			instreg[sy].op1reg4=treg;
			gotoxy(49,13);cprintf("%3s",hexa(treg));
		}
		
		/* Registo 60 */
		if (voy==7) o1r6a=voi;
		if (voy==8) o1r6d=voi;
		
		if (voy==7 || voy==8) 
		{
			treg=16*o1r6a+o1r6d;
			instreg[sy].op1reg6=treg;
			gotoxy(49,16);cprintf("%3s",hexa(treg));
		}
		
		/* Registo 80 */
		if (voy==9) o1r8s=voi;
		if (voy==10) o1r8r=voi;
		
		if (voy==9 || voy==10)
		{
			treg=16*o1r8s+o1r8r;
			instreg[sy].op1reg8=treg;
			gotoxy(49,18);cprintf("%3s",hexa(treg));
		}
		
		/* Registo E0 */
		if (voy==11)
		{
			o1ree=voi;
			treg=o1ree;
			instreg[sy].op1rege=treg;
			gotoxy(49,21);cprintf("%3s",hexa(treg));
		}
	
		/* Registo C0 - Feedback Modulation */
		if (voy==12)
		{
			rcf=voi;
			treg=2*rcf+rca;
			instreg[sy].regc0=treg;
			gotoxy(77,23);cprintf("%3s",hexa(treg));
		}
	}
	else /* Operador 2 */
	{
		/* Registo 20 */
		if (voy==0) o2r2t=voi;
		if (voy==1) o2r2v=voi;
		if (voy==2) o2r2s=voi;
		if (voy==3) o2r2k=voi;
		if (voy==4) o2r2h=voi;
		
		if (voy==0 || voy==1 || voy==2 || voy==3 || voy==4)
		{
			treg=128*o2r2t+64*o2r2v+32*o2r2s+16*o2r2k+o2r2h;
			instreg[sy].op2reg2=treg;
			gotoxy(77,7);cprintf("%3s",hexa(treg));
		}
	
		/* Registo 40 */
		if (voy==5) o2r4k=voi;
		if (voy==6) o2r4v=voi;
		
		if (voy==5 || voy==6)
		{
			treg=64*o2r4k+o2r4v;
			instreg[sy].op2reg4=treg;
			gotoxy(77,13);cprintf("%3s",hexa(treg));
		}
		
		/* Registo 60 */
		if (voy==7) o2r6a=voi;
		if (voy==8) o2r6d=voi;
		
		if (voy==7 || voy==8) 
		{
			treg=16*o2r6a+o2r6d;
			instreg[sy].op2reg6=treg;
			gotoxy(77,16);cprintf("%3s",hexa(treg));
		}
		
		/* Registo 80 */
		if (voy==9) o2r8s=voi;
		if (voy==10) o2r8r=voi;
		
		if (voy==9 || voy==10)
		{
			treg=16*o2r8s+o2r8r;
			instreg[sy].op2reg8=treg;
			gotoxy(77,18);cprintf("%3s",hexa(treg));
		}
		
		/* Registo E0 */
		if (voy==11)
		{
			o2ree=voi;
			treg=o2ree;
			instreg[sy].op2rege=treg;
			gotoxy(77,21);cprintf("%3s",hexa(treg));
		}
	
		/* Registo C0 - AM/FM Synthesis */
		if (voy==12)
		{
			rca=voi;
			treg=2*rcf+rca;
			instreg[sy].regc0=treg;
			gotoxy(77,23);cprintf("%3s",hexa(treg));
		}
	}
	
}

/* Edita os Registos dos Operadores */
void inputreg(void)
{

	cc=0;
	dd=0;
	uu=0;
	keypressed=0;
	
	/* Registos */
	if (KB_code>=KB_0 && KB_code<=KB_9 && roc>0)
	{
		keypressed=1;
		uu=KB_code-48;
		roed=1;
		printcursor(trox[orox],troy[oroy],trox[rox],troy[roy],4);
	};
	
	if (KB_code==KB_BACKSPACE && roc<3 && roed==1)
	{
		roc++;
	
		if (roc==1)
		{
			vro[2]=vro[1];
			vro[1]=vro[0];
			vro[0]=' ';
		}
		
		if (roc==2)
		{
			vro[2]=vro[1];
			vro[1]=' ';
			vro[0]=' ';
		}
		
		if (roc==3)
		{
			vro[2]=' ';
			vro[1]=' ';
			vro[0]=' ';
		}
		
		vro[3]='\0';
		
		gotoxy(trox[rox]+2,troy[roy]);
		cprintf("%s",vro);
	};
	
	if (keypressed==1 && roed==1)
	{
		roc--;
	
		if (roc==0)
		{
			vro[0]=vro[1];
			vro[1]=vro[2];
		}
		
		if (roc==1)
		{
			vro[0]=' ';
			vro[1]=vro[2];
		}
		
		if (roc==2)
		{
			vro[0]=' ';
			vro[1]=' ';
		}
		
		vro[2]=uu+48;
		vro[3]='\0';
		
		gotoxy(trox[rox]+2,troy[roy]);
		cprintf("%s",vro);
	}
	
	if (KB_code==KB_RETURN && roed==1)
		updatereg();
	
}

/* Coloca os Novos Registos nos Operadores */
void updatereg(void)
{
	
	roc=3;
		
	if (vro[0]==' ') cc=0;
	else cc=(int)((vro[0]-48)*100);
		
	if (vro[1]==' ') dd=0;
	else dd=(int)((vro[1]-48)*10);
		
	if (vro[2]==' ') uu=0;
	else uu=(int)(vro[2]-48);
	
	roi=cc+uu+dd;
	
	if (roi>255) roi=255;
	
	if (roy==5 && roi>15) roi=15;
			
	gotoxy(trox[rox]+2,troy[roy]);cprintf("%3d",roi);
	gotoxy(trox[rox]+12,troy[roy]);cprintf("%3s",hexa(roi));
		
	printcursor(trox[orox],troy[oroy],trox[rox],troy[roy],2);
	roed=0;

	if (rox==0) /* Operador 1 */
	{
		if (roy==0) instreg[sy].op1reg2=roi;
				
		if (roy==1) instreg[sy].op1reg4=roi;
		
		if (roy==2) instreg[sy].op1reg6=roi;
		
		if (roy==3) instreg[sy].op1reg8=roi;
		
		if (roy==4) instreg[sy].op1rege=roi;
	
		if (roy==5) instreg[sy].regc0=roi;
	}
	else	/* Operador 2 */
	{
		if (roy==0) instreg[sy].op2reg2=roi;
				
		if (roy==1) instreg[sy].op2reg4=roi;
		
		if (roy==2) instreg[sy].op2reg6=roi;
		
		if (roy==3) instreg[sy].op2reg8=roi;
		
		if (roy==4) instreg[sy].op2rege=roi;
	}
	
}

/* Edita o Nome do Instrumento FM */
void inputnome(void)
{
	
	uu=0;
	keypressed=0;

	/* Nome */
	if (KB_code>=32 && KB_code<=127 && noc<19 && noed==1)
	{
		keypressed=1;
		uu=KB_code;
	};
	
	if (KB_code==KB_BACKSPACE && noc>0 && noed==1)
	{
		noc--;
		vron[noc]='\0';
		textbackground(4);
		textcolor(14);
		gotoxy(35+noc,5);
		cprintf("%c",vron[noc]);
		printcursornome(35+noc+1,5,35+noc,5,2);
	};
		
	if (keypressed==1 && noed==1)
	{
		printcursornome(35+noc,5,35+noc+1,5,2);
		vron[noc]=uu;
		textbackground(4);
		textcolor(14);
		gotoxy(35+noc,5);
		cprintf("%c",vron[noc]);
		noc++;
	}
	
	if (KB_code==KB_RETURN && noed==1)
		updatenome();
	
}

/* Coloca o Novo Nome no Instrumento FM */
void updatenome(void)
{

	if (noc==0) sprintf(vron,"Vazio-%d\0'",sy);
	else vron[noc]='\0';
	
	noed=0;
	noc=0;
	
	sprintf(instreg[sy].nome,vron);

	textbackground(10);
	textcolor(15);
	gotoxy(35,5);
	cprintf("%-20s",vron);

	printinstfm();
	
	textbackground(0);
	textcolor(7);
	
}

/* Grava os Instrumentos FM para um Ficheiro */
int gravainstfm(void)
{

	FILE *fp;
	
	fp=fopen("FmIntCrt.dat","wb");
	
	if(fp==NULL)
    {
      sprintf(txtsts,"Impossivel Criar Instrumentos FM!\0");
	  return(1);
    }
	
	if (fwrite(instreg,sizeof(instreg),1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Gravar Instrumentos FM!\0");
		fclose(fp);
		return(2);
	}

	sprintf(txtsts,"Instrumentos FM Gravados com Sucesso!\0");

	fclose(fp);

	return(0);
}

/* Lê os Instrumentos FM a partir de um Ficheiro */
int leinstfm(void)
{

	FILE *fp;
	
	fp=fopen("FmIntCrt.dat","rb");
    
	if(fp==NULL)
    {
      sprintf(txtsts,"Impossivel Abrir Instrumentos FM!\0");
	  return(1);
    }
	
	if (fread(instreg,sizeof(instreg),1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Ler Instrumentos FM!\0");
		fclose(fp);
		return(2);
	}
			
	sprintf(txtsts,"Instrumentos FM Lidos com Sucesso!\0");
	fclose(fp);

	return(0);
	
}

/* Insere Instrumento FM */
void insereinstfm(int inst)
{
	
	int i;
	
	/* Roda os restantes Instrumentos FM uma posição para Cima */
	for (i=128; i>inst; i--)
	{
		sprintf(instreg[i].nome,instreg[i-1].nome);
		instreg[i].num=instreg[i-1].num;
		instreg[i].regc0=instreg[i-1].regc0;
		instreg[i].op1reg2=instreg[i-1].op1reg2;
		instreg[i].op1reg4=instreg[i-1].op1reg4;
		instreg[i].op1reg6=instreg[i-1].op1reg6;
		instreg[i].op1reg8=instreg[i-1].op1reg8;
		instreg[i].op1rege=instreg[i-1].op1rege;
		instreg[i].op2reg2=instreg[i-1].op2reg2;
		instreg[i].op2reg4=instreg[i-1].op2reg4;
		instreg[i].op2reg6=instreg[i-1].op2reg6;
		instreg[i].op2reg8=instreg[i-1].op2reg8;
		instreg[i].op2rege=instreg[i-1].op2rege;
	}
	
	/* Insere o Novo Instrumento FM */
	sprintf(instreg[inst].nome,"vazio\0");
	instreg[inst].num=inst;
	instreg[inst].regc0=0;
	instreg[inst].op1reg2=0;
	instreg[inst].op1reg4=0;
	instreg[inst].op1reg6=0;
	instreg[inst].op1reg8=0;
	instreg[inst].op1rege=0;
	instreg[inst].op2reg2=0;
	instreg[inst].op2reg4=0;
	instreg[inst].op2reg6=0;
	instreg[inst].op2reg8=0;
	instreg[inst].op2rege=0;
	
	printinstfm();
	
	if (menuv==0) printval();
	else printreg();
	
	sprintf(txtsts,"Instrumento Criado com Sucesso!\0");
	printestado();
		
}

/* Apaga Instrumento FM */
void apagainstfm(int inst)
{
	int i;
	
	/* Roda os restantes Instrumentos FM uma posição para Baixo */
	for (i=inst; i<127; i++)
	{
		sprintf(instreg[i].nome,instreg[i+1].nome);
		instreg[i].num=instreg[i+1].num;
		instreg[i].regc0=instreg[i+1].regc0;
		instreg[i].op1reg2=instreg[i+1].op1reg2;
		instreg[i].op1reg4=instreg[i+1].op1reg4;
		instreg[i].op1reg6=instreg[i+1].op1reg6;
		instreg[i].op1reg8=instreg[i+1].op1reg8;
		instreg[i].op1rege=instreg[i+1].op1rege;
		instreg[i].op2reg2=instreg[i+1].op2reg2;
		instreg[i].op2reg4=instreg[i+1].op2reg4;
		instreg[i].op2reg6=instreg[i+1].op2reg6;
		instreg[i].op2reg8=instreg[i+1].op2reg8;
		instreg[i].op2rege=instreg[i+1].op2rege;
	}
	
	/* "Zera" a Posição 127 */
	sprintf(instreg[127].nome,"vazio\0");
	instreg[127].num=127;
	instreg[127].regc0=0;
	instreg[127].op1reg2=0;
	instreg[127].op1reg4=0;
	instreg[127].op1reg6=0;
	instreg[127].op1reg8=0;
	instreg[127].op1rege=0;
	instreg[127].op2reg2=0;
	instreg[127].op2reg4=0;
	instreg[127].op2reg6=0;
	instreg[127].op2reg8=0;
	instreg[127].op2rege=0;
	
	printinstfm();
	
	if (menuv==0) printval();
	else printreg();
		
	sprintf(txtsts,"Instrumento FM Apagado com Sucesso!\0");
	printestado();
	
	
}

/* Converte Decimal para Hexadecimal */
char *hexa(int deca)
{

	static char *hex;
	
	char gg[3];
	short h,l;
	
	hex="00\0";
	
	h=(short)(deca/16);
	l=(short)(deca-h*16);
	
	if (h<10) gg[0]=h+48;
	else gg[0]=h+55;
	
	if (l<10) gg[1]=l+48;
	else gg[1]=l+55;
	
	gg[2]='\0';
	
	sprintf(hex,gg);
			
	return(hex);
	
}

void criainst()
{

	int i;

/*	for (i=0; i<128; i++)
	{
		printf("%s\n",instreg[i].nome);
		printf("%d\n",instreg[i].num);
		printf("%d\n",instreg[i].regc0);
		printf("%d\n",instreg[i].op1reg2);
		printf("%d\n",instreg[i].op1reg4);
		printf("%d\n",instreg[i].op1reg6);
		printf("%d\n",instreg[i].op1reg8);
		printf("%d\n",instreg[i].op1rege);
		printf("%d\n",instreg[i].op2reg2);
		printf("%d\n",instreg[i].op2reg4);
		printf("%d\n",instreg[i].op2reg6);
		printf("%d\n",instreg[i].op2reg8);
		printf("%d\n",instreg[i].op2rege);
		printf("\n");
	}

	
	for (i=0; i<128; i++)
	{
		sprintf(instreg[i].nome,"vazio\0");
		instreg[i].num=i;
		instreg[i].regc0=0;
		instreg[i].op1reg2=0;
		instreg[i].op1reg4=0;
		instreg[i].op1reg6=0;
		instreg[i].op1reg8=0;
		instreg[i].op1rege=0;
		instreg[i].op2reg2=0;
		instreg[i].op2reg4=0;
		instreg[i].op2reg6=0;
		instreg[i].op2reg8=0;
		instreg[i].op2rege=0;
	}

*/
	for (i=13; i<128; i++)
	{
		sprintf(instreg[i].nome,"vazio\0",i);
	}
	
/*	for (i=0; i<128; i++)
	{
		instreg[i].num=i;
	}
*/

}