/*
/*
/*  Philips SAA1099 Instrument Creator 1.3
/*
/*  CMS Card/Game Blaster, Sam Coupé
/*
/*  Para compilar: tcc SaIntCrt.c
/*
/*  (C) 2023 Penisoft / MadAxe
/*
*/

/* Includes */
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

/* Portos da Placa de Som SAA1099 */
#define CMS_REG_1	 0x221	/* Porto dos Registos Canais 1-6  */
#define CMS_DAT_1	 0x220 	/* Porto da Data 	  Canais 1-6  */
#define CMS_REG_7  	 0x223	/* Porto dos Registos Canais 7-12 */
#define CMS_DAT_7  	 0x222 	/* Porto da Data 	  Canais 7-12 */

/* Registos da Placa de Som SAA1099 */
#define SAA_MIN_REG 0x00	/* Primeiro Registo */
#define SAA_MAX_REG 0x1F	/* Último Registo   */

/* Frequência das Notas SAA1099 */
#define C	83	/* Dó */
#define Cs	107	/* Dó Sustenido */
#define D	130	/* Ré */
#define Ds	151	/* Ré Sustenido */
#define E	172	/* Mi */
#define F	191	/* Fá */
#define Fs	209	/* Fá Sustenido */
#define G	226	/* Sol */
#define Gs	242	/* Sol Sustenido */
#define A	3	/* Lá */
#define As	31	/* Lá Sustenido */
#define B	58	/* Si */
#define Cm	83	/* Dó Maior */
#define Cms	107	/* Dó Maior Sustenido */

/* Prototypes */
void screenopen(void);
void screenclose(void);

int inicializa(void);
void mainmenu(void);

void resetsa(void);
void writesa1(int reg, int value);
void writesa7(int reg, int value);
void getenvlsa(void);
void envelopsa(void);
void precriasomsa(int freqindex);
void setvolsa(void);
void criasomsa(int freqindex);
void stopsomsa(void);

int leinstsa(void);
int gravainstsa(void);

void printdata(void);
void menuval(void);

void printcursor(short onx, short ony, short nx, short ny, short nc);
void printcursornome(short onx, short ony, short nx, short ny, short nc);
void printestado(void);
void printoct(void);
void printinstsa(void);
void printval(void);
void printhelp(void);

void alteracab(void);
void navega(void);
void navegainstsa(void);
void navegaval(void);
void navegahelp(void);

void inputval(void);
void updateval(void);

void inputnome(void);
void updatenome(void);

void insereinstsa(int inst);
void apagainstsa(int inst);

char *hexa(int deca);

void criainst();

/* Teclado */
int KB_code=0;
int estado=0;
int menu;

/* Navegação nos Instrumentos SAA1099 */
int sy,py,instcolor;
int valcolor;

/* Oitava */
int oitavasa=2;
int volumesa;
int playenvlsa;
int factorsa=5;

/* Navegação nos Valores */
unsigned short vox,voy,ovox,ovoy;
unsigned short voed,voc,voi;
unsigned short tvoy[7]={7,8,10,11,12,14,16};
unsigned short tvox[2]={42,70};

unsigned short noed,noc,noi;

/* Introdução de Dados Valores / Registos dos Operadores */
char vro[4],vron[20];
char tccc[3];
int cc=0,dd=0,uu=0;
int keypressed=0;

/* Help */
int help;

/* Definição dos Registos para cada Instrumento SAA1099 */
struct instregssa
{
	char nome[20];
	unsigned char num;
	unsigned char freqenb;
	unsigned char noiseenb;
	unsigned char attack;
	unsigned char sustain;
	unsigned char decay;
	unsigned char volume;
	unsigned char repete;
} instregsa[128];

unsigned char notassa[14]={C,Cs,D,Ds,E,F,Fs,G,Gs,A,As,B,Cm,Cms};
unsigned char octvssa[14]={0,0,0,0,0,0,0,0,0,1,1,1,1,1};
unsigned char envlssa[3];
unsigned short cntenvlssa[3];
unsigned short cntenvlsusssa;



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

	resetsa();

	textbackground(0);
	textcolor(7);
	clrscr();
	
	textcolor(11);
	cprintf("Philips SAA1099 Instrument Creator 1.3\r\n");
	textcolor(10);
	cprintf("(C) 2023 Penisoft / MadAxe\r\n");
	
	_setcursortype(_NORMALCURSOR);
	
}

/* Inicializa o Programa */
int inicializa(void)
{

	int f;

	clrscr();
	
	textcolor(11);
	cprintf("Philips SAA1099 Instrument Creator 1.3\r\n");
	textcolor(10);
	cprintf("(C) 2023 Penisoft / MadAxe\r\n\r\n");
	
	textcolor(15);

//	criainst();

	f=leinstsa();
	if (f!=0)
	{
		cprintf("%s\r\n",txtsts);
		return(f);
	}
	
//	criainst();
	
	resetsa();

	/* Menu */
	menu=0;
	estado=0;
	
	/* Help */
	help=0;
		
	/* Instrumentos SAA1099 */
	py=0;
	sy=0;
	instcolor=14;
	valcolor=15;
	
	/* Valores */
	vox=0;
	voy=0;
	ovox=vox;
	ovoy=voy;
	voed=0;
	voc=3;
	
	/* Alteração do Nome do Instrumento */
	noed=0;
	noc=0;
	
	return(0);
	
}

/* Main Menu */
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

/* Faz o Reset à Placa de Som SAA1099 */
void resetsa()
{

	int i;
	
	for (i=SAA_MIN_REG; i<SAA_MAX_REG; i++)
	{
		writesa1(i,0);
		writesa7(i,0);
	}
	
	
	writesa1(0x1C,2);	/* Reset Frequência Canais 1-6  */
	writesa1(0x1C,1); 	/* Todos os Canais 1-6 Ativados */
	
	writesa7(0x1C,2);  /* Reset Frequência Canais 7-12  */
	writesa7(0x1C,1); 	/* Todos os Canais 7-12 Ativados */
	
}

/* Escreve um Valor num Registo Canais 1-6 SAA1099 */
void writesa1(int reg, int value)
{
	
	int val;
	
	val=outp(CMS_REG_1,reg);
	val=outp(CMS_DAT_1,value);
	
	val++;
	
}

/* Escreve um Valor num Registo Canais 7-12 SAA1099 */
void writesa7(int reg, int value)
{
	
	int val;
	
	val=outp(CMS_REG_7,reg);
	val=outp(CMS_DAT_7,value);
	
	val++;
}

/* Saca os Valores do Envelope SAA1099 */
void getenvlsa(void)
{
	
	envlssa[0]=instregsa[sy].attack;
	envlssa[1]=instregsa[sy].sustain;
	envlssa[2]=instregsa[sy].decay;
	
	cntenvlssa[0]=factorsa*instregsa[sy].attack;
	cntenvlssa[1]=factorsa*instregsa[sy].sustain;
	cntenvlssa[2]=factorsa*instregsa[sy].decay;
	
	if (envlssa[1]!=0) cntenvlsusssa=15;

	if (envlssa[0]!=0 || envlssa[1]!=0 || envlssa[2]!=0) playenvlsa=1;
	
	if (envlssa[0]==0) volumesa=instregsa[sy].volume;
	else volumesa=0;
	
}

/* Produz um Envelope SAA1099*/
void envelopsa(void)
{
	
	/* Attack */
	if (envlssa[0]>0)
	{
		cntenvlssa[0]--;
		if (cntenvlssa[0]==0) 
		{
			cntenvlssa[0]=factorsa*instregsa[sy].attack;
			volumesa++;
			if (volumesa==instregsa[sy].volume)
			{
				envlssa[0]=0;
				if (envlssa[1]==0 && envlssa[2]==0) playenvlsa=0;
			}
		}
	}

	/* Sustain */
	if (envlssa[0]==0 && envlssa[1]>0)
	{
		cntenvlssa[1]--;
		if (cntenvlssa[1]==0)
		{
			cntenvlssa[1]=factorsa*instregsa[sy].sustain;
			cntenvlsusssa--;
			if (cntenvlsusssa==0)
			{
				envlssa[1]=0;
				if (envlssa[2]==0)
				{
					volumesa=0;
					playenvlsa=0;
				}
			}
		}
	}
	
	/* Decay */
	if (envlssa[0]==0 && envlssa[1]==0 && envlssa[2]>0)
	{
		cntenvlssa[2]--;
		if (cntenvlssa[2]==0)
		{
			cntenvlssa[2]=factorsa*instregsa[sy].decay;
			volumesa--;
			if (volumesa==0)
			{
				envlssa[2]=0;
				playenvlsa=0;
			}
		}
	}	
	
	if (playenvlsa==0 && instregsa[sy].repete==1) getenvlsa();
	
}

/* Prepara a criação de um Som SAA1099 */
void precriasomsa(int freqindex)
{
	
	sprintf(txtsts,"Playing %s, Frequencia: %d\0",nnotas[freqindex],notassa[freqindex]);
	printestado();
	
	playenvlsa=0;
	
	getenvlsa();
	
	criasomsa(freqindex);
	
}

/* Apenas atualiza o Volume do Canal SAA1099 em caso de Envelope */
void setvolsa(void)
{
	
	writesa1(0x00,volumesa*16+volumesa);		/* Volume */
	
	writesa1(0x014,instregsa[sy].freqenb); 		/* Frequência Enable */
	writesa1(0x015,instregsa[sy].noiseenb);		/* Noise Enable */
				
}

/* Produz um Som SAA1099 */
void criasomsa(int freqindex)
{
	
	writesa1(0x00,volumesa*16+volumesa);		/* Volume */
	writesa1(0x08,notassa[freqindex]); 			/* Frequência */
	writesa1(0x010,oitavasa+octvssa[freqindex]);  /* Oitava */

		
	writesa1(0x014,instregsa[sy].freqenb); 		/* Frequência Enable */
	writesa1(0x015,instregsa[sy].noiseenb);		/* Noise Enable */
				
}

/* Desliga o Canal SAA1099 */
void stopsomsa(void)
{

	writesa1(0x00,0);		/* Volume  */
	
	playenvlsa=0;
	
	sprintf(txtsts,"Som Desligado\0");
	printestado();
	
}

/* Imprime os Instrumentos SAA1099 e os Valores */
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
	
	/* Instrumentos SAA1099 */
	printinstsa();
	
	/* Valores */
	menuval();
	printval();
			
	alteracab();
	
}

/* Menu dos Valores dos Operadores */
void menuval(void)
{
	
	textbackground(0);
	textcolor(11);
	
	gotoxy(28,5);cprintf("Nome");
	gotoxy(56,5);cprintf("Posicao");
	
	gotoxy(28,7);cprintf("Tom");
	gotoxy(28,8);cprintf("Ruido");
	gotoxy(28,10);cprintf("Attack");
	gotoxy(28,11);cprintf("Sustain");
	gotoxy(28,12);cprintf("Decay");
	
	gotoxy(28,14);cprintf("Volume");
	gotoxy(28,16);cprintf("Repete");
	
	printcursor(tvox[ovox],tvoy[ovoy],tvox[vox],tvoy[voy],2);
	
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
	cprintf("Instrumentos SAA1099");
	
	textcolor(valcolor);
	gotoxy(45,3);cprintf("Valores");
	
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
	gotoxy(12,22);cprintf("%d",oitavasa);
	
}

/* Imprime a Informação dos Instrumentos SAA1099 */
void printinstsa(void)
{
	
	int i;
		
	for (i=py; i<py+16; i++)
	{
		
		gotoxy(5,i-py+5);
		textbackground(2);
		textcolor(15);	
				
		if (i==sy) textbackground(1);
			
		cprintf("%-20s",instregsa[i].nome);
		
	}
	
}

/* Imprime a Informação sobre os Valores */
void printval(void)
{
	
	/* Nome do Instrumento */	
	textbackground(10);
	textcolor(15);
	gotoxy(35,5);cprintf("%-20s",instregsa[sy].nome);

	textbackground(0);
	textcolor(7);
	
	/* Posição do Instrumento no Bank */
	gotoxy(64,5);cprintf("%3d",instregsa[sy].num);
	
	/* Valores */
	gotoxy(44,7);cprintf("%3d",instregsa[sy].freqenb);
	gotoxy(44,8);cprintf("%3d",instregsa[sy].noiseenb);
	
	gotoxy(44,10);cprintf("%3d",instregsa[sy].attack);
	gotoxy(44,11);cprintf("%3d",instregsa[sy].sustain);
	gotoxy(44,12);cprintf("%3d",instregsa[sy].decay);
	
	gotoxy(44,14);cprintf("%3d",instregsa[sy].volume);
	gotoxy(44,16);cprintf("%3d",instregsa[sy].repete);
	
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
		
		gotoxy(5,6);cprintf("ALT - Alterna entre os Instrumentos SAA1099 e os Valores.");
	
		gotoxy(5,8);cprintf("Instrumentos SAA1099");gotoxy(5,9);cprintf("--------------------");
		gotoxy(5,10);cprintf("Cursor Cima/Cursor Baixo seleciona o Instrumento SAA1099.");
		gotoxy(5,11);cprintf("Insert - Insere novo Instrumento SAA1099.");
		gotoxy(5,12);cprintf("Delete - Apaga o Instrumento SAA1099 corrente.");
	
		gotoxy(5,14);cprintf("Valores");gotoxy(5,15);cprintf("-------");
		gotoxy(5,16);cprintf("Cursores navegam nos Valores.");
		gotoxy(5,17);cprintf("CTRL+N para alterar o Nome do Instrumento SAA1099.");
		gotoxy(5,18);cprintf("Enter para terminar alteracao do nome.");
		gotoxy(5,19);cprintf("0-9 para novos valores dos Valores.");
		gotoxy(5,20);cprintf("Enter para terminar alteracao dos Valores.");
	
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
		
		gotoxy(5,19);cprintf("F2 - Le os Instrumentos SAA1099 a partir do Ficheiro SaIntCrt.dat.");
		gotoxy(5,20);cprintf("F3 - Grava os Instrumentos SAA1099 para o Ficheiro SaIntCrt.dat.");
				
		gotoxy(5,23);cprintf("Cursor Esquerda: Pagina Anterior.                  Return: Volta ao Menu.");
	}
	
}

/* Navega nos Instrumentos / Valores / Teclado */
void navega(void)
{
	
	int i;
	
	do
	{
		while (!kbhit())
		{
			if (playenvlsa==1)
			{
				envelopsa();
				setvolsa();
			}
		}
		
		KB_code=getch();
		
		if (KB_code==KB_F1)	/* F1 - Help */
		{
			estado=1;
			help=0;
		}
		
		if (KB_code==KB_F2)	/* F2 - Load */
		{
			i=leinstsa();
			printestado();
			i++;
		}
		
		if (KB_code==KB_F3)	/* F3 - Save */
		{
			i=gravainstsa();
			printestado();
			i++;
		}
		
		if (KB_code==KB_ESCAPE)	estado=10; /* Escape - Sair */

		/* Alterna entre Instrumentos e Valores */
		if (KB_code==KB_TAB) menu=1-menu; /* TAB */
		
		if (menu==0)
		{
			alteracab();
			navegainstsa();
		}
		else
		{
			alteracab();
			navegaval();
		
		}
		
		/* Oitava */
		if (KB_code==KB_MENOS && oitavasa>0)	/* Tecla - */
		{
			oitavasa--;
			printoct();
		}
		
		if (KB_code==KB_MAIS && oitavasa<7)	/* Tecla + */
		{
			oitavasa++;
			printoct();
		}
		
		if (KB_code==KB_INS)	/* INSERT - Insere Instrumento */
			insereinstsa(sy);
			
		if (KB_code==KB_DEL)	/* DELETE - Apaga Instrumento */
			apagainstsa(sy);
		
		/* Teclado Musical */
		if (noed==0)
		{
			switch(KB_code)
			{
				case KB_Z:precriasomsa(0);break;	/* Tecla Z - Dó */
				case KB_S:precriasomsa(1);break;	/* Tecla S - Dó Sustenido */
				case KB_X:precriasomsa(2);break;	/* Tecla X - Ré */
				case KB_D:precriasomsa(3);break;	/* Tecla D - Ré Sustenido */
				case KB_C:precriasomsa(4);break;	/* Tecla C - Mi */
				case KB_V:precriasomsa(5);break;	/* Tecla V - Fá */
				case KB_G:precriasomsa(6);break;	/* Tecla G - Fá Sustenido */
				case KB_B:precriasomsa(7);break;	/* Tecla B - Sol */
				case KB_H:precriasomsa(8);break;	/* Tecla H - Sol Sustenido */
				case KB_N:precriasomsa(9);break;	/* Tecla N - Lá */
				case KB_J:precriasomsa(10);break;	/* Tecla J - Lá Sustenido */
				case KB_M:precriasomsa(11);break;	/* Tecla M - Si */
				case KB_VIRGULA:precriasomsa(12);break;	/* Tecla , - Dó Maior */
				case KB_L:precriasomsa(13);break;	/* Tecla L - Dó Maior Sustenido */
				case KB_O:stopsomsa();break;	/* Desliga o Canal */
			}
		}
		
	//	gotoxy(1,2);
	//	printf("%d ",KB_code);
			
	}
	while (estado==0);
	
}

/* Navega nos Instrumentos SAA1099 */
void navegainstsa(void)
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
	
	printinstsa();
	printval();
				
}

/* Navega nos Valores */
void navegaval(void)
{
	
/*	if (KB_code==KB_LEFT && vox>0)	/* Esquerda 
	{
		if (voed==1) updateval();
		if (noed==1) updatenome();
		vox--;
		printcursor(tvox[ovox],tvoy[ovoy],tvox[vox],tvoy[voy],2);
		ovox=vox;
	};
		
	if (KB_code==KB_RIGHT && vox<1) /* Direita 
	{
		if (voed==1) updateval();
		if (noed==1) updatenome();
		vox++;
		printcursor(tvox[ovox],tvoy[ovoy],tvox[vox],tvoy[voy],2);
		ovox=vox;
	};
*/			
	if (KB_code==KB_UP && voy>0)	/* Cima */
	{
		if (voed==1) updateval();
		if (noed==1) updatenome();
		voy--;
		printcursor(tvox[ovox],tvoy[ovoy],tvox[vox],tvoy[voy],2);
		ovoy=voy;
	};
		
	if (KB_code==KB_DOWN && voy<6)	/* Baixo */
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
		
		memcpy(vron,instregsa[sy].nome,20);
		gotoxy(35,5);cprintf("%-20s",vron);
		
		noc=0;
		while(vron[noc]!=0) noc++;
		
		printcursornome(35+noc,5,35+noc,5,2);
	}

	if (noed==1) inputnome();
	else inputval();

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

/* Edita os Valores */
void inputval(void)
{
	
	cc=0;
	dd=0;
	uu=0;
	keypressed=0;
	
	/* Valores */
	if (KB_code>=KB_0 && KB_code<=KB_9 && voc>0)
	{
		keypressed=1;
		uu=KB_code-48;
		voed=1;
		printcursor(tvox[ovox],tvoy[ovoy],tvox[vox],tvoy[voy],2);
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
	
	if (voy==2 && voi>127) voi=127;
	if (voy==3 && voi>127) voi=127;
	if (voy==4 && voi>127) voi=127;
	
	if (voy==5 && voi>15) voi=15;
	if (voy==6 && voi>1) voi=1;
				
	gotoxy(tvox[vox]+2,tvoy[voy]);cprintf("%3d",voi);
		
	printcursor(tvox[ovox],tvoy[ovoy],tvox[vox],tvoy[voy],2);
	voed=0;

	if (voy==0) instregsa[sy].freqenb=voi;
	if (voy==1) instregsa[sy].noiseenb=voi;
	
	if (voy==2) instregsa[sy].attack=voi;
	if (voy==3) instregsa[sy].sustain=voi;
	if (voy==4) instregsa[sy].decay=voi;
	
	if (voy==5) instregsa[sy].volume=voi;
	if (voy==6) instregsa[sy].repete=voi;
	
			
}

/* Edita o Nome do Instrumento */
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

/* Coloca o Novo Nome no Instrumento */
void updatenome(void)
{
	
	if (noc==0) sprintf(vron,"vazio");
	else vron[noc]='\0';
	
	noed=0;
	noc=0;
	
	memcpy(instregsa[sy].nome,vron,20);

	textbackground(10);
	textcolor(15);
	gotoxy(35,5);
	cprintf("%-20s",vron);

	printinstsa();
	
	textbackground(0);
	textcolor(7);
	
}

/* Grava os Instrumentos SAA1099 para um Ficheiro */
int gravainstsa(void)
{

	FILE *fp;
	
	fp=fopen("SaIntCrt.dat","wb");
	
	if(fp==NULL)
    {
      sprintf(txtsts,"Impossivel Criar Instrumentos SAA1099!\0");
	  return(1);
    }
	
	if (fwrite(instregsa,sizeof(instregsa),1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Gravar Instrumentos SAA1099!\0");
		fclose(fp);
		return(2);
	}

	sprintf(txtsts,"Instrumentos SAA1099 Gravados com Sucesso!\0");

	fclose(fp);

	return(0);
}

/* Lê os Instrumentos SAA1099 a partir de um Ficheiro */
int leinstsa(void)
{

	FILE *fp;
	
	fp=fopen("SaIntCrt.dat","rb");
    
	if(fp==NULL)
    {
      sprintf(txtsts,"Impossivel Abrir Instrumentos SAA1099!\0");
	  return(1);
    }
	
	if (fread(instregsa,sizeof(instregsa),1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Ler Instrumentos SAA1099!\0");
		fclose(fp);
		return(2);
	}
			
	sprintf(txtsts,"Instrumentos SAA1099 Lidos com Sucesso!\0");
	fclose(fp);

	return(0);
	
}

/* Insere Instrumento SAA1099 */
void insereinstsa(int inst)
{
	
	int i;
	
	/* Roda os restantes Instrumentos SAA1099 uma posição para Cima */
	for (i=127; i>inst; i--)
	{
		memcpy(instregsa[i].nome,instregsa[i-1].nome,20);
		instregsa[i].num=i;
		instregsa[i].freqenb=instregsa[i-1].freqenb;
		instregsa[i].noiseenb=instregsa[i-1].noiseenb;
		instregsa[i].attack=instregsa[i-1].attack;
		instregsa[i].sustain=instregsa[i-1].sustain;
		instregsa[i].decay=instregsa[i-1].decay;
		instregsa[i].volume=instregsa[i-1].volume;
		instregsa[i].repete=instregsa[i-1].repete;
	}
	
	/* Insere o Novo Instrumento SAA1099 */
	memset(instregsa[inst].nome,0,20);
	sprintf(instregsa[inst].nome,"vazio\0");
	instregsa[inst].num=inst;
	instregsa[inst].freqenb=0;
	instregsa[inst].noiseenb=0;
	instregsa[inst].attack=0;
	instregsa[inst].sustain=0;
	instregsa[inst].decay=0;
	instregsa[inst].volume=0;
	instregsa[inst].repete=0;
	
	printinstsa();
	printval();
			
	sprintf(txtsts,"Instrumento SAA1099 Criado com Sucesso!\0");
	printestado();
		
}

/* Apaga Instrumento SAA1099 */
void apagainstsa(int inst)
{
	int i;
	
	/* Roda os restantes Instrumentos SAA1099 uma posição para Baixo */
	for (i=inst; i<127; i++)
	{
		memcpy(instregsa[i].nome,instregsa[i+1].nome,20);
		instregsa[i].num=i;
		instregsa[i].freqenb=instregsa[i+1].freqenb;
		instregsa[i].noiseenb=instregsa[i+1].noiseenb;
		instregsa[i].attack=instregsa[i+1].attack;
		instregsa[i].sustain=instregsa[i+1].sustain;
		instregsa[i].decay=instregsa[i+1].decay;
		instregsa[i].volume=instregsa[i+1].volume;
		instregsa[i].repete=instregsa[i+1].repete;
	}
	
	/* "Zera" a Posição 127 */
	memset(instregsa[127].nome,0,20);
	sprintf(instregsa[127].nome,"vazio\0");
	instregsa[127].num=127;
	instregsa[127].freqenb=0;
	instregsa[127].noiseenb=0;
	instregsa[127].attack=0;
	instregsa[127].sustain=0;
	instregsa[127].decay=0;
	instregsa[127].volume=0;
	instregsa[127].repete=0;
	
	printinstsa();
	printval();
			
	sprintf(txtsts,"Instrumento SAA1099 Apagado com Sucesso!\0");
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
		memset(instregsa[i].nome,0,20);
		sprintf(instregsa[i].nome,"vazio\0");
		instregsa[i].num=i;
		instregsa[i].freqenb=instregsa[i+1].freqenb;
		instregsa[i].noiseenb=instregsa[i+1].noiseenb;
		instregsa[i].attack=instregsa[i+1].attack;
		instregsa[i].sustain=instregsa[i+1].sustain;
		instregsa[i].decay=instregsa[i+1].decay;
		instregsa[i].volume=instregsa[i+1].volume;
		instregsa[i].repete=instregsa[i+1].repete;
	}

*/
/*	for (i=9; i<128; i++)
	{
		memset(instregsa[i].nome,0,20);
		sprintf(instregsa[i].nome,"vazio\0");
	}
*/	
	for (i=0; i<128; i++)
	{
		instregsa[i].num=i;
	}

}