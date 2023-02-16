/*
/*        FM Player 1.5
/*
/*      Leitor de Músicas FM
/*    AdLib / Sound Blaster Pro
/*  
/*  Para compilar: tcc FMPlayer.c  
/*
*/

/* Includes */
#include <stdio.h>
#include <dos.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <alloc.h>
#include <time.h>

/* Portos da Placa de Som FM AdLib */
#define ADL_REG		0x388	/* Porto dos Registos/Status na Adlib */
#define ADL_DAT		0x389 	/* Porto dos Dados na Adlib           */

/* Portos da Placa de Som FM Sound Blaster Pro */
#define SBP_REG		0x220	/* Porto Base dos Registos/Status na SB PRO */
#define SBP_DAT		0x221 	/* Porto Base dos Dados na SB PRO           */

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

#define SIZEPATINFOFM 1152	/* Tamanho da Pattern FM */
#define SIZEROWFM 18		/* Número de Bytes das Rows FM */
#define FMNUNCANAIS 9		/* Número de Canais FM */

void screenopen(void);
void screenclose(void);

int inicializa(void);
void mainmenu(void);
void printdados(void);

int lemusicafm(void);

void carregapatfm(void);
void playmusicafm(void);

/* AdLib */
void resetfm(void);
void initfm(void);
void writefm(int reg,int value);
int readfm(void);
void criasomfm(int canalg,int canalo,int freqindex);
void stopsomfm(int canalg);

/* Sound Blaster Pro */
void resetsb(void);
void initsb(void);
void writelsb(int reg,int value);
void writersb(int reg,int value);
int readlsb(void);
int readrsb(void);
void criasomsb(int canalg,int canalo,int freqindex);
void stopsomsb(int canalg);

void printestado(void);

/* Teclado */
int KB_code=0;

/* Tipo de Placa de Som */
int cardtype;

/* Ficheiros */
FILE *fp;
char nomefilefm[13];

/* Instrumentos FM */
unsigned char octinstfm[31];
unsigned char sifm;

/* Patterns FM */
unsigned char position;
unsigned char pattern,py;
unsigned char row;
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

/* Música FM */
unsigned char playfm;
unsigned char songpos[128];

/* Manipulação de bits */
unsigned char hi,lo;

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
} instreg[31];

/* Definição do Cabeçalho da Música FM */
struct cabfilesfm
{
	char nome[20];
	unsigned char songlen;
	unsigned char maxpat;
} cabfilefm;

short notasfm[13]={FM_V,FM_C,FM_Cs,FM_D,FM_Ds,FM_E,FM_F,FM_Fs,FM_G,FM_Gs,FM_A,FM_As,FM_B};
short canaisfm[FMNUNCANAIS]={FM_CNL_0,FM_CNL_1,FM_CNL_2,FM_CNL_3,FM_CNL_4,FM_CNL_5,FM_CNL_6,FM_CNL_7,FM_CNL_8};

/* Estado */
char txtsts[70];


/* Main */
int main(int argc, char *argv[])
{
	
	if(argc==2)
	{
		sprintf(nomefilefm,argv[1]);
		cardtype=0;
	}
	else if(argc==3)
	{
		sprintf(nomefilefm,argv[1]);
		if (strcmp(argv[2],"sb")==0) cardtype=1;
		else cardtype=0;
	}
	else
	{
		sprintf(txtsts,"\r\nUtilizacao: FmPlayer NomeMusica.fm soundcard");
		printestado();
		sprintf(txtsts,"\r\nsoundcard=sb para Sound Blaster Pro");
		printestado();
		sprintf(txtsts,"AdLib por defeito");
		printestado();
		return(1);
	}
	
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

	textbackground(0);
	textcolor(7);
	clrscr();	
	
	_setcursortype(_NOCURSOR);

	cprintf("\r\n");	
	textcolor(11);
	cprintf(" FM Player 1.5 - ");
	textcolor(10);
	cprintf(" (C) 2023 Penisoft / MadAxe\r\n");
	
	textcolor(7);
	
}

/* Volta para o MS-Dos */
void screenclose(void)
{

	/* Liberta a Memória Alocada */
	free(patinfofm);
	
	if (cardtype==0) resetfm();
	else resetsb();
		
	textbackground(0);
	textcolor(7);
	clrscr();	
	
	textcolor(11);
	cprintf("FM Player 1.5\r\n");
	textcolor(10);
	cprintf("(C) 2023 Penisoft / MadAxe\r\n");
	
	_setcursortype(_NORMALCURSOR);
	
}

/* Inicializa o Programa */
int inicializa(void)
{
	
	int f,i;
	
	/* Reset Som FM */
	if (cardtype==0) /* AdLib */
	{
		resetfm();
		initfm();
	}
	else			/* Sound Blaster PRO */
	{
		resetsb();
		initsb();
	}
	
	/* Lê Música FM */
	f=lemusicafm();
	if (f!=0)
	{
		printestado();
		return(f);
	}
	
	/* Navegação nas Patterns */
	position=0;
	py=0;
	
	/* Navegação nos Instrumentos FM */
	sifm=0;
	
	/* Música */
	playfm=1;

	return(0);
	
}

/* Main Menu */
void mainmenu(void)
{

	printdados();

	carregapatfm();

	do
	{
		playmusicafm();
	}
	while (!kbhit() && playfm==1);
	
	if (playfm==1) KB_code=getch();
	
}

/* Imprime dados da Música FM */
void printdados(void)
{
	

	cprintf("\r\n");
	textcolor(14);
	cprintf(" ");
	printestado();
	
	cprintf("\r\n");
	textcolor(3);
	cprintf(" Nome: ");
	textcolor(15);
	cprintf("%s\r\n",cabfilefm.nome);
	
	textcolor(3);
	cprintf(" Posicoes: ");
	textcolor(15);
	cprintf("%d\r\n",cabfilefm.songlen);
	
	textcolor(3);
	cprintf(" Patterns: ");
	textcolor(15);
	cprintf("%d\r\n",cabfilefm.maxpat);
	
	cprintf("\r\n");
	textcolor(13);
	cprintf(" Placa de Som: ");
	textcolor(15);
	if (cardtype==0) cprintf("AdLib.");
	else cprintf("Sound Blaster Pro.");
	
	cprintf("\r\n");
	cprintf("\r\n");
	textcolor(11);
	cprintf(" Qualquer tecla para sair.");
	
}

/* Faz o Reset à Placa de Som FM AdLib */
void resetfm(void)
{
	int i;
	
	for (i=FM_MIN_REG; i<FM_MAX_REG; i++) writefm(i,0);
	
}

/* Inicializa a Placa de Som FM AdLib */
void initfm(void)
{

	/* Waveform Enable para todos os Canais */
	writefm(0x01,0x20);
	
	/* Tremolo e Vibrato Enable para todos os Canais */
	writefm(0xBD,0xC0);
	
}

/* Escreve um Valor num Registo FM AdLib */
void writefm(int reg, int value)
{
	
	int i,val;
	
	val=outp(ADL_REG,reg);					/* Escreve o Registo no Porto Register/Status */
	for (i = 0; i <6; i++) inp(ADL_REG);	/* Espera 12 ciclos */
	
	val=outp(ADL_DAT,value);				/* Escreve o Valor no Porto Data */
	for (i = 0; i <35; i++) inp(ADL_REG);   /* Espera 12 ciclos */
	
	val++;
	
}

/* Lê um Valor do Porto Status/Register FM AdLib */
int readfm(void)
{
	return (inp(ADL_REG));
}

/* Produz um Som FM AdLib */
void criasomfm(int canalg,int canalo,int freqindex)
{
	
	int freq,freql,freqh;
	int regb0;

	freq=notasfm[freqindex];
		
	freqh=(freq & 0xFF00)>>8;
	freql=(freq & 0X00FF);
	
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
	
	writefm(0xB0+canalg,regb0);  			/* Play Note, Oitava, Frequência Hi Byte */
			
}

/* Desliga o Canal FM AdLib */
void stopsomfm(int canalg)
{
	
	/* Desliga o Canal */
	writefm(0xB0+canalg,0x00);
	
}

/* Faz o Reset à Placa de Som FM Sound Blaster Pro */
void resetsb(void)
{
	int i;
	
	for (i=FM_MIN_REG; i<FM_MAX_REG; i++)
	{
		writelsb(i,0);
		writersb(i,0);
	}
	
}

/* Inicializa a Placa de Som FM Sound Blaster Pro */
void initsb(void)
{

	/* Waveform Enable para todos os Canais */
	writelsb(0x01,0x20);
	writersb(0x01,0x20);
	
	/* Tremolo e Vibrato Enable para todos os Canais */
	writelsb(0xBD,0xC0);
	writersb(0xBD,0xC0);
	
}

/* Escreve um Valor num Registo no Speaker Esquerdo Sound Blaster Pro */
void writelsb(int reg, int value)
{
	
	int i,val;
	
	val=outp(SBP_REG,reg);					/* Escreve o Registo no Porto Register/Status */
	for (i = 0; i <6; i++) inp(SBP_REG);	/* Espera 12 ciclos */
	
	val=outp(SBP_DAT,value);				/* Escreve o Valor no Porto Data */
	for (i = 0; i <35; i++) inp(SBP_REG);   /* Espera 12 ciclos */
	
	val++;
	
}

/* Escreve um Valor num Registo no Speaker Direito Sound Blaster Pro */
void writersb(int reg, int value)
{
	
	int i,val;
	
	val=outp(SBP_REG+2,reg);					/* Escreve o Registo no Porto Register/Status */
	for (i = 0; i <6; i++) inp(SBP_REG+2);	    /* Espera 12 ciclos */
	
	val=outp(SBP_DAT+2,value);				    /* Escreve o Valor no Porto Data */
	for (i = 0; i <35; i++) inp(SBP_REG+2);    /* Espera 12 ciclos */
	
	val++;
	
}

/* Lê um Valor do Porto Status/Register no Speaker Esquerdo Sound Blaster Pro */
int readlsb(void)
{
	return (inp(SBP_REG));
}

/* Lê um Valor do Porto Status/Register no Speaker Esquerdo Sound Blaster Pro */
int readrsb(void)
{
	return (inp(SBP_REG+2));
}

/* Produz um Som FM Sound Blaster Pro */
void criasomsb(int canalg,int canalo,int freqindex)
{
	
	int freq,freql,freqh;
	int regb0;

	freq=notasfm[freqindex];
		
	freqh=(freq & 0xFF00)>>8;
	freql=(freq & 0X00FF);

	if (canalg==0 || canalg==2 || canalg==4 || canalg==6 || canalg==8) /* Speaker Esquerdo */
	{
		/* Desliga o Canal */
		regb0=0*1+4*oitavafm+freqh;
		writelsb(0xB0+canalg,regb0);

		/* Toca o Instrumento */
		regb0=32*1+4*oitavafm+freqh;
	
		/* Operador 1 */
		writelsb(canalo,instreg[sifm].op1reg2); /* Tremolo, Vibrato, Hold Note, Keyboard Scaling Rate, Harmonic */
		writelsb(canalo+0x20,instreg[sifm].op1reg4); /* Scale Level, Volume Level */
		writelsb(canalo+0x40,instreg[sifm].op1reg6); /* Attack, Decay */
		writelsb(canalo+0x60,instreg[sifm].op1reg8); /* Sustain, Release */
		writelsb(canalo+0xC0,instreg[sifm].op1rege); /* Envelope Type */
	
		/* Operador 2 */
		writelsb(canalo+0x3,instreg[sifm].op2reg2); 	/* Tremolo, Vibrato, Hold Note, Keyboard Scaling Rate, Harmonic */
		writelsb(canalo+0x23,volfm); 				/* Scale Level, Volume Level */
		writelsb(canalo+0x43,instreg[sifm].op2reg6); /* Attack, Decay */
		writelsb(canalo+0x63,instreg[sifm].op2reg8); /* Sustain, Release */
		writelsb(canalo+0xC3,instreg[sifm].op2rege); /* Envelope Type */

		/* Geral */
		writelsb(0xC0+canalg,instreg[sifm].regc0);  	/* Feedback Modulation, FM Synthesis ON/OFF */
		writelsb(0xA0+canalg,freql); 				/* Frequência Low Byte */
	
		writelsb(0xB0+canalg,regb0);  			/* Play Note, Oitava, Frequência Hi Byte */
	}
	else	/* Speaker Direito */
	{
		/* Desliga o Canal */
		regb0=0*1+4*oitavafm+freqh;
		writersb(0xB0+canalg,regb0);

		/* Toca o Instrumento */
		regb0=32*1+4*oitavafm+freqh;
	
		/* Operador 1 */
		writersb(canalo,instreg[sifm].op1reg2); /* Tremolo, Vibrato, Hold Note, Keyboard Scaling Rate, Harmonic */
		writersb(canalo+0x20,instreg[sifm].op1reg4); /* Scale Level, Volume Level */
		writersb(canalo+0x40,instreg[sifm].op1reg6); /* Attack, Decay */
		writersb(canalo+0x60,instreg[sifm].op1reg8); /* Sustain, Release */
		writersb(canalo+0xC0,instreg[sifm].op1rege); /* Envelope Type */
	
		/* Operador 2 */
		writersb(canalo+0x3,instreg[sifm].op2reg2); 	/* Tremolo, Vibrato, Hold Note, Keyboard Scaling Rate, Harmonic */
		writersb(canalo+0x23,volfm); 				/* Scale Level, Volume Level */
		writersb(canalo+0x43,instreg[sifm].op2reg6); /* Attack, Decay */
		writersb(canalo+0x63,instreg[sifm].op2reg8); /* Sustain, Release */
		writersb(canalo+0xC3,instreg[sifm].op2rege); /* Envelope Type */

		/* Geral */
		writersb(0xC0+canalg,instreg[sifm].regc0);  	/* Feedback Modulation, FM Synthesis ON/OFF */
		writersb(0xA0+canalg,freql); 				/* Frequência Low Byte */
	
		writersb(0xB0+canalg,regb0);  			/* Play Note, Oitava, Frequência Hi Byte */
	}	
	
}

/* Desliga o Canal FM Sound Blaster Pro */
void stopsomsb(int canalg)
{
	
	sprintf(txtsts,"Som Desligado\0");
	printestado();
	
	/* Desliga o Canal */
	writelsb(0xB0+canalg,0x00);
	writersb(0xB0+canalg,0x00);
	
}


/* Imprime o Estado */
void printestado(void)
{
	
	cprintf("%-70s\r\n",txtsts);
		
}

/* Lê a Música FM a partir de um Ficheiro */
int lemusicafm(void)
{

	int i,s;
	
	/* Abre o Ficheiro para Leitura Binária */
	fp=fopen(nomefilefm,"rb");
	if(fp==NULL)
    {
      sprintf(txtsts,"Impossivel Abrir Musica FM!\0");
	  return(1);
    }

	/* Cabeçalho da Música FM */
	if (fread(&cabfilefm,sizeof(cabfilefm),1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Ler Musica FM!\0");
		fclose(fp);
		return(2);
	}
	
	/* Informação das Posições FM */
	if (fread(songpos,sizeof(songpos),1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Ler Musica FM!\0");
		fclose(fp);
		return(2);
	}
	
	/* Aloca Memória para as Patterns FM */
	patinfofm=(char*)calloc(SIZEPATINFOFM*cabfilefm.maxpat,sizeof(char));
	
	/* Informação das Patterns FM */
	if (fread(patinfofm,SIZEPATINFOFM*cabfilefm.maxpat,1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Ler Musica FM!\0");
		fclose(fp);
		return(2);
	}
	
	/* Informação dos Instrumentos FM */
	if (fread(instreg,sizeof(instreg),1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Ler Musica FM!\0");
		fclose(fp);
		return(2);
	}
	
	/* Correcção Oitava Instrumentos FM */
	if (fread(octinstfm,sizeof(octinstfm),1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Gravar Musica FM!\0");
		fclose(fp);
		return(2);
	}

	sprintf(txtsts,"Musica FM Lida com Sucesso!\0");

	fclose(fp);

	return(0);
}

/* Lê a Informação da Pattern FM corrente */
void carregapatfm(void)
{

	int i,j;
	int x,y;
	
	pattern=songpos[position];
	py=0;
	
	y=0;
	for (i=pattern*SIZEPATINFOFM; i<((pattern*SIZEPATINFOFM)+SIZEPATINFOFM); i+=SIZEROWFM)
	{
		x=0;
		for(j=0; j<SIZEROWFM; j+=2)
		{
			
			/* Byte 0 - Frequência, Oitava e Número do Efeito */
			fmefectnum[y][x]=(patinfofm[i+j+0] & 0xC0)>>6;
			fmoct[y][x]=(patinfofm[i+j+0] & 0x30)>>4;
			fmfreq[y][x]=(patinfofm[i+j+0] & 0X0F);
			
			/* Byte 1 - Valor do Efeito e Número do Sample */
			hi=(patinfofm[i+j+1] & 0xE0)>>5;
			lo=(patinfofm[i+j+1] & 0X1F);
			
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

/* Toca a Música Convertida FM */
void playmusicafm(void)
{
	
	int i,j;
	int salta=0,saltap;

	if (position<cabfilefm.songlen)
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
				
				sifm=fmsamplenum[i][j]-1;
				
			//	if (fmefectnum[i][j]==3)		/* Se houver controlo de Volume na Pattern */
			//	{
			//		hi=(int)(instreg[sifm].op2reg4/64);
			//		volfm=hi+(63-fmefectpar[i][j]);
			//	}
			//	else volfm=instreg[sifm].op2reg4;
			
				volfm=instreg[sifm].op2reg4;
			
				oitavafm=fmoct[i][j]+octinstfm[fmsamplenum[i][j]-1];
				
				if (cardtype==0)
					criasomfm(j,canaisfm[j],fmfreq[i][j]);	/* AdLib */
				else
					criasomsb(j,canaisfm[j],fmfreq[i][j]); /* Sound Blaster Pro */
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
			}
		}
		
		/* Salta para a Próxima Posição */
		if (salta==1)
		{
			row=0;
			position++;
			carregapatfm();
		}
		
		/* Salta para a Posição definida pelo Parâmetro */
		if (salta==2)
		{
			row=0;
			position=saltap;
			carregapatfm();
		}
			
	}
	else
	{
		playfm=0;
		for (i=0; i<FMNUNCANAIS; i++) stopsomfm(i);
		sprintf(txtsts,"Musica Parada\0");
		printestado();
	}

}