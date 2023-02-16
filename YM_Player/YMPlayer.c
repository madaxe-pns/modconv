/*

   Atari YM Player 1.2
	
    (C) 2023 Penisoft
	
*/

/* Default Includes */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* Atari Includes */
#include <tos.h>
#include <string.h>
#include <aes.h>
#include <ext.h>
#include <screen.h>


#define SIZEPATINFOAY 256	/* Tamanho da Pattern YM2149F */
#define SIZEROWAY 4			/* Número de Bytes das Rows YM2149F */
#define AYNUNCANAIS 3		/* Número de Canais YM2149F */

#define KB_ESCAPE 27
#define KB_SPACE 32
#define KB_L 108
#define KB_Q 113


void inicializa(void);
void mainmenu(void);
void mainloop(void);
void screenopen(void);
void screenclose(void);
int selecfile(void);
int fileselect(void);

int lemusicaay(void);
void navegamenu(void);
void printmenu(void);
void printdados(void);

void carregapatay(void);
void playmusicaay(void);

/* YM2149F */
void getenvlay(unsigned char canal);
void envelopay(unsigned char canal);
void precriasomay(unsigned char canal,int freqindex);
void setvolay(unsigned char canal);
void criasomay(unsigned char canal,int freqindex);
void resetay(void);

void printestado(void);

void gettosver(void);

/* File Selector */
char path[200],file[32];
int button,ret;

/* Manipulação de Bits */
unsigned char setbit(unsigned char n,unsigned char k);
unsigned char clearbit(unsigned char n,unsigned char k);
unsigned char togglebit(unsigned char n,unsigned char k);

/* Instrumentos YM2149F */
unsigned char octinstay[31];
short siay;

/* Patterns YM2149F */
unsigned char position;
unsigned char pattern,py;
unsigned char row;
unsigned char oitavaay[AYNUNCANAIS];
unsigned char canalinstay[AYNUNCANAIS];
unsigned char volumeay[AYNUNCANAIS];
unsigned char *patinfoay;
unsigned char freqenbay=0;
unsigned char noiseenbay=0;
int factoray=1;
unsigned char playenvlay[AYNUNCANAIS];
unsigned char envlsay[AYNUNCANAIS][3];
unsigned short cntenvlsay[AYNUNCANAIS][3];
unsigned short cntenvlsussay[AYNUNCANAIS];

/* Informação das Rows YM2149F */
unsigned char ayefectnum[64][AYNUNCANAIS];
unsigned char ayfreq[64][AYNUNCANAIS];
unsigned char ayoct[64][AYNUNCANAIS];
unsigned char ayefectpar[64][AYNUNCANAIS];
unsigned char aysamplenum[64][AYNUNCANAIS];
unsigned short mixer;

/* Música YM2149F */
unsigned char playay;
unsigned char songpos[128];

/* Tempo */
time_t time1,time2;
clock_t tmus1,tmus2;

/* Manipulação de bits */
unsigned char hi,lo;

/* Ficheiros */
FILE *fp;
char nomefileay[232]="maple.may\0";

/* Estado */
char txtsts[40];
int estado=0;

/* Teclado */
int KB_code=0;

/* Velocidade do Processador */
char *ADSPEED=(char*)0xF10000UL;

/* Memória do YM2149F */
char *REGSEL=(char*)0xFF8800UL;
char *REGDAT=(char*)0xFF8802UL;

/* Vídeo */
int old_rez;
int old_colors[16];
int new_colors[16];

long *phscreen;
long *lgscreen;

/* AES version */
int aesversion;

/* TOS version */
long tosversion;

/* Memória do TOS version */
long *TOSVER=(long*)0x4F2UL;

extern unsigned char freqoray[192];

/* Definição dos Registos para cada Instrumento YM2149F */
struct instregays
{
	unsigned char num;
	unsigned char freqenb;
	unsigned char noiseenb;
	unsigned char attack;
	unsigned char sustain;
	unsigned char decay;
	unsigned char volume;
	unsigned char repete;
} instregay[31];

/* Definição do Cabeçalho da Música YM2149F */
struct cabfilesay
{
	char nome[20];
	unsigned char songlen;
	unsigned char maxpat;
} cabfileay;

/* Main */
int main(void)
{
	
	screenopen();

	mainmenu();
	
	screenclose(); 
	
	return(0);
  
}

/* Inicializa o Modo Gráfico 640*200*4 */
void screenopen(void)
{
	
	int i;
	
/*	appl_init();
	aesversion=_GemParBlk.global[0];
	appl_exit();
	
	Supexec((long (*)( ))(gettosver));
*/
/*	Aloca a Memória para as Patterns */	
	patinfoay=(unsigned char*)calloc(SIZEPATINFOAY,sizeof(char));
			
/*	Vai buscar o Physbase e o Logbase */
	phscreen=Physbase();
	lgscreen=Logbase();

/*	Vai buscar a Resolução Antiga */
	old_rez=Getrez();

/* 	Vai buscar as Cores Antigas */
	for (i=0; i<16; i++)
		old_colors[i]=Setcolor(i,-1);
	
/* Define as Novas Cores */
	new_colors[0]=0;
	new_colors[1]=0x0777;
	new_colors[2]=0x0770;
	new_colors[3]=0x0067;
	
/*	Muda Para a Nova Resolução */
	Setscreen(lgscreen,phscreen,1);

/*	Muda Para as Novas Cores */	
	Setpalette(new_colors);
	
}

void screenclose(void)
{

/*	Liberta a Memória Alocada */	
	free(patinfoay);

/* 	Volta para as Cores Antigas */
	Setpalette(old_colors);

/*	Volta para a Resolução Antiga */	
	Setscreen(lgscreen,phscreen,old_rez);
	
}

/* Chama o File Dialog */
int selecfile(void)
{
	
	int f;

	f=fileselect();
	
	if (f!=0)
	{
		sprintf(txtsts,"Operacao Abortada!\0");
		return(1);
	}
	else
		if(ret!=0)
		{
			sprintf(txtsts,"Ficheiro nao Encontrado!\0");
			return(1);
		}

	strcpy(nomefileay,file);
	
	return(0);
	
}


/* Executa o File Dialog */
int fileselect(void)
{

	int i;

	path[0]='A'+Dgetdrv();
	path[1]=':';
	Dgetpath(path+2,0);
	strcat(path,"\\*.MAY");
	file[0]=0;


/*	if (aesversion>=0x0140)
		fsel_exinput(path,file,&button,"Load Music");
	else
*/		fsel_input(path,file,&button);

	if(button==0) return(1);
                                  
	for (i=(int)(strlen(path)); i>2; i--)
		if(path[i]=='\\') break;
	
	strcpy(path+i+1,file);
		
	ret=Fsfirst(path,0);

	return(0);
	
}

/* Main Menu */
void mainmenu(void)
{
	
	while (estado!=10)
	{
		switch (estado)
		{
			case 0:navegamenu();break;
			case 1:inicializa();break;
			case 2:Supexec((long (*)( ))(mainloop));break;
		}
	}
	
}

/* Inicializa o Programa */
void inicializa(void)
{
	
	int f;
	
	/* Música YM2149F */
	playay=0;

/*	File Dialog */	
	f=selecfile();
	if (f!=0)
	{
		estado=0;
		return;
	}
		
	/* Lê Música YM2149F */
	f=lemusicaay();
	if (f!=0)
	{
		estado=0;
		return;
	}
	
	estado=2;

}

/* Main Menu */
void mainloop(void)
{

	int i;
	
	/*	Muda para 8 MHz */
/*	*ADSPEED=0;*/
	
	printdados();

	/* Reset Som YM2149F */
	resetay();
	
	/* Navegação nas Patterns */
	pattern=0;
	position=0;
	py=0;
	row=0;
	
	/* Navegação nos Instrumentos YM2149F */
	siay=0;
	
	/* Música YM2149F */
	playay=1;
	
	carregapatay();

	tmus1=clock();
	
	do
	{
		while (!kbhit())
		{
			for (i=0; i<AYNUNCANAIS; i++)
				if (playenvlay[i]!=0)
				{
					siay=canalinstay[i];
					envelopay(i);
					setvolay(i);
				}

			tmus2=clock();
			if (tmus2>tmus1+23) playmusicaay();
		}
		
		KB_code=getch();
		
		if (KB_code==KB_L) estado=1;
		if (KB_code==KB_Q) estado=10;
		
	}
	while (estado==0);
	
/* 	Reset Som YM2149F */
	resetay();
	
}

/* Lê a Música YM2149F a partir de um Ficheiro */
int lemusicaay(void)
{

/*	int i,s; */

	/* Abre o Ficheiro para Leitura Binária */
	fp=fopen(nomefileay,"rb");
	if(fp==NULL)
    {
		sprintf(txtsts,"Impossivel Abrir Musica YM2149F!\0");
		return(1);
    }

	/* Cabeçalho da Música SAA1099 */
	if (fread(&cabfileay,sizeof(cabfileay),1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Ler Musica YM2149F!\0");
		fclose(fp);
		return(2);
	}
	
	/* Informação das Posições YM2149F */
	if (fread(songpos,sizeof(songpos),1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Ler Musica YM2149F!\0");
		fclose(fp);
		return(2);
	}
	
	/* Aloca a Nova Memória para as Patterns YM2149F */
	patinfoay=(unsigned char*)realloc(patinfoay,SIZEPATINFOAY*cabfileay.maxpat);
	
	/* Informação das Patterns YM2149F */
	if (fread(patinfoay,SIZEPATINFOAY*cabfileay.maxpat,1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Ler Musica YM2149F!\0");
		fclose(fp);
		return(2);
	}
	
	/* Informação dos Instrumentos YM2149F */
	if (fread(instregay,sizeof(instregay),1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Ler Musica YM2149F!\0");
		fclose(fp);
		return(2);
	}
	
	/* Correcção Oitava Instrumentos YM2149F */
	if (fread(octinstay,sizeof(octinstay),1,fp)==0)
	{
		sprintf(txtsts,"Erro ao Ler Musica YM2149F!\0");
		fclose(fp);
		return(2);
	}

	sprintf(txtsts,"Musica YM2149F Lida com Sucesso!\0");

	fclose(fp);

	return(0);
}

/* Imprime o Menu */
void printmenu(void)
{

	Clear_home(); 
	
    Goto_pos(0,0);
	Paper(0);
    Ink(1);
	printf("Atari YM Player 1.2");
	
    Goto_pos(2,0);
	Ink(2);
	printf("L - Carrega Musica\n\n");
	printf("Q - Sair");
	
	Goto_pos(6,0);
	Ink(1);
	printestado();
	
	Goto_pos(25,0);
    Ink(1);
	printf("(C) 2023 Penisoft / MadAxe");
	
	
	
/*	Goto_pos(20,0);
    Ink(1);
	printf("%u",tosversion);
*/	
	
}
	
/* Navega no Menu */
void navegamenu(void)
{
	
	printmenu();
	
	do
	{
		while (!kbhit()){}
		KB_code=getch();
		
		if (KB_code==KB_L) estado=1;
		if (KB_code==KB_Q) estado=10;
		
	}
	while (estado==0);
	
}

/* Imprime dados da Música YM2149F */
void printdados(void)
{

	Clear_home();

	printmenu();
	
	Goto_pos(8,0);
	Ink(3);
	printf("Nome: ");
	Ink(2);
	printf("%s",cabfileay.nome);
	
	Goto_pos(9,0);
	Ink(3);
	printf("Posicoes: ");
	Ink(2);
	printf("%d",cabfileay.songlen);
	
	Goto_pos(10,0);
	Ink(3);
	printf("Patterns: ");
	Ink(2);
	printf("%d",cabfileay.maxpat);
	
	Goto_pos(25,0);
    Ink(1);
	printf("(C) 2023 Penisoft / MadAxe");
	
}

/* Imprime o Estado */
void printestado(void)
{
	
	printf("%s\n",txtsts);
		
}

/* Lê a Informação da Pattern YM2149F corrente */
void carregapatay(void)
{

	int i,j;
	int x,y;
	
	pattern=songpos[position];
	py=0;
	
	y=0;
	for (i=pattern*SIZEPATINFOAY; i<((pattern*SIZEPATINFOAY)+SIZEPATINFOAY); i+=SIZEROWAY)
	{
		x=0;
		for(j=0; j<AYNUNCANAIS; j++)
		{
			
			/* Byte 0,1,2 - Frequência, Oitava e Low Número do Instrumento */
			aysamplenum[y][x]=(patinfoay[i+j+0] & 0xC0)>>6;
			ayoct[y][x]=(patinfoay[i+j+0] & 0x30)>>4;
			ayfreq[y][x]=(patinfoay[i+j+0] & 0X0F);
			
			
			/* Byte 3 - Hi Número do Instrumento e Número do Efeito */
			switch (j)
			{
				case 0:hi=((patinfoay[i+3] & 0xC0)>>6);break;
				case 1:hi=((patinfoay[i+3] & 0x30)>>4);break;
				case 2:hi=((patinfoay[i+3] & 0x0C)>>2);break;
			}
			
			aysamplenum[y][x]=(hi<<2)+aysamplenum[y][x];
			ayefectnum[y][x]=(patinfoay[i+3] & 0X03);
		
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

/* Toca a Música Convertida YM2149F */
void playmusicaay(void)
{
	
	int i,j;
	int salta=0,saltap;
	
	tmus1=clock();

	if (position<cabfileay.songlen)
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
				
				siay=aysamplenum[i][j]-1;
				
			/*	if (saefectnum[i][j]==3)		/* Se houver controlo de Volume na Pattern */
			/*		volumesa[j]=saefectpar[i][j];
			/*	else volumesa[j]=instregsa[sisa].volume;
			*/
				volumeay[j]=instregay[siay].volume;			/* Volume */
			
				oitavaay[j]=ayoct[i][j]+octinstay[siay];	/* Correcção da Oitava */
				
				canalinstay[j]=siay;						/* Instrumento a ser usado no Canal currente */
				
				precriasomay(j,ayfreq[i][j]);
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
				carregapatay();
			}
		}
		
		/* Salta para a Próxima Posição */
		if (salta==1)
		{
			row=0;
			position++;
			carregapatay();
		}
		
		/* Salta para a Posição definida pelo Parâmetro */
		if (salta==2)
		{
			row=0;
			position=saltap;
			carregapatay();
		}
			
	}
	else
	{
		pattern=0;
		position=0;
		py=0;
		row=0;
		siay=0;
		carregapatay();
	}

}

/* Saca os Valores do Envelope YM2149F */
void getenvlay(unsigned char canal)
{
	
	envlsay[canal][0]=instregay[siay].attack;
	envlsay[canal][1]=instregay[siay].sustain;
	envlsay[canal][2]=instregay[siay].decay;
	
	cntenvlsay[canal][0]=factoray*instregay[siay].attack;
	cntenvlsay[canal][1]=factoray*instregay[siay].sustain;
	cntenvlsay[canal][2]=factoray*instregay[siay].decay;
	
	if (envlsay[canal][1]!=0) cntenvlsussay[canal]=15;

	if (envlsay[canal][0]!=0 || envlsay[canal][1]!=0 || envlsay[canal][2]!=0) playenvlay[canal]=1;
	
	if (envlsay[canal][0]==0) volumeay[canal]=instregay[siay].volume;
	else volumeay[canal]=0;
	
}

/* Produz um Envelope YM2149F */
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
			{
				envlsay[canal][0]=0;
				if (envlsay[canal][1]==0 && envlsay[canal][2]==0) playenvlay[canal]=0;
			}
		}
	}

	/* Sustain */
	if (envlsay[canal][0]==0 && envlsay[canal][1]>0)
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

	/* Decay */
	if (envlsay[canal][0]==0 && envlsay[canal][1]==0 && envlsay[canal][2]>0)
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
	
	if (playenvlay[canal]==0 && instregay[siay].repete==1) getenvlay(canal);
	
}

/* Apenas atualiza o Volume dos Canais YM2149F em caso de Envelope */
void setvolay(unsigned char canal)
{
	
	if (canal==0)					/* Volume do Canal A */
	{
		*REGSEL=8;
		*REGDAT=volumeay[canal];
	}
		
	if (canal==1)					/* Volume do Canal B */
	{
		*REGSEL=9;
		*REGDAT=volumeay[canal];
	}
	
	if (canal==2)					/* Volume do Canal C */
	{
		*REGSEL=10;
		*REGDAT=volumeay[canal];
	}

}

/* Prepara a criação de um Som YM2149F */
void precriasomay(unsigned char canal,int freqindex)
{

	playenvlay[canal]=0;
	
	getenvlay(canal);
	
	criasomay(canal,freqindex);
	
}

/* Produz um Som YM2149F */
void criasomay(unsigned char canal,int freqindex)
{
	
	unsigned char freqlow,freqhi;
	
	freqlow=freqoray[(oitavaay[canal]-1)*24+(freqindex-1)*2];
	freqhi=freqoray[(oitavaay[canal]-1)*24+(freqindex-1)*2+1];
	
	if (canal==0)
	{
		*REGSEL=0;				/* Fine Picth do Canal A */
		*REGDAT=freqlow;
		*REGSEL=1;				/* Course Picth do Canal A */
		*REGDAT=freqhi;
		*REGSEL=8;				/* Volume do Canal A */
		*REGDAT=volumeay[canal];

		if (instregay[siay].freqenb==1) mixer=clearbit(mixer,1); /* Frequência Enable Canal A */
		else mixer=setbit(mixer,1);
		
		if (instregay[siay].noiseenb==1) mixer=clearbit(mixer,4); /* Noise Enable Canal A */
		else mixer=setbit(mixer,4);
	}
	
	if (canal==1)
	{
		*REGSEL=2;				/* Fine Picth do Canal B */
		*REGDAT=freqlow;
		*REGSEL=3;				/* Course Picth do Canal B */
		*REGDAT=freqhi;
		*REGSEL=9;				/* Volume do Canal B */
		*REGDAT=volumeay[canal];
		
		if (instregay[siay].freqenb==1) mixer=clearbit(mixer,2); /* Frequência Enable Canal B */
		else mixer=setbit(mixer,2);
		
		if (instregay[siay].noiseenb==1) mixer=clearbit(mixer,5); /* Noise Enable Canal B */
		else mixer=setbit(mixer,5);
	}
	
	if (canal==2)
	{
		*REGSEL=4;				/* Fine Picth do Canal C */
		*REGDAT=freqlow;
		*REGSEL=5;				/* Course Picth do Canal C */
		*REGDAT=freqhi;
		*REGSEL=10;				/* Volume do Canal C */
		*REGDAT=volumeay[canal];
		
		if (instregay[siay].freqenb==1) mixer=clearbit(mixer,3); /* Frequência Enable Canal C */
		else mixer=setbit(mixer,3);
		
		if (instregay[siay].noiseenb==1) mixer=clearbit(mixer,6); /* Noise Enable Canal C */
		else mixer=setbit(mixer,6);
		
	}

	*REGSEL=7;		/* Mixer */
	*REGDAT=mixer;

}

/* Desliga o Som YM2149F */
void resetay(void)
{

	int i;

	for (i=0; i<14; i++)
	{
		*REGSEL=i;
		*REGDAT=0;
	}
	
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

void gettosver(void)
{

/*	tosversion=*(long*)TOSVER;*/
	tosversion=*(long*)0x4F2UL;
	
}