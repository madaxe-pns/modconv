/*
/*         SAA Player 1.5
/*
/*  Leitor de Músicas Philips SAA1099
/*  CMS Card/Game Blaster, Sam Coupé
/*
/*   Para compilar: tcc SAAPlayr.c
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

#define SIZEPATINFOSA 768	/* Tamanho da Pattern SAA1099 */
#define SIZEROWSA 12		/* Número de Bytes das Rows SAA1099 */
#define SAANUNCANAIS 6		/* Número de Canais SAA1099 */

void screenopen(void);
void screenclose(void);

int inicializa(void);
void mainmenu(void);
void printdados(void);

int lemusicasa(void);

void carregapatsa(void);
void playmusicasa(void);

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

/* Estado */
void printestado(void);

/* Tempo */
unsigned long letempo(void);

/* Manipulação de Bits */
unsigned char setbit(unsigned char n,unsigned char k);
unsigned char clearbit(unsigned char n,unsigned char k);
unsigned char togglebit(unsigned char n,unsigned char k);

/* Teclado */
int KB_code=0;

/* Modo de Som : Mono ou Estéreo */
int soundtype;

/* Ficheiros */
FILE *fp;
char nomefilesa[13];

/* Instrumentos SAA1099 */
unsigned char octinstsa[31];
short sisa;

/* Patterns SAA1099 */
unsigned char position;
unsigned char pattern,py;
unsigned char row;
unsigned char oitavasa[SAANUNCANAIS];
unsigned char canalinstsa[SAANUNCANAIS];
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

/* Música SAA1099 */
unsigned char playsa;
unsigned char songpos[128];
unsigned long playsastart,playsaend;

/* Manipulação de bits */
unsigned char hi,lo;

/* Time */
struct time tempo;

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
} instregsa[31];

/* Definição do Cabeçalho da Música SAA1099 */
struct cabfilessa
{
	char nome[20];
	unsigned char songlen;
	unsigned char maxpat;
} cabfilesa;

short notassa[13]={SA_V,SA_C,SA_Cs,SA_D,SA_Ds,SA_E,SA_F,SA_Fs,SA_G,SA_Gs,SA_A,SA_As,SA_B};
unsigned octsa[2][3]={0,0,0,0,0,0};

/* Estado */
char txtsts[70];


/* Main */
int main(int argc, char *argv[])
{
	
	if(argc==2)
	{
		sprintf(nomefilesa,argv[1]);
		soundtype=1;
	}
	else if(argc==3)
	{
		sprintf(nomefilesa,argv[1]);
		if (strcmp(argv[2],"mono")==0) soundtype=0;
		else soundtype=1;
	}
	else
	{
		sprintf(txtsts,"\r\nUtilizacao: SAAPlayr NomeMusica.saa soundtype");
		printestado();
		sprintf(txtsts,"\r\nsoundtype=mono para Mono");
		printestado();
		sprintf(txtsts,"Estereo por defeito");
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
	cprintf(" SAA Player 1.5 - ");
	textcolor(10);
	cprintf(" (C) 2022 Penisoft / MadAxe\r\n");
	
	textcolor(7);
	
}

/* Volta para o MS-Dos */
void screenclose(void)
{

	/* Liberta a Memória Alocada */
	free(patinfosa);
	
	resetsa();
		
	textbackground(0);
	textcolor(7);
	clrscr();	
	
	textcolor(11);
	cprintf("SAA Player 1.5\r\n");
	textcolor(10);
	cprintf("(C) 2022 Penisoft / MadAxe\r\n");
	
	_setcursortype(_NORMALCURSOR);
	
}

/* Inicializa o Programa */
int inicializa(void)
{
	
	int f,i;
	
	/* Reset Som SAA1099 */
	resetsa();
		
	/* Lê Música SAA1099 */
	f=lemusicasa();
	if (f!=0)
	{
		printestado();
		return(f);
	}
	
	/* Navegação nas Patterns */
	position=0;
	py=0;
	
	/* Navegação nos Instrumentos SAA1099 */
	sisa=0;
	
	/* Música */
	playsa=1;

	return(0);
	
}

/* Main Menu */
void mainmenu(void)
{

	int i;

	printdados();

	carregapatsa();

	playsastart=letempo();
	
	do
	{
		for (i=0; i<SAANUNCANAIS; i++)
			if (playenvlsa[i]!=0)
			{
				sisa=canalinstsa[i];
				envelopsa(i);
				setvolsa(i);
			}

		playsaend=letempo();
		if (playsaend-playsastart>=11) playmusicasa();
	}
	while (!kbhit() && playsa==1);
	
	if (playsa==1) KB_code=getch();
	
}

/* Imprime dados da Música SAA */
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
	cprintf("%s\r\n",cabfilesa.nome);
	
	textcolor(3);
	cprintf(" Posicoes: ");
	textcolor(15);
	cprintf("%d\r\n",cabfilesa.songlen);
	
	textcolor(3);
	cprintf(" Patterns: ");
	textcolor(15);
	cprintf("%d\r\n",cabfilesa.maxpat);
	
	cprintf("\r\n");
	textcolor(13);
	cprintf(" Tipo de Som: ");
	textcolor(15);
	if (!soundtype) cprintf("Mono.");
	else cprintf("Estereo.");
	
	cprintf("\r\n");
	cprintf("\r\n");
	textcolor(11);
	cprintf(" Qualquer tecla para sair.");
	
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

	if (envlssa[canal][0]!=0 || envlssa[canal][1]!=0 || envlssa[canal][2]!=0) playenvlsa[canal]=1;
	
	if (envlssa[canal][0]==0) volumesa[canal]=instregsa[sisa].volume;
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
	
	criasomsa(canal,freqindex);
	
}

/* Apenas atualiza o Volume dos Canais SAA1099 em caso de Envelope */
void setvolsa(unsigned char canal)
{
	
	if (soundtype==0)												/* Volume */
		writesa1(0x00+canal,(volumesa[canal]<<4)+volumesa[canal]);	/* Mono */
	else
	{
		if (canal==0 || canal==2 || canal==4)						/* Estéreo */
			writesa1(0x00+canal,volumesa[canal]<<4);
		else
			writesa1(0x00+canal,volumesa[canal]);
	}
				
}

/* Produz um Som SAA1099 */
void criasomsa(unsigned char canal,int freqindex)
{
	
	if (soundtype==0)												/* Volume */
		writesa1(0x00+canal,(volumesa[canal]<<4)+volumesa[canal]);	/* Mono */
	else
	{
		if (canal==0 || canal==2 || canal==4)						/* Estéreo */
			writesa1(0x00+canal,volumesa[canal]<<4);
		else
			writesa1(0x00+canal,volumesa[canal]);
	}
		
	writesa1(0x08+canal,notassa[freqindex]); 			/* Frequência */
		
	if (canal==0 || canal==1)
	{
		if (canal==0)
			octsa[0][0]=(octsa[0][0] & 0xF0)+oitavasa[canal];
		else
			octsa[0][0]=(oitavasa[canal]<<4)+(octsa[0][0] & 0X0F);
		
		writesa1(0x010,octsa[0][0]);  			/* Oitava */
	}
		
	if (canal==2 || canal==3)
	{
		if (canal==2)
			octsa[0][1]=(octsa[0][1] & 0xF0)+oitavasa[canal];
		else
			octsa[0][1]=(oitavasa[canal]<<4)+(octsa[0][1] & 0X0F);

		writesa1(0x011,octsa[0][1]);  			/* Oitava */
	}
		
	if (canal==4 || canal==5)
	{
		if (canal==4)
			octsa[0][2]=(octsa[0][2] & 0xF0)+oitavasa[canal];
		else
			octsa[0][2]=(oitavasa[canal]<<4)+(octsa[0][2] & 0X0F);

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

/* Imprime o Estado */
void printestado(void)
{
	
	cprintf("%-70s\r\n",txtsts);
		
}

/* Lê a Música SAA1099 a partir de um Ficheiro */
int lemusicasa(void)
{

	int i,s;
	
	/* Abre o Ficheiro para Leitura Binária */
	fp=fopen(nomefilesa,"rb");
	if(fp==NULL)
    {
      sprintf(txtsts,"Impossivel Abrir Musica SAA1099!\0");
	  return(1);
    }

	/* Cabeçalho da Música SAA1099 */
	if (fread(&cabfilesa,sizeof(cabfilesa),1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Ler Musica SAA1099!\0");
		fclose(fp);
		return(2);
	}
	
	/* Informação das Posições SAA1099 */
	if (fread(songpos,sizeof(songpos),1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Ler Musica SAA1099!\0");
		fclose(fp);
		return(2);
	}
	
	/* Aloca Memória para as Patterns SAA1099 */
	patinfosa=(char*)calloc(SIZEPATINFOSA*cabfilesa.maxpat,sizeof(char));
	
	/* Informação das Patterns SAA1099 */
	if (fread(patinfosa,SIZEPATINFOSA*cabfilesa.maxpat,1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Ler Musica SAA1099!\0");
		fclose(fp);
		return(2);
	}
	
	/* Informação dos Instrumentos SAA1099 */
	if (fread(instregsa,sizeof(instregsa),1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Ler Musica SAA1099!\0");
		fclose(fp);
		return(2);
	}
	
	/* Correcção Oitava Instrumentos SAA1099 */
	if (fread(octinstsa,sizeof(octinstsa),1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Ler Musica SAA1099!\0");
		fclose(fp);
		return(2);
	}

	sprintf(txtsts,"Musica SAA1099 Lida com Sucesso!\0");

	fclose(fp);

	return(0);
}

/* Lê a Informação da Pattern SAA1099 corrente */
void carregapatsa(void)
{

	int i,j;
	int x,y;
	
	pattern=songpos[position];
	py=0;
	
	y=0;
	for (i=pattern*SIZEPATINFOSA; i<((pattern*SIZEPATINFOSA)+SIZEPATINFOSA); i+=SIZEROWSA)
	{
		x=0;
		for(j=0; j<SIZEROWSA; j+=2)
		{
			
			/* Byte 0 - Frequência, Oitava e Número do Efeito */
			saefectnum[y][x]=(patinfosa[i+j+0] & 0xC0)>>6;
			saoct[y][x]=(patinfosa[i+j+0] & 0x30)>>4;
			safreq[y][x]=(patinfosa[i+j+0] & 0X0F);
			
			/* Byte 1 - Valor do Efeito e Número do Sample */
			hi=(patinfosa[i+j+1] & 0xE0)>>5;
			lo=(patinfosa[i+j+1] & 0X1F);
			
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

/* Toca a Música Convertida SAA1099 */
void playmusicasa(void)
{
	
	int i,j;
	int salta=0,saltap;

	playsastart=letempo();

	if (position<cabfilesa.songlen)
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
				
				sisa=sasamplenum[i][j]-1;
				
			//	if (saefectnum[i][j]==3)		/* Se houver controlo de Volume na Pattern */
			//		volumesa[j]=saefectpar[i][j];
			//	else volumesa[j]=instregsa[sisa].volume;
			
				volumesa[j]=instregsa[sisa].volume;
			
				oitavasa[j]=saoct[i][j]+octinstsa[sisa];	/* Correcção da Oitava */
				
				if (safreq[i][j]>9) oitavasa[j]++;
				
				canalinstsa[j]=sisa;
				
				precriasomsa(j,safreq[i][j]);
				
			}
		}

		/* Fica na Pattern Corrente e Passa para a Próxima Row */
		if (salta==0)
		{
			
			row++;
			if (row==64)
			{
				row=0;
				position++;
				carregapatsa();
			}
		}
		
		/* Salta para a Próxima Posição */
		if (salta==1)
		{
			row=0;
			position++;
			carregapatsa();
		}
		
		/* Salta para a Posição definida pelo Parâmetro */
		if (salta==2)
		{
			row=0;
			position=saltap;
			carregapatsa();
		}
			
	}
	else
	{
		for (i=0; i<SAANUNCANAIS; i++) stopsomsa(i);
		row=0;
		playsa=0;
		sprintf(txtsts,"Musica Parada\0");
		printestado();
		
	}

}

/* Lê o Tempo */
unsigned long letempo(void)
{

	gettime(&tempo);
	return(tempo.ti_hour*360000+tempo.ti_min*6000+tempo.ti_sec*100+tempo.ti_hund);
	
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