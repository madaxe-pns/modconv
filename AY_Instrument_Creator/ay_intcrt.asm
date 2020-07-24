; AY Player 1.0
; Player de Instrumentos Musicais Criados no 
; AY_Instrument_Creator 1.3
; (C) 2020 Penisoft / MadAxe

.org 25000

 jp inicio

; Definições dos Instrumentos
nome .db 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
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
tabinst .dw 32000

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

; Portos do AY-3-8912
ayctrl equ 65533
aydata equ 49149

; Tabela das Frequências das Notas AY
tabfreq equ 30000

; Informação sobre a Nota
oitava .db 4
nota .db 0
freqlow .db 0
freqhi .db 0
mixer .db 63
canal .db 0

; Pausa
pausa .db 0

; Saída
sai .db 0

; Textos
texto000 .db 16,6,17,1,22,0,0,"AY Instrument Player 1.0        "
		 .db 16,5,17,0,22,2,0,"Nome:"
		 .db 16,5,17,0,22,4,0,"Posicao:"
		 .db 16,5,17,0,22,6,0,"Tom:"
		 .db 16,5,17,0,22,7,0,"Noise:"
		 .db 16,5,17,0,22,9,0,"Attack:"
		 .db 16,5,17,0,22,10,0,"Sustain:"
		 .db 16,5,17,0,22,11,0,"Decay:"
		 .db 16,5,17,0,22,13,0,"Volume:"
		 .db 16,5,17,0,22,14,0,"Repete:"
		 .db 16,5,17,0,22,16,0,"Oitava:"
		 .db 16,5,17,0,22,18,0,"Canal:"
		 .db 16,7,17,7,22,20,0," ",17,0,"S",17,7," ",17,0,"D  "
		 .db 17,7," ",17,0,"G",17,7," ",17,0,"H",17,7," ",17,0,"J"
		 .db 16,0,17,7,22,21,0,"Z X C V B N M",17,0
texto001 .db 0

texto010 .db 16,6,17,1,22,1,0,"                                ",16,7,17,0
texto011 .db 0

texto020 .db 16,0,17,7,22,0,0,"AY Instrument Player 1.0"
		 .db 16,0,17,7,22,1,0,"(C) 2020 Penisoft / MadAxe"
texto021 .db 0

texto030 .db 16,6,17,1,22,1,0,"Playing   ",16,7,17,0
texto031 .db 0

texto040 .db 16,6,17,1,22,1,0,"Som Parado",16,7,17,0
texto041 .db 0

texto0a0 .db 16,7,17,0,22,18,9,"A"
texto0a1 .db 0

texto0b0 .db 16,7,17,0,22,18,9,"B"
texto0b1 .db 0

texto0c0 .db 16,7,17,0,22,18,9,"C"
texto0c1 .db 0


; Início do Programa
inicio:

 call inicializaay
 call clearscreen
 call printmenu
 call printdata
 call printoitava
 call printcanal

; Loop Princípal
main:

; Chama a Rotina de Leitura do Teclado
 call navega

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
 call nz,playsom

 ld a,(sai)	; Verifica a Saída
 cp 1
 jr nz,main

 jp fim


; Navega nas Opções
navega:

; Instrumentos
 ld bc,57342
 in a,(c)
 bit 0,a ; Tecla 'p'
 call z, incinst ; Incrementa o Index do Instrumento

 ld bc,57342
 in a,(c)
 bit 1,a ; Tecla 'o'
 call z, decinst ; Decrementa o Index do Instrumento

; Oitavas
 ld bc,61438
 in a,(c)
 bit 0,a ; Tecla '0'
 call z, incoctv ; Incrementa a Oitava

 ld bc,61438
 in a,(c)
 bit 1,a ; Tecla '9'
 call z, decoctv ; Decrementa a Oitava

; Canais
 ld bc,63486
 in a,(c)
 bit 0,a ; Tecla '1'
 call z, setcanala ; Seleciona o Canal A

 ld bc,63486
 in a,(c)
 bit 1,a ; Tecla '2'
 call z, setcanalb ; Seleciona o Canal B

 ld bc,63486
 in a,(c)
 bit 2,a ; Tecla '3'
 call z, setcanalc ; Seleciona o Canal C

; Notas
 ld bc,65278
 in a,(c)
 bit 1,a ; Tecla 'Z'
 call z, notado ; Toca um Dó

 ld bc,65278
 in a,(c)
 bit 2,a ; Tecla 'X'
 call z, notare ; Toca um Ré

 ld bc,65278
 in a,(c)
 bit 3,a ; Tecla 'C'
 call z, notami ; Toca um Mi

 ld bc,65278
 in a,(c)
 bit 4,a ; Tecla 'V'
 call z, notafa ; Toca um Fá

 ld bc,32766
 in a,(c)
 bit 4,a ; Tecla 'B'
 call z, notasol ; Toca um Sol

 ld bc,32766
 in a,(c)
 bit 3,a ; Tecla 'N'
 call z, notala ; Toca um Lá

 ld bc,32766
 in a,(c)
 bit 2,a ; Tecla 'M'
 call z, notasi ; Toca um Si

 ld bc,65022
 in a,(c)
 bit 1,a ; Tecla 'S'
 call z, notadost ; Toca um Dó Sustenido

 ld bc,65022
 in a,(c)
 bit 2,a ; Tecla 'D'
 call z, notarest ; Toca um Ré Sustenido

 ld bc,65022
 in a,(c)
 bit 4,a ; Tecla 'G'
 call z, notafast ; Toca um Fá Sustenido

 ld bc,49150
 in a,(c)
 bit 4,a ; Tecla 'H'
 call z, notasolst ; Toca um Sol Sustenido

 ld bc,49150
 in a,(c)
 bit 3,a ; Tecla 'J'
 call z, notalast ; Toca um Lá Sustenido

; Desliga o Som 
 ld bc,64510
 in a,(c)
 bit 4,a ; Tecla 'T'
 call z, stopsom ; Desliga o Som

; "Escape" - Sai para o "DOS"
 ld bc,64510
 in a,(c)
 bit 0,a ; Tecla 'q'
 call z, saida ; Sai do Programa

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
 push af
 ld a,255
lpausa2:
 dec a
 jr nz,lpausa2
 pop af
 dec a
 jr nz,lpausa1

 ret


; Incrementa o Index do Instrumento
incinst:

 ld a,(instindex)
 cp 127
 jr z,fimincinst ; Se já estivermos na Última Posição não Incrementa os Indexes

 inc a
 ld (instindex),a

 ld hl,(tabinst)
 ld de,28
 add hl,de
 ld (tabinst),hl

 call printdata

; Chama a Rotina que faz Pausa
 ld a,75
 ld (pausa),a
 call fazpausa

fimincinst:
 ret


; Decrementa o Index do Instrumento
decinst:

 ld a,(instindex)
 cp 0
 jr z,fimdecinst ; Se já estivermos na Primeira Posição não Decrementa os Indexes

 dec a
 ld (instindex),a

 ld hl,(tabinst)
 ld de,28
 sbc hl,de
 ld (tabinst),hl

 call printdata

; Chama a Rotina que faz Pausa
 ld a,75
 ld (pausa),a
 call fazpausa

fimdecinst:
 ret


; Incrementa a Oitava
incoctv:

 ld a,(oitava)
 cp 8
 jr z,fimincoctv ; Se já estivermos na Última Posição não Incrementa a Oitava

 inc a
 ld (oitava),a

 call printoitava

; Chama a Rotina que faz Pausa
 ld a,128
 ld (pausa),a
 call fazpausa

fimincoctv:
 ret


; Decrementa a Oitava
decoctv:

 ld a,(oitava)
 cp 1
 jr z,fimdecoctv ; Se já estivermos na Primeira Posição não Decrementa a Oitava

 dec a
 ld (oitava),a

 call printoitava

; Chama a Rotina que faz Pausa
 ld a,128
 ld (pausa),a
 call fazpausa

fimdecoctv:
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


; Seleciona o Canal A
setcanala:

 ld a,0
 ld (canal),a

 call printcanal

 ret


; Seleciona o Canal B
setcanalb:

 ld a,1
 ld (canal),a

 call printcanal

 ret


; Seleciona o Canal C
setcanalc:

 ld a,2
 ld (canal),a

 call printcanal

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


; Imprime a Informação do Instrumento
printdata:

 ld a,2		; Ecrã superior
 call 5633

 ld ix,(tabinst)  ; Nome do Instrumento

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
 ld a,9
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


 ld ix,(tabinst)  

 ld a,22	; Número do Instrumento
 rst 10H
 ld a,4
 rst 10H
 ld a,9
 rst 10H

 ld a,(ix+20)
 ld (num),a
 ld b,0
 ld c,a
 call 11563          
 call 11747

 ld a,' '
 rst 10H
 ld a,' '
 rst 10H

 ld a,22	; Tom
 rst 10H
 ld a,6
 rst 10H
 ld a,9
 rst 10H

 ld a,(ix+21)
 ld (tom),a
 ld b,0
 ld c,a
 call 11563          
 call 11747

 ld a,22	; Noise
 rst 10H
 ld a,7
 rst 10H
 ld a,9
 rst 10H

 ld a,(ix+22)
 ld (noise),a
 ld b,0
 ld c,a
 call 11563          
 call 11747

 ld a,22	; Attack
 rst 10H
 ld a,9
 rst 10H
 ld a,9
 rst 10H

 ld a,(ix+23)
 ld (attack),a
 ld b,0
 ld c,a
 call 11563          
 call 11747

 ld a,' '
 rst 10H
 ld a,' '
 rst 10H

 ld a,22	; Sustain
 rst 10H
 ld a,10
 rst 10H
 ld a,9
 rst 10H

 ld a,(ix+24)
 ld (sustain),a
 ld b,0
 ld c,a
 call 11563          
 call 11747

 ld a,' '
 rst 10H
 ld a,' '
 rst 10H

 ld a,22	; Decay
 rst 10H
 ld a,11
 rst 10H
 ld a,9
 rst 10H

 ld a,(ix+25)
 ld (decay),a
 ld b,0
 ld c,a
 call 11563          
 call 11747

 ld a,' '
 rst 10H
 ld a,' '
 rst 10H

 ld a,22	; Volume
 rst 10H
 ld a,13
 rst 10H
 ld a,9
 rst 10H

 ld a,(ix+26)
 ld (volume),a
 ld b,0
 ld c,a
 call 11563          
 call 11747

 ld a,' '
 rst 10H

 ld a,22	; Repete
 rst 10H
 ld a,14
 rst 10H
 ld a,9
 rst 10H

 ld a,(ix+27)
 ld (repete),a
 ld b,0
 ld c,a
 call 11563          
 call 11747

 ret


; Imprime a Oitava
printoitava:

 ld a,2		; Ecrã superior
 call 5633

 ld a,22	; Oitava
 rst 10H
 ld a,16
 rst 10H
 ld a,9
 rst 10H

 ld a,(oitava)
 ld b,0
 ld c,a
 call 11563          
 call 11747

 ret


; Imprime o Canal
printcanal:

 ld a,2		; Ecrã superior
 call 5633

 ld a,22	; Canal
 rst 10H
 ld a,18
 rst 10H
 ld a,9
 rst 10H

 ld a,(canal)
 cp 0
 jr z,pcanala
 cp 1
 jr z,pcanalb
 cp 2
 jr z,pcanalc

pcanala:
 ld de,texto0a0
 ld bc,texto0a1-texto0a0
 call 8252
 jr fimprintcanal

pcanalb:
 ld de,texto0b0
 ld bc,texto0b1-texto0b0
 call 8252
 jr fimprintcanal

pcanalc:
 ld de,texto0c0
 ld bc,texto0c1-texto0c0
 call 8252

fimprintcanal:
 ret


;coloca os valores do som nos respetivos portos
outer:

 ld bc,ayctrl
 out (c),d
 ld bc,aydata
 out (c),e

 ret

;inicializa o leitor da música
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

 jp fimpreparasom

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

 jp fimpreparasom

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
 
fimpreparasom:
 ld a,1		; Ecrã inferior
 call 5633

 ld de,texto030
 ld bc,texto031-texto030
 call 8252

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
 ld d,0 	; Fine Picth do Canal A
 ld a,(freqlowca)
 ld e,a
 call outer
 ld d,1 	; Course Picth do Canal A
 ld a,(freqhica)
 ld e,a
 call outer
 ld d,8 	; Volume do Canal A
 ld a,(volica)
 ld e,a
 call outer

 ld hl,mixer
 ld a,(tomca)	; Verifica o Tom no Canal A
 cp 1
 jr z,canalatom
 set 0,(hl)	; Desable Tom no Canal A
 jr vercanalanoise 
canalatom:
 res 0,(hl)	; Enable Tom no Canal A

vercanalanoise:
 ld a,(noiseca) ; Verifica Ruído no Canal A
 cp 1
 jr z,canalanoise
 set 3,(hl)   ; Desable Noise no Canal A
 jr plscanalb
canalanoise:
 res 3,(hl)	  ; Enable Noise no Canal A

;canalamixer:
; ld d,7
; ld a,(mixer)
; ld e,a
; call outer
; jp saiplaysom

; Canal B
plscanalb:
 ld d,2 	; Fine Picth do Canal B
 ld a,(freqlowcb)
 ld e,a
 call outer
 ld d,3 	; Course Picth do Canal B
 ld a,(freqhicb)
 ld e,a
 call outer
 ld d,9 	; Volume do Canal B
 ld a,(volicb)
 ld e,a
 call outer

 ld hl,mixer
 ld a,(tomcb)	; Verifica o Tom no Canal B
 cp 1
 jr z,canalbtom
 set 1,(hl) ; Desable Tom no Canal B
 jr vercanalbnoise
canalbtom:
 res 1,(hl) ; Enable Tom no Canal B

vercanalbnoise:
 ld a,(noisecb) ; Verifica Ruído no Canal B
 cp 1
 jr z,canalbnoise
 set 4,(hl)   ; Desable Noise no Canal B
 jr plscanalc
canalbnoise:
 res 4,(hl)   ; Enable Noise no Canal B

;canalbmixer:
; ld d,7
; ld a,(mixer)
; ld e,a
; call outer
; jr saiplaysom

; Canal C
plscanalc:
 ld d,4 	; Fine Picth do Canal C
 ld a,(freqlowcc)
 ld e,a
 call outer
 ld d,5 	; Course Picth do Canal C
 ld a,(freqhicc)
 ld e,a
 call outer
 ld d,10 	; Volume do Canal C
 ld a,(volicc)
 ld e,a
 call outer

 ld hl,mixer
 ld a,(tomcc)	; Verifica o Tom no Canal C
 cp 1
 jr z,canalctom
 set 2,(hl) ; Desable Tom no Canal C
 jr vercanalcnoise
canalctom:
 res 2,(hl) ; Enable Tom no Canal C

vercanalcnoise:
 ld a,(noisecc) ; Verifica Ruído no Canal C
 cp 1
 jr z,canalcnoise
 set 5,(hl)   ; Desable Noise no Canal C
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


; Desliga o Som
stopsom:

 call inicializaay

 ld a,1		; Ecrã inferior
 call 5633

 ld de,texto040
 ld bc,texto041-texto040
 call 8252

 ret


; Vai Buscar a Frequência à Tabela de Frequências
getfreq:

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
 ld d,0
 ld e,a
 add hl,de

 ld a,(hl)
 ld (freqlow),a
 inc hl
 ld a,(hl)
 ld (freqhi),a

 ret


; Toca um Dó
notado:

 ld a,0
 ld (nota),a

 call preparasom

 ret


; Toca um Dó Sustenido
notadost:

 ld a,2
 ld (nota),a

 call preparasom

 ret


; Toca um Ré
notare:

 ld a,4
 ld (nota),a

 call preparasom

 ret


; Toca um Ré Sustenido
notarest:

 ld a,6
 ld (nota),a

 call preparasom

 ret


; Toca um Mi
notami:

 ld a,8
 ld (nota),a

 call preparasom

 ret


; Toca um Fá
notafa:

 ld a,10
 ld (nota),a

 call preparasom

 ret


; Toca um Fá Sustenido
notafast:

 ld a,12
 ld (nota),a

 call preparasom

 ret


; Toca um Sol
notasol:

 ld a,14
 ld (nota),a

 call preparasom

 ret


; Toca um Sol Sustenido
notasolst:

 ld a,16
 ld (nota),a

 call preparasom

 ret


; Toca um Lá
notala:

 ld a,18
 ld (nota),a

 call preparasom

 ret


; Toca um Lá Sustenido
notalast:

 ld a,20
 ld (nota),a

 call preparasom

 ret


; Toca um Si
notasi:

 ld a,22
 ld (nota),a

 call preparasom

 ret


; Envelope do Canal A
envcanala:

; Attack Canal A
 ld a,(attackca)	; Verifica se estamos numa fase de Attack
 cp 0
 jp z,ecasustain	; Se não passa para o próximo Passo - Sustain

 ld hl,cntattackca 	; Decrementa o Contador do Attack
 dec (hl)
 jp nz,fimenvcanala	; Se não for Zero então e sai da Rotina

incvolca:
 ld a,(volfca)		
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

 ld hl,cntsustainca	;Decrementa o Contador do Sustain
 dec (hl)
 jp nz,fimenvcanala	; Se não for Zero então e sai da Rotina

decsusca:
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
 jp fimenvcanala

; Decay Canal A
ecadecay:
 ld a,(decayca)		; Verifica se estamos numa fase de Decay
 cp 0
 jp z,fimenvcanala	; Se não Sai da Rotina

 ld hl,cntdecayca 	; Decrementa o Contador do Decay
 dec (hl)
 jp nz,fimenvcanala	; Se não for Zero então e sai da Rotina

decvolca:
 ld a,(volfca)
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

; Attack Canal B
 ld a,(attackcb)	; Verifica se estamos numa fase de Attack
 cp 0
 jp z,ecbsustain	; Se não passa para o próximo Passo - Sustain

 ld hl,cntattackcb 	; Decrementa o Contador do Attack
 dec (hl)
 jp nz,fimenvcanalb	; Se não for Zero então e sai da Rotina

incvolcb:
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

decsuscb:
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

decvolcb:
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

; Attack Canal C
 ld a,(attackcc)	; Verifica se estamos numa fase de Attack
 cp 0
 jp z,eccsustain	; Se não passa para o próximo Passo - Sustain

 ld hl,cntattackcc 	; Decrementa o Contador do Attack
 dec (hl)
 jp nz,fimenvcanalc	; Se não for Zero então e sai da Rotina

incvolcc:
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

decsuscc:
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

decvolcc:
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

 ld a,22	
 rst 10H
 ld a,3
 rst 10H
 ld a,20
 rst 10H

 ld a,(attackca)
 ld b,0
 ld c,a
 call 11563          
 call 11747

 ld a,' '
 rst 10H
 ld a,' '
 rst 10H

 ld a,22
 rst 10H
 ld a,4
 rst 10H
 ld a,20
 rst 10H

 ld a,(sustaiinca)
 ld b,0
 ld c,a
 call 11563          
 call 11747

 ld a,' '
 rst 10H
 ld a,' '
 rst 10H

 ld a,22
 rst 10H
 ld a,5
 rst 10H
 ld a,20
 rst 10H

 ld a,(decayca)
 ld b,0
 ld c,a
 call 11563          
 call 11747

 ld a,' '
 rst 10H
 ld a,' '
 rst 10H

 ld a,22
 rst 10H
 ld a,6
 rst 10H
 ld a,20
 rst 10H

 ld a,(playenvca)
 ld b,0
 ld c,a
 call 11563          
 call 11747

 ld a,' '
 rst 10H
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