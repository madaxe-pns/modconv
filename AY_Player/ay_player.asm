; Player de Músicas AY convertidas
; no ModConv

.org 25000

 jp inicio

; Cabeçalho
cab .dw 30000
nome .db 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
songlen .db 0
maxpat .db 0

; SongPos
position .db 0
songpos .dw 30022

; Patterns
pattern .db 0
patinfoini .dw 30150
patinfo .dw 0
row .db 0

; Definições dos Instrumentos
num .db 0
tom .db 0
noise .db 0
attack .db 0
sustain .db 0
decay .db 0
volume .db 0
repete .db 0
playenv .db 0

instindex .db 0
tabinstini .dw 0
tabinst .dw 0

; Definições da Oitava
instocta .db 0
tabocta .dw 0

; Definições Canal A
playenvca .db 0
attackia .db 0
attackca .db 0
cntattackca .db 0
sustainia .db 0
sustainca .db 0
cntsustainca .db 0
sustaiinca .db 0
decayia .db 0
decayca .db 0
cntdecayca .db 0
volumeia .db 0
volica .db 0
volfca .db 0
freqlowca .db 0
freqhica .db 0
tomca .db 0
noiseca .db 0
repeteca .db 0
effectca .db 0
effectparca .db 0
factora	.db 1

; Definições Canal B
playenvcb .db 0
attackib .db 0
attackcb .db 0
cntattackcb .db 0
sustainib .db 0
sustaincb .db 0
sustaiincb .db 0
cntsustaincb .db 0
decayib .db 0
decaycb .db 0
cntdecaycb .db 0
volumeib .db 0
volicb .db 0
volfcb .db 0
freqlowcb .db 0
freqhicb .db 0
tomcb .db 0
noisecb .db 0
repetecb .db 0
effectcb .db 0
effectparcb .db 0
factorb	.db 1

; Definições Canal C
playenvcc .db 0
attackic .db 0
attackcc .db 0
cntattackcc .db 0
sustainic .db 0
sustaincc .db 0
sustaiincc .db 0
cntsustaincc .db 0
decayic .db 0
decaycc .db 0
cntdecaycc .db 0
volumeic .db 0
volicc .db 0
volfcc .db 0
freqlowcc .db 0
freqhicc .db 0
tomcc .db 0
noisecc .db 0
repetecc .db 0
effectcc .db 0
effectparcc .db 0
factorc	.db 1

; Portos do AY-3-8912
ayctrl equ 65533
aydata equ 49149

; Tabela das Frequências das Notas AY
tabfreq equ 28100

; Informação sobre a Nota
oitava .db 4
nota .db 0
freqlow .db 0
freqhi .db 0
mixer .db 63
canal .db 0

; Play Musica
playmus .db 0

; Pausa
pausa .db 0

; Saída
sai .db 0

; Textos
texto000 .db 16,4,17,0,22,0,0,"AY Player 1.5                   "
		 .db 16,5,17,0,22,2,0,"Nome:"
		 .db 16,5,17,0,22,3,0,"Posicoes:"
		 .db 16,5,17,0,22,4,0,"Patterns:"
		 .db 16,6,17,0,22,6,0,"'Q' para sair"
		 .db 16,6,17,0,22,7,0,"'R' para recomecar"

texto001 .db 0

texto010 .db 16,4,17,0,22,1,0,"(C) 2022 Penisoft / MadAxe",16,7,17,0
texto011 .db 0

texto020 .db 16,0,17,7,22,0,0,"AY Player 1.5"
		 .db 16,0,17,7,22,1,0,"(C) 2022 Penisoft / MadAxe"
texto021 .db 0


; Início do Programa
inicio:

 call inicializaay
 call inicializa
 call clearscreen
 call printmenu
 call printdata

; Loop Princípal
main:

; Chama a Rotina de Leitura do Teclado
 call navega

; Chama a Rotina que Toca a Música
 ld a,(playmus)
 cp 1
 call z,playmusica

; Verifica se há Envelope no Canal A
 ld a,(playenvca)
 cp 1
 call z,envcanala

; Verifica se há Envelope no Canal B
 ld a,(playenvcb)
 cp 1
 call z,envcanalb

; Verifica se há Envnlope no Canal C
 ld a,(playenvcc)
 cp 1
 call z,envcanalc

 ld a,(playenv)
 cp 0
 call nz,setvol
; call nz,playsom

 ld a,(sai)	; Verifica a Saída
 cp 1
 jr nz,main

 jp fim


; Inicializa o Programa
inicializa:

 ld hl,(patinfoini)	; Patterns
 ld (patinfo),hl

 ld ix,(cab)
 ld a,(ix+20)		; SongLen
 ld (songlen),a
 ld a,(ix+21)		; MaxPat
 ld (maxpat),a

 ld hl,(patinfoini) ; Instrumentos
 ld a,(maxpat)

lincinst:
 inc h
 dec a
 jr nz,lincinst

 ld (tabinstini),hl

 ld de,248			; Correção das Oitavas
 add hl,de
 ld (tabocta),hl

 ld a,0
 ld (position),a

 ld hl,(songpos)	; Vai Buscar a Posição da SongPos
 ld a,(hl)
 ld (pattern),a		; Vai Buscar a Pattern correspondente a Posição da SonPos

 ld a,1				; Play Música
 ld (playmus),a

 call carregapat

 ret


; Navega nas Opções
navega:

; "Escape" - Sai para o "DOS"
 ld bc,64510
 in a,(c)
 bit 0,a ; Tecla 'q'
 call z, saida ; Sai do Programa

; "Reinício" - Faz o Restart da Música
 ld bc,64510
 in a,(c)
 bit 3,a ; Tecla 'r'
 call z, inicializa ; Inicializa o Programa


 ret


; Sai do Programa
saida:

 ld a,1
 ld (sai),a
 
 ret


; Rotina de Pausa
fazpausa:

 ld a,(pausa)
lpausa1:
 ld b,255
lpausa2:
 djnz lpausa2
 dec a
 jr nz,lpausa1

 ret


; Apaga o Écran
clearscreen:

;Border Preto
 ld a,0
 call 8859

;Paper Preto e Ink Branco
 ld a,$7
 ld hl,23693
 ld (hl),a
 call 3503

 ret


;Imprime o Menu
printmenu:

 ld a,2		; Ecrã superior
 call 5633

 ld de,texto000
 ld bc,texto001-texto000
 call 8252

 ld a,1		; Ecrã inferior
 call 5633

 ld de,texto010
 ld bc,texto011-texto010
 call 8252

 ret


; Imprime a Informação Módulo
printdata:

 ld a,2		; Ecrã superior
 call 5633

 ld ix,(cab)  ; Nome do Módulo

 ld a,16	; Ink
 rst 10H
 ld a,7
 rst 10H
 ld a,17	; Paper
 rst 10H
 ld a,0
 rst 10H
 ld a,22	; AT 2,9
 rst 10H
 ld a,2
 rst 10H
 ld a,6
 rst 10H

 ld a,20
printstring:
 push af
 ld a,(ix)
 cp 0
 jr nz,fimprintstring
 ld a,32
fimprintstring:
 rst 10H
 inc ix
 pop af
 dec a
 jr nz,printstring

 ld a,22	; Número de Posições
 rst 10H
 ld a,3
 rst 10H
 ld a,10
 rst 10H

 ld a,(songlen)
 ld b,0
 ld c,a
 call 11563          
 call 11747

 ld a,' '
 rst 10H
 ld a,' '
 rst 10H

 ld a,22	; Número de Patterns
 rst 10H
 ld a,4
 rst 10H
 ld a,10
 rst 10H

 ld a,(maxpat)
 ld b,0
 ld c,a
 call 11563          
 call 11747

; call controlo

 ret


; Lê a Informação da Pattern corrente
carregapat:

 ld hl,(songpos)	; Vai Buscar a Posição da SongPos
 ld a,(position)
 ld d,0
 ld e,a
 add hl,de
 ld a,(hl)
 ld (pattern),a		; Vai Buscar a Pattern correspondente a Posição da SonPos

 ld hl,(patinfoini)	; Aponta a PatInfo para a Pattern Corrente
 ld a,(pattern)

incpat:
 inc h
 dec a
 jr nz,incpat

 ld (patinfo),hl

 ld a,0				; Coloca a Row a Zero (Início da Pattern)
 ld (row),a
 
 ret


; Lê a Informação da Row Canal A corrente
getrowa:

; Canal A
 ld ix,(patinfo)

 ld a,(ix)
 push af
 res 7,a
 res 6,a
 res 5,a
 res 4,a
 ld (nota),a	; Nota Canal A
 pop af
 push af
 res 7,a
 res 6,a
 srl a
 srl a
 srl a
 srl a
 ld (oitava),a	; Oitava Canal A
 pop af
 srl a
 srl a
 srl a
 srl a
 srl a
 srl a
 ld (instindex),a	; Low Byte Instrumento Canal A

 ld a,(ix+3)		; Hi Byte Instrumento Canal A
 push af
 srl a
 srl a
 srl a
 srl a
 srl a
 srl a
 sla a
 sla a
 ld hl,instindex
 add a,(hl)
 ld (instindex),a ; Instrumento Canal A

 pop af			; Efeito Canal A
 res 7,a
 res 6,a
 res 5,a
 res 4,a
 res 3,a
 res 2,a
 ld (effectca),a

 bit 0,a	; Parâmetro Canal A se Efeito for 1
 jr z,fimgetrowa1

 ld a,1
 ld (effectca),a

 ld a,(ix+3)
 srl a
 ld (effectparca),a
 jr fimgetrowa2

fimgetrowa1:
 ld a,0
 ld (effectparca),a

fimgetrowa2:
 ret


; Lê a Informação da Row Canal B corrente
getrowb:

; Canal B
 ld ix,(patinfo)

 ld a,(ix+1)
 push af
 res 7,a
 res 6,a
 res 5,a
 res 4,a
 ld (nota),a	; Nota Canal B
 pop af
 push af
 res 7,a
 res 6,a
 srl a
 srl a
 srl a
 srl a
 ld (oitava),a	; Oitava Canal B
 pop af
 srl a
 srl a
 srl a
 srl a
 srl a
 srl a
 ld (instindex),a	; Low Byte Instrumento Canal B

 ld a,(ix+3)		; Hi Byte Instrumento Canal B
 push af
 res 7,a
 res 6,a
 srl a
 srl a
 srl a
 srl a
 sla a
 sla a
 ld hl,instindex
 add a,(hl)
 ld (instindex),a ; Instrumento Canal B

 pop af			; Efeito Canal B
 res 7,a
 res 6,a
 res 5,a
 res 4,a
 res 3,a
 res 2,a
 ld (effectcb),a

 bit 0,a	; Parâmetro Canal B se Efeito for 1
 jr z,fimgetrowb1

 ld a,1
 ld (effectcb),a

 ld a,(ix+3)
 srl a
 ld (effectparcb),a
 jr fimgetrowb2

fimgetrowb1:
 ld a,0
 ld (effectparcb),a

fimgetrowb2:
 ret


; Lê a Informação da Row Canal C corrente
getrowc:

; Canal C
 ld ix,(patinfo)

 ld a,(ix+2)
 push af
 res 7,a
 res 6,a
 res 5,a
 res 4,a
 ld (nota),a	; Nota Canal C
 pop af
 push af
 res 7,a
 res 6,a
 srl a
 srl a
 srl a
 srl a
 ld (oitava),a	; Oitava Canal C
 pop af
 srl a
 srl a
 srl a
 srl a
 srl a
 srl a
 ld (instindex),a	; Low Byte Instrumento Canal C

 ld a,(ix+3)		; Hi Byte Instrumento Canal B
 push af
 res 7,a
 res 6,a
 res 5,a
 res 4,a
 srl a
 srl a
 sla a
 sla a
 ld hl,instindex
 add a,(hl)
 ld (instindex),a ; Instrumento Canal B

 pop af			; Efeito Canal B
 res 7,a
 res 6,a
 res 5,a
 res 4,a
 res 3,a
 res 2,a
 ld (effectcc),a

 bit 0,a	; Parâmetro Canal C se Efeito for 1
 jr z,fimgetrowc1

 ld a,1
 ld (effectcc),a

 ld a,(ix+3)
 srl a
 ld (effectparcc),a
 jr fimgetrowc2

fimgetrowc1:
 ld a,0
 ld (effectparcc),a

fimgetrowc2:
 ret

; Lê a Informação do Instrumento Corrent
getinst:

 ld hl,(tabinstini)
 ld a,(instindex)
 dec a
 jr z,nxtgetinst ; Se já estivermos na Primeira Posição não Incrementa a Tabela

lgetinst:
 ld d,0
 ld e,8
 add hl,de
 dec a
 jr nz,lgetinst

nxtgetinst:
 ld (tabinst),hl

 ld ix,(tabinst)  

 ld a,(ix) 		; Número do Instrumento
 ld (num),a

 ld a,(ix+1)	; Tom
 ld (tom),a

 ld a,(ix+2)	; Noise
 ld (noise),a

 ld a,(ix+3)	; Attack
 ld (attack),a

 ld a,(ix+4)	; Sustain
 ld (sustain),a

 ld a,(ix+5)	; Decay
 ld (decay),a

 ld a,(ix+6)	; Volume
 ld (volume),a

 ld a,(ix+7)	; Repete
 ld (repete),a

 ret


; Vai Buscar a Respetiva Correção da Oitava
getoitava:

 ld hl,(tabocta)
 ld a,(instindex)
 dec a
 jr z,nxtgetoitava ; Se já estivermos na Primeira Posição não Incrementa a Tabela

 ld d,0
 ld e,a
 add hl,de

nxtgetoitava:
 ld a,(oitava)
 ld b,(hl)
 add a,b
 ld (oitava),a

 ret


; Toca a Música
playmusica:

 ld a,(23672)
 cp 6
 jp c,fimplaymusica

 ld a,0
 ld (23672),a

 ld a,(position)
 ld hl,songlen
 cp (hl)
 jp z,stopmusica

; Canal A
musca:
 ld a,0
 ld (canal),a

 call getrowa	; Vai Buscar a Informação ao Canal A na Row Corrente

 ld a,(effectca)
 cp 2			; Se o Efeito for 2 Salta para a Próxima Posição
 jp z,saltanxt

 cp 1			; Se o Efeito for 1 Salta para a Posição Definida pelo Parâmetro
 jp nz,tocaca
 ld a,(effectparca)
 ld (position),a
 jp saltapp

tocaca:
 ld a,(nota)	; Se não houver nota a Tocar 
 cp 0
 jp z,muscb

 call getinst
 call getoitava
 call preparasom

; Canal B
muscb:
 ld a,1
 ld (canal),a

 call getrowb	; Vai Buscar a Informação ao Canal B na Row Corrente

 ld a,(effectcb)
 cp 2			; Se o Efeito for 2 Salta para a Próxima Posição
 jp z,saltanxt

 cp 1			; Se o Efeito for 1 Salta para a Posição Definida pelo Parâmetro
 jp nz,tocacb
 ld a,(effectparcb)
 ld (position),a
 jp saltapp

tocacb:
 ld a,(nota)	; Se não houver nota a Tocar 
 cp 0
 jp z,muscc

 call getinst
 call getoitava
 call preparasom

; Canal C
muscc:
 ld a,2
 ld (canal),a

 call getrowc	; Vai Buscar a Informação ao Canal C na Row Corrente

 ld a,(effectcc)
 cp 2			; Se o Efeito for 2 Salta para a Próxima Posição
 jp z,saltanxt

 cp 1			; Se o Efeito for 1 Salta para a Posição Definida pelo Parâmetro
 jp nz,tocacc
 ld a,(effectparcc)
 ld (position),a
 jp saltapp

tocacc:
 ld a,(nota)	; Se não houver nota a Tocar 
 cp 0
 jp z,naosalta

 call getinst
 call getoitava
 call preparasom

naosalta:
 ld a,(row)
 cp 63
 jp z,saltanxt 	; Se já estivermos na Última Posição não Incrementa a Row

 inc a			; Incrementa a Row
 ld (row),a

 ld hl,(patinfo)
 ld de,4
 add hl,de
 ld (patinfo),hl

 jp fimplaymusica

saltanxt:		; Salta para a Próxima Posição
 ld a,0
 ld (row),a
 ld hl,position
 inc (hl)
 call carregapat
 jp fimplaymusica

saltapp:		; Salta para a Posição definida pelo Parâmetro 
 ld a,0
 ld (row),a
 call carregapat
 jp fimplaymusica

stopmusica:
 ld a,0
 ld (playmus),a

fimplaymusica:
 ret


; Coloca os valores do som nos respetivos portos
outer:

 ld bc,ayctrl
 out (c),d
 ld bc,aydata
 out (c),e

 ret

;Inicializa o leitor da música
inicializaay:

 ld d,0 	;fine picth do canal A
 ld e,0
 call outer
 ld d,1 	;course picth do canal A
 ld e,0
 call outer
 ld d,2 	;fine picth do canal B
 ld e,0
 call outer
 ld d,3 	;course picth do canal B
 ld e,0
 call outer
 ld d,4 	;fine picth do canal C
 ld e,0
 call outer
 ld d,5 	;course picth do canal C
 ld e,0
 call outer
 ld d,7 	;mixer: sem tom nem ruído nos canais ABC
 ld e,63
 call outer
 ld d,6 	;picth do ruído
 ld e,31
 call outer
 ld d,8 	;volume do canal A:
 ld e,0
 call outer
 ld d,9 	;volume do canal B
 ld e,0
 call outer
 ld d,10 	;volume do canal C
 ld e,0
 call outer
 ld d,11 	;duração fine do envelope
 ld e,0
 call outer
 ld d,12 	;duração course do envelope
 ld e,0
 call outer
 ld d,13 	;tipo de envelope
 ld e,0
 call outer

 ret


; Prepara a Criação de um Som
preparasom:

; Vai Busca a Frequência da Oitava+Nota
 call getfreq

; Verifica se é usado o Envelope
 ld a,(attack)
 cp 0
 jp nz,envelope

 ld a,(sustain)
 cp 0
 jp nz,envelope

 ld a,(decay)
 cp 0
 jp nz,envelope

; Se não houver Envelope então toca logo a Nota
 ld a,(canal)
 cp 0
 jr z,psgetvolfreqa

 ld a,(canal)
 cp 1
 jp z,psgetvolfreqb

 ld a,(canal)
 cp 2
 jp z,psgetvolfreqc

psgetvolfreqa:		; Vai buscar o Volume, a Frequência, o Tom e o Noise do Canal A
 ld hl,playenv
 res 0,(hl)
 ld a,0
 ld (playenvca),a
 ld a,(volume)
 ld (volica),a
 ld a,(freqhi)
 ld (freqhica),a
 ld a,(freqlow)
 ld (freqlowca),a
 ld a,(tom)
 ld (tomca),a
 ld a,(noise)
 ld (noiseca),a
 jr fimpsgetvol

psgetvolfreqb:		; Vai buscar o Volume, a Frequência, o Tom e o Noise do Canal B
 ld hl,playenv
 res 1,(hl)
 ld a,0
 ld (playenvcb),a
 ld a,(volume)
 ld (volicb),a
 ld a,(freqhi)
 ld (freqhicb),a
 ld a,(freqlow)
 ld (freqlowcb),a
 ld a,(tom)
 ld (tomcb),a
 ld a,(noise)
 ld (noisecb),a
 jr fimpsgetvol

psgetvolfreqc:		; Vai buscar o Volume, a Frequência, o Tom e o Noise do Canal C
 ld hl,playenv
 res 2,(hl)
 ld a,0
 ld (playenvcc),a
 ld a,(volume)
 ld (volicc),a
 ld a,(freqhi)
 ld (freqhicc),a
 ld a,(freqlow)
 ld (freqlowcc),a
 ld a,(tom)
 ld (tomcc),a
 ld a,(noise)
 ld (noisecc),a

fimpsgetvol:
 call playsom
 jp fimpreparasom

; Vai Buscar os Valores para o Envelope
envelope:
 ld a,(canal)
 cp 0
 jr z,psenvca

 ld a,(canal)
 cp 1
 jr z,psenvcb

 ld a,(canal)
 cp 2
 jp z,psenvcc

; Prepara o Envelope do Canal A
psenvca:
 ld a,(attack)
 ld (attackia),a
 ld a,(sustain)
 ld (sustainia),a
 ld a,(decay)
 ld (decayia),a

 ld a,(volume)
 ld (volumeia),a
 ld a,(repete)
 ld (repeteca),a

 ld a,(freqhi)
 ld (freqhica),a
 ld a,(freqlow)
 ld (freqlowca),a

 ld a,(tom)
 ld (tomca),a
 ld a,(noise)
 ld (noiseca),a

 call getenvca
 jp fimpsgetvol

; Prepara o Envelope do Canal B
psenvcb:
 ld a,(attack)
 ld (attackib),a
 ld a,(sustain)
 ld (sustainib),a
 ld a,(decay)
 ld (decayib),a

 ld a,(volume)
 ld (volumeib),a
 ld a,(repete)
 ld (repetecb),a

 ld a,(freqhi)
 ld (freqhicb),a
 ld a,(freqlow)
 ld (freqlowcb),a

 ld a,(tom)
 ld (tomcb),a
 ld a,(noise)
 ld (noisecb),a

 call getenvcb
 jp fimpsgetvol

; Prepara o Envelope do Canal C
psenvcc:
 ld a,(attack)
 ld (attackic),a
 ld a,(sustain)
 ld (sustainic),a
 ld a,(decay)
 ld (decayic),a

 ld a,(volume)
 ld (volumeic),a
 ld a,(repete)
 ld (repetecc),a

 ld a,(freqhi)
 ld (freqhicc),a
 ld a,(freqlow)
 ld (freqlowcc),a

 ld a,(tom)
 ld (tomcc),a
 ld a,(noise)
 ld (noisecc),a

 call getenvcc
 jp fimpsgetvol
 
fimpreparasom:
 ld a,1		; Ecrã inferior
 call 5633

 ret


; Envelope do Canal A
getenvca:

 ld hl,playenv
 set 0,(hl)

 ld a,1
 ld (playenvca),a

 ld a,(attackia)
 ld (attackca),a
 ld (cntattackca),a

 ld a,(sustainia)
 ld (sustainca),a
 ld (cntsustainca),a
 ld a,15
 ld (sustaiinca),a

 ld a,(decayia)
 ld (decayca),a
 ld (cntdecayca),a

 ld a,(volumeia)
 ld (volica),a
 ld a,0
 ld (volfca),a

 ld a,(attackca)
 cp 0
 jr z,fimgetenvca

 ld a,0
 ld (volica),a
 ld a,(volumeia)
 ld (volfca),a
 
fimgetenvca:
 ret


; Envelope do Canal B
getenvcb:

 ld hl,playenv
 set 1,(hl)

 ld a,1
 ld (playenvcb),a

 ld a,(attackib)
 ld (attackcb),a
 ld (cntattackcb),a

 ld a,(sustainib)
 ld (sustaincb),a
 ld (cntsustaincb),a
 ld a,15
 ld (sustaiincb),a

 ld a,(decayib)
 ld (decaycb),a
 ld (cntdecaycb),a

 ld a,(volumeib)
 ld (volicb),a
 ld a,0
 ld (volfcb),a

 ld a,(attackcb)
 cp 0
 jr z,fimgetenvcb

 ld a,0
 ld (volicb),a
 ld a,(volumeib)
 ld (volfcb),a

fimgetenvcb:
 ret


; Envelope do Canal C
getenvcc:

 ld hl,playenv
 set 2,(hl)

 ld a,1
 ld (playenvcc),a

 ld a,(attackic)
 ld (attackcc),a
 ld (cntattackcc),a

 ld a,(sustainic)
 ld (sustaincc),a
 ld (cntsustaincc),a
 ld a,15
 ld (sustaiincc),a

 ld a,(decayic)
 ld (decaycc),a
 ld (cntdecaycc),a

 ld a,(volumeic)
 ld (volicc),a
 ld a,0
 ld (volfcc),a

 ld a,(attackcc)
 cp 0
 jr z,fimgetenvcc

 ld a,0
 ld (volicc),a
 ld a,(volumeic)
 ld (volfcc),a

fimgetenvcc:
 ret


; Toca um Som
playsom:

; ld a,(canal) ; Vai buscar o Canal Selecionado
; cp 0
; jr z, plscanala
; cp 1
; jp z, plscanalb
; cp 2
; jp z, plscanalc

; Canal A
plscanala:
 ld d,0 		; Fine Picth do Canal A
 ld a,(freqlowca)
 ld e,a
 call outer
 ld d,1 		; Course Picth do Canal A
 ld a,(freqhica)
 ld e,a
 call outer
 ld d,8 		; Volume do Canal A
 ld a,(volica)
 ld e,a
 call outer

 ld hl,mixer
 ld a,(tomca)	; Verifica o Tom no Canal A
 cp 1
 jr z,canalatom
 set 0,(hl)		; Desable Tom no Canal A
 jr vercanalanoise 
canalatom:
 res 0,(hl)		; Enable Tom no Canal A

vercanalanoise:
 ld a,(noiseca) ; Verifica Ruído no Canal A
 cp 1
 jr z,canalanoise
 set 3,(hl)   	; Desable Noise no Canal A
 jr plscanalb
canalanoise:
 res 3,(hl)	  	; Enable Noise no Canal A

;canalamixer:
; ld d,7
; ld a,(mixer)
; ld e,a
; call outer
; jp saiplaysom

; Canal B
plscanalb:
 ld d,2 		; Fine Picth do Canal B
 ld a,(freqlowcb)
 ld e,a
 call outer
 ld d,3 		; Course Picth do Canal B
 ld a,(freqhicb)
 ld e,a
 call outer
 ld d,9 		; Volume do Canal B
 ld a,(volicb)
 ld e,a
 call outer

 ld hl,mixer
 ld a,(tomcb)	; Verifica o Tom no Canal B
 cp 1
 jr z,canalbtom
 set 1,(hl) 	; Desable Tom no Canal B
 jr vercanalbnoise
canalbtom:
 res 1,(hl) 	; Enable Tom no Canal B

vercanalbnoise:
 ld a,(noisecb) ; Verifica Ruído no Canal B
 cp 1
 jr z,canalbnoise
 set 4,(hl)   	; Desable Noise no Canal B
 jr plscanalc
canalbnoise:
 res 4,(hl)   	; Enable Noise no Canal B

;canalbmixer:
; ld d,7
; ld a,(mixer)
; ld e,a
; call outer
; jr saiplaysom

; Canal C
plscanalc:
 ld d,4 		; Fine Picth do Canal C
 ld a,(freqlowcc)
 ld e,a
 call outer
 ld d,5 		; Course Picth do Canal C
 ld a,(freqhicc)
 ld e,a
 call outer
 ld d,10 		; Volume do Canal C
 ld a,(volicc)
 ld e,a
 call outer

 ld hl,mixer
 ld a,(tomcc)	; Verifica o Tom no Canal C
 cp 1
 jr z,canalctom
 set 2,(hl) 	; Desable Tom no Canal C
 jr vercanalcnoise
canalctom:
 res 2,(hl) 	; Enable Tom no Canal C

vercanalcnoise:
 ld a,(noisecc) ; Verifica Ruído no Canal C
 cp 1
 jr z,canalcnoise
 set 5,(hl)   	; Desable Noise no Canal C
 jr canalmixer
canalcnoise:
 res 5,(hl)   ; Enable Noise no Canal C

canalmixer:
 ld d,7
 ld a,(mixer)
 ld e,a
 call outer

saiplaysom:
 ret


; Apenas atualiza o Volume dos Canais em caso de Envelope
setvol:

; ld a,(canal) ; Vai buscar o Canal Selecionado
; cp 0
; jr z, svcanala
; cp 1
; jp z, svcanalb
; cp 2
; jp z, svcanalc

; Canal A
svcanala:
 ld d,8 		; Volume do Canal A
 ld a,(volica)
 ld e,a
 call outer

; Canal B
svcanalb:
 ld d,9 		; Volume do Canal B
 ld a,(volicb)
 ld e,a
 call outer

; Canal C
svcanalc:
 ld d,10 		; Volume do Canal C
 ld a,(volicc)
 ld e,a
 call outer

saisetvol:
 ret


; Desliga o Som
stopsom:

 call inicializaay

 ld a,1		; Ecrã inferior
 call 5633

 ret


; Vai Buscar a Frequência à Tabela de Frequências
getfreq:

 ld a,(oitava)
 cp 0
 jr nz,nxtgetfrq

 ld a,(nota)
 cp 0
 jr nz,nxtgetfrq

 ld a,0
 ld (freqlow),a
 ld (freqhi),a

 jr fimgetfreq

nxtgetfrq:
 ld hl,tabfreq

 ld a,(oitava)
 dec a
 jr z, assnota	; Se for a Primeira Oitava não faz nada

 ld de,24
assoctva:
 add hl,de
 dec a
 jr nz, assoctva

assnota:
 ld a,(nota)
 dec a
 sla a
 ld d,0
 ld e,a
 add hl,de

 ld a,(hl)
 ld (freqlow),a
 inc hl
 ld a,(hl)
 ld (freqhi),a

fimgetfreq:
 ret


; Envelope do Canal A
envcanala:

 ld hl,factora
 ld a,(hl)
 cp 0
 jr z,senvcanala
 dec (hl)
 jp fimenvcanala2	

senvcanala
 ld (hl),1

; Attack Canal A
 ld a,(attackca)	; Verifica se estamos numa fase de Attack
 cp 0
 jp z,ecasustain	; Se não passa para o próximo Passo - Sustain

 ld hl,cntattackca 	; Decrementa o Contador do Attack
 dec (hl)
 jp nz,fimenvcanala	; Se não for Zero então e sai da Rotina

 ld a,(volfca)		; Se for Zero
 ld hl,volica       
 inc (hl)			; Incrementa o Volume do Canal A
 cp (hl)			; Verifica se o Volume Atual é igual ao Volume Final
 jp z,fmincvolca	; Se Sim Termina a Fase de Attack
					
 ld a,(attackca)	
 ld (cntattackca),a	; Faz o Reset ao Contador do Attack e Sai da Rotina
 jp fimenvcanala

fmincvolca:
 ld a,0
 ld (attackca),a	; Desliga o Modo de Attack
 ld (volfca),a		; Coloca o Voume Final a Zero

 ld a,(sustainca)	; Verifica se há Fase de Sustain
 cp 0 
 jp nz,fimenvcanala ; Se houver sai da Rotina

 ld a,(decayca)		; Verifica se há Fase de Decay
 cp 0 
 jp nz,fimenvcanala ; Se houver sai da Rotina
 
 ld a,0				; Se não houver Sustain nem Decay Desliga o Envelope
 ld (playenvca),a	; do Canal A e sai da Rotina
 ld hl,playenv
 res 0,(hl)
 jp fimenvcanala

; Sustain Canal A
ecasustain:
 ld a,(sustainca)	; Verifica se estamos numa fase de Sustain
 cp 0
 jp z,ecadecay		; Se não passa para o próximo Passo - Decay

 ld hl,cntsustainca	; Decrementa o Contador do Sustain
 dec (hl)
 jp nz,fimenvcanala	; Se não for Zero então e sai da Rotina

 ld hl,sustaiinca	; Decrementa o Sustain Atual
 dec (hl)
 jp z,fmindecsusca	; Se for Zero Termina a Fase de Sustain

 ld a,(sustainca)	
 ld (cntsustainca),a; Faz o Reset ao Contador do Attack e Sai da Rotina
 jp fimenvcanala

fmindecsusca:
 ld a,0
 ld (sustainca),a	; Desliga o Modo de Sustain

 ld a,(decayca)		; Verifica se há Fase de Decay
 cp 0 
 jp nz,fimenvcanala ; Se houver sai da Rotina
 
 ld a,0				; Se não houver Decay Desliga o Envelope
 ld (playenvca),a	; do Canal A e sai da Rotina
 ld (volica),a		; Desliga o Volume do Canal A
 ld hl,playenv
 res 0,(hl)
 jp fimenvcanala

; Decay Canal A
ecadecay:
 ld a,(decayca)		; Verifica se estamos numa fase de Decay
 cp 0
 jp z,fimenvcanala	; Se não Sai da Rotina

 ld hl,cntdecayca 	; Decrementa o Contador do Decay
 dec (hl)
 jp nz,fimenvcanala	; Se não for Zero então e sai da Rotina

 ld a,(volfca)		; Se for Zero
 ld hl,volica
 dec (hl)			; Decrementa o Volume do Canal A
 cp (hl)			; Verifica se o Volume Atual é igual ao Volume Final
 jp z,fmdecvolca	; Se Sim Termina a Fase de Decay

 ld a,(decayca)	
 ld (cntdecayca),a	; Faz o Reset ao Contador do Decay e Sai da Rotina
 jp fimenvcanala

fmdecvolca:
 ld a,0
 ld (decayca),a		; Desliga o Modo de Decay

 ld (playenvca),a	; Desliga o Envelope do Canal A e sai da Rotina
 ld hl,playenv
 res 0,(hl)
	
fimenvcanala:
; call playsom

 ld a,(playenvca)	; Verifica se o Repete está Enable
 cp 0
 jr nz,fimenvcanala2
 ld a,(repeteca)
 cp 0
 jr z,fimenvcanala2	; Se estiver Desable então sai da Rotina

 call getenvca		; Se estiver Enable começa o Envelope do Início

fimenvcanala2:
 ret


; Envelope do Canal B
envcanalb:

 ld hl,factorb
 ld a,(hl)
 cp 0
 jr z,senvcanalb
 dec (hl)
 jp fimenvcanalb2	

senvcanalb
 ld (hl),1

; Attack Canal B
 ld a,(attackcb)	; Verifica se estamos numa fase de Attack
 cp 0
 jp z,ecbsustain	; Se não passa para o próximo Passo - Sustain

 ld hl,cntattackcb 	; Decrementa o Contador do Attack
 dec (hl)
 jp nz,fimenvcanalb	; Se não for Zero então e sai da Rotina

 ld a,(volfcb)		
 ld hl,volicb
 inc (hl)			; Incrementa o Volume do Canal B
 cp (hl)			; Verifica se o Volume Atual é igual ao Volume Final
 jp z,fmincvolcb	; Se Sim Termina a Fase de Attack
 
 ld a,(attackcb)	
 ld (cntattackcb),a	; Faz o Reset ao Contador do Attack e Sai da Rotina
 jp fimenvcanalb

fmincvolcb:
 ld a,0
 ld (attackcb),a	; Desliga o Modo de Attack
 ld (volfcb),a		; Coloca o Voume Final a Zero

 ld a,(sustaincb)	; Verifica se há Fase de Sustain
 cp 0 
 jp nz,fimenvcanalb ; Se houver sai da Rotina

 ld a,(decaycb)		; Verifica se há Fase de Decay
 cp 0 
 jp nz,fimenvcanalb ; Se houver sai da Rotina
 
 ld a,0				; Se não houver Sustain nem Decay Desliga o Envelope
 ld (playenvcb),a	; do Canal B e sai da Rotina
 ld hl,playenv
 res 1,(hl)
 jp fimenvcanalb

; Sustain Canal B
ecbsustain:
 ld a,(sustaincb)	; Verifica se estamos numa fase de Sustain
 cp 0
 jp z,ecbdecay		; Se não passa para o próximo Passo - Decay

 ld hl,cntsustaincb	;Decrementa o Contador do Sustain
 dec (hl)
 jp nz,fimenvcanalb	; Se não for Zero então e sai da Rotina

 ld hl,sustaiincb	; Decrementa o Sustain Atual
 dec (hl)
 jp z,fmindecsuscb	; Se for Zero Termina a Fase de Sustain

 ld a,(sustaincb)	
 ld (cntsustaincb),a; Faz o Reset ao Contador do Attack e Sai da Rotina
 jp fimenvcanalb

fmindecsuscb:
 ld a,0
 ld (sustaincb),a	; Desliga o Modo de Sustain

 ld a,(decaycb)		; Verifica se há Fase de Decay
 cp 0 
 jp nz,fimenvcanalb ; Se houver sai da Rotina
 
 ld a,0				; Se não houver Decay Desliga o Envelope
 ld (playenvcb),a	; do Canal B e sai da Rotina
 ld (volicb),a		; Desliga o Volume do Canal B
 ld hl,playenv
 res 1,(hl)
 jp fimenvcanalb

; Decay Canal B
ecbdecay:
 ld a,(decaycb)		; Verifica se estamos numa fase de Decay
 cp 0
 jp z,fimenvcanalb	; Se não Sai da Rotina

 ld hl,cntdecaycb 	; Decrementa o Contador do Decay
 dec (hl)
 jp nz,fimenvcanalb	; Se não for Zero então e sai da Rotina

 ld a,(volfcb)		
 ld hl,volicb
 dec (hl)			; Decrementa o Volume do Canal B
 cp (hl)			; Verifica se o Volume Atual é igual ao Volume Final
 jp z,fmdecvolcb	; Se Sim Termina a Fase de Decay
 
 ld a,(decaycb)	
 ld (cntdecaycb),a	; Faz o Reset ao Contador do Decay e Sai da Rotina
 jp fimenvcanalb

fmdecvolcb:
 ld a,0
 ld (decaycb),a		; Desliga o Modo de Decay
				
 ld (playenvcb),a	; Desliga o Envelope do Canal B e sai da Rotina
 ld hl,playenv
 res 1,(hl)

fimenvcanalb:
; call playsom

 ld a,(playenvcb)	; Verifica se o Repete está Enable
 cp 0
 jr nz,fimenvcanalb2
 ld a,(repetecb)
 cp 0
 jr z,fimenvcanalb2	; Se estiver Desable então sai da Rotina

 call getenvcb		; Se estiver Enable começa o Envelope do Início

fimenvcanalb2:
 ret


; Envelope do Canal C
envcanalc:

 ld hl,factorc
 ld a,(hl)
 cp 0
 jr z,senvcanalc
 dec (hl)
 jp fimenvcanalc2	

senvcanalc
 ld (hl),1

; Attack Canal C
 ld a,(attackcc)	; Verifica se estamos numa fase de Attack
 cp 0
 jp z,eccsustain	; Se não passa para o próximo Passo - Sustain

 ld hl,cntattackcc 	; Decrementa o Contador do Attack
 dec (hl)
 jp nz,fimenvcanalc	; Se não for Zero então e sai da Rotina

 ld a,(volfcc)		
 ld hl,volicc
 inc (hl)			; Incrementa o Volume do Canal C
 cp (hl)			; Verifica se o Volume Atual é igual ao Volume Final
 jp z,fmincvolcc	; Se Sim Termina a Fase de Attack
 
 ld a,(attackcc)	
 ld (cntattackcc),a	; Faz o Reset ao Contador do Attack e Sai da Rotina
 jp fimenvcanalc

fmincvolcc:
 ld a,0
 ld (attackcc),a	; Desliga o Modo de Attack
 ld (volfcc),a		; Coloca o Voume Final a Zero

 ld a,(sustaincc)	; Verifica se há Fase de Sustain
 cp 0 
 jp nz,fimenvcanalc ; Se houver sai da Rotina

 ld a,(decaycc)		; Verifica se há Fase de Decay
 cp 0 
 jp nz,fimenvcanalc ; Se houver sai da Rotina
 
 ld a,0				; Se não houver Sustain nem Decay Desliga o Envelope
 ld (playenvcc),a	; do Canal C e sai da Rotina
 ld hl,playenv
 res 2,(hl)
 jp fimenvcanalc

; Sustain Canal C
eccsustain:
 ld a,(sustaincc)	; Verifica se estamos numa fase de Sustain
 cp 0
 jp z,eccdecay		; Se não passa para o próximo Passo - Decay

 ld hl,cntsustaincc	;Decrementa o Contador do Sustain
 dec (hl)
 jp nz,fimenvcanalc	; Se não for Zero então e sai da Rotina

 ld hl,sustaiincc	; Decrementa o Sustain Atual
 dec (hl)
 jp z,fmindecsuscc	; Se for Zero Termina a Fase de Sustain

 ld a,(sustaincc)	
 ld (cntsustaincc),a; Faz o Reset ao Contador do Attack e Sai da Rotina
 jp fimenvcanalc

fmindecsuscc:
 ld a,0
 ld (sustaincc),a	; Desliga o Modo de Sustain

 ld a,(decaycc)		; Verifica se há Fase de Decay
 cp 0 
 jp nz,fimenvcanalc ; Se houver sai da Rotina
 
 ld a,0				; Se não houver Decay Desliga o Envelope
 ld (playenvcc),a	; do Canal C e sai da Rotina
 ld (volicc),a		; Desliga o Volume do Canal C
 ld hl,playenv
 res 2,(hl)
 jp fimenvcanalc

; Decay Canal C
eccdecay:
 ld a,(decaycc)		; Verifica se estamos numa fase de Decay
 cp 0
 jp z,fimenvcanalc	; Se não Sai da Rotina

 ld hl,cntdecaycc 	; Decrementa o Contador do Decay
 dec (hl)
 jp nz,fimenvcanalc	; Se não for Zero então e sai da Rotina

 ld a,(volfcc)		
 ld hl,volicc
 dec (hl)			; Decrementa o Volume do Canal C
 cp (hl)			; Verifica se o Volume Atual é igual ao Volume Final
 jp z,fmdecvolcc	; Se Sim Termina a Fase de Decay

 ld a,(decaycc)	
 ld (cntdecaycc),a	; Faz o Reset ao Contador do Decay e Sai da Rotina
 jp fimenvcanalc

fmdecvolcc:
 ld a,0
 ld (decaycc),a		; Desliga o Modo de Decay

 ld (playenvcc),a	; Desliga o Envelope do Canal C e sai da Rotina
 ld hl,playenv
 res 2,(hl)

fimenvcanalc:
; call playsom

 ld a,(playenvcc)	; Verifica se o Repete está Enable
 cp 0
 jr nz,fimenvcanalc2
 ld a,(repetecc)
 cp 0
 jr z,fimenvcanalc2	; Se estiver Desable então sai da Rotina

 call getenvcc		; Se estiver Enable começa o Envelope do Início

fimenvcanalc2:
 ret


; Imprime Valores de Controlo
controlo:

 ld a,2		; Ecrã superior
 call 5633

 ld a,22	; Row
 rst 10H
 ld a,13
 rst 10H
 ld a,20
 rst 10H

; ld a,(row)
 ld bc,(23672)
 call 11563          
 call 11747

 ld a,' '
 rst 10H

 ret


; Fim do Programa
fim:

 call inicializaay

; Border Branco
 ld a,7
 call 8859

; Paper Branco e Ink Preto
 ld a,56
 ld hl,23693
 ld (hl),a
 call 3503

 ld a,2		; Ecrã superior
 call 5633

; Imprime Texto de Saída
 ld de,texto020
 ld bc,texto021-texto020
 call 8252

 ret

.end