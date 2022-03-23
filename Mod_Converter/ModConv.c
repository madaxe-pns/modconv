/*
/*  Mod Converter 1.6
/*	
/*  Converte um ficheiro .Mod para:
/*      AY-3-8912 / YM2149F - ZX Spectrum 128K, Atari ST
/*      Philips SAA1099 : CMS Card/Game Blaster, Sam Coupé
/*      Yamaha YM3812 - OPL2 : Adlib, Sound Blaster, Sound Blaster Pro
/*
/*  Para compilar: tcc -ml ModConv.c
/*
/*  (C) 2022 Penisoft / MadAxe
/*

/* Includes */
#include <stdio.h>
#include <dos.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <alloc.h>
#include <time.h>

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

#define KB_A 97
#define KB_B 98
#define KB_C 99
#define KB_D 100
#define KB_E 101
#define KB_G 103
#define KB_I 105
#define KB_O 111
#define KB_P 112
#define KB_Q 113
#define KB_R 114
#define KB_S 115
#define KB_T 116
#define KB_U 117
#define KB_V 118
#define KB_W 119

#define KB_UP 72
#define KB_DOWN 80
#define KB_LEFT 75
#define KB_RIGHT 77

#define KB_RETURN 13
#define KB_ESCAPE 27
#define KB_TAB 9
#define KB_CTRL_TAB 148
#define KB_DEL 83

#define KB_F1 59
#define KB_F2 60
#define KB_F3 61
#define KB_F4 62
#define KB_F5 63
#define KB_F6 64

#define KB_PAGE_UP 73
#define KB_PAGE_DOWN 81
#define KB_HOME 71
#define KB_END 79
#define KB_CTRL_LEFT 115
#define KB_CTRL_RIGHT 116
#define KB_ALT_LEFT 155
#define KB_ALT_RIGHT 157

#define KB_MAIS 43
#define KB_MENOS 45

/* DSP */
#define SB_ADR		0x220		 /* Porto Base da Placa de Som */
#define DSP_CMD		SB_ADR+0x0C	 /* Porto dos Comandos do DSP  */
#define DSP_DAT		SB_ADR+0x0A  /* Porto dos Dados do DSP     */
#define DSP_RST		SB_ADR+0x06  /* Porto do Reset do DSP      */
#define DSP_BUF		SB_ADR+0x0E  /* Porto do Reset do DSP      */

/* Portos da Placa de Som FM */
#define ADL_REG		0x388	/* Porto dos Registos/Status na Adlib */
#define ADL_DAT		0x389 	/* Porto dos Dados na Adlib           */

/* Registos da Placa de Som FM */
#define FM_MIN_REG 0x01	/* Primeiro Registo */
#define FM_MAX_REG 0xF5	/* Último Registo   */

/* Registos dos Canais FM */
#define FM_CNL_0 0x20	/* Canal 1 */
#define FM_CNL_1 0x21	/* Canal 2 */
#define FM_CNL_2 0x22	/* Canal 3 */
#define FM_CNL_3 0x28	/* Canal 4 */
#define FM_CNL_4 0x29	/* Canal 5 */
#define FM_CNL_5 0x2A	/* Canal 6 */
#define FM_CNL_6 0x30	/* Canal 7 */
#define FM_CNL_7 0x31	/* Canal 8 */
#define FM_CNL_8 0x32	/* Canal 9 */

/* Frequência das Notas FM */
#define FM_V	0	/* Sem Nota */
#define FM_C	262	/* Dó */
#define FM_Cs	278	/* Dó Sustenido */
#define FM_D	294	/* Ré */
#define FM_Ds	312	/* Ré Sustenido */
#define FM_E	330	/* Mi */
#define FM_F	350	/* Fá */
#define FM_Fs	370	/* Fá Sustenido */
#define FM_G	392	/* Sol */
#define FM_Gs	416	/* Sol Sustenido */
#define FM_A	441	/* Lá */
#define FM_As	467	/* Lá Sustenido */
#define FM_B	494	/* Si */

/* Portos da Placa de Som SAA1099 */
#define CMS_REG_1	 0x221	/* Porto dos Registos Canais 1-6  */
#define CMS_DAT_1	 0x220 	/* Porto da Data 	  Canais 1-6  */
#define CMS_REG_7  	 0x223	/* Porto dos Registos Canais 7-12 */
#define CMS_DAT_7  	 0x222 	/* Porto da Data 	  Canais 7-12 */

/* Registos da Placa de Som SAA1099 */
#define SAA_MIN_REG 0x00	/* Primeiro Registo */
#define SAA_MAX_REG 0x1F	/* Último Registo   */

/* Frequência das Notas SAA1099 */
#define SA_V	0	/* Sem Nota */
#define SA_C	83	/* Dó */
#define SA_Cs	107	/* Dó Sustenido */
#define SA_D	130	/* Ré */
#define SA_Ds	151	/* Ré Sustenido */
#define SA_E	172	/* Mi */
#define SA_F	191	/* Fá */
#define SA_Fs	209	/* Fá Sustenido */
#define SA_G	226	/* Sol */
#define SA_Gs	242	/* Sol Sustenido */
#define SA_A	3	/* Lá */
#define SA_As	31	/* Lá Sustenido */
#define SA_B	58	/* Si */

#define SIZEPATINFOMOD 1024	/* Tamanho da Pattern MOD */
#define SIZEROWMOD 16		/* Número de Bytes das Rows MOD */

#define FMNUNCANAIS 9	/* Número de Canais FM */
#define SAANUNCANAIS 6	/* Número de Canais SAA1099 */
#define AYNUNCANAIS 3	/* Número de Canais AY-3-8912 */

#define SIZEPATINFOFM 1152	/* Tamanho da Pattern FM */
#define SIZEPATINFOSA 768	/* Tamanho da Pattern SAA1099 */
#define SIZEPATINFOAY 256	/* Tamanho da Pattern AY-3-8912 */

#define SIZEROWFM 18	/* Número de Bytes das Rows FM */
#define SIZEROWSA 12	/* Número de Bytes das Rows SAA1099 */
#define SIZEROWAY 4		/* Número de Bytes das Rows AY-3-8912 */

/* Prototypes */
void screenopen(void);
void screenclose(void);
void libertamem(void);

int inicializa(void);
void mainmenu(void);

void printmod(void);
void printestado(void);
void printocttxt(void);
void printoctfm(void);
void printoctsa(void);
void printoctay(void);
void printvolfm(void);
void printvolsa(void);
void printvolay(void);

void alteracabmod(void);
void carregapat(void);
void printcab(void);
void printpat(void);
void printsam(void);
void printsaminfo(void);

int lemodulo(void);
void ledados(void);

int leinstfm(void);
int lestsfm(void);
int gravastsfm(void);
int gravamusicafm(void);

int leinstsa(void);
int lestssa(void);
int gravastssa(void);
int gravamusicasa(void);

int leinstay(void);
int lestsay(void);
int leayplayer(void);
int gravastsay(void);
int gravamusicaay(void);
int gravaayplayer(void);

void navegamod(void);
void navegapat(void);
void navegasam(void);

void alteracabconv(void);
void alteracabpanel(void);
void printhlpinst(void);
void printhlpcnl(void);

void resetsaminstfm();
void resetcnlmodcnlfm();

void printfm(void);
void printinstfm(void);
void printsaminstfmtxt(void);
void printsaminstfm(void);
void printcnlmod(void);
void printcnlfm(void);

void navegafm(void);
void navegainstfm(void);
void navegacnlmod(void);
void navegacnlfm(void);

void convertmodfm(void);
void convfrntfm(int freq);

void printpatconvfm(void);
void alteracabconvfm(void);
void carregapatfm(void);
void printcnlonfm(void);
void printpatfm(void);
void navegapatfm(void);

void playmusicafm(void);

void resetsaminstsa();
void resetcnlmodcnlsa();

void printsa(void);
void printinstsa(void);
void printsaminstsatxt(void);
void printsaminstsa(void);
void printcnlsa(void);

void navegasa(void);
void navegainstsa(void);
void navegacnlsa(void);

void convertmodsa(void);
void convfrntsa(int freq);

void printpatconvsa(void);
void alteracabconvsa(void);
void carregapatsa(void);
void printcnlonsa(void);
void printpatsa(void);
void navegapatsa(void);

void playmusicasa(void);

void resetsaminstay();
void resetcnlmodcnlay();

void printay(void);
void printinstay(void);
void printsaminstaytxt(void);
void printsaminstay(void);
void printcnlay(void);

void navegaay(void);
void navegainstay(void);
void navegacnlay(void);

void convertmoday(void);
void convfrntay(int freq);

void printpatconvay(void);
void alteracabconvay(void);
void carregapatay(void);
void printcnlonay(void);
void printpatay(void);
void navegapatay(void);

void playmusicaay(void);

char *convfrnt(int freq);
char *hexa4(int deca);
char *hexa8(int deca);

/* DSP */
int resetdsp(void);
void writedsp(int value);
int readdsp(void);
void playsom(void);

/* FM */
void resetfm(void);
void initfm(void);
void writefm(int reg, int value);
int readfm(void);
void criasomfm(int canalg,int canalo,int freqindex);
void stopsomfm(int canalg);

/* SAA1099 */
void resetsa(void);
void writesa1(int reg, int value);
void writesa7(int reg, int value);
void getenvlsa(unsigned char canal);
void envelopsa(unsigned char canal);
void precriasomsa(unsigned char canal,int freqindex);
void setvolsa(unsigned char canal);
void criasomsa(unsigned char canal,int freqindex);
void stopsomsa(int canal);

/* AY-3-8912 */
void getenvlay(unsigned char canal);
void envelopay(unsigned char canal);
void precriasomay(unsigned char canal,int freqindex);
void setvolay(unsigned char canal);
void criasomay(unsigned char canal,int freqindex);
void stopsomay(int canal);

/* Manipulação de Bits */
unsigned char setbit(unsigned char n,unsigned char k);
unsigned char clearbit(unsigned char n,unsigned char k);
unsigned char togglebit(unsigned char n,unsigned char k);

/* Tempo */
unsigned long letempo(void);

/* Extensão dos Ficheiros */
void splitfiles(void);

/* Help */
void printhelp(void);
void navegahelp(void);

/* Variáveis */
/* Teclado */
int KB_code=0;

/* Ficheiros */
FILE *fp;
char nomefilemod[13];//="helico.mod\0";
char nomefilefm[13],nomefilestsfm[13];
char nomefilesa[13],nomefilestssa[13];
char nomefileay[13],nomefilestsay[13];
char nomefileayplr[13];
char nomefileinstfm[13]="FmIntCrt.dat\0";
char nomefileinstsa[13]="SaIntCrt.dat\0";
char nomefileinstay[13]="AYIntCrt.dat\0";
char nomefileayplrdat[13]="ayplayer.dat\0";;

/* Cabeçalho */
unsigned char *fpdata;

/* Nome do Módulo*/
unsigned char offs_nome=0;
unsigned char size_nome=20;
char nome[20];

/* Informação dos Samples */
unsigned char offs_samples=20;
unsigned char size_nomesample=22;
unsigned char size_nextsample=30;
char nomesample[31][22];
unsigned char lensample[31][2];
unsigned char ftsample[31];
unsigned char volsample[31];
unsigned char rpstsample[31][2];
unsigned char rplnsample[31][2];
unsigned char tisample[31];
	
/* Número de Posições */
int offs_songlen=950;
unsigned char songlen;

/* Número Máximo de Posições Possível */
int offs_maxpatpos=951;
unsigned char maxpatpos;
	
/* Posição das Patterns */
int offs_songpos=952;
unsigned char songpos[128];
unsigned char maxpat;
	
/* Número Mágico: M.K. */
int offs_magicnum=1080;
unsigned char size_magicnum=4;
char magicnum[5];
	
/* Informação das Patterns */
int offs_patterns=1084;
unsigned char *patinfo;

/* Informação das Rows */
unsigned char hi,lo;
unsigned char samplenum[64][4];
int period[64][4];
unsigned char efectnum[64][4];
unsigned char efectpar[64][4];
char nota[64][4][4];

/* Tabela Patterns */
short py;
short position;
short pattern;
short patcolor;
short row;

/* Tabela Samples */
short sy,ny;
short samcolor;
short samx,samy,sami;

/* Samples */
struct samdata
{
	unsigned long size;
	unsigned char *data;
} saminfo[31];


/* Help */
short help;

/* DSP */
int numsamoff;
int numsamples;

/* Menus e Estado */
int menu,menup;
int estado,oldestado,oldestadohlp;

/* Painel Samples/Canais Menu Conversão */
short pcnlcolor,pcnlbkcolor;
short psamcolor,psambkcolor;
short pinstcolor,pinstbkcolor;
short pcnlmodcolor,pcnlmodbkcolor;
short pcnlconvcolor,pcnlconvbkcolor;

/* Tabela Instrumentos FM */
short sifm,pifm;

/* Tabela Canais Módulo */
short cnlmody;

/* Tabela Canais FM */
short cnlfmy;

/* Conversão Samples -> Instrumentos FM */
unsigned char saminstfm[2][31];
char octinstfm[31];
unsigned char volinstfm[31];

/* Conversão Canais MOD -> Canais FM */
unsigned char cnlmodcnlfm[FMNUNCANAIS];

/* Patterns FM */
unsigned char oitavafm;
unsigned char freqfm;
unsigned char volfm;
unsigned char *patinfofm;

/* Informação das Rows FM*/
unsigned char fmefectnum[64][FMNUNCANAIS];
unsigned char fmfreq[64][FMNUNCANAIS];
unsigned char fmoct[64][FMNUNCANAIS];
unsigned char fmefectpar[64][FMNUNCANAIS];
unsigned char fmsamplenum[64][FMNUNCANAIS];

/* Toca Música FM */
int playfm;
int fimsongfm;
clock_t playfmstart,playfmend;

/* Tabela Instrumentos SAA1099 */
short sisa,pisa;

/* Tabela Canais SAA1099 */
short cnlsay;

/* Conversão Samples -> Instrumentos SAA1099 */
unsigned char saminstsa[2][31];
char octinstsa[31];
unsigned char volinstsa[31];

/* Conversão Canais MOD -> Canais SAA1099 */
unsigned char cnlmodcnlsa[SAANUNCANAIS];

/* Patterns SAA1099 */
unsigned char oitavasacnv;
unsigned char freqsacnv;
unsigned char oitavasa[SAANUNCANAIS];
unsigned char canalinstsa[SAANUNCANAIS];
unsigned char freqsa[SAANUNCANAIS];
unsigned char volumesa[SAANUNCANAIS];
unsigned char *patinfosa;
unsigned char freqenbsa=0;
unsigned char noiseenbsa=0;
int factorsa=3;
unsigned char playenvlsa[SAANUNCANAIS];
unsigned char envlssa[SAANUNCANAIS][3];
unsigned short cntenvlssa[SAANUNCANAIS][3];
unsigned short cntenvlsusssa[SAANUNCANAIS];

/* Informação das Rows SAA1099*/
unsigned char saefectnum[64][SAANUNCANAIS];
unsigned char safreq[64][SAANUNCANAIS];
unsigned char saoct[64][SAANUNCANAIS];
unsigned char saefectpar[64][SAANUNCANAIS];
unsigned char sasamplenum[64][SAANUNCANAIS];

/* Toca Música SAA1099 */
int playsa;
int fimsongsa;
unsigned long playsastart,playsaend;

/* Tabela Instrumentos AY-3-8912 */
short siay,piay;

/* Tabela Canais AY-3-8912 */
short cnlayy;

/* Conversão Samples -> Instrumentos AY-3-8912 */
unsigned char saminstay[2][31];
char octinstay[31];
unsigned char volinstay[31];

/* Conversão Canais MOD -> Canais AY-3-8912 */
unsigned char cnlmodcnlay[AYNUNCANAIS];

/* Patterns AY-3-8912 */
unsigned char oitavaaycnv;
unsigned char freqaycnv;
unsigned char oitavaay[AYNUNCANAIS];
unsigned char freqay[AYNUNCANAIS];
unsigned char volumeay[AYNUNCANAIS];
unsigned char *patinfoay;
unsigned char freqenbay=0;
unsigned char noiseenbay=0;
int factoray=3;
unsigned char playenvlay[AYNUNCANAIS];
unsigned char envlsay[AYNUNCANAIS][3];
unsigned short cntenvlsay[AYNUNCANAIS][3];
unsigned short cntenvlsussay[AYNUNCANAIS];

/* Informação das Rows AY-3-8912 */
unsigned char ayefectnum[64][AYNUNCANAIS];
unsigned char ayfreq[64][AYNUNCANAIS];
unsigned char ayoct[64][AYNUNCANAIS];
unsigned char ayefectpar[64][AYNUNCANAIS];
unsigned char aysamplenum[64][AYNUNCANAIS];

/* Toca Música AY-3-8912 */
int playay;
int fimsongay;
unsigned long playaystart,playayend;

/* Time */
struct time tempo;

//struct time playstart,playend;

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
} instreg[128],saveinstregfm[31];

/* Definição do Cabeçalho da Música FM */
struct cabfilesfm
{
	char nome[20];
	unsigned char songlen;
	unsigned char maxpat;
} cabfilefm;

/* Definição dos Registos para cada Instrumento SAA1099 */
struct instregsas
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
} instregsa[128],saveinstregsa[31];

/* Definição do Cabeçalho da Música SAA1099 */
struct cabfilessa
{
	char nome[20];
	unsigned char songlen;
	unsigned char maxpat;
} cabfilesa;

/* Definição dos Registos para cada Instrumento AY-3-8912 */
struct instregays
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
} instregay[128];

/* Definição dos Registos para cada Instrumento AY-3-8912 para Gravação da Música */
struct saveinstregsay
{
	unsigned char num;
	unsigned char freqenb;
	unsigned char noiseenb;
	unsigned char attack;
	unsigned char sustain;
	unsigned char decay;
	unsigned char volume;
	unsigned char repete;
} saveinstregay[31];

/* Definição do Cabeçalho da Música AY-3-8912 */
struct cabfilesay
{
	char nome[20];
	unsigned char songlen;
	unsigned char maxpat;
} cabfileay;

/* FM */
short notasfm[13]={FM_V,FM_C,FM_Cs,FM_D,FM_Ds,FM_E,FM_F,FM_Fs,FM_G,FM_Gs,FM_A,FM_As,FM_B};
short canaisfm[FMNUNCANAIS]={FM_CNL_0,FM_CNL_1,FM_CNL_2,FM_CNL_3,FM_CNL_4,FM_CNL_5,FM_CNL_6,FM_CNL_7,FM_CNL_8};
short cnlonfm[FMNUNCANAIS]={1,1,1,1,1,1,1,1,1};

/* SAA1099 */
short notassa[13]={SA_V,SA_C,SA_Cs,SA_D,SA_Ds,SA_E,SA_F,SA_Fs,SA_G,SA_Gs,SA_A,SA_As,SA_B};
short cnlonsa[SAANUNCANAIS]={1,1,1,1,1,1};
unsigned octsa[2][3]={0,0,0,0,0,0};

/* AY-3-8912 */
short notasay[13]={SA_V,SA_C,SA_Cs,SA_D,SA_Ds,SA_E,SA_F,SA_Fs,SA_G,SA_Gs,SA_A,SA_As,SA_B};
short cnlonay[AYNUNCANAIS]={1,1,1};
unsigned octay[2][3]={0,0,0,0,0,0};

char pconvnotas[13][3]={"--\0","C \0","C#\0","D \0","D#\0",
					"E \0","F \0","F#\0","G \0","G#\0",
					"A \0","A#\0","B \0"};
					
unsigned char ayplayer[3606];
unsigned char aycablen[2];
unsigned char aycab[18];
unsigned char aycabchks[1];
unsigned char aymuslen[3];
unsigned char *aymus;
unsigned char aymuschks[1];

/* Estado */
char txtsts[79];


/* Main */
int main(int argc, char *argv[])
{
	
	if(argc==2)
		sprintf(nomefilemod,argv[1]);
	else
	{
		clrscr();
		textcolor(11);
		cprintf("Mod Converter 1.6\r\n");
		textcolor(10);
		cprintf("(C) 2022 Penisoft / MadAxe\r\n");
		textcolor(15);
		cprintf("\nUtilizacao: ModConv NomeMusica.mod\n");
		return(4);
	}
	
	if (inicializa()!=0)
	{
		libertamem();
		return(1);
	}

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
	
	libertamem();

	resetfm();
	resetsa();
	
	textbackground(0);
	textcolor(7);
	clrscr();	
	
	textcolor(11);
	cprintf("Mod Converter 1.6\r\n");
	textcolor(10);
	cprintf("(C) 2022 Penisoft / MadAxe\r\n");
	
	_setcursortype(_NORMALCURSOR);
	
//	for (i=0; i<16; i++) printf("%d-%d    %d-%d\n",i,saminstay[0][i],i+15,saminstay[0][i+15]);
//	for (i=0; i<16; i++) printf("%d-%d    %d-%d\n",i,saminstfm[0][i],i+15,saminstfm[0][i+15]);
//	for (i=0; i<16; i++) printf("%d-%d    %d-%d\n",i,saminstsa[0][i],i+15,saminstsa[0][i+15]);
	
}

/* Liberta a Memória Alocada */
void libertamem(void)
{
	
	int i;

	/* Liberta a Memória Alocada */
	free(patinfoay);
	free(patinfosa);
	free(patinfofm);
	free(patinfo);
	
	for (i=0; i<31; i++)
		if (tisample[i]) free(saminfo[i].data);
	
}

/* Inicializa o Programa */
int inicializa(void)
{
	
	int f,i;
	
	textbackground(0);
	textcolor(7);
	clrscr();
	
	textcolor(11);
	cprintf("Mod Converter 1.6\r\n");
	textcolor(10);
	cprintf("(C) 2022 Penisoft / MadAxe\r\n");
	cprintf("\r\n");
	
	textcolor(15);
	
	/* Reset Som FM */
	resetfm();
	initfm();
	
	/* Reset Som SAA1099 */
	resetsa();

	/* Reset DSP */
	if (resetdsp()!=0)
	{
		cprintf("%s\r\n",txtsts);
		return(3);
	}

	/* Lê Módulo */
	f=lemodulo();
	if (f!=0)
	{
		cprintf("%s\r\n",txtsts);
		return(f);
	}
	
	/* Lê Instrumentos FM */
	f=leinstfm();
	if (f!=0)
	{
		cprintf("%s\r\n",txtsts);
		return(f);
	}
	
	/* Lê Instrumentos SAA1099 */
	f=leinstsa();
	if (f!=0)
	{
		cprintf("%s\r\n",txtsts);
		return(f);
	}
	
	/* Lê Instrumentos AY-3-8912 */
	f=leinstay();
	if (f!=0)
	{
		cprintf("%s\r\n",txtsts);
		return(f);
	}
	
	/* Lê Leitor AY-3-8912 (ayplayer.dat) */
	f=leayplayer();
	if (f!=0)
	{
		cprintf("%s\r\n",txtsts);
		return(f);
	}
	
	/* Converte as Extensões dos Ficheiros */
	splitfiles();

	/* Menu */
	menu=0;
	menup=0;
	estado=0;
	oldestado=0;
		
	/* Help */
	help=0;
	oldestadohlp=0;
	
	/* Navegação nas Patterns */
	position=0;
	py=0;
	patcolor=14;
	
	/* Navegação nos Samples */
	sy=0;
	ny=0;
	samcolor=7;
	
	/* Navegação no Painel Samples/Canais no Menu de Conversão */
	pcnlcolor=7;
	psamcolor=14;
	pinstcolor=7;
	pcnlmodcolor=14;
	pcnlconvcolor=7;
	
	/* Navegação nos Instrumentos FM */
	sifm=0;
	pifm=0;
		
	/* Navegação nos Canais do Módulo */
	cnlmody=4;
		
	/* Navegação nos Canais FM */
	cnlfmy=0;
	
	/* Conversão Samples -> Instrumentos FM */
	resetsaminstfm();
	
	/* Conversão Canais MOD -> Canais FM */
	resetcnlmodcnlfm();
	
	/* Navegação nos Instrumentos SAA1099 */
	sisa=0;
	pisa=0;
		
	/* Navegação nos Canais SAA1099 */
	cnlsay=0;
	
	/* Conversão Samples -> Instrumentos SAA1099 */
	resetsaminstsa();
	
	/* Conversão Canais MOD -> Canais SAA1099 */
	resetcnlmodcnlsa();
	
	/* Navegação nos Instrumentos AY-3-8912 */
	siay=0;
	piay=0;
		
	/* Navegação nos Canais AY-3-8912 */
	cnlsay=0;
	
	/* Conversão Samples -> Instrumentos AY-3-8912 */
	resetsaminstay();
	
	/* Conversão Canais MOD -> Canais AY-3-8912 */
	resetcnlmodcnlay();

	return(0);
	
}

/* Main Menu */
void mainmenu(void)
{

	do
	{
		switch(estado)
		{
			case 0:				/* Módulo */
			{
				printmod();
				navegamod();
				break;
			}
			case 1:				/* FM */
			{
				printfm();
				navegafm();
				break;
			}
			case 2:				/* SAA1099 */
			{
				printsa();
				navegasa();
				break;
			}
			case 3:				/* AY-3-8912 */
			{
				printay();
				navegaay();
				break;
			}
			case 4:				/* Patterns Conv */
			{
				switch(oldestado)
				{
					case 1:		/* Patterns FM */
					{
						printpatconvfm();
						navegapatfm();
						break;
					}
					case 2:		/* Patterns SAA1099 */
					{
						printpatconvsa();
						navegapatsa();
						break;
					}
					case 3:		/* Patterns AY-3-8912 */
					{
						printpatconvay();
						navegapatay();
						break;
					}
				}
				break;
			}
			case 5:
			{
				printhelp();
				navegahelp();
				break;
			}
		}
	}
	while (estado!=10);
	
}

/* Menu do Módulo */
void printmod(void)
{
	
	int i;
	
	textbackground(0);
	textcolor(7);
	clrscr();
	
	/* Menu */
	textbackground(1);
	textcolor(15);
	gotoxy(1,1);
	cprintf(" F1-Help  F2-Modulo  F3-FM  F4-SAA1099  F5-AY/YM  F6-Play Conv         ESC-Sair ");
	gotoxy(1,25);
	cprintf("                                                                               ");
		
	/* Cabeçalho */
	textbackground(0);
	textcolor(10);
	gotoxy(1,3); cprintf("Pagina do Modulo");
	textcolor(3);
	gotoxy(1,4); cprintf("Nome:");
	gotoxy(1,5); cprintf("Posicoes:");
	gotoxy(1,6); cprintf("Posicoes Possiveis:");
	gotoxy(1,7); cprintf("Patterns:");
	gotoxy(1,8); cprintf("Numero Magico:");
	
	/* Samples */
	textcolor(11);
	gotoxy(55,19);cprintf("Tamanho:");
	gotoxy(55,20);cprintf("FineTune:");
	gotoxy(55,21);cprintf("Volume:");
	gotoxy(55,22);cprintf("Repeat Start:");
	gotoxy(55,23);cprintf("Repeat Length:");
	
	/* Menu */
	menu=0;
	
	/* Navegação nas Patterns */
	position=0;
	py=0;
	
	/* Navegação nos Samples */
	sy=0;
	ny=0;
	samx=55;
	samy=5;
	sami=12;
	
	/* Cabeçalho */
	printcab();
	alteracabmod();
			
	/* Patterns */
	carregapat();
	printpat();
	
	/* Samples */
	printsam();
	
	/* Estado */
	sprintf(txtsts,"Todos os Ficheiros Lidos com Sucesso!\0");
	printestado();
	
}

/* Imprime o Estado */
void printestado(void)
{
	
	textbackground(1);
	textcolor(15);
	gotoxy(1,25);cprintf("%-79s",txtsts);
	
	textbackground(0);
		
}

/* Imprime o Texto da Oitava e Volume */
void printocttxt(void)
{
	textcolor(11);
	gotoxy(54,7);cprintf("Correcao Oitava");
	gotoxy(54,10);cprintf("Afinacao Volume");
	
	textcolor(15);
	gotoxy(59,6);

}

/* Imprime a Oitava FM */
void printoctfm(void)
{

	textbackground(1);
	textcolor(15);
	gotoxy(59,8);
	if (saminstfm[0][ny]==128) cprintf(" -- ");
	else
		cprintf(" %2d ",octinstfm[ny]);
	textbackground(0);
	
}

/* Imprime a Oitava SAA1099 */
void printoctsa(void)
{

	textbackground(1);
	textcolor(15);
	gotoxy(59,8);
	if (saminstsa[0][ny]==128) cprintf(" -- ");
	else
		cprintf(" %2d ",octinstsa[ny]);
	textbackground(0);
	
}

/* Imprime a Oitava AY-3-8912 */
void printoctay(void)
{

	textbackground(1);
	textcolor(15);
	gotoxy(59,8);
	if (saminstay[0][ny]==128) cprintf(" -- ");
	else
		cprintf(" %2d ",octinstay[ny]);
	textbackground(0);
	
}

/* Imprime o Volume FM */
void printvolfm(void)
{

	textbackground(1);
	textcolor(15);
	gotoxy(59,11);
	if (saminstfm[0][ny]==128) cprintf(" -- ");
	else
		cprintf(" %2d ",63-volinstfm[ny]);
	textbackground(0);
	
}

/* Imprime o Volume SAA1099 */
void printvolsa(void)
{

	textbackground(1);
	textcolor(15);
	gotoxy(59,11);
	if (saminstsa[0][ny]==128) cprintf(" -- ");
	else
		cprintf(" %2d ",volinstsa[ny]);
	textbackground(0);
	
}

/* Imprime o Volume AY-3-8912 */
void printvolay(void)
{

	textbackground(1);
	textcolor(15);
	gotoxy(59,11);
	if (saminstay[0][ny]==128) cprintf(" -- ");
	else
		cprintf(" %2d ",volinstay[ny]);
	textbackground(0);
	
}

/* Altera as Cores do Painel Patterns/Samples do Módulo */
void alteracabmod(void)
{
	
	textbackground(0);
	
	if (menu==0)
	{
		patcolor=14;
		samcolor=15;
	}
	else
	{
		patcolor=15;
		samcolor=14;	
	}
		
	textcolor(patcolor);
	gotoxy(1,10);
	cprintf("Position - %d , Pattern - %d  \n\n",position,pattern);
	
	textcolor(samcolor);
	gotoxy(55,3);
	cprintf("Samples");

	textcolor(7);
	
}

/* Lê a Informação da Pattern corrente */
void carregapat(void)
{

	int i,j;
	int x,y;
	
	/* Vai buscar a Pattern correspondente a respetiva Position */
	pattern=songpos[position];
	py=0;
	
	textcolor(patcolor);
	gotoxy(1,10);
	cprintf("Position - %d , Pattern - %d  \n\n",position,pattern);

	/* Percorre todas as Rows */
	y=0;
	for (i=pattern*SIZEPATINFOMOD; i<((pattern*SIZEPATINFOMOD)+SIZEPATINFOMOD); i+=SIZEROWMOD)
	{
		/* Percorre os 4 Canais */
		x=0;
		for(j=0; j<SIZEROWMOD; j+=4)
		{
			hi=(int)(patinfo[i+j+0]/16);
			lo=(int)(patinfo[i+j+0]-hi*16);
	
			period[y][x]=lo*256+patinfo[i+j+1];			/* Frequencia */
			stpcpy(nota[y][x],convfrnt(period[y][x]));
		
			lo=(int)(patinfo[i+j+2]/16);				/* Sample */
			samplenum[y][x]=hi*16+lo;
	
			efectnum[y][x]=(int)(patinfo[i+j+2]-lo*16);	/* Número do Efeito */
			efectpar[y][x]=patinfo[i+j+3];				/* Parâmetro do Efeito */
			
			x++;
		}
		y++;
	}
		
}

/* Imprime o Cabeçalho do Módulo */
void printcab(void)
{

	textbackground(0);
	textcolor(15);
	gotoxy(7,4); cprintf("%s",nome);
	gotoxy(11,5); cprintf("%d",songlen);
	gotoxy(21,6); cprintf("%d",maxpatpos);
	gotoxy(11,7); cprintf("%d",maxpat);
	gotoxy(16,8); cprintf("%s",magicnum);
	
}

/* Imprime a Informação da Pattern corrente */
void printpat(void)
{
	
	int i;
	
	textbackground(0);
	textcolor(7);
	gotoxy(1,12);
	for (i=py; i<py+12; i++)
	{
		cprintf("%2d: ",i);
		cprintf("%3s %2d %s%s | ",nota[i][0],samplenum[i][0],hexa4(efectnum[i][0]),hexa8(efectpar[i][0]));
		cprintf("%3s %2d %s%s | ",nota[i][1],samplenum[i][1],hexa4(efectnum[i][1]),hexa8(efectpar[i][1]));
		cprintf("%3s %2d %s%s | ",nota[i][2],samplenum[i][2],hexa4(efectnum[i][2]),hexa8(efectpar[i][2]));
		cprintf("%3s %2d %s%s ",nota[i][3],samplenum[i][3],hexa4(efectnum[i][3]),hexa8(efectpar[i][3]));
	
		cprintf("\r\n");
	}
	
}

/* Imprime a Informação dos Samples */
void printsam()
{
	
	int i;
		
	for (i=sy; i<sy+sami; i++)
	{
		
		gotoxy(samx,i-sy+samy);
		
		if (tisample[i]==1) textcolor(15);
			
		cprintf("%2d:",i+1);
		
		textcolor(7);
		
		if (i==ny)
		{
			textbackground(7);
			textcolor(0);		
		}
	
		cprintf("%-22s",nomesample[i]);
		
		textbackground(0);
		textcolor(7);
	}
	
	/* Imprime Informação sobre o Sample Selecionado */
	if (estado==0) printsaminfo();
	
}

/* Imprime Informação sobre o Sample Selecionado */
void printsaminfo(void)
{
	
	unsigned long rss,rls;
	
	rss=(rpstsample[ny][0]*256+rpstsample[ny][1])*2;
	rls=(rplnsample[ny][0]*256+rplnsample[ny][1])*2;
	
	textcolor(15);
	gotoxy(55,18);cprintf("%-22s",nomesample[ny]);
	textcolor(7);
	gotoxy(64,19);cprintf("%-6u",saminfo[ny].size); 
	gotoxy(65,20);cprintf("%-2d",ftsample[ny]);
	gotoxy(63,21);cprintf("%-2d",volsample[ny]);
	gotoxy(69,22);cprintf("%-6u",rss);
	gotoxy(70,23);cprintf("%-6u",rls);
	
}

/* Lê o Módulo a partir de um Ficheiro */
int lemodulo(void)
{

	int i;
	
	/* Abre o Ficheiro para Leitura Binária */
	fp=fopen(nomefilemod,"rb");
    if(fp==NULL)
    {
		sprintf(txtsts,"Impossivel Abrir Modulo!\0");
		return(1);
    }
			
	/* Vai para o princípio do Ficheiro */
	fseek(fp,0,SEEK_SET); 
		
	/* Aloca Memória para o Cabeçalho */
	fpdata=(char*)calloc(1084,sizeof(char));
	if (fpdata==NULL)
	{
		sprintf(txtsts,"Erro ao Alocar Memoria - Cabecalho!\0");
		fclose(fp);
		return(3);
	}
		
	/* Lê o Cabeçalho do Ficheiro */
	if (fread(fpdata,1084,1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Ler Modulo - Cabecalho!\0");
		fclose(fp);
		free(fpdata);
		return(2);
	}
		
	ledados();
		
	free(fpdata);
		
	/* Aloca Memória para as Patterns */
	patinfo=(char*)calloc(SIZEPATINFOMOD*maxpat,sizeof(char));
	if (patinfo==NULL)
	{
		sprintf(txtsts,"Erro ao Alocar Memoria - Patterns!\0");
		fclose(fp);
		return(3);
	}
	
	/* Aloca Memória para as Patterns FM */
	patinfofm=(char*)calloc(SIZEPATINFOFM*maxpat,sizeof(char));
	if (patinfofm==NULL)
	{
		sprintf(txtsts,"Erro ao Alocar Memoria - Patterns FM!\0");
		fclose(fp);
		return(3);
	}
	
	/* Aloca Memória para as Patterns SAA1099 */
	patinfosa=(char*)calloc(SIZEPATINFOSA*maxpat,sizeof(char));
	if (patinfosa==NULL)
	{
		sprintf(txtsts,"Erro ao Alocar Memoria - Patterns SAA1099!\0");
		fclose(fp);
		return(3);
	}
	
	/* Aloca Memória para as Patterns AY-3-8912 */
	patinfoay=(char*)calloc(SIZEPATINFOAY*maxpat,sizeof(char));
	if (patinfoay==NULL)
	{
		sprintf(txtsts,"Erro ao Alocar Memoria - Patterns AY-3-8912!\0");
		fclose(fp);
		return(3);
	}
		
	/* Lê as Patterns do Ficheiro */
	if (fread(patinfo,SIZEPATINFOMOD*maxpat,1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Ler Modulo - Patterns!\0");
		fclose(fp);
		return(2);
	}
	
	/* Aloca Memória para os Samples */
	for(i=0; i<31; i++)
		if (tisample[i])
		{	
			saminfo[i].data=(char*)calloc(saminfo[i].size,sizeof(char));
			if (saminfo[i].data==NULL)
			{
				sprintf(txtsts,"Erro ao Alocar Memoria - Sample: %d\0",i);
				fclose(fp);
				return(3);
			}
		}
		
	/* Lê os Samples do Ficheiro */
	for(i=0; i<31; i++)
		if (tisample[i])
			if (fread(saminfo[i].data,1,saminfo[i].size,fp)==0)
			{
				sprintf(txtsts,"Erro ao Ler Modulo - Sample: %d!\0",i);
				fclose(fp);
				return(2);
			}
		
	sprintf(txtsts,"Modulo Lido com Sucesso!\0");
	
	fclose(fp);
		
	return(0);
		
}

/* Lê os Dados do Módulo para as respetivas variáveis */
void ledados(void)
{

	int i,j;
	
	/* Lê o Nome do Módulo*/
	for (i=0; i<size_nome; i++)
		nome[i]=fpdata[offs_nome+i];
	
	/* Lê a Informação dos Samples */
	for(i=0; i<31; i++)
	{
		/* Nome do Sample */
		for (j=0; j<size_nomesample; j++)
			nomesample[i][j]=fpdata[offs_samples+(size_nextsample*i)+j];
		
		/* Tamanho do Sample */
		lensample[i][0]=fpdata[offs_samples+(size_nextsample*i)+22];
		lensample[i][1]=fpdata[offs_samples+(size_nextsample*i)+23];
		saminfo[i].size=(lensample[i][0]*256+lensample[i][1])*2;
		
		if (!saminfo[i].size) tisample[i]=0;
			else tisample[i]=1;
		
		/* FineTune do Sample */
		ftsample[i]=fpdata[offs_samples+(size_nextsample*i)+24];
		
		/* Volume do Sample */
		volsample[i]=fpdata[offs_samples+(size_nextsample*i)+25];
		
		/* Repeat Start do Sample */
		rpstsample[i][0]=fpdata[offs_samples+(size_nextsample*i)+26];
		rpstsample[i][1]=fpdata[offs_samples+(size_nextsample*i)+27];
		
		/* Repeat Length do Sample */
		rplnsample[i][0]=fpdata[offs_samples+(size_nextsample*i)+28];
		rplnsample[i][1]=fpdata[offs_samples+(size_nextsample*i)+29];
	}
	
	/* Lê o Número de Posições */
	songlen=fpdata[offs_songlen];

	/* Lê a Posição das Patterns */
	maxpat=0;
	for (i=0; i<songlen; i++)
	{
		songpos[i]=fpdata[offs_songpos+i];
		if (songpos[i]>maxpat)
			maxpat=songpos[i]; /* Saca o Número de Patterns */
	}
	maxpat++;
	
	/* Lê o Número Máximo de Posições Possível */
	maxpatpos=fpdata[offs_maxpatpos];
	
	/* Lê o Número Mágico: M.K. */
	for (i=0; i<size_magicnum; i++)
		magicnum[i]=fpdata[offs_magicnum+i];
	magicnum[4]='\0';
	
}

/* Lê os Instrumentos FM a partir de um Ficheiro */
int leinstfm(void)
{

	FILE *fp;
	
	fp=fopen(nomefileinstfm,"rb");
    
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

/* Lê o Estado FM a partir de um Ficheiro */
int lestsfm(void)
{

	FILE *fp;
	
	fp=fopen(nomefilestsfm,"rb");
    
	if(fp==NULL)
    {
      sprintf(txtsts,"Impossivel Abrir Estado FM!\0");
	  return(1);
    }
	
	/* Informação dos Instrumentos FM */
	if (fread(saminstfm,sizeof(saminstfm),1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Ler Estado FM!\0");
		fclose(fp);
		return(2);
	}

	/* Correcção Oitava Instrumentos FM */	
	if (fread(octinstfm,sizeof(octinstfm),1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Ler Estado FM!\0");
		fclose(fp);
		return(2);
	}
	
	/* Afinação Volume Instrumentos FM */	
	if (fread(volinstfm,sizeof(volinstfm),1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Ler Estado FM!\0");
		fclose(fp);
		return(2);
	}
	
	/* Informação dos Canais FM */
	if (fread(cnlmodcnlfm,sizeof(cnlmodcnlfm),1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Ler Estado FM!\0");
		fclose(fp);
		return(2);
	}
			
	sprintf(txtsts,"Estado FM Lido com Sucesso!\0");
	fclose(fp);

	return(0);
	
}

/* Grava o Estado FM para um Ficheiro */
int gravastsfm(void)
{

	FILE *fp;
	
	fp=fopen(nomefilestsfm,"wb");
	
	if(fp==NULL)
    {
      sprintf(txtsts,"Impossivel Criar Estado FM!\0");
	  return(1);
    }

	/* Informação dos Instrumentos FM */
	if (fwrite(saminstfm,sizeof(saminstfm),1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Gravar Estado FM!\0");
		fclose(fp);
		return(2);
	}
	
	/* Correcção Oitava Instrumentos FM */
	if (fwrite(octinstfm,sizeof(octinstfm),1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Gravar Estado FM!\0");
		fclose(fp);
		return(2);
	}
	
	/* Afinação Volume Instrumentos FM */
	if (fwrite(volinstfm,sizeof(volinstfm),1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Gravar Estado FM!\0");
		fclose(fp);
		return(2);
	}
	
	/* Informação dos Canais FM */
	if (fwrite(cnlmodcnlfm,sizeof(cnlmodcnlfm),1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Gravar Estado FM!\0");
		fclose(fp);
		return(2);
	}
	
	sprintf(txtsts,"Estado FM Gravado com Sucesso!\0");

	fclose(fp);

	return(0);
}

/* Grava a Música FM para um Ficheiro */
int gravamusicafm(void)
{

	FILE *fp;
	
	int i,s;
	
	/* Cabeçalho da Música FM */
	sprintf(cabfilefm.nome,nome);
	cabfilefm.songlen=songlen;
	cabfilefm.maxpat=maxpat;
	
	/* Instrumentos */
	for (i=0; i<31; i++)
		if (saminstfm[0][i]!=128)
		{
			s=saminstfm[0][i];
			sprintf(saveinstregfm[i].nome,instreg[s].nome);
			saveinstregfm[i].num=instreg[s].num;
			saveinstregfm[i].regc0=instreg[s].regc0;
			saveinstregfm[i].op1reg2=instreg[s].op1reg2;
			saveinstregfm[i].op1reg4=instreg[s].op1reg4;
			saveinstregfm[i].op1reg6=instreg[s].op1reg6;
			saveinstregfm[i].op1reg8=instreg[s].op1reg8;
			saveinstregfm[i].op1rege=instreg[s].op1rege;
			saveinstregfm[i].op2reg2=instreg[s].op2reg2;
		//	saveinstregfm[i].op2reg4=instreg[s].op2reg4;
			saveinstregfm[i].op2reg4=volinstfm[i];
			saveinstregfm[i].op2reg6=instreg[s].op2reg6;
			saveinstregfm[i].op2reg8=instreg[s].op2reg8;
			saveinstregfm[i].op2rege=instreg[s].op2rege;	
		}
		else
		{
			sprintf(saveinstregfm[i].nome,"\0");
			saveinstregfm[i].num=0;
			saveinstregfm[i].regc0=0;
			saveinstregfm[i].op1reg2=0;
			saveinstregfm[i].op1reg4=0;
			saveinstregfm[i].op1reg6=0;
			saveinstregfm[i].op1reg8=0;
			saveinstregfm[i].op1rege=0;
			saveinstregfm[i].op2reg2=0;
			saveinstregfm[i].op2reg4=0;
			saveinstregfm[i].op2reg6=0;
			saveinstregfm[i].op2reg8=0;
			saveinstregfm[i].op2rege=0;
		}
	
	fp=fopen(nomefilefm,"wb");
	
	if(fp==NULL)
    {
      sprintf(txtsts,"Impossivel Criar Musica FM!\0");
	  return(1);
    }

	/* Cabeçalho da Música FM */
	if (fwrite(&cabfilefm,sizeof(cabfilefm),1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Gravar Musica FM!\0");
		fclose(fp);
		return(2);
	}
	
	/* Informação das Posições FM */
	if (fwrite(songpos,sizeof(songpos),1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Gravar Musica FM!\0");
		fclose(fp);
		return(2);
	}
	
	/* Informação das Patterns FM */
	if (fwrite(patinfofm,SIZEPATINFOFM*maxpat,1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Gravar Musica FM!\0");
		fclose(fp);
		return(2);
	}
	
	/* Informação dos Instrumentos FM */
	if (fwrite(saveinstregfm,sizeof(saveinstregfm),1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Gravar Musica FM!\0");
		fclose(fp);
		return(2);
	}
	
	/* Correcção Oitava Instrumentos FM */
	if (fwrite(octinstfm,sizeof(octinstfm),1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Gravar Musica FM!\0");
		fclose(fp);
		return(2);
	}

	sprintf(txtsts,"Musica FM Gravada com Sucesso!\0");

	fclose(fp);

	return(0);
}

/* Lê os Instrumentos SAA1099 a partir de um Ficheiro */
int leinstsa(void)
{

	FILE *fp;
	
	fp=fopen(nomefileinstsa,"rb");
    
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

/* Lê o Estado SAA1099 a partir de um Ficheiro */
int lestssa(void)
{

	FILE *fp;
	
	fp=fopen(nomefilestssa,"rb");
    
	if(fp==NULL)
    {
      sprintf(txtsts,"Impossivel Abrir Estado SAA1099!\0");
	  return(1);
    }

	/* Informação dos Instrumentos SAA1099 */
	if (fread(saminstsa,sizeof(saminstsa),1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Ler Estado SAA1099!\0");
		fclose(fp);
		return(2);
	}
	
	/* Correcção Oitava Instrumentos SAA1099 */
	if (fread(octinstsa,sizeof(octinstsa),1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Ler Estado SAA1099!\0");
		fclose(fp);
		return(2);
	}
	
	/* Afinação Volume Instrumentos SAA1099 */
	if (fread(volinstsa,sizeof(volinstsa),1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Ler Estado SAA1099!\0");
		fclose(fp);
		return(2);
	}
	
	/* Informação dos Canais SAA1099 */
	if (fread(cnlmodcnlsa,sizeof(cnlmodcnlsa),1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Ler Estado SAA1099!\0");
		fclose(fp);
		return(2);
	}
			
	sprintf(txtsts,"Estado SAA1099 Lido com Sucesso!\0");
	fclose(fp);

	return(0);
	
}

/* Grava o Estado SAA1099 para um Ficheiro */
int gravastssa(void)
{

	FILE *fp;
	
	fp=fopen(nomefilestssa,"wb");
	
	if(fp==NULL)
    {
      sprintf(txtsts,"Impossivel Criar Estado SAA1099!\0");
	  return(1);
    }

	/* Informação dos Instrumentos SAA1099 */
	if (fwrite(saminstsa,sizeof(saminstsa),1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Gravar Estado SAA1099!\0");
		fclose(fp);
		return(2);
	}

	/* Correcção Oitava Instrumentos SAA1099 */
	if (fwrite(octinstsa,sizeof(octinstsa),1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Gravar Estado SAA1099!\0");
		fclose(fp);
		return(2);
	}
	
	/* Afinação Volume Instrumentos SAA1099 */
	if (fwrite(volinstsa,sizeof(volinstsa),1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Gravar Estado SAA1099!\0");
		fclose(fp);
		return(2);
	}

	/* Informação dos Canais SAA1099 */
	if (fwrite(cnlmodcnlsa,sizeof(cnlmodcnlsa),1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Gravar Estado SAA1099!\0");
		fclose(fp);
		return(2);
	}

	sprintf(txtsts,"Estado SAA1099 Gravado com Sucesso!\0");

	fclose(fp);

	return(0);
}

/* Grava a Música SAA1099 para um Ficheiro */
int gravamusicasa(void)
{

	FILE *fp;
	
	int i,s;
	
	/* Cabeçalho da Música SAA1099 */
	sprintf(cabfilesa.nome,nome);
	cabfilesa.songlen=songlen;
	cabfilesa.maxpat=maxpat;
	
	/* Instrumentos */
	for (i=0; i<31; i++)
		if (saminstsa[0][i]!=128)
		{
			s=saminstsa[0][i];
			sprintf(saveinstregsa[i].nome,instregsa[s].nome);
			saveinstregsa[i].num=instregsa[s].num;
			saveinstregsa[i].freqenb=instregsa[s].freqenb;
			saveinstregsa[i].noiseenb=instregsa[s].noiseenb;
			saveinstregsa[i].attack=instregsa[s].attack;
			saveinstregsa[i].sustain=instregsa[s].sustain;
			saveinstregsa[i].decay=instregsa[s].decay;
		//	saveinstregsa[i].volume=instregsa[s].volume;
			saveinstregsa[i].volume=volinstsa[i];
			saveinstregsa[i].repete=instregsa[s].repete;
		}
		else
		{
			sprintf(saveinstregsa[i].nome,"\0");
			saveinstregsa[i].num=0;
			saveinstregsa[i].freqenb=0;
			saveinstregsa[i].noiseenb=0;
			saveinstregsa[i].attack=0;
			saveinstregsa[i].sustain=0;
			saveinstregsa[i].decay=0;
			saveinstregsa[i].volume=0;
			saveinstregsa[i].repete=0;
		}
	
	fp=fopen(nomefilesa,"wb");
	
	if(fp==NULL)
    {
      sprintf(txtsts,"Impossivel Criar Musica SAA1099!\0");
	  return(1);
    }

	/* Cabeçalho da Música SAA1099 */
	if (fwrite(&cabfilesa,sizeof(cabfilesa),1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Gravar Musica SAA1099!\0");
		fclose(fp);
		return(2);
	}
	
	/* Informação das Posições SAA1099 */
	if (fwrite(songpos,sizeof(songpos),1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Gravar Musica SAA1099!\0");
		fclose(fp);
		return(2);
	}
	
	/* Informação das Patterns SAA1099 */
	if (fwrite(patinfosa,SIZEPATINFOSA*maxpat,1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Gravar Musica SAA1099!\0");
		fclose(fp);
		return(2);
	}
	
	/* Informação dos Instrumentos SAA1099 */
	if (fwrite(saveinstregsa,sizeof(saveinstregsa),1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Gravar Musica SAA1099!\0");
		fclose(fp);
		return(2);
	}
	
	/* Correcção Oitava Instrumentos SAA1099 */
	if (fwrite(octinstsa,sizeof(octinstsa),1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Gravar Musica SAA1099!\0");
		fclose(fp);
		return(2);
	}

	sprintf(txtsts,"Musica SAA1099 Gravada com Sucesso!\0");

	fclose(fp);

	return(0);
}

/* Lê os Instrumentos AY-3-8912 a partir de um Ficheiro */
int leinstay(void)
{

	FILE *fp;
	
	fp=fopen(nomefileinstay,"rb");
    
	if(fp==NULL)
    {
      sprintf(txtsts,"Impossivel Abrir Instrumentos AY-3-8912!\0");
	  return(1);
    }
	
	if (fread(instregay,sizeof(instregay),1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Ler Instrumentos AY-3-8912!\0");
		fclose(fp);
		return(2);
	}
			
	sprintf(txtsts,"Instrumentos AY-3-8912 Lidos com Sucesso!\0");
	fclose(fp);

	return(0);
	
}

/* Lê o ficheiro ayplayer.dat */
int leayplayer(void)
{
	
	FILE *fp;
	
	fp=fopen(nomefileayplrdat,"rb");
    
	if(fp==NULL)
    {
      sprintf(txtsts,"Impossivel Abrir Ficheiro ayplayer.dat!\0");
	  return(1);
    }
	
	if (fread(ayplayer,sizeof(ayplayer),1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Ler Ficheiro ayplayer.dat!\0");
		fclose(fp);
		return(2);
	}
			
	fclose(fp);

	return(0);
	
}

/* Lê o Estado AY-3-8912 a partir de um Ficheiro */
int lestsay(void)
{

	FILE *fp;
	
	fp=fopen(nomefilestsay,"rb");
    
	if(fp==NULL)
    {
      sprintf(txtsts,"Impossivel Abrir Estado AY-3-8912!\0");
	  return(1);
    }

	/* Informação dos Instrumentos AY-3-8912 */
	if (fread(saminstay,sizeof(saminstay),1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Ler Estado AY-3-8912!\0");
		fclose(fp);
		return(2);
	}
	
	/* Correcção Oitava Instrumentos AY-3-8912 */
	if (fread(octinstay,sizeof(octinstay),1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Ler Estado AY-3-8912!\0");
		fclose(fp);
		return(2);
	}
	
	/* Afinacao Volume Instrumentos AY-3-8912 */
	if (fread(volinstay,sizeof(volinstay),1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Ler Estado AY-3-8912!\0");
		fclose(fp);
		return(2);
	}
	
	/* Informação dos Canais AY-3-8912 */
	if (fread(cnlmodcnlay,sizeof(cnlmodcnlay),1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Ler Estado AY-3-8912!\0");
		fclose(fp);
		return(2);
	}
			
	sprintf(txtsts,"Estado AY-3-8912 Lido com Sucesso!\0");
	fclose(fp);

	return(0);
	
}

/* Grava o Estado AY-3-8912 para um Ficheiro */
int gravastsay(void)
{

	FILE *fp;
	
	fp=fopen(nomefilestsay,"wb");
	
	if(fp==NULL)
    {
      sprintf(txtsts,"Impossivel Criar Estado AY-3-8912!\0");
	  return(1);
    }

	/* Informação dos Instrumentos AY-3-8912 */
	if (fwrite(saminstay,sizeof(saminstay),1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Gravar Estado AY-3-8912!\0");
		fclose(fp);
		return(2);
	}

	/* Correcção Oitava Instrumentos AY-3-8912 */
	if (fwrite(octinstay,sizeof(octinstay),1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Gravar Estado AY-3-8912!\0");
		fclose(fp);
		return(2);
	}
	
	/* Afinacao Volume Instrumentos AY-3-8912 */
	if (fwrite(volinstay,sizeof(volinstay),1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Gravar Estado AY-3-8912!\0");
		fclose(fp);
		return(2);
	}

	/* Informação dos Canais AY-3-8912 */
	if (fwrite(cnlmodcnlay,sizeof(cnlmodcnlay),1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Gravar Estado AY-3-8912!\0");
		fclose(fp);
		return(2);
	}

	sprintf(txtsts,"Estado AY-3-8912 Gravado com Sucesso!\0");

	fclose(fp);

	return(0);
}

/* Grava a Música AY-3-8912 para um Ficheiro */
int gravamusicaay(void)
{

	FILE *fp;
	
	int i,s;
	
	/* Cabeçalho da Música AY-3-8912 */
	sprintf(cabfileay.nome,nome);
	cabfileay.songlen=songlen;
	cabfileay.maxpat=maxpat;
	
	/* Instrumentos */
	for (i=0; i<31; i++)
		if (saminstay[0][i]!=128)
		{
			s=saminstay[0][i];
			saveinstregay[i].num=instregay[s].num;
			saveinstregay[i].freqenb=instregay[s].freqenb;
			saveinstregay[i].noiseenb=instregay[s].noiseenb;
			saveinstregay[i].attack=instregay[s].attack;
			saveinstregay[i].sustain=instregay[s].sustain;
			saveinstregay[i].decay=instregay[s].decay;
		//	saveinstregay[i].volume=instregay[s].volume;
			saveinstregay[i].volume=volinstay[i];
			saveinstregay[i].repete=instregay[s].repete;
		}
		else
		{
			saveinstregay[i].num=0;
			saveinstregay[i].freqenb=0;
			saveinstregay[i].noiseenb=0;
			saveinstregay[i].attack=0;
			saveinstregay[i].sustain=0;
			saveinstregay[i].decay=0;
			saveinstregay[i].volume=0;
			saveinstregay[i].repete=0;
		}
	
	fp=fopen(nomefileay,"wb");
	
	if(fp==NULL)
    {
      sprintf(txtsts,"Impossivel Criar Musica AY-3-8912!\0");
	  return(1);
    }

	/* Cabeçalho da Música AY-3-8912 */
	if (fwrite(&cabfileay,sizeof(cabfileay),1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Gravar Musica AY-3-8912!\0");
		fclose(fp);
		return(2);
	}
	
	/* Informação das Posições AY-3-8912 */
	if (fwrite(songpos,sizeof(songpos),1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Gravar Musica AY-3-8912!\0");
		fclose(fp);
		return(2);
	}
	
	/* Informação das Patterns AY-3-8912 */
	if (fwrite(patinfoay,SIZEPATINFOAY*maxpat,1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Gravar Musica AY-3-8912!\0");
		fclose(fp);
		return(2);
	}
	
	/* Informação dos Instrumentos AY-3-8912 */
	if (fwrite(saveinstregay,sizeof(saveinstregay),1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Gravar Musica AY-3-8912!\0");
		fclose(fp);
		return(2);
	}
	
	/* Correcção Oitava Instrumentos AY-3-8912 */
	if (fwrite(octinstay,sizeof(octinstay),1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Gravar Musica AY-3-8912!\0");
		fclose(fp);
		return(2);
	}

	sprintf(txtsts,"Musica AY-3-8912 Gravada com Sucesso!\0");

	fclose(fp);

	return(0);
}

/* Grava o Ficheiro AY Player para ser Lido nos Emuladores */
int gravaayplayer(void)
{
	
	FILE *fp;
	
	unsigned long lengthOfFile;
	
	int i;
	
	/* Abre o Ficheiro .MAY acabado de gravar */
	fp=fopen(nomefileay,"rb");
    if(fp==NULL)
    {
      printf("Impossível abrir ficheiro!\n");
	  return(1);
    }

	/* Lê o Tamanho do Ficheiro */
	fseek(fp,0,SEEK_END);
	lengthOfFile = ftell(fp);
	
	/* Aloca Memória para a Música */
	aymus=(char*)calloc(lengthOfFile,sizeof(char));
	
	fseek(fp,0,SEEK_SET);
	
	if (fread(aymus,lengthOfFile,1,fp)==0)
	{
		printf("Erro ao ler Ficheiro!\n");
		free(aymus);
		fclose(fp);
		return(2);
	}
	
	fclose(fp);
	
	/* Cabeçalho */
	aycablen[0]=0x13;	/* Tamanho do Cabeçalho (19) */
	aycablen[1]=0;
	
	aycab[0]=0;		/* FLAG : Header */
	aycab[1]=3;		/* Tipo de Dados */

	/* Nome do Bloco de Dados */
	aycab[2]='m';aycab[3]='u';aycab[4]='s';aycab[5]='i';aycab[6]='c';
	aycab[7]='a';aycab[8]=' ';aycab[9]=' ';aycab[10]=' ';aycab[11]=' ';
	
	/* Divide o Tamanho do Ficheiro em Hi e Lo */
	hi=(int)(lengthOfFile/256);
	lo=(int)(lengthOfFile-hi*256);
	
	aycab[12]=lo;
	aycab[13]=hi;
	
	/* Endereço de Memória do Bloco de Dados */
	hi=(int)(30000/256);
	lo=(int)(30000-hi*256);
	
	aycab[14]=lo;
	aycab[15]=hi;
	
	hi=(int)(32768/256);
	lo=(int)(32768-hi*256);
	
	aycab[16]=lo;
	aycab[17]=hi;
	
	aycabchks[0]=0;	/* Checksum do Cabeçalho */
	
	for(i=0; i<sizeof(aycab); i++)
		aycabchks[0]=aycabchks[0]^aycab[i];
	
	/* Bloco de Dados */
	/* Adiciona 2 ao Tamanho do Ficheiro para contar com a FLAG e o Checksum */
	lengthOfFile+=2;
	
	/* Divide o Tamanho do Ficheiro em Hi e Lo */
	hi=(int)(lengthOfFile/256);
	lo=(int)(lengthOfFile-hi*256);
	
	aymuslen[0]=lo;
	aymuslen[1]=hi;
	aymuslen[2]=0xFF;	/* FLAG : Bloco de Dados */
	
	aymuschks[0]=0xFF;	/* Checksum do Bloco de Dados */
	
	for(i=0; i<lengthOfFile-2; i++)
		aymuschks[0]=aymuschks[0]^aymus[i];
	
	/* Grava o Ficheiro */
	fp=fopen(nomefileayplr,"wb");
	
	if(fp==NULL)
    {
      sprintf(txtsts,"Impossivel Criar Ficheiro %s!\0",nomefileayplr);
	  free(aymus);
	  return(1);
    }
	
	if (fwrite(ayplayer,sizeof(ayplayer),1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Gravar Ficheiro %s!\0",nomefileayplr);
		free(aymus);
		fclose(fp);
		return(2);
	}
	
	if (fwrite(aycablen,sizeof(aycablen),1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Gravar Ficheiro %s!\0",nomefileayplr);
		free(aymus);
		fclose(fp);
		return(2);
	}
	
	if (fwrite(aycab,sizeof(aycab),1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Gravar Ficheiro %s!\0",nomefileayplr);
		free(aymus);
		fclose(fp);
		return(2);
	}
	
	if (fwrite(aycabchks,sizeof(aycabchks),1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Gravar Ficheiro %s!\0",nomefileayplr);
		free(aymus);
		fclose(fp);
		return(2);
	}
	
	if (fwrite(aymuslen,sizeof(aymuslen),1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Gravar Ficheiro %s!\0",nomefileayplr);
		free(aymus);
		fclose(fp);
		return(2);
	}
	
	if (fwrite(aymus,lengthOfFile-2,1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Gravar Ficheiro %s!\0",nomefileayplr);
		free(aymus);
		fclose(fp);
		return(2);
	}
	
	if (fwrite(aymuschks,sizeof(aymuschks),1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Gravar Ficheiro %s!\0",nomefileayplr);
		free(aymus);
		fclose(fp);
		return(2);
	}
	
	free(aymus);
	fclose(fp);

	return(0);
	
}

/* Navega nos Paineis das Patterns/Samples do Módulo */
void navegamod(void)
{
	
	do
	{
		KB_code=getch();
		
		if (KB_code==KB_F1)					/* F1 - HELP */
		{
			estado=5;
			help=0;
			oldestadohlp=0;
		}
		
		if (KB_code==KB_F3) estado=1;		/* F3 - FM */
		
		if (KB_code==KB_F4) estado=2;		/* F4 - SAA1099 */
		
		if (KB_code==KB_F5) estado=3;		/* F5 - AY-3-8912 */
		
		if (KB_code==KB_TAB) menu=1-menu;
		
		if (KB_code==KB_ESCAPE) estado=10;
										
		if (menu==0)
		{
			alteracabmod();
			navegapat();
		}
		else
		{
			alteracabmod();
			navegasam();
		}
		
		/* Temporário */
	//	gotoxy(1,9);
	//	printf("%d ",KB_code);
			
	}
	while (estado==0);
	
}

/* Navega nas Patterns do Módulo */
void navegapat(void)
{
	
	/* Posições */
	if (KB_code==KB_LEFT && position>0)
	{
		position--;
		carregapat();
	}
				
    if (KB_code==KB_RIGHT && position<songlen-1)
	{
		position++;
		carregapat();
	}
		
	
	if (KB_code==KB_CTRL_LEFT)
	{
		position=0;
		carregapat();
	}
				
    if (KB_code==KB_CTRL_RIGHT)
	{
		position=songlen-1;
		carregapat();
	}
		
	if (KB_code==KB_ALT_LEFT)
	{
		position-=10;
		if (position<0) position=0;
		carregapat();
	}
					
    if (KB_code==KB_ALT_RIGHT)
	{
		position+=10;
		if (position>songlen-1) position=songlen-1;
		carregapat();
	}
		
	
	/* Rows */
	if (KB_code==KB_UP && py>0) py--;
			
	if (KB_code==KB_DOWN && py<52) py++;
	
	
	if (KB_code==KB_HOME) py=0;
						
	if (KB_code==KB_END) py=52;

						
	if (KB_code==KB_PAGE_UP)
	{
		py-=12;
		if (py<0) py=0;
	}
	
	if (KB_code==KB_PAGE_DOWN)
	{
		py+=12;
		if (py>52) py=52;
	}
	
	printpat();
		
}

/* Navega nos Samples do Módulo */
void navegasam()
{

	if (KB_code==KB_P) playsom();

	if (KB_code==KB_UP && ny>0)
	{
		ny--;
		if (ny<31-sami && sy>0) sy--;
	}
			
	if (KB_code==KB_DOWN && ny<30)
	{
		ny++;
		if (ny>sami-1 && sy<31-sami) sy++;
	}
		

	if (KB_code==KB_HOME)
	{
		ny=0;
		sy=0;
	}
			
	if (KB_code==KB_END)
	{
		ny=30;
		sy=31-sami;
	}

			
	if (KB_code==KB_PAGE_UP)
	{
		sy-=sami;
		if (sy<0) sy=0;
		ny=sy;
	}
			
	if (KB_code==KB_PAGE_DOWN)
	{
		sy+=sami;
		if (sy>31-sami) sy=31-sami;
		ny=sy;
	}
		
	/* Instrumentos FM */
	if (KB_code==KB_DEL && estado==1)	/* Delete - Remmove o Instrumento FM atribuído ao Sample Selecionado */
	{
		saminstfm[0][ny]=128;
		saminstfm[1][ny]=0;
		printsaminstfm();
		sprintf(txtsts,"Instrumento atribuido ao Sample '%s' Removido\0",nomesample[ny]);
		printestado();
	}
	
	/* Instrumentos SAA1099 */
	if (KB_code==KB_DEL && estado==2)	/* Delete - Remmove o Instrumento SAA1099 atribuído ao Sample Selecionado */
	{
		saminstsa[0][ny]=128;
		saminstsa[1][ny]=0;
		printsaminstsa();
		sprintf(txtsts,"Instrumento atribuido ao Sample '%s' Removido\0",nomesample[ny]);
		printestado();
	}
	
	/* Instrumentos AY-3-8912 */
	if (KB_code==KB_DEL && estado==3)	/* Delete - Remmove o Instrumento AY-3-8912 atribuído ao Sample Selecionado */
	{
		saminstay[0][ny]=128;
		saminstay[1][ny]=0;
		printsaminstay();
		sprintf(txtsts,"Instrumento atribuido ao Sample '%s' Removido\0",nomesample[ny]);
		printestado();
	}
	
	printsam();
	
	/* Instrumentos FM */
	if (estado==1)
	{
		if (saminstfm[0][ny]!=128)
		{
			sifm=saminstfm[0][ny];
			pifm=saminstfm[1][ny];
			printinstfm();
		}
		printoctfm();
		printsaminstfm();
		printvolfm();
	}
	
	/* Instrumentos SAA1099 */
	if (estado==2)
	{
		if (saminstsa[0][ny]!=128)
		{
			sisa=saminstsa[0][ny];
			pisa=saminstsa[1][ny];
			printinstsa();
		}
		printoctsa();
		printsaminstsa();
		printvolsa();
	}
	
	/* Instrumentos AY-3-8912 */
	if (estado==3)
	{
		if (saminstay[0][ny]!=128)
		{
			siay=saminstay[0][ny];
			piay=saminstay[1][ny];
			printinstay();
		}
		printoctay();
		printsaminstay();
		printvolay();
	}
}

/* Menu de Conversão para FM - Estado = 1 */
void printfm(void)
{

	textbackground(0);
	textcolor(7);
	clrscr();
	
	/* Menu */
	textbackground(1);
	textcolor(15);
	gotoxy(1,1);
	cprintf(" F1-Help  F2-Modulo  F3-FM  F4-SAA1099  F5-AY/YM  F6-Play Conv         ESC-Sair ");
	gotoxy(1,25);
	cprintf("                                                                               ");
	
	textbackground(0);
	textcolor(11);
	gotoxy(22,3);
	cprintf("Pagina de Conversao de MOD para FM");

	/* Menu */
	menu=0;
	menup=0;
	
	/* Navegação nos Samples */
	sy=0;
	ny=0;
	samx=2;
	samy=8;
	sami=16;
	
	/* Navegação nos Instrumentos FM */
	sifm=0;
	pifm=0;
	
	/* Navegação nos Canais do Módulo */
	cnlmody=4;
		
	/* Navegação nos Canais FM */
	cnlfmy=0;

	/* Cabeçalhos */
	alteracabconv();
	alteracabpanel();

	/* Samples */
	printsam();
	
	/* Instrumentos FM */
	printinstfm();
	
	/* Instrumento FM atribuído ao Sample */
	printsaminstfmtxt();
	printsaminstfm();
	
	/* Oitava */
	printocttxt();
	printoctfm();
	printvolfm();
		
	/* Estado */
	sprintf(txtsts,"Pagina de Conversao de MOD para FM.\0");
	printestado();
	
}

/* Altera as Cores do Menu Samples/Canais do Menu de Conversão */
void alteracabconv(void)
{
	
	int i;
	
	textbackground(0);
	
	for (i=5; i<24; i++)
	{
		gotoxy(2,i);
		cprintf("                                                                             ");	
	}
	
	if (menu==0)
	{
		psamcolor=14;
		psambkcolor=12;
		pcnlcolor=14;
		pcnlbkcolor=0;
		printhlpinst();
	}
	else
	{
		psamcolor=14;
		psambkcolor=0;
		pcnlcolor=14;
		pcnlbkcolor=12;
		printhlpcnl();
	}
		
	textcolor(psamcolor);
	textbackground(psambkcolor);
	gotoxy(2,5);
	cprintf("   Samples   ");
	
	textcolor(pcnlcolor);
	textbackground(pcnlbkcolor);
	gotoxy(55,5);
	cprintf("   Canais   ");
	
	textcolor(7);
	textbackground(0);
	
}

/* Altera as Cores do Painel Samples/Canais do Menu de Conversão */
void alteracabpanel(void)
{
	
	textbackground(0);
	
	if (menu==0)
	{
		if (menup==0)
		{
			psamcolor=15;
			psambkcolor=13;
			pinstcolor=15;
			pinstbkcolor=0;
		}
		else
		{
			psamcolor=15;
			psambkcolor=0;
			pinstcolor=15;
			pinstbkcolor=13;
		}
		
		textcolor(psamcolor);
		textbackground(psambkcolor);
		gotoxy(2,7);
		cprintf("         Samples         ");
	
		textcolor(pinstcolor);
		textbackground(pinstbkcolor);
		gotoxy(29,7);
		cprintf("    Instrumentos    ");
	}
	else
	{
		if (menup==0)
		{
			pcnlconvcolor=15;
			pcnlconvbkcolor=13;
			pcnlmodcolor=15;
			pcnlmodbkcolor=0;
		}
		else
		{
			pcnlconvcolor=15;
			pcnlconvbkcolor=0;
			pcnlmodcolor=15;
			pcnlmodbkcolor=13;
		}
		
		textcolor(pcnlconvcolor);
		textbackground(pcnlconvbkcolor);
		gotoxy(3,7);
		if (estado==1) cprintf(" Canais FM ");
		if (estado==2) cprintf("Canais SAA1099");
		if (estado==3) cprintf("Canais AY-3-8912");
	
		textcolor(pcnlmodcolor);
		textbackground(pcnlmodbkcolor);
		gotoxy(29,7);
		cprintf(" Canais MOD ");
	}
		
	textcolor(7);
	textbackground(0);
	
}

/* Imprime as Teclas do Menu de Conversão - Samples */
void printhlpinst(void)
{
	
	textbackground(0);
	textcolor(11);
	gotoxy(54,17);
	cprintf("ALT - Alterna entre");
	gotoxy(54,18);
	cprintf("Samples e Instrumentos");
	
	gotoxy(54,20);
	cprintf("CTRL + ALT - Para ");
	gotoxy(54,21);
	cprintf("configurar Canais");
	
}

/* Imprime as Teclas do Menu de Conversão - Canais */
void printhlpcnl(void)
{
	
	textbackground(0);
	textcolor(11);
	gotoxy(54,9);
	cprintf("ALT - Alterna entre");
	gotoxy(54,10);
	switch (estado)
	{
		case 1:cprintf("Canais FM e Canais MOD");break;
		case 2:cprintf("Canais SAA e Canais MOD");break;
		case 3:cprintf("Canais AY e Canais MOD");break;
	}
	
	gotoxy(54,12);
	cprintf("CTRL + ALT - Para");
	gotoxy(54,13);
	cprintf("configurar Samples");
	
}

/* Reset a Atribuição de Instrumentos FM aos Samples */
void resetsaminstfm()
{
	
	int i;
	
	for (i=0; i<31; i++)
	{
		saminstfm[0][i]=128;
		saminstfm[1][i]=0;
		octinstfm[i]=2;
		volinstfm[i]=63;
	}
	
}

/* Reset a Atribuição de Canais MOD aos Canais FM */
void resetcnlmodcnlfm()
{
	int i;
	
	for (i=0; i<4; i++) cnlmodcnlfm[i]=i;
	for (i=4; i<FMNUNCANAIS; i++) cnlmodcnlfm[i]=4;
	
}

/* Imprime a Informação dos Instrumentos FM */
void printinstfm(void)
{
	
	int i;

	for (i=pifm; i<pifm+16; i++)
	{
		
		gotoxy(29,i-pifm+8);
		textbackground(2);
		textcolor(15);	
				
		if (i==sifm) textbackground(1);
			
		cprintf("%-20s",instreg[i].nome);
		
	}
	
	textbackground(0);
	
}

/* Imprime o Texto do Instrumento FM atribuido ao Sample selecionado */
void printsaminstfmtxt(void)
{
	textcolor(10);
	gotoxy(54,13);cprintf("Instrumento FM");
}

/* Imprime o Instrumento FM atribuido ao Sample selecionado */
void printsaminstfm(void)
{
	
	textcolor(15);
	gotoxy(54,14);
	if (saminstfm[0][ny]==128)
		cprintf("Sem Instrumento");
	else cprintf("%-20s",instreg[saminstfm[0][ny]].nome);
	textbackground(0);
	
}

/* Imprime os Canais do Módulo */
void printcnlmod(void)
{
	
	int i;
	
	for (i=9; i<14; i++)
	{
		gotoxy(29,i);
		textbackground(0);
		textcolor(15);	
		
		if (i-9==cnlmody) textbackground(1);
		
		if (i==13) cprintf("Sem Canal MOD");
		else
			cprintf("Canal MOD %d ",i-9);
	}
	
	textbackground(0);
	
}

/* Imprime os Canais FM */
void printcnlfm(void)
{
	
	int i;
	
	for (i=9; i<18; i++)
	{
		gotoxy(3,i);
		textbackground(0);
		textcolor(15);
		
		if (i-9==cnlfmy) textbackground(2);
		
		cprintf("Canal FM %d ",i-9);
	}
	
	textbackground(0);
	
}

/* Navega no Menu de Conversão para FM */
void navegafm(void)
{
	
	int f;
	
	do
	{
		KB_code=getch();
		
		if (KB_code==KB_F1)					/* F1 - HELP */
		{
			estado=5;
			oldestadohlp=1;
			if (menu==0) help=2;
				else help=4;
		}
		
		if (KB_code==KB_F2) estado=0;	/* F2 - Módulo */
		
		if (KB_code==KB_F4) estado=2;	/* F4 - SAA1099 */
		
		if (KB_code==KB_F5) estado=3;	/* F5 - AY-3-8912 */
		
		if (KB_code==KB_F6)				/* F6 - Pattern Conv / Toca Música Convertida */
		{
			oldestado=1;
			estado=4;
		}
		
		if (KB_code==KB_CTRL_TAB)
		{
			menu=1-menu;
			
			alteracabconv();
			alteracabpanel();

			if (menu==0)
			{
				printsam();
				printinstfm();
				printocttxt();
				printoctfm();
				printvolfm();
				printsaminstfmtxt();
				printsaminstfm();
			}
			else
			{
				printcnlfm();
				printcnlmod();
			}
			
		}
				
		if (KB_code==KB_ESCAPE) estado=10;
		
		if (KB_code==KB_TAB)
		{
			menup=1-menup;
			alteracabpanel();
		}
												
		if (menu==0)
		{
			if (menup==0) navegasam();
			else navegainstfm();
		}
		else
		{
			if (menup==0) navegacnlfm();
			else navegacnlmod();
		}
		
		/* Oitava */
		if (KB_code==KB_MENOS && octinstfm[ny]>-3)	/* Tecla - */
		{
			octinstfm[ny]--;
			printoctfm();
		}
		
		if (KB_code==KB_MAIS && octinstfm[ny]<7)	/* Tecla + */
		{
			octinstfm[ny]++;
			printoctfm();
		}
		
		/* Volume */
		if (KB_code==KB_B && 63-volinstfm[ny]<63)	/* Tecla B */
		{
			volinstfm[ny]--;
			printvolfm();
		}
		
		if (KB_code==KB_V && 63-volinstfm[ny]>0)	/* Tecla V */
		{
			volinstfm[ny]++;
			printvolfm();
		}
		
		/* Converte o MOD para FM */
		if (KB_code==KB_C) convertmodfm();
		
		/* Toca o Sample Selecionado */
		if (KB_code==KB_P) playsom();
		
		/* Toca o Instrumento FM Selecionado */
		if (KB_code==KB_I)
		{
			oitavafm=4;
			volfm=instreg[sifm].op2reg4;
			criasomfm(0,FM_CNL_0,1);
		}
		
		/* Para o Instrumento FM Selecionado */
		if (KB_code==KB_O) stopsomfm(0);
		
		/* Reset às Atribuições Instrumentos/Canais */
		if (KB_code==KB_R)
		{
			if (menu==0)
			{
				resetsaminstfm();
				printsam();
				printinstfm();
				printocttxt();
				printoctfm();
				printvolfm();
				printsaminstfmtxt();
				printsaminstfm();
				sprintf(txtsts,"Atribuicao de Instrumentos Anulada!\0");
				printestado();
			}
			else
			{	
				resetcnlmodcnlfm();
				printcnlfm();
				printcnlmod();
				sprintf(txtsts,"Atribuicao de Canais Anulada!\0");
				printestado();
			
			}
		}
		
		/* Grava o Estado FM */
		if (KB_code==KB_Q)
		{
			f=gravastsfm();
			printestado();
			f++;
		}
		
		/* Lê o Estado FM */
		if (KB_code==KB_E)
		{
			f=lestsfm();
			printestado();
			f++;
		}
		
		/* Grava a Música FM */
		if (KB_code==KB_G)
		{
			f=gravamusicafm();
			printestado();
			f++;
		}
		
	}
	while (estado==1);
	
}

/* Navega nos Instrumentos FM */
void navegainstfm(void)
{

	if (KB_code==KB_UP && sifm>0)		/* Cima */
	{
		sifm--;
		if (sifm<112 && pifm>0) pifm--;
	}
			
	if (KB_code==KB_DOWN && sifm<127) /* Baixo */
	{
		sifm++;
		if (sifm>15 && pifm<112) pifm++;
	}
	
	
	if (KB_code==KB_HOME)	/* Home */
	{
		pifm=0;
		sifm=0;
	}
						
	if (KB_code==KB_END)	/* End */
	{
		pifm=112;
		sifm=127;
	}

						
	if (KB_code==KB_PAGE_UP)	/* Page UP */
	{
		pifm-=16;
		if (pifm<0) pifm=0;
		sifm=pifm;
	}
	
	if (KB_code==KB_PAGE_DOWN)	/* Page DOWN */
	{
		pifm+=16;
		if (pifm>112) pifm=112;
		sifm=pifm;
	}
	
	if (KB_code==KB_RETURN && tisample[ny])	/* RETURN - Atribui o Instrumento FM para o Sample Selecionado */
	{
		saminstfm[0][ny]=sifm;
		saminstfm[1][ny]=pifm;
		volinstfm[ny]=instreg[sifm].op2reg4;
		printsaminstfm();
		printoctfm();
		printvolfm();
		sprintf(txtsts,"Instrumento '%s' atribuido ao Sample '%s'\0",instreg[sifm].nome,nomesample[ny]);
		printestado();
	}
	
	printinstfm();
		
}

/* Navega nos Canais do MOD */
void navegacnlmod(void)
{
	
	if (KB_code==KB_UP && cnlmody>0) cnlmody--;		/* Cima */
				
	if (KB_code==KB_DOWN && cnlmody<4) cnlmody++;	/* Baixo */
	
	
	if (KB_code==KB_HOME || KB_code==KB_PAGE_UP) cnlmody=0;		/* Home */
							
	if (KB_code==KB_END || KB_code==KB_PAGE_DOWN) cnlmody=4;	/* End */
							
	if (KB_code==KB_RETURN)	/* RETURN - Atribui o Canal MOD ao Canal FM/SAA/AY Selecionado */
	{
		
		if (estado==1) cnlmodcnlfm[cnlfmy]=cnlmody;
		if (estado==2) cnlmodcnlsa[cnlsay]=cnlmody;
		if (estado==3) cnlmodcnlay[cnlayy]=cnlmody;
		
		if (cnlmody==4)
		{
			if (estado==1) sprintf(txtsts,"Sem Canal MOD atribuido ao Canal FM %d\0",cnlfmy);
			if (estado==2) sprintf(txtsts,"Sem Canal MOD atribuido ao Canal SAA %d\0",cnlsay);
			if (estado==3) sprintf(txtsts,"Sem Canal MOD atribuido ao Canal AY %d\0",cnlayy);
		}
		else
		{
			if (estado==1) sprintf(txtsts,"Canal MOD %d atribuido ao Canal FM %d\0",cnlmody,cnlfmy);
			if (estado==2) sprintf(txtsts,"Canal MOD %d atribuido ao Canal SAA %d\0",cnlmody,cnlsay);
			if (estado==3) sprintf(txtsts,"Canal MOD %d atribuido ao Canal AY %d\0",cnlmody,cnlayy);
		}
		printestado();
		
	}
	
	printcnlmod();
		
}

/* Navega nos Canais FM */
void navegacnlfm(void)
{
	
	if (KB_code==KB_UP && cnlfmy>0) cnlfmy--;		/* Cima */
				
	if (KB_code==KB_DOWN && cnlfmy<8) cnlfmy++;		/* Baixo */
	
	
	if (KB_code==KB_HOME || KB_code==KB_PAGE_UP) cnlfmy=0;		/* Home */
							
	if (KB_code==KB_END || KB_code==KB_PAGE_DOWN) cnlfmy=8;		/* End */
	
	if (KB_code==KB_DEL)	/* Delete - Remove o Canal MOD Atribuído ao Canal FM Selecionado */
	{
		cnlmodcnlfm[cnlfmy]=4;
		sprintf(txtsts,"Canal MOD atribuido ao Canal FM %d Removido\0",cnlmody,cnlfmy);
		printestado();
	}
			
	cnlmody=cnlmodcnlfm[cnlfmy];
	printcnlmod();
	printcnlfm();
	
}

/* Converte o MOD para FM */
void convertmodfm(void)
{
	
	int i,j;
	int x,y;
	int p,h;
	
	unsigned char fmefect,fmfectpar,fmsam;
	
	memset(patinfofm,0,SIZEPATINFOFM*maxpat);
	
	h=0;
	/* Percorre por todas as Patterns */
	for (pattern=0; pattern<maxpat; pattern++)
	{
		
		/* Percorre as 64 Rows da respetiva Pattern */
		y=0;
		for (i=pattern*SIZEPATINFOMOD; i<((pattern*SIZEPATINFOMOD)+SIZEPATINFOMOD); i+=SIZEROWMOD)
		{
			/* Percorre os 4 Canais da respetiva Row */
			x=0;
			for(j=0; j<SIZEROWMOD; j+=4)
			{
				hi=(int)(patinfo[i+j+0]/16);
				lo=(int)(patinfo[i+j+0]-hi*16);
	
				period[y][x]=lo*256+patinfo[i+j+1];
		
				lo=(int)(patinfo[i+j+2]/16);
				samplenum[y][x]=hi*16+lo;
	
				efectnum[y][x]=(int)(patinfo[i+j+2]-lo*16);
				efectpar[y][x]=patinfo[i+j+3];
				
				/* Percorre todos os Canais FM */
				for (p=0; p<FMNUNCANAIS; p++)
				{
					
					/* Verifica se o Canal FM está ativo para o respetivo Canal MOD */				
					if (cnlmodcnlfm[p]==x)
					{
						convfrntfm(period[y][x]); 	/* Frequência e Oitava 	*/
						fmsam=samplenum[y][x];		/* Número do Sample 	*/
						fmefect=0;					/* Número do Efeito		*/
						fmfectpar=0;				/* Parâmetro do Efeito	*/
						
						switch (efectnum[y][x])
						{
							case 11:fmefect=1;fmfectpar=efectpar[y][x];break;			/* Salta para Pattern */
							case 13:fmefect=2;fmfectpar=0;break;						/* Salta para a próxima Pattern */
							case 12:fmefect=3;fmfectpar=(char)(efectpar[y][x]/9);break; /* Volume */
						}
						
						if (fmsam==0)	/* Se não houver Sample Atribuído */
						{
							oitavafm=0;
							freqfm=0;
						}
						
						if (period[y][x]==0)	/* Se não houver Frequência */
						{
							fmsam=0;
							if (fmefect==3 && fmfectpar!=0)	/* Se o Volume for Diferente de 0 */
							{
								fmefect=0;
								fmfectpar=0;
							}
						}
						
						/* Verifica se o Sample tem um Instrumento FM Atribuído */
						if (saminstfm[0][fmsam-1]!=128)
						{
							patinfofm[h+p*2]=fmefect*64+oitavafm*16+freqfm;
							if (fmefect==1) patinfofm[h+p*2+1]=fmfectpar;
							else patinfofm[h+p*2+1]=fmfectpar*32+fmsam;
						}
						else
						{
							patinfofm[h+p*2]=0;
							patinfofm[h+p*2+1]=0;
						}
					}
				}
				x++;
			}
			y++;
			h+=SIZEROWFM;
		}
	}
	
	sprintf(txtsts,"Modulo Convertido para FM com Sucesso! F6 para toca-lo.\0");
	printestado();

}

/* Converte as Frequências do MOD para as Frequências FM */
void convfrntfm(int freq)
{
	
	oitavafm=0;
	freqfm=0;
	
	switch (freq)
	{
		case 0:freqfm=0;oitavafm=0;break;
		
		case 856:freqfm=1;oitavafm=1;break;
		case 808:freqfm=2;oitavafm=1;break;
		case 762:freqfm=3;oitavafm=1;break;
		case 763:freqfm=3;oitavafm=1;break;
		case 720:freqfm=4;oitavafm=1;break;
		case 678:freqfm=5;oitavafm=1;break;
		case 679:freqfm=5;oitavafm=1;break;
		case 640:freqfm=6;oitavafm=1;break;
		case 641:freqfm=6;oitavafm=1;break;
		case 604:freqfm=7;oitavafm=1;break;
		case 605:freqfm=7;oitavafm=1;break;
		case 570:freqfm=8;oitavafm=1;break;
		case 571:freqfm=8;oitavafm=1;break;
		case 538:freqfm=9;oitavafm=1;break;
		case 539:freqfm=9;oitavafm=1;break;
		case 508:freqfm=10;oitavafm=1;break;
		case 509:freqfm=10;oitavafm=1;break;
		case 480:freqfm=11;oitavafm=1;break;
		case 453:freqfm=12;oitavafm=1;break;
		
		case 428:freqfm=1;oitavafm=2;break;
		case 404:freqfm=2;oitavafm=2;break;
		case 381:freqfm=3;oitavafm=2;break;
		case 360:freqfm=4;oitavafm=2;break;
		case 339:freqfm=5;oitavafm=2;break;
		case 340:freqfm=5;oitavafm=2;break;
		case 320:freqfm=6;oitavafm=2;break;
		case 321:freqfm=6;oitavafm=2;break;
		case 302:freqfm=7;oitavafm=2;break;
		case 303:freqfm=7;oitavafm=2;break;
		case 285:freqfm=8;oitavafm=2;break;
		case 286:freqfm=8;oitavafm=2;break;
		case 269:freqfm=9;oitavafm=2;break;
		case 270:freqfm=9;oitavafm=2;break;
		case 254:freqfm=10;oitavafm=2;break;
		case 240:freqfm=11;oitavafm=2;break;
		case 226:freqfm=12;oitavafm=2;break;
		case 227:freqfm=12;oitavafm=2;break;

		case 214:freqfm=1;oitavafm=3;break;
		case 202:freqfm=2;oitavafm=3;break;
		case 190:freqfm=3;oitavafm=3;break;
		case 191:freqfm=3;oitavafm=3;break;
		case 180:freqfm=4;oitavafm=3;break;
		case 170:freqfm=5;oitavafm=3;break;
		case 160:freqfm=6;oitavafm=3;break;
		case 151:freqfm=7;oitavafm=3;break;
		case 143:freqfm=8;oitavafm=3;break;
		case 135:freqfm=9;oitavafm=3;break;
		case 127:freqfm=10;oitavafm=3;break;
		case 120:freqfm=11;oitavafm=3;break;
		case 113:freqfm=12;oitavafm=3;break;
		
	}
	
}

/* Imprime as Patterns FM depois de Convertidas */
void printpatconvfm(void)
{
	
	int i;
	
	textbackground(0);
	textcolor(7);
	clrscr();
	
	/* Menu */
	textbackground(1);
	textcolor(15);
	gotoxy(1,1);
	cprintf(" F1-Help  F2-Modulo  F3-FM  F4-SAA1099  F5-AY/YM  F6-Play Conv         ESC-Sair ");
	gotoxy(1,25);
	cprintf("                                                                               ");
		
	/* Menu */
	menu=0;
	
	playfm=0;
	
	/* Cabeçalho */
	alteracabconvfm();
		
	/* Navegação nas Patterns FM */
	position=0;
	py=0;
	
	/* Patterns */
	carregapatfm();
	printpatfm();
	
	/* Estado */
	sprintf(txtsts,"Tecla P para tocar musica FM convertida.\0");
	printestado();
	
}

/* Altera os Canais Visíveis no Menu Patterns depois de Convertidas*/
void alteracabconvfm(void)
{

	textbackground(0);
	textcolor(11);
	
	gotoxy(1,3);
	cprintf("Patterns FM : ");
	if (menu==0)
	{
		textcolor(10);
		cprintf("Canais 0 - 3");
		textcolor(13);
		cprintf("   (ALT para Canais 4 - 8)");
	}
	else
	{
		textcolor(10);
		cprintf("Canais 4 - 8");
		textcolor(13);
		cprintf("   (ALT para Canais 0 - 3)");
	}
	
	printcnlonfm();
	
}

/* Lê a Informação da Pattern FM corrente */
void carregapatfm(void)
{

	int i,j;
	int x,y;
	
	pattern=songpos[position];
	py=0;
	
	textbackground(0);
	textcolor(14);
	gotoxy(1,5);
	cprintf("Position - %d , Pattern - %d  \n\n",position,pattern);

	y=0;
	for (i=pattern*SIZEPATINFOFM; i<((pattern*SIZEPATINFOFM)+SIZEPATINFOFM); i+=SIZEROWFM)
	{
		x=0;
		for(j=0; j<SIZEROWFM; j+=2)
		{
			
			/* Byte 0 - Frequência, Oitava e Número do Efeito */
			hi=(int)(patinfofm[i+j+0]/64);
			lo=(int)(patinfofm[i+j+0]-hi*64);
			
			fmefectnum[y][x]=hi;
			
			hi=(int)(lo/16);
			lo=(int)(lo-hi*16);
	
			fmoct[y][x]=hi;
			fmfreq[y][x]=lo;
			
			/* Byte 1 - Valor do Efeito e Número do Sample */
			hi=(int)(patinfofm[i+j+1]/32);
			lo=(int)(patinfofm[i+j+1]-hi*32);
			
			switch (fmefectnum[y][x])
			{
				case 0:fmefectpar[y][x]=0;fmsamplenum[y][x]=lo;break;
				case 1:fmefectpar[y][x]=hi*32+lo;fmsamplenum[y][x]=0;break;
				case 2:fmefectpar[y][x]=0;fmsamplenum[y][x]=0;break;
				case 3:fmefectpar[y][x]=hi*9;fmsamplenum[y][x]=lo;break;
			}
			
			x++;
		}
		y++;
	}
	
}

/* Imprime se os Canais FM estão ON ou OFF */
void printcnlonfm(void)
{

	int i,j;
	int p,f;

	if (menu==0)
	{
		p=0;
		f=4;
	}
	else
	{
		p=4;
		f=FMNUNCANAIS;
	}
	
	j=0;
	for (i=p; i<f; i++)
	{
		if (cnlonfm[i]==1)
		{
			textbackground(2);
			textcolor(15);
			gotoxy((j*12)+5,7);
			cprintf(" ON ");
			textbackground(0);
			cprintf(" ");
		}
		else
		{
			textbackground(4);
			textcolor(15);
			gotoxy((j*12)+5,7);
			cprintf(" OFF ");
		}
		j++;
	}
	
	if (menu==0)
	{
		textbackground(0);
		cprintf("            ");
	}
	
}

/* Imprime a Informação da Pattern FM corrente */
void printpatfm(void)
{

	int i,j;
	int p,f;
	char o;
	
	if (menu==0)
	{
		p=0;
		f=4;
	}
	else
	{
		p=4;
		f=FMNUNCANAIS;
	}
	
	textbackground(0);
	textcolor(7);
	gotoxy(1,8);
	for (i=py; i<py+16; i++)
	{
		cprintf("%2d: ",i);
		
		for (j=p; j<f; j++)
		{
			if (fmoct[i][j]==0) o='-';
			else o=fmoct[i][j]+48;
		
			cprintf("%s%c %2d %d%02X  ",pconvnotas[fmfreq[i][j]],o,fmsamplenum[i][j],fmefectnum[i][j],fmefectpar[i][j]);
			
		}
		if (menu==0)cprintf("           ");
		cprintf("\r\n");
	}
	
}

/* Navega nas Patterns FM */
void navegapatfm(void)
{
	
	int i;
	
	do
	{
		
		while (!kbhit())
		{
			if (playfm==1) playmusicafm();
		};
	
		KB_code=getch();
		
		if (KB_code==KB_F1)					/* F1 - HELP */
		{
			estado=5;
			oldestadohlp=4;
			help=6;
		}
		
		if (KB_code==KB_P)	/* Toca a Música FM por Inteiro a partir da Position atual*/
		{
			row=0;
			playfm=1;
			fimsongfm=songlen;
			sprintf(txtsts,"Tocando Musica\0");
			printestado();
			
		//	gettime(&playstart);
			
		}
		
		if (KB_code==KB_U)	/* Toca apenas a Pattern FM atual*/
		{
			row=0;
			playfm=1;
			fimsongfm=position+1;
			sprintf(txtsts,"Tocando Pattern\0");
			printestado();
		}
		
		if (KB_code==KB_S)	/* Para a Música FM */
		{
			for (i=0; i<FMNUNCANAIS; i++) stopsomfm(i);
			row=0;
			playfm=0;
			sprintf(txtsts,"Musica Parada\0");
			printestado();
		}
		
		if (KB_code==KB_TAB)
		{
			menu=1-menu;
			alteracabconvfm();
			carregapatfm();
		}
			
		if (KB_code==KB_F2) estado=0;
		
		if (KB_code==KB_F3) estado=1;
		
		if (KB_code==KB_F4) estado=2;
		
		if (KB_code==KB_F5) estado=3;
		
		if (KB_code==KB_ESCAPE) estado=10;
	
		/* Posições */
		if (KB_code==KB_LEFT && position>0)
		{
			position--;
			carregapatfm();
		}
				
		if (KB_code==KB_RIGHT && position<songlen-1)
		{
			position++;
			carregapatfm();
		}
		
	
		if (KB_code==KB_CTRL_LEFT)
		{
			position=0;
			carregapatfm();
		}
				
		if (KB_code==KB_CTRL_RIGHT)
		{
			position=songlen-1;
			carregapatfm();
		}
		
		if (KB_code==KB_ALT_LEFT)
		{
			position-=10;
			if (position<0) position=0;
			carregapatfm();
		}
					
		if (KB_code==KB_ALT_RIGHT)
		{
			position+=10;
			if (position>songlen-1) position=songlen-1;
			carregapatfm();
		}
		
	
		/* Rows */
		if (KB_code==KB_UP && py>0) py--;
			
		if (KB_code==KB_DOWN && py<48) py++;
	
	
		if (KB_code==KB_HOME) py=0;
						
		if (KB_code==KB_END) py=48;

						
		if (KB_code==KB_PAGE_UP)
		{
			py-=16;
			if (py<0) py=0;
		}
	
		if (KB_code==KB_PAGE_DOWN)
		{
			py+=16;
			if (py>48) py=48;
		}
	
		printpatfm();
		
		/* Canais ON/OFF */
		if (KB_code>=KB_1 && KB_code<=KB_9)
		{
			cnlonfm[KB_code-48-1]=1-cnlonfm[KB_code-48-1];
			printcnlonfm();
		}
		
	}
	while (estado==4);
	
}

/* Toca a Música Convertida FM */
void playmusicafm(void)
{
	
	int i,j;
	int salta=0,saltap;

	if (position<fimsongfm)
	{
		
		i=row;
		for (j=0; j<FMNUNCANAIS; j++)
		{
						
			if (fmefectnum[i][j]==2) salta=1;	/* Salta para a Próxima Pattern */
			
			if (fmefectnum[i][j]==1)			/* Salta para a Posição Definida pelo Parâmetro */
			{
				salta=2;
				saltap=fmefectpar[i][j];
			}
				
			if (fmfreq[i][j]!=0 && salta==0)	/* Se a Frequência não for Zero e se não houver Saltos */
			{
				
				sifm=saminstfm[0][fmsamplenum[i][j]-1];
				
			//	if (fmefectnum[i][j]==3)		/* Se houver controlo de Volume na Pattern */
			//	{
			//		hi=(int)(instreg[sifm].op2reg4/64);
			//		volfm=hi+(63-fmefectpar[i][j]);
			//	}
			//	else volfm=instreg[sifm].op2reg4;
			
			//	volfm=instreg[sifm].op2reg4;
				
				volfm=volinstfm[fmsamplenum[i][j]-1];
			
				oitavafm=fmoct[i][j]+octinstfm[fmsamplenum[i][j]-1]; /* Correcção da Oitava */
				
				criasomfm(j,canaisfm[j],fmfreq[i][j]);
				
			}
		}

		/* Fica na Pattern Corrente e Passa para a Próxima Row */
		if (salta==0)
		{
			delay(115);
	
			row++;
			if (row==64)
			{
				row=0;
				position++;
				carregapatfm();
				printpatfm();
			}
		}
		
		/* Salta para a Próxima Posição */
		if (salta==1)
		{
			row=0;
			position++;
			carregapatfm();
			printpatfm();
		}
		
		/* Salta para a Posição definida pelo Parâmetro */
		if (salta==2)
		{
			row=0;
			position=saltap;
			carregapatfm();
			printpatfm();
		}
			
	}
	else
	{
		playfm=0;
		for (i=0; i<FMNUNCANAIS; i++) stopsomfm(i);
		sprintf(txtsts,"Musica Parada\0");
		printestado();
		
	/*	gettime(&playend);
		sprintf(txtsts,"%02d:%02d:%02d:%02d - %02d:%02d:%02d:%02d\0",playstart.ti_hour,playstart.ti_min,playstart.ti_sec,playstart.ti_hund,
			playend.ti_hour,playend.ti_min,playend.ti_sec,playend.ti_hund);
		printestado();
	*/	
	}

}

/* Reset a Atribuição de Instrumentos SAA1099 aos Samples */
void resetsaminstsa()
{
	
	int i;
	
	for (i=0; i<31; i++)
	{
		saminstsa[0][i]=128;
		saminstsa[1][i]=0;
		octinstsa[i]=0;
		volinstsa[i]=0;
	}
	
}

/* Reset a Atribuição de Canais MOD aos Canais SAA1099 */
void resetcnlmodcnlsa()
{
	int i;
	
	for (i=0; i<4; i++) cnlmodcnlsa[i]=i;
	for (i=4; i<SAANUNCANAIS; i++) cnlmodcnlsa[i]=4;
	
}

/* Menu de Conversão para SAA1099 - Estado = 2 */
void printsa(void)
{

	textbackground(0);
	textcolor(7);
	clrscr();
	
	/* Menu */
	textbackground(1);
	textcolor(15);
	gotoxy(1,1);
	cprintf(" F1-Help  F2-Modulo  F3-FM  F4-SAA1099  F5-AY/YM  F6-Play Conv         ESC-Sair ");
	gotoxy(1,25);
	cprintf("                                                                               ");
	
	textbackground(0);
	textcolor(11);
	gotoxy(19,3);
	cprintf("Pagina de Conversao de MOD para SAA1099");

	/* Menu */
	menu=0;
	menup=0;
	
	/* Navegação nos Samples */
	sy=0;
	ny=0;
	samx=2;
	samy=8;
	sami=16;
	
	/* Navegação nos Instrumentos SAA1099 */
	sisa=0;
	pisa=0;
	
	/* Navegação nos Canais do Módulo */
	cnlmody=4;
		
	/* Navegação nos Canais SAA1099 */
	cnlsay=0;

	/* Cabeçalhos */
	alteracabconv();
	alteracabpanel();

	/* Samples */
	printsam();
	
	/* Instrumentos SAA1099 */
	printinstsa();
	
	/* Instrumento SAA1099 atribuido ao Sample */
	printsaminstsatxt();
	printsaminstsa();
	
	/* Oitava */
	printocttxt();
	printoctsa();
	printvolsa();
		
	/* Estado */
	sprintf(txtsts,"Pagina de Conversao de MOD para SAA1099.\0");
	printestado();
	
}

/* Imprime a Informação dos Instrumentos SAA1099 */
void printinstsa(void)
{
	
	int i;

	for (i=pisa; i<pisa+16; i++)
	{
		
		gotoxy(29,i-pisa+8);
		textbackground(2);
		textcolor(15);	
				
		if (i==sisa) textbackground(1);
			
		cprintf("%-20s",instregsa[i].nome);
		
	}
	
	textbackground(0);
	
}

/* Imprime o Texto do Instrumento SAA1099 atribuido ao Sample selecionado */
void printsaminstsatxt(void)
{
	textcolor(10);
	gotoxy(54,13);cprintf("Instrumento SAA1099");
}

/* Imprime o Instrumento SAA1099 atribuido ao Sample selecionado */
void printsaminstsa(void)
{
	
	textcolor(15);
	gotoxy(55,14);
	if (saminstsa[0][ny]==128) cprintf("Sem Instrumento");
	else cprintf("%-20s",instregsa[saminstsa[0][ny]].nome);
	textbackground(0);
	
}

/* Imprime os Canais SAA1099 */
void printcnlsa(void)
{
	
	int i;
	
	for (i=9; i<15; i++)
	{
		gotoxy(3,i);
		textbackground(0);
		textcolor(15);
		
		if (i-9==cnlsay) textbackground(2);
		
		cprintf("Canal SAA %2d ",i-9);
	}
	
	textbackground(0);
	
}

/* Navega no Menu de Conversão para SAA1099 */
void navegasa(void)
{
	
	int f;
	
	do
	{
		
		while (!kbhit())
		{
			if (playenvlsa[0]!=0)
			{
				envelopsa(0);
				criasomsa(0,freqsa[0]);
			}
		}
		
		KB_code=getch();
		
		if (KB_code==KB_F1)					/* F1 - HELP */
		{
			estado=5;
			oldestadohlp=2;
			if (menu==0) help=2;
				else help=4;
		}
		
		if (KB_code==KB_F2) estado=0;		/* F2 - Módulo */
		
		if (KB_code==KB_F3) estado=1;		/* F3 - FM */
		
		if (KB_code==KB_F5) estado=3;		/* F5 - AY-3-8912 */
		
		if (KB_code==KB_F6)					/* F6 - Pattern Conv / Toca Música Convertida */
		{
			oldestado=2;
			estado=4;
		}
		
		if (KB_code==KB_CTRL_TAB)
		{
			menu=1-menu;
			
			alteracabconv();
			alteracabpanel();

			if (menu==0)
			{
				printsam();
				printinstsa();
				printocttxt();
				printoctsa();
				printvolsa();
				printsaminstsatxt();
				printsaminstsa();
			}
			else
			{
				printcnlsa();
				printcnlmod();
			}
			
		}
				
		if (KB_code==KB_ESCAPE) estado=10;
		
		if (KB_code==KB_TAB)
		{
			menup=1-menup;
			alteracabpanel();
		}
												
		if (menu==0)
		{
			if (menup==0) navegasam();
			else navegainstsa();
		}
		else
		{
			if (menup==0) navegacnlsa();
			else navegacnlmod();
		}
		
		/* Oitava */
		if (KB_code==KB_MENOS && octinstsa[ny]>-3)	/* Tecla - */
		{
			octinstsa[ny]--;
			printoctsa();
		}
		
		if (KB_code==KB_MAIS && octinstsa[ny]<7)	/* Tecla + */
		{
			octinstsa[ny]++;
			printoctsa();
		}
		
		/* Volume */
		if (KB_code==KB_V && volinstsa[ny]>0)	/* Tecla V */
		{
			volinstsa[ny]--;
			printvolsa();
		}
		
		if (KB_code==KB_B && volinstsa[ny]<15)	/* Tecla B */
		{
			volinstsa[ny]++;
			printvolsa();
		}
		
		/* Converte o MOD para SAA1099 */
		if (KB_code==KB_C) convertmodsa();
		
		/* Toca o Sample Selecionado */
		if (KB_code==KB_P) playsom();
		
		/* Toca o Instrumento SAA1099 Selecionado */
		if (KB_code==KB_I)
		{
			oitavasa[0]=2;
			precriasomsa(0,1);
		}
		
		/* Para o Instrumento SAA1099 Selecionado */
		if (KB_code==KB_O)
		{
			playenvlsa[0]=0;
			stopsomsa(0);
		}
		
		/* Reset às Atribuições Instrumentos/Canais */
		if (KB_code==KB_R)
		{
			if (menu==0)
			{
				resetsaminstsa();
				printsam();
				printinstsa();
				printocttxt();
				printoctsa();
				printvolsa();
				printsaminstsatxt();
				printsaminstsa();
				sprintf(txtsts,"Atribuicao de Instrumentos Anulada!\0");
				printestado();
			}
			else
			{	
				resetcnlmodcnlsa();
				printcnlsa();
				printcnlmod();
				sprintf(txtsts,"Atribuicao de Canais Anulada!\0");
				printestado();
			
			}
		}
		
		/* Grava o Estado SAA1099 */
		if (KB_code==KB_Q)
		{
			f=gravastssa();
			printestado();
			f++;
		}
		
		/* Lê o Estado SAA1099 */
		if (KB_code==KB_E)
		{
			f=lestssa();
			printestado();
			f++;
		}
		
		/* Grava a Música SA */
		if (KB_code==KB_G)
		{
			f=gravamusicasa();
			printestado();
			f++;
		}
		
	}
	while (estado==2);
	
}

/* Navega nos Instrumentos SAA1099 */
void navegainstsa(void)
{

	if (KB_code==KB_UP && sisa>0)		/* Cima */
	{
		sisa--;
		if (sisa<112 && pisa>0) pisa--;
	}
			
	if (KB_code==KB_DOWN && sisa<127) /* Baixo */
	{
		sisa++;
		if (sisa>15 && pisa<112) pisa++;
	}
	
	
	if (KB_code==KB_HOME)	/* Home */
	{
		pisa=0;
		sisa=0;
	}
						
	if (KB_code==KB_END)	/* End */
	{
		pisa=112;
		sisa=127;
	}

						
	if (KB_code==KB_PAGE_UP)	/* Page UP */
	{
		pisa-=16;
		if (pisa<0) pisa=0;
		sisa=pisa;
	}
	
	if (KB_code==KB_PAGE_DOWN)	/* Page DOWN */
	{
		pisa+=16;
		if (pisa>112) pisa=112;
		sisa=pisa;
	}
	
	if (KB_code==KB_RETURN && tisample[ny])	/* RETURN - Atribui o Instrumento SAA1099 para o Sample Selecionado */
	{
		saminstsa[0][ny]=sisa;
		saminstsa[1][ny]=pisa;
		volinstsa[ny]=instregsa[sisa].volume;
		printsaminstsa();
		printoctsa();
		printvolsa();
		sprintf(txtsts,"Instrumento '%s' atribuido ao Sample '%s'\0",instregsa[sisa].nome,nomesample[ny]);
		printestado();
	}
	
	printinstsa();
		
}

/* Navega nos Canais SAA1099 */
void navegacnlsa(void)
{
	
	if (KB_code==KB_UP && cnlsay>0) cnlsay--;		/* Cima */
				
	if (KB_code==KB_DOWN && cnlsay<5) cnlsay++;	/* Baixo */
	
	
	if (KB_code==KB_HOME || KB_code==KB_PAGE_UP) cnlsay=0;		/* Home */
							
	if (KB_code==KB_END || KB_code==KB_PAGE_DOWN) cnlsay=5;	/* End */
	
	if (KB_code==KB_DEL)	/* Delete - Remove o Canal MOD Atribuído ao Canal FM Selecionado */
	{
		cnlmodcnlsa[cnlsay]=4;
		sprintf(txtsts,"Canal MOD atribuido ao Canal SAA1099 %d Removido\0",cnlmody,cnlsay);
		printestado();
	}
			
	cnlmody=cnlmodcnlsa[cnlsay];
	printcnlmod();
	printcnlsa();
	
}

/* Converte o MOD para SAA1099 */
void convertmodsa(void)
{
	
	int i,j;
	int x,y;
	int p,h;
	
	unsigned char saefect,safectpar,sasam;
	
	memset(patinfosa,0,SIZEPATINFOSA*maxpat);
	
	h=0;
	/* Percorre por todas as Patterns */
	for (pattern=0; pattern<maxpat; pattern++)
	{
		
		/* Percorre as 64 Rows da respetiva Pattern */
		y=0;
		for (i=pattern*SIZEPATINFOMOD; i<((pattern*SIZEPATINFOMOD)+SIZEPATINFOMOD); i+=SIZEROWMOD)
		{
			/* Percorre os 4 Canais da respetiva Row */
			x=0;
			for(j=0; j<SIZEROWMOD; j+=4)
			{
				hi=(int)(patinfo[i+j+0]/16);
				lo=(int)(patinfo[i+j+0]-hi*16);
	
				period[y][x]=lo*256+patinfo[i+j+1];
		
				lo=(int)(patinfo[i+j+2]/16);
				samplenum[y][x]=hi*16+lo;
	
				efectnum[y][x]=(int)(patinfo[i+j+2]-lo*16);
				efectpar[y][x]=patinfo[i+j+3];
				
				/* Percorre todos os Canais SAA1099 */
				for (p=0; p<SAANUNCANAIS; p++)
				{
					
					/* Verifica se o Canal SAA1099 está ativo para o respetivo Canal MOD */	
					if (cnlmodcnlsa[p]==x)
					{
						convfrntsa(period[y][x]); 	/* Frequência e Oitava 	*/
						sasam=samplenum[y][x];		/* Número do Sample 	*/
						saefect=0;					/* Número do Efeito		*/
						safectpar=0;				/* Parâmetro do Efeito	*/
						
						switch (efectnum[y][x])
						{
							case 11:saefect=1;safectpar=efectpar[y][x];break;			/* Salta para Pattern */
							case 13:saefect=2;safectpar=0;break;						/* Salta para a próxima Pattern */
							case 12:saefect=3;safectpar=(char)(efectpar[y][x]/9);break; /* Volume */
						}
						
						if (sasam==0)	/* Se não houver Sample Atribuído */
						{
							oitavasacnv=0;
							freqsacnv=0;
						}
						
						if (period[y][x]==0)	/* Se não houver Frequência */
						{
							sasam=0;
							if (saefect==3 && safectpar!=0)	/* Se o Volume for Diferente de 0 */
							{
								saefect=0;
								safectpar=0;
							}
						}
						
						/* Verifica se o Sample tem um Instrumento SAA1099 Atribuído */
						if (saminstsa[0][sasam-1]!=128)
						{
							patinfosa[h+p*2]=saefect*64+oitavasacnv*16+freqsacnv;
							if (saefect==1) patinfosa[h+p*2+1]=safectpar;
							else patinfosa[h+p*2+1]=safectpar*32+sasam;
						}
						else
						{
							patinfosa[h+p*2]=0;
							patinfosa[h+p*2+1]=0;
						}
					}
				}
				x++;
			}
			y++;
			h+=SIZEROWSA;
		}
	}
	
	sprintf(txtsts,"Modulo Convertido para SAA1099 com Sucesso! F6 para toca-lo.\0");
	printestado();

}

/* Converte as Frequências do MOD para as Frequências SAA1099 */
void convfrntsa(int freq)
{
	
	oitavasacnv=0;
	freqsacnv=0;
	
	switch (freq)
	{
		case 0:freqsacnv=0;oitavasacnv=0;break;
		
		case 856:freqsacnv=1;oitavasacnv=1;break;
		case 808:freqsacnv=2;oitavasacnv=1;break;
		case 762:freqsacnv=3;oitavasacnv=1;break;
		case 763:freqsacnv=3;oitavasacnv=1;break;
		case 720:freqsacnv=4;oitavasacnv=1;break;
		case 678:freqsacnv=5;oitavasacnv=1;break;
		case 679:freqsacnv=5;oitavasacnv=1;break;
		case 640:freqsacnv=6;oitavasacnv=1;break;
		case 641:freqsacnv=6;oitavasacnv=1;break;
		case 604:freqsacnv=7;oitavasacnv=1;break;
		case 605:freqsacnv=7;oitavasacnv=1;break;
		case 570:freqsacnv=8;oitavasacnv=1;break;
		case 571:freqsacnv=8;oitavasacnv=1;break;
		case 538:freqsacnv=9;oitavasacnv=1;break;
		case 539:freqsacnv=9;oitavasacnv=1;break;
		case 508:freqsacnv=10;oitavasacnv=1;break;
		case 509:freqsacnv=10;oitavasacnv=1;break;
		case 480:freqsacnv=11;oitavasacnv=1;break;
		case 453:freqsacnv=12;oitavasacnv=1;break;
		
		case 428:freqsacnv=1;oitavasacnv=2;break;
		case 404:freqsacnv=2;oitavasacnv=2;break;
		case 381:freqsacnv=3;oitavasacnv=2;break;
		case 360:freqsacnv=4;oitavasacnv=2;break;
		case 339:freqsacnv=5;oitavasacnv=2;break;
		case 340:freqsacnv=5;oitavasacnv=2;break;
		case 320:freqsacnv=6;oitavasacnv=2;break;
		case 321:freqsacnv=6;oitavasacnv=2;break;
		case 302:freqsacnv=7;oitavasacnv=2;break;
		case 303:freqsacnv=7;oitavasacnv=2;break;
		case 285:freqsacnv=8;oitavasacnv=2;break;
		case 286:freqsacnv=8;oitavasacnv=2;break;
		case 269:freqsacnv=9;oitavasacnv=2;break;
		case 270:freqsacnv=9;oitavasacnv=2;break;
		case 254:freqsacnv=10;oitavasacnv=2;break;
		case 240:freqsacnv=11;oitavasacnv=2;break;
		case 226:freqsacnv=12;oitavasacnv=2;break;
		case 227:freqsacnv=12;oitavasacnv=2;break;

		case 214:freqsacnv=1;oitavasacnv=3;break;
		case 202:freqsacnv=2;oitavasacnv=3;break;
		case 190:freqsacnv=3;oitavasacnv=3;break;
		case 191:freqsacnv=3;oitavasacnv=3;break;
		case 180:freqsacnv=4;oitavasacnv=3;break;
		case 170:freqsacnv=5;oitavasacnv=3;break;
		case 160:freqsacnv=6;oitavasacnv=3;break;
		case 151:freqsacnv=7;oitavasacnv=3;break;
		case 143:freqsacnv=8;oitavasacnv=3;break;
		case 135:freqsacnv=9;oitavasacnv=3;break;
		case 127:freqsacnv=10;oitavasacnv=3;break;
		case 120:freqsacnv=11;oitavasacnv=3;break;
		case 113:freqsacnv=12;oitavasacnv=3;break;
		
	}
	
}

/* Imprime as Patterns SAA1099 depois de Convertidas */
void printpatconvsa(void)
{
	
	int i;
	
	textbackground(0);
	textcolor(7);
	clrscr();
	
	/* Menu */
	textbackground(1);
	textcolor(15);
	gotoxy(1,1);
	cprintf(" F1-Help  F2-Modulo  F3-FM  F4-SAA1099  F5-AY/YM  F6-Play Conv         ESC-Sair ");
	gotoxy(1,25);
	cprintf("                                                                               ");
		
	/* Menu */
	menu=0;
	
	playsa=0;
	
	/* Cabeçalho */
	alteracabconvsa();
		
	/* Navegação nas Patterns SAA1099 */
	position=0;
	py=0;
	
	/* Patterns SAA1099*/
	carregapatsa();
	printpatsa();
	
	/* Estado */
	sprintf(txtsts,"Tecla P para tocar musica SAA1099 convertida.\0");
	printestado();
	
}

/* Altera os Canais Visíveis no Menu Patterns SAA1099 depois de Convertidas*/
void alteracabconvsa(void)
{

	textbackground(0);
	textcolor(11);
	
	gotoxy(1,3);
	cprintf("Patterns SAA1099 : ");
//	if (menu==0)
//	{
		textcolor(10);
		cprintf("Canais 0 - 5");
/*	}
	else
	{
		textcolor(10);
		cprintf("Canais 6 - 11");
	}
*/	
	printcnlonsa();
	
}

/* Lê a Informação da Pattern SAA1099 corrente */
void carregapatsa(void)
{

	int i,j;
	int x,y;
	
	pattern=songpos[position];
	py=0;
	
	textbackground(0);
	textcolor(14);
	gotoxy(1,5);
	cprintf("Position - %d , Pattern - %d  \n\n",position,pattern);

	y=0;
	for (i=pattern*SIZEPATINFOSA; i<((pattern*SIZEPATINFOSA)+SIZEPATINFOSA); i+=SIZEROWSA)
	{
		x=0;
		for(j=0; j<SIZEROWSA; j+=2)
		{
			
			/* Byte 0 - Frequência, Oitava e Número do Efeito */
			hi=(int)(patinfosa[i+j+0]/64);
			lo=(int)(patinfosa[i+j+0]-hi*64);
			
			saefectnum[y][x]=hi;
			
			hi=(int)(lo/16);
			lo=(int)(lo-hi*16);
	
			saoct[y][x]=hi;
			safreq[y][x]=lo;
			
			/* Byte 1 - Valor do Efeito e Número do Sample */
			hi=(int)(patinfosa[i+j+1]/32);
			lo=(int)(patinfosa[i+j+1]-hi*32);
			
			switch (saefectnum[y][x])
			{
				case 0:saefectpar[y][x]=0;sasamplenum[y][x]=lo;break;
				case 1:saefectpar[y][x]=hi*32+lo;sasamplenum[y][x]=0;break;
				case 2:saefectpar[y][x]=0;sasamplenum[y][x]=0;break;
				case 3:saefectpar[y][x]=hi*2;sasamplenum[y][x]=lo;break;
			}
			
			x++;
		}
		y++;
	}
	
}

/* Imprime se os Canais SAA1099 estão ON ou OFF */
void printcnlonsa(void)
{

	int i,j;
	int p,f;

	p=0;
	f=SAANUNCANAIS;
	
	j=0;
	for (i=p; i<f; i++)
	{
		if (cnlonsa[i]==1)
		{
			textbackground(2);
			textcolor(15);
			gotoxy((j*12)+5,7);
			cprintf(" ON ");
			textbackground(0);
			cprintf(" ");
		}
		else
		{
			textbackground(4);
			textcolor(15);
			gotoxy((j*12)+5,7);
			cprintf(" OFF ");
		}
		j++;
	}
	
}

/* Imprime a Informação da Pattern SAA1099 corrente */
void printpatsa(void)
{

	int i,j;
	int p,f;
	char o;
	
	p=0;
	f=SAANUNCANAIS;
	
	textbackground(0);
	textcolor(7);
	gotoxy(1,8);
	for (i=py; i<py+16; i++)
	{
		cprintf("%2d: ",i);
		
		for (j=p; j<f; j++)
		{
			if (saoct[i][j]==0) o='-';
			else o=saoct[i][j]+48;
		
			cprintf("%s%c %2d %d%02X  ",pconvnotas[safreq[i][j]],o,sasamplenum[i][j],saefectnum[i][j],saefectpar[i][j]);
		}	
		cprintf("\r\n");
	}
	
}

/* Navega nas Patterns SAA1099 */
void navegapatsa(void)
{
	
	int i;
	
	do
	{
		
		while (!kbhit())
		{
			if (playsa==1)
			{
				for (i=0; i<SAANUNCANAIS; i++)
					if (playenvlsa[i]!=0)
					{
						sisa=playenvlsa[i];
					//	sisa=canalinstsa[i];
						envelopsa(i);
					//	criasomsa(i,freqsa[i]);
						setvolsa(i);
					}

				playsaend=letempo();
				if (playsaend-playsastart>=11) playmusicasa();
			}
		};
	
		KB_code=getch();
		
		if (KB_code==KB_F1)					/* F1 - HELP */
		{
			estado=5;
			oldestadohlp=4;
			help=6;
		}
		
		if (KB_code==KB_P)	/* Toca a Música SA por Inteiro a partir da Position atual*/
		{
			row=0;
			playsa=1;
			fimsongsa=songlen;
			sprintf(txtsts,"Tocando Musica\0");
			printestado();
			
		//	gettime(&playstart);
			
		}
		
		if (KB_code==KB_U)	/* Toca apenas a Pattern SA atual*/
		{
			row=0;
			playsa=1;
			fimsongsa=position+1;
			sprintf(txtsts,"Tocando Pattern\0");
			printestado();
		}
		
		if (KB_code==KB_S) /* Para a Música SA */
		{
			for (i=0; i<SAANUNCANAIS; i++) stopsomsa(i);
			row=0;
			playsa=0;
			sprintf(txtsts,"Musica Parada\0");
			printestado();
		}
		
		if (KB_code==KB_F2) estado=0;
		
		if (KB_code==KB_F3) estado=1;
		
		if (KB_code==KB_F4) estado=2;
		
		if (KB_code==KB_F5) estado=3;
		
		if (KB_code==KB_ESCAPE) estado=10;
	
		/* Posições */
		if (KB_code==KB_LEFT && position>0)
		{
			position--;
			carregapatsa();
		}
				
		if (KB_code==KB_RIGHT && position<songlen-1)
		{
			position++;
			carregapatsa();
		}
		
	
		if (KB_code==KB_CTRL_LEFT)
		{
			position=0;
			carregapatsa();
		}
				
		if (KB_code==KB_CTRL_RIGHT)
		{
			position=songlen-1;
			carregapatsa();
		}
		
		if (KB_code==KB_ALT_LEFT)
		{
			position-=10;
			if (position<0) position=0;
			carregapatsa();
		}
					
		if (KB_code==KB_ALT_RIGHT)
		{
			position+=10;
			if (position>songlen-1) position=songlen-1;
			carregapatsa();
		}
		
	
		/* Rows */
		if (KB_code==KB_UP && py>0) py--;
			
		if (KB_code==KB_DOWN && py<48) py++;
	
	
		if (KB_code==KB_HOME) py=0;
						
		if (KB_code==KB_END) py=48;

						
		if (KB_code==KB_PAGE_UP)
		{
			py-=16;
			if (py<0) py=0;
		}
	
		if (KB_code==KB_PAGE_DOWN)
		{
			py+=16;
			if (py>48) py=48;
		}
	
		printpatsa();
		
		/* Canais ON/OFF */
		if (KB_code>=KB_1 && KB_code<=KB_6)
		{
			cnlonsa[KB_code-48-1]=1-cnlonsa[KB_code-48-1];
			printcnlonsa();
		}
		
	}
	while (estado==4);
	
}

/* Toca a Música Convertida SAA1099 */
void playmusicasa(void)
{
	
	int i,j;
	int salta=0,saltap;

	playsastart=letempo();

	if (position<fimsongsa)
	{
		
		i=row;
		for (j=0; j<SAANUNCANAIS; j++)
		{
						
			if (saefectnum[i][j]==2) salta=1;			/* Salta para a Próxima Pattern */
			
			if (saefectnum[i][j]==1)					/* Salta para a Posição Definida pelo Parâmetro */
			{
				salta=2;
				saltap=saefectpar[i][j];
			}
				
			if (safreq[i][j]!=0 && salta==0)			/* Se a Frequência não for Zero e se não houver Saltos */
			{
				
				sisa=saminstsa[0][sasamplenum[i][j]-1];
				
			//	if (saefectnum[i][j]==3)		/* Se houver controlo de Volume na Pattern */
			//		volumesa[j]=saefectpar[i][j];
			//	else volumesa[j]=instregsa[sisa].volume;
			
			//	volumesa[j]=instregsa[sisa].volume;
				volumesa[j]=volinstsa[sasamplenum[i][j]-1];
				
				//canalinstsa[j]=sisa;
			
				oitavasa[j]=saoct[i][j]+octinstsa[sasamplenum[i][j]-1];	/* Correcção da Oitava */
				
				if (safreq[i][j]>9) oitavasa[j]++;
				
				precriasomsa(j,safreq[i][j]);
			//	criasomsa(j,safreq[i][j]);
				
			}
		}

		/* Fica na Pattern Corrente e Passa para a Próxima Row */
		if (salta==0)
		{
			
		//	delay(125);
	
			row++;
			if (row==64)
			{
				row=0;
				position++;
				carregapatsa();
				printpatsa();
			}
		}
		
		/* Salta para a Próxima Posição */
		if (salta==1)
		{
			row=0;
			position++;
			carregapatsa();
			printpatsa();
		}
		
		/* Salta para a Posição definida pelo Parâmetro */
		if (salta==2)
		{
			row=0;
			position=saltap;
			carregapatsa();
			printpatsa();
		}
			
	}
	else
	{
		for (i=0; i<SAANUNCANAIS; i++) stopsomsa(i);
		row=0;
		playsa=0;
		sprintf(txtsts,"Musica Parada\0");
		printestado();
		
	/*	gettime(&playend);
		sprintf(txtsts,"%02d:%02d:%02d:%02d - %02d:%02d:%02d:%02d\0",playstart.ti_hour,playstart.ti_min,playstart.ti_sec,playstart.ti_hund,
			playend.ti_hour,playend.ti_min,playend.ti_sec,playend.ti_hund);
		printestado();
	*/	
	}

}

///////////////////////////////////////////*************************///////////////

/* Reset a Atribuição de Instrumentos AY-3-8912 aos Samples */
void resetsaminstay()
{
	
	int i;
	
	for (i=0; i<31; i++)
	{
		saminstay[0][i]=128;
		saminstay[1][i]=0;
		octinstay[i]=2;
		volinstay[i]=0;
	}
	
}

/* Reset a Atribuição de Canais MOD aos Canais AY-3-8912 */
void resetcnlmodcnlay()
{
	int i;
	
	for (i=0; i<AYNUNCANAIS; i++) cnlmodcnlay[i]=i;
	
}

/* Menu de Conversão para AY-3-8912 - Estado = 3 */
void printay(void)
{

	textbackground(0);
	textcolor(7);
	clrscr();
	
	/* Menu */
	textbackground(1);
	textcolor(15);
	gotoxy(1,1);
	cprintf(" F1-Help  F2-Modulo  F3-FM  F4-SAA1099  F5-AY/YM  F6-Play Conv         ESC-Sair ");
	gotoxy(1,25);
	cprintf("                                                                               ");
	
	textbackground(0);
	textcolor(11);
	gotoxy(17,3);
	cprintf("Pagina de Conversao de MOD para AY-3-8912");

	/* Menu */
	menu=0;
	menup=0;
	
	/* Navegação nos Samples */
	sy=0;
	ny=0;
	samx=2;
	samy=8;
	sami=16;
	
	/* Navegação nos Instrumentos AY-3-8912 */
	siay=0;
	piay=0;
	
	/* Navegação nos Canais do Módulo */
	cnlmody=4;
		
	/* Navegação nos Canais AY-3-8912 */
	cnlayy=0;

	/* Cabeçalhos */
	alteracabconv();
	alteracabpanel();

	/* Samples */
	printsam();
	
	/* Instrumentos AY-3-8912 */
	printinstay();
	
	/* Instrumento AY-3-8912 atribuido ao Sample */
	printsaminstaytxt();
	printsaminstay();
	
	/* Oitava */
	printocttxt();
	printoctay();
	printvolay();
		
	/* Estado */
	sprintf(txtsts,"Pagina de Conversao de MOD para AY-3-8912.\0");
	printestado();
	
}

/* Imprime a Informação dos Instrumentos AY-3-8912 */
void printinstay(void)
{
	
	int i;

	for (i=piay; i<piay+16; i++)
	{
		
		gotoxy(29,i-piay+8);
		textbackground(2);
		textcolor(15);	
				
		if (i==siay) textbackground(1);
			
		cprintf("%-20s",instregay[i].nome);
		
	}
	
	textbackground(0);
	
}

/* Imprime o Texto do Instrumento AY-3-8912 atribuido ao Sample selecionado */
void printsaminstaytxt(void)
{
	textcolor(10);
	gotoxy(54,13);cprintf("Instrumento AY-3-8912");
}

/* Imprime o Instrumento AY-3-8912 atribuido ao Sample selecionado */
void printsaminstay(void)
{
	
	textcolor(15);
	gotoxy(55,14);
	if (saminstay[0][ny]==128) cprintf("Sem Instrumento");
	else cprintf("%-20s",instregay[saminstay[0][ny]].nome);
	textbackground(0);
	
}

/* Imprime os Canais AY-3-8912 */
void printcnlay(void)
{
	
	int i;
	
	for (i=9; i<12; i++)
	{
		gotoxy(3,i);
		textbackground(0);
		textcolor(15);
		
		if (i-9==cnlayy) textbackground(2);
		
		cprintf("Canal AY %2d ",i-9);
	}
	
	textbackground(0);
	
}

/* Navega no Menu de Conversão para AY-3-8912 */
void navegaay(void)
{
	
	int f;
	
	do
	{
		
		while (!kbhit())
		{
			if (playenvlay[0]!=0)
			{
				envelopay(0);
				criasomay(0,freqay[0]);
			//	setvolay(0);
			}
			
		}
		
		KB_code=getch();
		
		if (KB_code==KB_F1)					/* F1 - HELP */
		{
			estado=5;
			oldestadohlp=3;
			if (menu==0) help=2;
				else help=4;
		}
		
		if (KB_code==KB_F2) estado=0;		/* F2 - Módulo */
		
		if (KB_code==KB_F3) estado=1;		/* F3 - FM */
		
		if (KB_code==KB_F4) estado=2;		/* F4 - SAA1099 */
		
		if (KB_code==KB_F6)					/* F6 - Pattern Conv / Toca Música Convertida */
		{
			oldestado=3;
			estado=4;
		}
		
		if (KB_code==KB_CTRL_TAB)
		{
			menu=1-menu;
			
			alteracabconv();
			alteracabpanel();

			if (menu==0)
			{
				printsam();
				printinstay();
				printocttxt();
				printoctay();
				printvolay();
				printsaminstaytxt();
				printsaminstay();
			}
			else
			{
				printcnlay();
				printcnlmod();
			}
			
		}
				
		if (KB_code==KB_ESCAPE) estado=10;
		
		if (KB_code==KB_TAB)
		{
			menup=1-menup;
			alteracabpanel();
		}
												
		if (menu==0)
		{
			if (menup==0) navegasam();
			else navegainstay();
		}
		else
		{
			if (menup==0) navegacnlay();
			else navegacnlmod();
		}
		
		/* Oitava */
		if (KB_code==KB_MENOS && octinstay[ny]>-3)	/* Tecla - */
		{
			octinstay[ny]--;
			printoctay();
		}
		
		if (KB_code==KB_MAIS && octinstay[ny]<7)	/* Tecla + */
		{
			octinstay[ny]++;
			printoctay();
		}
		
		/* Volume */
		if (KB_code==KB_V && volinstay[ny]>0)	/* Tecla V */
		{
			volinstay[ny]--;
			printvolay();
		}
		
		if (KB_code==KB_B && volinstay[ny]<15)	/* Tecla B */
		{
			volinstay[ny]++;
			printvolay();
		}
		
		/* Converte o MOD para AY-3-8912 */
		if (KB_code==KB_C) convertmoday();
		
		/* Toca o Sample Selecionado */
		if (KB_code==KB_P) playsom();
		
		/* Toca o Instrumento AY-3-8912 Selecionado */
		if (KB_code==KB_I)
		{
			oitavaay[0]=2;
			precriasomay(0,1);
		}
		
		/* Para o Instrumento AY-3-8912 Selecionado */
		if (KB_code==KB_O)
		{
			playenvlay[0]=0;
			stopsomay(0);
		}
		
		/* Reset às Atribuições Instrumentos/Canais */
		if (KB_code==KB_R)
		{
			if (menu==0)
			{
				resetsaminstay();
				printsam();
				printinstay();
				printocttxt();
				printoctay();
				printvolay();
				printsaminstaytxt();
				printsaminstay();
				sprintf(txtsts,"Atribuicao de Instrumentos Anulada!\0");
				printestado();
			}
			else
			{	
				resetcnlmodcnlay();
				printcnlay();
				printcnlmod();
				sprintf(txtsts,"Atribuicao de Canais Anulada!\0");
				printestado();
			
			}
		}
		
		/* Grava o Estado AY-3-8912 */
		if (KB_code==KB_Q)
		{
			f=gravastsay();
			printestado();
			f++;
		}
		
		/* Lê o Estado AY-3-8912 */
		if (KB_code==KB_E)
		{
			f=lestsay();
			printestado();
			f++;
		}
		
		/* Grava a Música AY-3-8912 */
		if (KB_code==KB_G)
		{
			f=gravamusicaay();
			f=gravaayplayer();
			printestado();
			f++;
		}
		
	}
	while (estado==3);
	
}

/* Navega nos Instrumentos AY-3-8912 */
void navegainstay(void)
{

	if (KB_code==KB_UP && siay>0)		/* Cima */
	{
		siay--;
		if (siay<112 && piay>0) piay--;
	}
			
	if (KB_code==KB_DOWN && siay<127) /* Baixo */
	{
		siay++;
		if (siay>15 && piay<112) piay++;
	}
	
	
	if (KB_code==KB_HOME)	/* Home */
	{
		piay=0;
		siay=0;
	}
						
	if (KB_code==KB_END)	/* End */
	{
		piay=112;
		siay=127;
	}

						
	if (KB_code==KB_PAGE_UP)	/* Page UP */
	{
		piay-=16;
		if (piay<0) piay=0;
		siay=piay;
	}
	
	if (KB_code==KB_PAGE_DOWN)	/* Page DOWN */
	{
		piay+=16;
		if (piay>112) piay=112;
		siay=piay;
	}
	
	if (KB_code==KB_RETURN && tisample[ny])	/* RETURN - Atribui o Instrumento AY-3-8912 para o Sample Selecionado */
	{
		saminstay[0][ny]=siay;
		saminstay[1][ny]=piay;
		volinstay[ny]=instregay[siay].volume;
		printsaminstay();
		printoctay();
		printvolay();
		sprintf(txtsts,"Instrumento '%s' atribuido ao Sample '%s'\0",instregay[siay].nome,nomesample[ny]);
		printestado();
	}
	
	printinstay();
		
}

/* Navega nos Canais AY-3-8912 */
void navegacnlay(void)
{
	
	if (KB_code==KB_UP && cnlayy>0) cnlayy--;		/* Cima */
				
	if (KB_code==KB_DOWN && cnlayy<2) cnlayy++;	/* Baixo */
	
	
	if (KB_code==KB_HOME || KB_code==KB_PAGE_UP) cnlayy=0;		/* Home */
							
	if (KB_code==KB_END || KB_code==KB_PAGE_DOWN) cnlayy=2;	/* End */
	
	if (KB_code==KB_DEL)	/* Delete - Remove o Canal MOD Atribuído ao Canal AY Selecionado */
	{
		cnlmodcnlay[cnlayy]=4;
		sprintf(txtsts,"Canal MOD atribuido ao Canal AY-3-8912 %d Removido\0",cnlmody,cnlayy);
		printestado();
	}
			
	cnlmody=cnlmodcnlay[cnlayy];
	printcnlmod();
	printcnlay();
	
}

/* Converte o MOD para AY-3-8912 */
void convertmoday(void)
{
	
	int i,j;
	int x,y;
	int p,h;
	int hic,loc;
	int hic2,loc2;
	
	unsigned char ayefect,ayfectpar,aysam;
	
	memset(patinfoay,0,SIZEPATINFOAY*maxpat);
		
	h=0;
	/* Percorre por todas as Patterns */
	for (pattern=0; pattern<maxpat; pattern++)
	{
		
		/* Percorre as 64 Rows da respetiva Pattern */
		y=0;
		for (i=pattern*SIZEPATINFOMOD; i<((pattern*SIZEPATINFOMOD)+SIZEPATINFOMOD); i+=SIZEROWMOD)
		{
			/* Percorre os 4 Canais da respetiva Row */
			x=0;
			ayefect=0;					/* Número do Efeito		*/
			ayfectpar=0;				/* Parâmetro do Efeito	*/
			for(j=0; j<SIZEROWMOD; j+=4)
			{
				hi=(int)(patinfo[i+j+0]/16);
				lo=(int)(patinfo[i+j+0]-hi*16);
	
				period[y][x]=lo*256+patinfo[i+j+1];
		
				lo=(int)(patinfo[i+j+2]/16);
				samplenum[y][x]=hi*16+lo;
	
				efectnum[y][x]=(int)(patinfo[i+j+2]-lo*16);
				efectpar[y][x]=patinfo[i+j+3];
				
				/* Percorre todos os Canais AY-3-8912 */
				for (p=0; p<AYNUNCANAIS; p++)
				{
					
					/* Verifica se o Canal AY-3-8912 está ativo para o respetivo Canal MOD */	
					if (cnlmodcnlay[p]==x)
					{
						convfrntay(period[y][x]); 	/* Frequência e Oitava 	*/
						aysam=samplenum[y][x];		/* Número do Sample 	*/
												
						switch (efectnum[y][x])
						{
							case 11:ayefect=1;ayfectpar=efectpar[y][x];break;	/* Salta para Pattern */
							case 13:ayefect=2;ayfectpar=0;break;				/* Salta para a próxima Pattern */
						//	case 12:ayefect=3;ayfectpar=(char)(efectpar[y][x]/9);break; /* Volume */
						}
						
						if (aysam==0)	/* Se não houver Sample Atribuído */
						{
							oitavaaycnv=0;
							freqaycnv=0;
						}
						
						if (period[y][x]==0)	/* Se não houver Frequência */
						{
							aysam=0;
						//	ayefect=0;
						//	ayfectpar=0;
						}
												
						/* Verifica se o Sample tem um Instrumento AY-3-8912 Atribuído */
						if (saminstay[0][aysam-1]!=128)
						{
							/* Byte 0,1,2 */
							hi=(int)(aysam/4);
							lo=(int)(aysam-hi*4);
							patinfoay[h+p]=lo*64+oitavaaycnv*16+freqaycnv;
													
							/* Byte 3 */
							switch (p)
							{
								case 0:
								{
									hic=(int)(patinfoay[h+3]/64);
									loc=(int)(patinfoay[h+3]-hic*64);
									hic=(int)(loc/4);
									patinfoay[h+3]=hi*64+hic*4+ayefect;
																	
								//	patinfoay[h+3]=hi*64+ayefect;
									break;
								}
								case 1: 
								{
									hic=(int)(patinfoay[h+3]/64);
									
									loc=(int)(patinfoay[h+3]-hic*64);
									
									hic2=(int)(loc/4);
									
									loc2=(int)(hic2/2);
									hic2=(int)(hic2-loc2*2);
									
									patinfoay[h+3]=hic*64+hi*16+hic2*4+ayefect;
								
								//	patinfoay[h+3]=hic*64+hi*16+ayefect;
									break;
								}
								case 2: 
								{
									hic=(int)(patinfoay[h+3]/16);
									patinfoay[h+3]=hic*16+hi*4+ayefect;
									break;
								}
							}
														
						}
						else
						{
							patinfoay[h+p]=0;
						//	patinfoay[h+3]=0;
						}
					}
					
				}
				
				if (ayefect!=0) patinfoay[h+3]=ayfectpar*2+ayefect;
				x++;
			}
			y++;
			h+=SIZEROWAY;
		}
	}
	
	sprintf(txtsts,"Modulo Convertido para AY-3-8912 com Sucesso! F6 para toca-lo.\0");
	printestado();

}

/* Converte as Frequências do MOD para as Frequências AY-3-8912 */
void convfrntay(int freq)
{
	
	oitavaaycnv=0;
	freqaycnv=0;
	
	switch (freq)
	{
		case 0:freqaycnv=0;oitavaaycnv=0;break;
		
		case 856:freqaycnv=1;oitavaaycnv=1;break;
		case 808:freqaycnv=2;oitavaaycnv=1;break;
		case 762:freqaycnv=3;oitavaaycnv=1;break;
		case 763:freqaycnv=3;oitavaaycnv=1;break;
		case 720:freqaycnv=4;oitavaaycnv=1;break;
		case 678:freqaycnv=5;oitavaaycnv=1;break;
		case 679:freqaycnv=5;oitavaaycnv=1;break;
		case 640:freqaycnv=6;oitavaaycnv=1;break;
		case 641:freqaycnv=6;oitavaaycnv=1;break;
		case 604:freqaycnv=7;oitavaaycnv=1;break;
		case 605:freqaycnv=7;oitavaaycnv=1;break;
		case 570:freqaycnv=8;oitavaaycnv=1;break;
		case 571:freqaycnv=8;oitavaaycnv=1;break;
		case 538:freqaycnv=9;oitavaaycnv=1;break;
		case 539:freqaycnv=9;oitavaaycnv=1;break;
		case 508:freqaycnv=10;oitavaaycnv=1;break;
		case 509:freqaycnv=10;oitavaaycnv=1;break;
		case 480:freqaycnv=11;oitavaaycnv=1;break;
		case 453:freqaycnv=12;oitavaaycnv=1;break;
		
		case 428:freqaycnv=1;oitavaaycnv=2;break;
		case 404:freqaycnv=2;oitavaaycnv=2;break;
		case 381:freqaycnv=3;oitavaaycnv=2;break;
		case 360:freqaycnv=4;oitavaaycnv=2;break;
		case 339:freqaycnv=5;oitavaaycnv=2;break;
		case 340:freqaycnv=5;oitavaaycnv=2;break;
		case 320:freqaycnv=6;oitavaaycnv=2;break;
		case 321:freqaycnv=6;oitavaaycnv=2;break;
		case 302:freqaycnv=7;oitavaaycnv=2;break;
		case 303:freqaycnv=7;oitavaaycnv=2;break;
		case 285:freqaycnv=8;oitavaaycnv=2;break;
		case 286:freqaycnv=8;oitavaaycnv=2;break;
		case 269:freqaycnv=9;oitavaaycnv=2;break;
		case 270:freqaycnv=9;oitavaaycnv=2;break;
		case 254:freqaycnv=10;oitavaaycnv=2;break;
		case 240:freqaycnv=11;oitavaaycnv=2;break;
		case 226:freqaycnv=12;oitavaaycnv=2;break;
		case 227:freqaycnv=12;oitavaaycnv=2;break;

		case 214:freqaycnv=1;oitavaaycnv=3;break;
		case 202:freqaycnv=2;oitavaaycnv=3;break;
		case 190:freqaycnv=3;oitavaaycnv=3;break;
		case 191:freqaycnv=3;oitavaaycnv=3;break;
		case 180:freqaycnv=4;oitavaaycnv=3;break;
		case 170:freqaycnv=5;oitavaaycnv=3;break;
		case 160:freqaycnv=6;oitavaaycnv=3;break;
		case 151:freqaycnv=7;oitavaaycnv=3;break;
		case 143:freqaycnv=8;oitavaaycnv=3;break;
		case 135:freqaycnv=9;oitavaaycnv=3;break;
		case 127:freqaycnv=10;oitavaaycnv=3;break;
		case 120:freqaycnv=11;oitavaaycnv=3;break;
		case 113:freqaycnv=12;oitavaaycnv=3;break;
		
	}
	
}

/* Imprime as Patterns AY-3-8912 depois de Convertidas */
void printpatconvay(void)
{
	
	int i;
	
	textbackground(0);
	textcolor(7);
	clrscr();
	
	/* Menu */
	textbackground(1);
	textcolor(15);
	gotoxy(1,1);
	cprintf(" F1-Help  F2-Modulo  F3-FM  F4-SAA1099  F5-AY/YM  F6-Play Conv         ESC-Sair ");
	gotoxy(1,25);
	cprintf("                                                                               ");
		
	/* Menu */
	menu=0;
	
	playay=0;
	
	/* Cabeçalho */
	alteracabconvay();
		
	/* Navegação nas Patterns AY-3-8912 */
	position=0;
	py=0;
	
	/* Patterns AY-3-8912 */
	carregapatay();
	printpatay();
	
	/* Estado */
	sprintf(txtsts,"Tecla P para tocar musica AY-3-8912 convertida.\0");
	printestado();
	
}

/* Imprime os Canais Visíveis no Menu Patterns AY-3-8912 depois de Convertidas*/
void alteracabconvay(void)
{

	textbackground(0);
	textcolor(11);
	
	gotoxy(1,3);
	cprintf("Patterns AY-3-8912 : ");
	if (menu==0)
	{
		textcolor(10);
		cprintf("Canais 0 - 2 ");
	}
		
	printcnlonay();
	
}

/* Lê a Informação da Pattern AY-3-8912 corrente */
void carregapatay(void)
{

	int i,j;
	int x,y;
	int loc,hic;
	
	pattern=songpos[position];
	py=0;
	
	textbackground(0);
	textcolor(14);
	gotoxy(1,5);
	cprintf("Position - %d , Pattern - %d  \n\n",position,pattern);

	y=0;
	for (i=pattern*SIZEPATINFOAY; i<((pattern*SIZEPATINFOAY)+SIZEPATINFOAY); i+=SIZEROWAY)
	{
		x=0;
		for(j=0; j<AYNUNCANAIS; j++)
		{
			
			/* Byte 0,1,2 - Frequência, Oitava e Low Número do Instrumento */
			hi=(int)(patinfoay[i+j+0]/64);
			lo=(int)(patinfoay[i+j+0]-hi*64);
			
			aysamplenum[y][x]=hi;
			
			hi=(int)(lo/16);
			lo=(int)(lo-hi*16);
	
			ayoct[y][x]=hi;
			ayfreq[y][x]=lo;
			
			/* Byte 3 - Hi Número do Instrumento e Número do Efeito */
			switch (j)
			{
				case 0:
				{
					hi=(int)(patinfoay[i+3]/64);
					loc=(int)(patinfoay[i+3]-hi*64);
					hic=(int)(loc/4);
					lo=(int)(loc-hic*4);
					break;
				}
				case 1:
				{
					hi=(int)(patinfoay[i+3]/64);
					loc=(int)(patinfoay[i+3]-hi*64);
					hi=(int)(loc/16);
					hic=(int)(loc-hi*16);
					loc=(int)(hic/4);
					lo=(int)(hic-loc*4);
					break;
				}
				case 2:
				{
					hi=(int)(patinfoay[i+3]/16);
					loc=(int)(patinfoay[i+3]-hi*16);
					hi=(int)(loc/4);
					lo=(int)(loc-hi*4);
					break;
				}
			}
			
			aysamplenum[y][x]=hi*4+aysamplenum[y][x];
			ayefectnum[y][x]=lo;
		
			if (ayefectnum[y][x] & 1)
			{
				ayefectnum[y][x]=1;
				ayefectpar[y][x]=(patinfoay[i+3]>>1);
			}
		
			x++;
		}
		y++;
	}
	
}

/* Imprime se os Canais AY-3-8912 estão ON ou OFF */
void printcnlonay(void)
{

	int i,j;
	int p,f;

	if (menu==0)
	{
		p=0;
		f=AYNUNCANAIS;
	}
		
	j=0;
	for (i=p; i<f; i++)
	{
		if (cnlonay[i]==1)
		{
			textbackground(2);
			textcolor(15);
			gotoxy((j*12)+5,7);
			cprintf(" ON ");
			textbackground(0);
			cprintf(" ");
		}
		else
		{
			textbackground(4);
			textcolor(15);
			gotoxy((j*12)+5,7);
			cprintf(" OFF ");
		}
		j++;
	}
	
}

/* Imprime a Informação da Pattern AY-3-8912 corrente */
void printpatay(void)
{

	int i,j;
	int p,f;
	char o;
	
	if (menu==0)
	{
		p=0;
		f=AYNUNCANAIS;
	}
		
	textbackground(0);
	textcolor(7);
	gotoxy(1,8);
	for (i=py; i<py+16; i++)
	{
		cprintf("%2d: ",i);
		
		for (j=p; j<f; j++)
		{
			if (ayoct[i][j]==0) o='-';
			else o=ayoct[i][j]+48;
		
			cprintf("%s%c %2d %d%02X  ",pconvnotas[ayfreq[i][j]],o,aysamplenum[i][j],ayefectnum[i][j],ayefectpar[i][j]);
		}	
		cprintf("\r\n");
	}
	
}

/* Navega nas Patterns AY-3-8912 */
void navegapatay(void)
{
	
	int i;
		
	do
	{
		
		while (!kbhit())
		{
			if (playay==1)
			{
				for (i=0; i<AYNUNCANAIS; i++)
					if (playenvlay[i]!=0)
					{
						siay=playenvlay[i];
						envelopay(i);
					//	criasomay(i,freqay[i]);
						setvolay(i);
					}

				playayend=letempo();
				if (playayend-playaystart>=11) playmusicaay();
			}
		};
	
		KB_code=getch();
		
		if (KB_code==KB_F1)					/* F1 - HELP */
		{
			estado=5;
			oldestadohlp=4;
			help=6;
		}
		
		if (KB_code==KB_P)	/* Toca a Música AY por Inteiro a partir da Position atual*/
		{
			row=0;
			playay=1;
			fimsongsa=songlen;
			sprintf(txtsts,"Tocando Musica\0");
			printestado();
			
		//	gettime(&playstart);
				
		}
		
		if (KB_code==KB_U)	/* Toca apenas a Pattern AY atual*/
		{
			row=0;
			playay=1;
			fimsongsa=position+1;
			sprintf(txtsts,"Tocando Pattern\0");
			printestado();
		}
		
		if (KB_code==KB_S) /* Para a Música AY */
		{
			for (i=0; i<AYNUNCANAIS; i++) stopsomay(i);
			row=0;
			playay=0;
			sprintf(txtsts,"Musica Parada\0");
			printestado();
		}
		
		if (KB_code==KB_F2) estado=0;
		
		if (KB_code==KB_F3) estado=1;
		
		if (KB_code==KB_F4) estado=2;
		
		if (KB_code==KB_F5) estado=3;
		
		if (KB_code==KB_ESCAPE) estado=10;
	
		/* Posições */
		if (KB_code==KB_LEFT && position>0)
		{
			position--;
			carregapatay();
		}
				
		if (KB_code==KB_RIGHT && position<songlen-1)
		{
			position++;
			carregapatay();
		}
		
	
		if (KB_code==KB_CTRL_LEFT)
		{
			position=0;
			carregapatay();
		}
				
		if (KB_code==KB_CTRL_RIGHT)
		{
			position=songlen-1;
			carregapatay();
		}
		
		if (KB_code==KB_ALT_LEFT)
		{
			position-=10;
			if (position<0) position=0;
			carregapatay();
		}
					
		if (KB_code==KB_ALT_RIGHT)
		{
			position+=10;
			if (position>songlen-1) position=songlen-1;
			carregapatay();
		}
		
	
		/* Rows */
		if (KB_code==KB_UP && py>0) py--;
			
		if (KB_code==KB_DOWN && py<48) py++;
	
	
		if (KB_code==KB_HOME) py=0;
						
		if (KB_code==KB_END) py=48;

						
		if (KB_code==KB_PAGE_UP)
		{
			py-=16;
			if (py<0) py=0;
		}
	
		if (KB_code==KB_PAGE_DOWN)
		{
			py+=16;
			if (py>48) py=48;
		}
	
		printpatay();
		
		/* Canais ON/OFF */
		if (KB_code>=KB_1 && KB_code<=KB_3)
		{
			cnlonay[KB_code-48-1]=1-cnlonay[KB_code-48-1];
			printcnlonay();
		}
		
	}
	while (estado==4);
	
}

/* Toca a Música Convertida AY-3-8912 */
void playmusicaay(void)
{
	
	int i,j;
	int salta=0,saltap;
	
	playaystart=letempo();

	if (position<fimsongsa)
	{
		
		i=row;
		for (j=0; j<AYNUNCANAIS; j++)
		{
						
			if (ayefectnum[i][j]==2) salta=1;			/* Salta para a Próxima Pattern */
			
			if (ayefectnum[i][j]==1)					/* Salta para a Posição Definida pelo Parâmetro */
			{
				salta=2;
				saltap=ayefectpar[i][j];
			}
				
			if (ayfreq[i][j]!=0 && salta==0)			/* Se a Frequência não for Zero e se não houver Saltos */
			{
				
				siay=saminstay[0][aysamplenum[i][j]-1];
				
			//	if (saefectnum[i][j]==3)		/* Se houver controlo de Volume na Pattern */
			//		volumesa[j]=saefectpar[i][j];
			//	else volumesa[j]=instregsa[sisa].volume;
			
			//	volumeay[j]=instregay[siay].volume;
				volumeay[j]=volinstay[aysamplenum[i][j]-1];
			
				oitavaay[j]=ayoct[i][j]+octinstay[aysamplenum[i][j]-1]-2;	/* Correcção da Oitava */
				
				if (ayfreq[i][j]>9) oitavaay[j]++;
				
				precriasomay(j,ayfreq[i][j]);
			//	criasomay(j,ayfreq[i][j]);
				
			}
		}

		/* Fica na Pattern Corrente e Passa para a Próxima Row */
		if (salta==0)
		{
			
		//	delay(125);
	
			row++;
			if (row==64)
			{
				row=0;
				position++;
				carregapatay();
				printpatay();
			}
		}
		
		/* Salta para a Próxima Posição */
		if (salta==1)
		{
			row=0;
			position++;
			carregapatay();
			printpatay();
		}
		
		/* Salta para a Posição definida pelo Parâmetro */
		if (salta==2)
		{
			row=0;
			position=saltap;
			carregapatay();
			printpatay();
		}
			
	}
	else
	{
		for (i=0; i<3; i++) stopsomay(i);
		row=0;
		playay=0;
		sprintf(txtsts,"Musica Parada\0");
		printestado();
		
	/*	gettime(&playend);
		sprintf(txtsts,"%02d:%02d:%02d:%02d - %02d:%02d:%02d:%02d\0",playstart.ti_hour,playstart.ti_min,playstart.ti_sec,playstart.ti_hund,
			playend.ti_hour,playend.ti_min,playend.ti_sec,playend.ti_hund);
		printestado();
	*/	
	}

}

/* Converte as Frequências do MOD para Notas da Pattern */
char *convfrnt(int freq)
{

	static char *note;
	
	note="???\0";
	
	switch (freq)
	{
		case 0:note="---\0";break;
		
		case 856:note="C 1\0";break;
		case 808:note="C#1\0";break;
		case 762:note="D 1\0";break;
		case 763:note="D 1\0";break;
		case 720:note="D#1\0";break;
		case 678:note="E 1\0";break;
		case 679:note="E 1\0";break;
		case 640:note="F 1\0";break;
		case 641:note="F 1\0";break;
		case 604:note="F#1\0";break;
		case 605:note="F#1\0";break;
		case 570:note="G 1\0";break;
		case 571:note="G 1\0";break;
		case 538:note="G#1\0";break;
		case 539:note="G#1\0";break;
		case 508:note="A 1\0";break;
		case 509:note="A 1\0";break;
		case 480:note="A#1\0";break;
		case 453:note="B 1\0";break;
		
		case 428:note="C 2\0";break;
		case 404:note="C#2\0";break;
		case 381:note="D 2\0";break;
		case 360:note="D#2\0";break;
		case 339:note="E 2\0";break;
		case 340:note="E 2\0";break;
		case 320:note="F 2\0";break;
		case 321:note="F 2\0";break;
		case 302:note="F#2\0";break;
		case 303:note="F#2\0";break;
		case 285:note="G 2\0";break;
		case 286:note="G 2\0";break;
		case 269:note="G#2\0";break;
		case 270:note="G#2\0";break;
		case 254:note="A 2\0";break;
		case 240:note="A#2\0";break;
		case 226:note="B 2\0";break;
		case 227:note="B 2\0";break;

		case 214:note="C 3\0";break;
		case 202:note="C#3\0";break;
		case 190:note="D 3\0";break;
		case 191:note="D 3\0";break;
		case 180:note="D#3\0";break;
		case 170:note="E 3\0";break;
		case 160:note="F 3\0";break;
		case 151:note="F#3\0";break;
		case 143:note="G 3\0";break;
		case 135:note="G#3\0";break;
		case 127:note="A 3\0";break;
		case 120:note="A#3\0";break;
		case 113:note="B 3\0";break;
		
	}
	
	return(note);
	
}

/* Converte Decimal para Hexadecimal - 4 bits*/
char *hexa4(int deca)
{

	static char *hex;
	
	char gg[2];
	short h,l;
	
	hex="0\0";
	
	h=(short)(deca/16);
	l=(short)(deca-h*16);
	
	if (l<10) gg[0]=l+48;
	else gg[0]=l+55;
	
	gg[1]='\0';
	
	sprintf(hex,gg);
			
	return(hex);
	
}

/* Converte Decimal para Hexadecimal - 8 bits*/
char *hexa8(int deca)
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

/* Faz o Reset ao DSP */
int resetdsp(void)
{
	
	int val;
	
	val=outp(DSP_RST,1);
	delay(10);
	val=outp(DSP_RST,0);
	delay(10);		
	val++;

	/* Verifica se o DSP foi inicializado */
	if (((inp(DSP_BUF) & 0x80)==0x80) && (inp(DSP_DAT)==0xAA))
	{
		sprintf(txtsts,"Placa de Som encontrada!\n");
		return(0);
	}
	else
	{
		sprintf(txtsts,"Placa de Som nao encontrada!\n");
		return(1);
	}
	
}

/* Escreve um Valor no Porto dos Comandos DSP */
void writedsp(int value)
{
	
	int val;
	
	while ((inp(DSP_CMD) & 128)!=0);	/* Espera que o Porto dos Comandos esteja desimpedido */
	
	val=outp(DSP_CMD,value);			/* Escreve o valor no Porto dos Comandos */
			
	val++;
	
}

/* Lê um Valor no Porto dos Dados DSP */
int readdsp(void)
{
	
	return(inp(DSP_DAT));	/* Lê o valor no Porto dos Dados */
			
		
}

/* Envia o som para o DSP */
void playsom(void)
{
	
	int j;
	unsigned long i,ls;
	
	sprintf(txtsts,"Playing %s\0",nomesample[ny]);
	printestado();
	
	writedsp(0xD1); /* Liga o Speaker */
	
	ls=(lensample[ny][0]*256+lensample[ny][1])*2;
		
	for (i=0; i<ls; i++)
	{
		writedsp(0x10);  						/* Indica ao DSP para enviar dados para o Speaker  */
		writedsp((saminfo[ny].data[i])-128);	/* Envia o Sample para o Speaker */
		
		for (j=0; j<100; j++){}					/* Executa uma "Pausa" */
	}
	
	writedsp(0xD3); /* Desliga o Speaker */

}

/* Faz o Reset à Placa de Som FM */
void resetfm(void)
{
	int i;
	
	for (i=FM_MIN_REG; i<FM_MAX_REG; i++) writefm(i,0);
	
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
void criasomfm(int canalg,int canalo,int freqindex)
{
	
	int freq,freql,freqh;
	int regb0;

	freq=notasfm[freqindex];
		
	freqh=(int)(freq/256);
	freql=(int)(freq-(freqh*256));
	
	if (playfm==0)
	{
		sprintf(txtsts,"Playing %s em C, oitava 4 - Frequencia: %d\0",instreg[sifm].nome,freq);
		printestado();
	}
	
	/* Desliga o Canal */
	regb0=0*1+4*oitavafm+freqh;
	writefm(0xB0+canalg,regb0);

	/* Toca o Instrumento */
	regb0=32*1+4*oitavafm+freqh;
	
	/* Operador 1 */
	writefm(canalo,instreg[sifm].op1reg2); /* Tremolo, Vibrato, Hold Note, Keyboard Scaling Rate, Harmonic */
	writefm(canalo+0x20,instreg[sifm].op1reg4); /* Scale Level, Volume Level */
	writefm(canalo+0x40,instreg[sifm].op1reg6); /* Attack, Decay */
	writefm(canalo+0x60,instreg[sifm].op1reg8); /* Sustain, Release */
	writefm(canalo+0xC0,instreg[sifm].op1rege); /* Envelope Type */
	
	/* Operador 2 */
	writefm(canalo+0x3,instreg[sifm].op2reg2); 	/* Tremolo, Vibrato, Hold Note, Keyboard Scaling Rate, Harmonic */
	writefm(canalo+0x23,volfm); 				/* Scale Level, Volume Level */
	writefm(canalo+0x43,instreg[sifm].op2reg6); /* Attack, Decay */
	writefm(canalo+0x63,instreg[sifm].op2reg8); /* Sustain, Release */
	writefm(canalo+0xC3,instreg[sifm].op2rege); /* Envelope Type */

	/* Geral */
	writefm(0xC0+canalg,instreg[sifm].regc0);  	/* Feedback Modulation, FM Synthesis ON/OFF */
	writefm(0xA0+canalg,freql); 				/* Frequência Low Byte */
	
	if (cnlonfm[canalg]==1)
		writefm(0xB0+canalg,regb0);  			/* Play Note, Oitava, Frequência Hi Byte */
			
}

/* Desliga o Canal FM*/
void stopsomfm(int canalg)
{
	
	sprintf(txtsts,"Som Desligado\0");
	printestado();
	
	/* Desliga o Canal */
	writefm(0xB0+canalg,0x00);
	
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
void getenvlsa(unsigned char canal)
{
	
	envlssa[canal][0]=instregsa[sisa].attack;
	envlssa[canal][1]=instregsa[sisa].sustain;
	envlssa[canal][2]=instregsa[sisa].decay;
	
	cntenvlssa[canal][0]=factorsa*instregsa[sisa].attack;
	cntenvlssa[canal][1]=factorsa*instregsa[sisa].sustain;
	cntenvlssa[canal][2]=factorsa*instregsa[sisa].decay;
	
	if (envlssa[canal][1]!=0) cntenvlsusssa[canal]=15;

	if (envlssa[canal][0]!=0 || envlssa[canal][1]!=0 || envlssa[canal][2]!=0) playenvlsa[canal]=sisa;
	
	if (envlssa[canal][0]==0) volumesa[canal]=instregsa[sisa].volume;
//	if (envlssa[canal][0]==0) volumesa[canal]=volinstsa[sisa];
	else volumesa[canal]=0;
	
}

/* Produz um Envelope SAA1099*/
void envelopsa(unsigned char canal)
{
	
	/* Attack */
	if (envlssa[canal][0]>0)
	{
		cntenvlssa[canal][0]--;
		if (cntenvlssa[canal][0]==0)
		{
			cntenvlssa[canal][0]=factorsa*instregsa[sisa].attack;
			volumesa[canal]++;
			if (volumesa[canal]==instregsa[sisa].volume)
		//	if (volumesa[canal]==volinstsa[sisa])
			{
				envlssa[canal][0]=0;
				if (envlssa[canal][1]==0 && envlssa[canal][2]==0) playenvlsa[canal]=0;
			}
		}
	}
	else
	{
		/* Sustain */
		if (envlssa[canal][1]>0)
		{
			cntenvlssa[canal][1]--;
			if (cntenvlssa[canal][1]==0)
			{
				cntenvlssa[canal][1]=factorsa*instregsa[sisa].sustain;
				cntenvlsusssa[canal]--;
				if (cntenvlsusssa[canal]==0)
				{
					envlssa[canal][1]=0;
					if (envlssa[canal][2]==0)
					{
						volumesa[canal]=0;
						playenvlsa[canal]=0;
					}
				}
			}
		}	
		else
		{
			/* Decay */
			if (envlssa[canal][2]>0)
			{
				cntenvlssa[canal][2]--;
				if (cntenvlssa[canal][2]==0)
				{
					cntenvlssa[canal][2]=factorsa*instregsa[sisa].decay;
					volumesa[canal]--;
					if (volumesa[canal]==0)
					{
						envlssa[canal][2]=0;
						playenvlsa[canal]=0;
					}
				}
			}	
		}
	}
	
	if (playenvlsa[canal]==0 && instregsa[sisa].repete==1) getenvlsa(canal);
	
}

/* Prepara a criação de um Som SAA1099 */
void precriasomsa(unsigned char canal,int freqindex)
{

	if (playsa==0)
	{
		sprintf(txtsts,"Playing %s em C, Oitava 2 - Frequencia: %d\0",instregsa[sisa].nome,notassa[freqindex]);
		printestado();
	}
		
	playenvlsa[canal]=0;
	
	getenvlsa(canal);
	
/*	if (playenvlsa[canal]!=0) freqsa[canal]=freqindex;
	else criasomsa(canal,freqindex); */
	
	criasomsa(canal,freqindex);
	
}

/* Apenas atualiza o Volume dos Canais SAA1099 em caso de Envelope */
void setvolsa(unsigned char canal)
{
	
	if (cnlonsa[canal]==1)
	{
		if (canal==0 || canal==2 || canal==4)						/* Estéreo */
				writesa1(0x00+canal,volumesa[canal]*16);
			else
				writesa1(0x00+canal,volumesa[canal]);
	}
	else writesa1(0x00+canal,0);
				
}

/* Produz um Som SAA1099 */
void criasomsa(unsigned char canal,int freqindex)
{
	
	unsigned char oct;
	
	if (cnlonsa[canal]==1)
	{
		if (canal==0 || canal==2 || canal==4)			/* Volume */
			writesa1(0x00+canal,volumesa[canal]*16);
		else
			writesa1(0x00+canal,volumesa[canal]);
	}
	else writesa1(0x00+canal,0);
		
	writesa1(0x08+canal,notassa[freqindex]); 			/* Frequência */
		
	if (canal==0 || canal==1)
	{
		if (canal==0)
		{
			hi=(int)(octsa[0][0]/16);
			oct=hi*16+oitavasa[canal];
		}
		else
		{
			hi=(int)(octsa[0][0]/16);
			lo=(int)(octsa[0][0]-hi*16);
			oct=oitavasa[canal]*16+lo;
		}
		octsa[0][0]=oct;
		writesa1(0x010,octsa[0][0]);  			/* Oitava */
	}
		
	if (canal==2 || canal==3)
	{
		if (canal==2)
		{
			hi=(int)(octsa[0][1]/16);
			oct=hi*16+oitavasa[canal];
		}
		else
		{
			hi=(int)(octsa[0][1]/16);
			lo=(int)(octsa[0][1]-hi*16);
			oct=oitavasa[canal]*16+lo;
		}
		octsa[0][1]=oct;
		writesa1(0x011,octsa[0][1]);  			/* Oitava */
	}
		
	if (canal==4 || canal==5)
	{
		if (canal==4)
		{
			hi=(int)(octsa[0][2]/16);
			oct=hi*16+oitavasa[canal];
		}
		else
		{
			hi=(int)(octsa[0][2]/16);
			lo=(int)(octsa[0][2]-hi*16);
			oct=oitavasa[canal]*16+lo;
		}
		octsa[0][2]=oct;
		writesa1(0x012,octsa[0][2]);  			/* Oitava */
	}
		
	if (instregsa[sisa].freqenb==1) freqenbsa=setbit(freqenbsa,canal+1); /* Frequência Enable */
	else freqenbsa=clearbit(freqenbsa,canal+1);
		
	if (instregsa[sisa].noiseenb==1) noiseenbsa=setbit(noiseenbsa,canal+1); /* Noise Enable */
	else noiseenbsa=clearbit(noiseenbsa,canal+1);
		
	writesa1(0x014,freqenbsa);  		/* Frequência Enable */
	writesa1(0x015,noiseenbsa);  		/* Noise Enable */
				
}

/* Desliga o Canal SAA1099 */
void stopsomsa(int canal)
{

	writesa1(0x00+canal,0);		/* Volume  */
	
	sprintf(txtsts,"Som Desligado\0");
	printestado();
	
}

/* Saca os Valores do Envelope AY-3-8912 */
void getenvlay(unsigned char canal)
{
	
	envlsay[canal][0]=instregay[siay].attack;
	envlsay[canal][1]=instregay[siay].sustain;
	envlsay[canal][2]=instregay[siay].decay;
	
	cntenvlsay[canal][0]=factoray*instregay[siay].attack;
	cntenvlsay[canal][1]=factoray*instregay[siay].sustain;
	cntenvlsay[canal][2]=factoray*instregay[siay].decay;
	
	if (envlsay[canal][1]!=0) cntenvlsussay[canal]=15;

	if (envlsay[canal][0]!=0 || envlsay[canal][1]!=0 || envlsay[canal][2]!=0) playenvlay[canal]=siay;
	
	if (envlsay[canal][0]==0) volumeay[canal]=instregay[siay].volume;
//	if (envlsay[canal][0]==0) volumeay[canal]=volinstay[siay];
	else volumeay[canal]=0;
	
}

/* Produz um Envelope AY-3-8912 */
void envelopay(unsigned char canal)
{
	
	/* Attack */
	if (envlsay[canal][0]>0)
	{
		cntenvlsay[canal][0]--;
		if (cntenvlsay[canal][0]==0)
		{
			cntenvlsay[canal][0]=factoray*instregay[siay].attack;
			volumeay[canal]++;
			if (volumeay[canal]==instregay[siay].volume)
		//	if (volumeay[canal]==volinstay[siay])
			{
				envlsay[canal][0]=0;
				if (envlsay[canal][1]==0 && envlsay[canal][2]==0) playenvlay[canal]=0;
			}
		}
	}
	else
	{
		/* Sustain */
		if (envlsay[canal][1]>0)
		{
			cntenvlsay[canal][1]--;
			if (cntenvlsay[canal][1]==0)
			{
				cntenvlsay[canal][1]=factoray*instregay[siay].sustain;
				cntenvlsussay[canal]--;
				if (cntenvlsussay[canal]==0)
				{
					envlsay[canal][1]=0;
					if (envlsay[canal][2]==0)
					{
						volumeay[canal]=0;
						playenvlay[canal]=0;
					}
				}
			}
		}	
		else
		{
			/* Decay */
			if (envlsay[canal][2]>0)
			{
				cntenvlsay[canal][2]--;
				if (cntenvlsay[canal][2]==0)
				{
					cntenvlsay[canal][2]=factoray*instregay[siay].decay;
					volumeay[canal]--;
					if (volumeay[canal]==0)
					{
						envlsay[canal][2]=0;
						playenvlay[canal]=0;
					}
				}
			}	
		}
	}
	
	if (playenvlay[canal]==0 && instregay[siay].repete==1) getenvlay(canal);
	
}

/* Prepara a criação de um Som AY-3-8912 */
void precriasomay(unsigned char canal,int freqindex)
{

	if (playay==0)
	{
		sprintf(txtsts,"Playing %s em C, Oitava 2 - Frequencia: %d\0",instregay[siay].nome,notasay[freqindex]);
		printestado();
	}
		
	playenvlay[canal]=0;
	
	getenvlay(canal);
	
//	if (playenvlay[canal]!=0) freqay[canal]=freqindex;
//	else criasomay(canal,freqindex);
	
	criasomay(canal,freqindex);
	
}

/* Apenas atualiza o Volume dos Canais AY-3-8912 em caso de Envelope */
void setvolay(unsigned char canal)
{
	
	if (cnlonay[canal]==1)
			writesa1(0x00+canal,volumeay[canal]*16+volumeay[canal]);	/* Mono */
	else
		writesa1(0x00+canal,0);
				
}

/* Produz um Som AY-3-8912 */
void criasomay(unsigned char canal,int freqindex)
{
	
	unsigned char oct;
	
	if (cnlonay[canal]==1) writesa1(0x00+canal,volumeay[canal]*16+volumeay[canal]);	/* Volume */
	else writesa1(0x00+canal,0);
		
	writesa1(0x08+canal,notasay[freqindex]); 			/* Frequência */
		
	if (canal==0 || canal==1)
	{
		if (canal==0)
		{
			hi=(int)(octay[0][0]/16);
			oct=hi*16+oitavaay[canal];
		}
		else
		{
			hi=(int)(octay[0][0]/16);
			lo=(int)(octay[0][0]-hi*16);
			oct=oitavaay[canal]*16+lo;
		}
		octay[0][0]=oct;
		writesa1(0x010,octay[0][0]);  			/* Oitava */
	}
		
	if (canal==2 || canal==3)
	{
		if (canal==2)
		{
			hi=(int)(octay[0][1]/16);
			oct=hi*16+oitavaay[canal];
		}
		else
		{
			hi=(int)(octay[0][1]/16);
			lo=(int)(octay[0][1]-hi*16);
			oct=oitavaay[canal]*16+lo;
		}
		octay[0][1]=oct;
		writesa1(0x011,octay[0][1]);  			/* Oitava */
	}
		
	if (instregay[siay].freqenb==1) freqenbay=setbit(freqenbay,canal+1); /* Frequência Enable */
	else freqenbay=clearbit(freqenbay,canal+1);
		
	if (instregay[siay].noiseenb==1) noiseenbay=setbit(noiseenbay,canal+1); /* Noise Enable */
	else noiseenbay=clearbit(noiseenbay,canal+1);
		
	writesa1(0x014,freqenbay);  		/* Frequência Enable */
	writesa1(0x015,noiseenbay);  		/* Noise Enable */
				
}

/* Desliga o Canal AY-3-8912 */
void stopsomay(int canal)
{

	writesa1(0x00+canal,0);		/* Volume  */
	
	sprintf(txtsts,"Som Desligado\0");
	printestado();
	
}

/* Liga o bit k do Byte n */ 
unsigned char setbit(unsigned char n,unsigned char k) 
{ 
    return (n | (1 << (k - 1))); 
} 
  
/* Desliga o bit k do Byte n */ 
unsigned char clearbit(unsigned char n,unsigned char k) 
{ 
    return (n & (~(1 << (k - 1)))); 
} 
  
/* Faz o Toggle do bit k do Byte n */
unsigned char togglebit(unsigned char n,unsigned char k) 
{ 
    return (n ^ (1 << (k - 1)));
}

/* Converte a Extensão dos Ficheiros para os Respetivos Formatos */
void splitfiles(void)
{

	int i;
	
	char nomefile[9];
	
	char extfm[5]=".fm\0";
	char extstfm[5]=".fms\0";
	
	char extsa[5]=".saa\0";
	char extstsa[5]=".sas\0";
	
	char extay[5]=".may\0";
	char extstay[5]=".ays\0";
	
	char extayplr[5]=".tap\0";
	
	/* Saca o nome do Ficheiro do MOD retirando-lhe a Extensão */
	for(i=0; i<strlen(nomefilemod)-4; i++) nomefile[i]=nomefilemod[i];
	
	nomefile[i]='\0';
	
	/* FM */
	strcpy(nomefilefm,nomefile);
	strcpy(nomefilestsfm,nomefile);
	
	strcat(nomefilefm,extfm);
	strcat(nomefilestsfm,extstfm);
	
	/* SAA1099 */
	strcpy(nomefilesa,nomefile);
	strcpy(nomefilestssa,nomefile);
	
	strcat(nomefilesa,extsa);
	strcat(nomefilestssa,extstsa);
	
	/* AY-3-8912 */
	strcpy(nomefileay,nomefile);
	strcpy(nomefilestsay,nomefile);
	strcpy(nomefileayplr,nomefile);
	
	strcat(nomefileay,extay);
	strcat(nomefilestsay,extstay);
	strcat(nomefileayplr,extayplr);
	
}

/* Lê o Tempo */
unsigned long letempo(void)
{

	gettime(&tempo);
	return(tempo.ti_hour*360000+tempo.ti_min*6000+tempo.ti_sec*100+tempo.ti_hund);
	
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
	cprintf(" F1-Help  F2-Modulo  F3-FM  F4-SAA1099  F5-AY/YM  F6-Play Conv         ESC-Sair ");
	
	textbackground(1);
	textcolor(15);
	for (i=3; i<25; i++)
	{
		gotoxy(3,i);
		cprintf("                                                                            ");
	}
	
	gotoxy(5,4);cprintf("Help:");
	gotoxy(11,4);cprintf("Pagina %d/8",help+1);
	
	if (help==0)
	{
		cprintf(" - Pagina do Modulo 1/2");
		
		gotoxy(5,6);cprintf("ALT - Alterna entre as Patterns e os Samples");
	
		gotoxy(5,8);cprintf("Patterns:");
	
		gotoxy(5,10);cprintf("Cursor Baixo : Row seguinte");
		gotoxy(5,11);cprintf("Cursor Cima : Row anterior");
		gotoxy(5,12);cprintf("Page Down : 12 Rows seguintes");
		gotoxy(5,13);cprintf("Page Up : 12 Rows anteriores");
		gotoxy(5,14);cprintf("End : Ultima Row");
		gotoxy(5,15);cprintf("Home : Primeira Row");
	
		gotoxy(5,16);cprintf("Cursor Direita : Posicao seguinte");
		gotoxy(5,17);cprintf("Cursor Esquerda : Posicao anterior");
		gotoxy(5,18);cprintf("ALT + Cursor Direita : 10 Posicoes seguintes");
		gotoxy(5,19);cprintf("ALT + Cursor Esquerda : 10 Posicoes anteriores");
		gotoxy(5,20);cprintf("CTRL + Cursor Direita : Ultima Posicao");
		gotoxy(5,21);cprintf("CTRL + Cursor Esquerda : Primeira Posicao");
			
		gotoxy(5,23);cprintf("Cursor Direita: Seguinte                             Return: Sai do Help");
	}
	
	if (help==1)
	{
		cprintf(" - Pagina do Modulo 2/2");
		
		gotoxy(5,6);cprintf("ALT - Alterna entre as Patterns e os Samples");
	
		gotoxy(5,8);cprintf("Samples:");
	
		gotoxy(5,10);cprintf("Cursor Baixo : Sample seguinte");
		gotoxy(5,11);cprintf("Cursor Cima : Sample anterior");
		gotoxy(5,13);cprintf("Page Down : 13 Samples seguintes");
		gotoxy(5,14);cprintf("Page Up : 13 Samples anteriores");
		gotoxy(5,16);cprintf("End : Ultimo Sample");
		gotoxy(5,17);cprintf("Home : Ultimo Sample");
	
		gotoxy(5,19);cprintf("P - Play Sample");
		
				
		gotoxy(5,23);cprintf("Cursor Direita: Seguinte, Cursor Esquerda: Anterior  Return: Sai do Help");
	}
	
	if (help==2)
	{
		cprintf(" - Pagina Conversao do Modulo 1/4 - Samples 1/2");
		
		gotoxy(5,6);cprintf("CTRL + ALT - Alterna entre Configuracao dos Samples e dos Canais");
		
		gotoxy(5,7);cprintf("ALT - Alterna entre os Samples e os Instrumentos");
		
		gotoxy(5,9);cprintf("Samples:");
	
		gotoxy(5,10);cprintf("Cursor Cima / Cursor Baixo : Sample seguinte / Sample anterior");
		gotoxy(5,11);cprintf("Page Down / Page Up : 16 Samples seguintes / 16 Samples anteriores");
		gotoxy(5,12);cprintf("End / Home : Ultimo Sample / Primeiro Sample");
		gotoxy(5,13);cprintf("P - Play Sample");
		
		gotoxy(5,15);cprintf("Instrumentos FM / SAA1099 / AY-3-8912:");
	
		gotoxy(5,16);cprintf("Cursor Cima / Cursor Baixo : Instrumento seguinte / Instrumento anterior");
		gotoxy(5,17);cprintf("Page Down : 16 Instrumentos seguintes");
		gotoxy(5,18);cprintf("Page Up : 16 Instrumentos anteriores");
		gotoxy(5,19);cprintf("End / Home : Ultimo Instrumento / Primeiro Instrumento");
		gotoxy(5,20);cprintf("I - Play Instrumento");
		gotoxy(5,21);cprintf("O - Stop Instrumento");
				
		gotoxy(5,23);cprintf("Cursor Direita: Seguinte, Cursor Esquerda: Anterior  Return: Sai do Help");
	}
	
	if (help==3)
	{
		cprintf(" - Pagina Conversao do Modulo 2/4 - Samples 2/2");
		
		gotoxy(5,6);cprintf("Return: Atribui o Instrumento ao Sample");
		gotoxy(5,7);cprintf("Delete: Apaga a Atribuicao do Instrumento ao Sample");
		
		gotoxy(5,9);cprintf("R: Apaga todas as Atribuicoes de Instrumentos aos Samples");
		
		gotoxy(5,11);cprintf("+ / - : Aumenta / Diminui Correcao da Oitava");
		gotoxy(5,12);cprintf("V / B : Aumenta / Diminui Afinacao do Volume");
		
		gotoxy(5,14);cprintf("Teclas Gerais:");
		
		gotoxy(5,16);cprintf("Q - Grava o Estado de Atribuicao de Instrumentos/Canais");
		gotoxy(5,17);cprintf("E - Le o Estado de Atribuicao de Instrumentos/Canais");
		
		gotoxy(5,19);cprintf("C : Converte o Modulo para FM / SAA1099 / AY-3-8912");
		
		gotoxy(5,21);cprintf("G : Grava a Musica Convertida");
		
		gotoxy(5,23);cprintf("Cursor Direita: Seguinte, Cursor Esquerda: Anterior  Return: Sai do Help");		
	}
	
	if (help==4)
	{
		cprintf(" - Pagina Conversao do Modulo 3/4 - Canais 1/2");
		
		gotoxy(5,6);cprintf("CTRL + ALT - Alterna entre Configuracao dos Canais e dos Samples");
		
		gotoxy(5,7);cprintf("ALT - Alterna entre os Canais FM/SAA1099/AY-3-8912 e os Canais MOD");
		
		gotoxy(5,9);cprintf("Canais FM / SAA1099 / AY-3-8912:");
	
		gotoxy(5,11);cprintf("Cursor Cima / Cursor Baixo : Canal seguinte / Canal anterior");
		gotoxy(5,12);cprintf("Page Down / Page Up : Ultimo Canal / Primeiro Canal");
		gotoxy(5,13);cprintf("End / Home : Ultimo Canal / Primeiro Canal");
				
		gotoxy(5,15);cprintf("Canais MOD:");
	
		gotoxy(5,17);cprintf("Cursor Cima / Cursor Baixo : Canal MOD seguinte / Canal MOD anterior");
		gotoxy(5,18);cprintf("Page Down / Page Up : Ultimo Canal MOD / Primeiro Canal MOD");
		gotoxy(5,19);cprintf("End / Home : Ultimo Canal MOD / Primeiro Canal MOD");
		
		gotoxy(5,23);cprintf("Cursor Direita: Seguinte, Cursor Esquerda: Anterior  Return: Sai do Help");	
	}
	
	if (help==5)
	{
		cprintf(" - Pagina Conversao do Modulo 4/4 - Canais 2/2");
		
		gotoxy(5,6);cprintf("Return: Atribui o Canal MOD ao Canal FM/SAA1099/AY-3-8912");
		gotoxy(5,7);cprintf("Delete: Apaga a Atribuicao do Canal MOD ao Canal FM/SAA1099/AY-3-8912");
		
		gotoxy(5,9);cprintf("R: Apaga todas as Atribuicoes de Canais MOD");
		gotoxy(5,10);cprintf("   aos Canais FM/SAA1099/AY-3-8912");
		
		gotoxy(5,12);cprintf("Teclas Gerais:");
		
		gotoxy(5,14);cprintf("Q - Grava o Estado de Atribuicao de Instrumentos/Canais");
		gotoxy(5,15);cprintf("E - Le o Estado de Atribuicao de Instrumentos/Canais");
		
		gotoxy(5,17);cprintf("C : Converte o Modulo para FM / SAA1099 / AY-3-8912");
		
		gotoxy(5,19);cprintf("G : Grava a Musica Convertida");
		
		gotoxy(5,23);cprintf("Cursor Direita: Seguinte, Cursor Esquerda: Anterior  Return: Sai do Help");	
	}
	
	if (help==6)
	{
		cprintf(" - Pagina de Patterns de Musica Convertida 1/2");
		
		gotoxy(5,6);cprintf("Patterns:");
	
		gotoxy(5,8);cprintf("Cursor Baixo : Row seguinte");
		gotoxy(5,9);cprintf("Cursor Cima : Row anterior");
		gotoxy(5,10);cprintf("Page Down : 12 Rows seguintes");
		gotoxy(5,11);cprintf("Page Up : 12 Rows anteriores");
		gotoxy(5,12);cprintf("End : Ultima Row");
		gotoxy(5,13);cprintf("Home : Primeira Row");
	
		gotoxy(5,15);cprintf("Cursor Direita : Posicao seguinte");
		gotoxy(5,16);cprintf("Cursor Esquerda : Posicao anterior");
		gotoxy(5,17);cprintf("ALT + Cursor Direita : 10 Posicoes seguintes");
		gotoxy(5,18);cprintf("ALT + Cursor Esquerda : 10 Posicoes anteriores");
		gotoxy(5,19);cprintf("CTRL + Cursor Direita : Ultima Posicao");
		gotoxy(5,20);cprintf("CTRL + Cursor Esquerda : Primeira Posicao");
		
		gotoxy(5,23);cprintf("Cursor Direita: Seguinte, Cursor Esquerda: Anterior  Return: Sai do Help");	
	}
	
	if (help==7)
	{
		cprintf(" - Pagina de Patterns de Musica Convertida 2/2");
		
		gotoxy(5,6);cprintf("Teclas Gerais:");
	
		gotoxy(5,8);cprintf("P - Play Musica");
		gotoxy(5,9);cprintf("U - Play Pattern Corrente");
		gotoxy(5,10);cprintf("S - Stop Play Musica / Pattern Corrente");
	
		gotoxy(5,12);cprintf("1,2,3 - Liga/Desliga Canais AY-3-8912");
		gotoxy(5,14);cprintf("1,2,3,4,5,6,7,8,9 - Liga/Desliga Canais FM");
		gotoxy(5,16);cprintf("1,2,3,4,5,6 - Liga/Desliga Canais SAA1099");
		
		gotoxy(5,18);cprintf("ALT - Mostra mais Canais FM");
		
		gotoxy(5,23);cprintf("Cursor Esquerda: Anterior                            Return: Sai do Help");
	}
	
}

/* Navega no Help */
void navegahelp(void)
{
	
	do
	{
		KB_code=getch();
		
		if (KB_code==KB_RETURN) estado=oldestadohlp;
		
		if (KB_code==KB_ESCAPE)	estado=10; /* Escape - Sair */
		
		if (KB_code==KB_LEFT && help>0)
		{
			help--;
			printhelp();
		}
	
		if (KB_code==KB_RIGHT && help<7)
		{
			help++;
			printhelp();
		}
	}
	while(estado==5);
	
}