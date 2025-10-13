.def TMP = R16
.def TMP1 = R18
.def B0 = R1
.def B1 = R2
.def B2 = R3
.def B3 = R4
.def B4 = R5
.def B5 = R6
.def H = R20
.def P = R21
.def D = R22
.def PARAM = R26
.def FLAG = R27
.def ADC_VAL = R28

.org $000
 JMP reset		; Указатель на начало программы
.org INT0addr
 JMP EXT_INT0	; Указатель на обработчик прерывания int0
.org INT1addr
 JMP EXT_INT1	; Указатель на обработчик прерывания int1
.org $020
 JMP ADC_
 
; Начальная настройка
reset:
; настройка исходных значений
   LDI P, 3
   LDI H, 6
   LDI D, 0
   LDI TMP, 0x01
   MOV B0, TMP
   LDI TMP, 0
   MOV B1, TMP
   MOV B2, TMP
   MOV B3, B0
   MOV B4, B1
   MOV B5, B2
   
   
; настройка таймеров T1, T2 
   LDI TMP, 0b00110001
   OUT TCCR1A, TMP
   LDI TMP, 0b00001001
   OUT TCCR1B, TMP
   
   LDI TMP, 0b01101001
   OUT TCCR2, TMP

   
; настройка портов ввода-вывода
   SER TMP 
   OUT DDRB, TMP 
   OUT DDRC, TMP 
   LDI TMP, 0xDF
   OUT DDRA, TMP
   LDI TMP, 0xF0
   OUT DDRD, TMP
   OUT PORTA, R1
   OUT PORTB, R2
   OUT PORTC, R3
   
; Установка вершины стека в конец ОЗУ
   LDI TMP, HIGH(RAMEND)	; Старшие разряды адреса
   OUT SPH, TMP
   LDI TMP, LOW(RAMEND)		; Младшие разряды адреса
   OUT SPL, TMP
   
   LDI R16, 0x0F
   OUT MCUCR, R16			; Настройка прерываний int0 и int1 на условие 0/1
   LDI R16, 0xC0
   OUT GICR, R16			; Разрешение прерываний int0 и int1
   OUT GIFR, R16			; Предотвращение срабатывания int0 и int1 при включении прерываний
   SEI
; Основной цикл
loop:
   
   SBRS FLAG, 0
   JMP DEMO
   JMP CONFIG

DEMO:
   LDI TMP, 66
   CALL DELAY_10MS
   
   MOV TMP, H
   CPI TMP, 5
   BRSH H_LOOP
   CPI TMP, 4
   BRLO H_LOOP_REV

   g_out:
   OUT PORTA, R1
   OUT PORTB, R3
   OUT PORTC, R2
   
   RJMP loop
   
   g_out_rev:
   OUT PORTA, R1
   OUT PORTB, R2
   OUT PORTC, R3
   
   RJMP loop

H_LOOP:
   BST R1, 7
   LSL R3 
   ROL R2 
   ROL R1 
   BLD R3, 0 
   DEC TMP
   CPI TMP, 4
   BRNE H_LOOP 
   JMP g_out

H_LOOP_REV:
   BST R1, 0
   LSR R3 
   ROR R2 
   ROR R1 
   BLD R3, 7 
   INC TMP
   CPI TMP, 4
   BRNE H_LOOP_REV
   JMP g_out_rev
   
CONFIG:
   CPI PARAM, 0
   BREQ dis_0
   CPI PARAM, 1
   BREQ dis_1
   CPI PARAM, 2
   BREQ dis_2
   CPI PARAM, 3
   BREQ dis_3
   CPI PARAM, 4
   BREQ dis_4
   CPI PARAM, 5
   BREQ dis_5
   

   dis_0:
   LDI TMP, 0b11100101
   OUT ADMUX, TMP
   ; ADEN=1, ADSC=1 (старт), ADIE=1, делитель=128
   LDI TMP, 0b11001111
   OUT ADCSRA, TMP
   RJMP DISPLAY_PARAM_0



   dis_5:
   RJMP DISPLAY_PARAM_5
   dis_1:
   RJMP DISPLAY_PARAM_1
   dis_2:
   RJMP DISPLAY_PARAM_2

   dis_3:
   LDI TMP, 0b11100101
   OUT ADMUX, TMP
   LDI TMP, 0b11001111
   OUT ADCSRA, TMP
   RCALL DELAY
   IN TMP, ADCSRA
   SBRC TMP, 6
   JMP get_param_h
   get_param_h:
   CPI ADC_VAL, 28
   BRSH adc_h_1
   LDI H, 0
   JMP adc_h_9
   adc_h_1:
   CPI ADC_VAL, 56
   BRSH adc_h_2
   LDI H, 1
   JMP adc_h_9
   adc_h_2:
   CPI ADC_VAL, 84
   BRSH adc_h_3
   LDI H, 2
   JMP adc_h_9
   adc_h_3:
   CPI ADC_VAL, 112
   BRSH adc_h_4
   LDI H, 3
   JMP adc_h_9
   adc_h_4:
   CPI ADC_VAL, 140
   BRSH adc_h_5
   LDI H, 4
   JMP adc_h_9
   adc_h_5:
   CPI ADC_VAL, 168
   BRSH adc_h_6
   LDI H, 5
   JMP adc_h_9
   adc_h_6:
   CPI ADC_VAL, 196
   BRSH adc_h_7
   LDI H, 6
   JMP adc_h_9
   adc_h_7:
   CPI ADC_VAL, 224
   BRSH adc_h_8
   LDI H, 7
   JMP adc_h_9
   adc_h_8:
   LDI H, 8
   adc_h_9:
   RJMP DISPLAY_PARAM_3

   dis_4:
   RJMP DISPLAY_PARAM_4

   
   
	;====================================================================
	;					Семейство функций вывода параметров
	;====================================================================  
DISPLAY_PARAM_0:
   LDI TMP, 0x08
   OUT PORTA, TMP
   CALL Display_b
   CALL DELAY
   
   LDI TMP, 0x04
   OUT PORTA, TMP
   CALL Display_0d
   CALL DELAY
   
   LDI TMP, 0x02
   OUT PORTA, TMP
   MOV TMP1, B3
   ANDI TMP1, 0xF0
   
   CPI TMP1, 0x00
   BRNE param_0_1_1
   CALL Display_0
   param_0_1_1:
   CPI TMP1, 0x10
   BRNE param_0_1_2
   CALL Display_1
   param_0_1_2:
   CPI TMP1, 0x20
   BRNE param_0_1_3
   CALL Display_2
   param_0_1_3:
   CPI TMP1, 0x30
   BRNE param_0_1_4
   CALL Display_3
   param_0_1_4:
   CPI TMP1, 0x40
   BRNE param_0_1_5
   CALL Display_4
   param_0_1_5:
   CPI TMP1, 0x50
   BRNE param_0_1_6
   CALL Display_5
   param_0_1_6:
   CPI TMP1, 0x60
   BRNE param_0_1_7
   CALL Display_6
   param_0_1_7:
   CPI TMP1, 0x70
   BRNE param_0_1_8
   CALL Display_7
   param_0_1_8:
   CPI TMP1, 0x80
   BRNE param_0_1_9
   CALL Display_8
   param_0_1_9:
   CPI TMP1, 0x90
   BRNE param_0_1_a
   CALL Display_9
   param_0_1_a:
   CPI TMP1, 0xA0
   BRNE param_0_1_b
   CALL Display_a
   param_0_1_b:
   CPI TMP1, 0xB0
   BRNE param_0_1_c
   CALL Display_b
   param_0_1_c:
   CPI TMP1, 0xC0
   BRNE param_0_1_d
   CALL Display_c
   param_0_1_d:
   CPI TMP1, 0xD0
   BRNE param_0_1_e
   CALL Display_d
   param_0_1_e:
   CPI TMP1, 0xE0
   BRNE param_0_1_f
   CALL Display_e
   param_0_1_f:
   CPI TMP1, 0xF0
   BRNE param_0_1_0
   CALL Display_f
   param_0_1_0:
   RCALL DELAY
   
   LDI TMP, 0x01
   OUT PORTA, TMP
   MOV TMP1, B3
   ANDI TMP1, 0x0F
   
   CPI TMP1, 0x00
   BRNE param_0_2_1
   CALL Display_0
   param_0_2_1:
   CPI TMP1, 0x01
   BRNE param_0_2_2
   CALL Display_1
   param_0_2_2:
   CPI TMP1, 0x02
   BRNE param_0_2_3
   CALL Display_2
   param_0_2_3:
   CPI TMP1, 0x03
   BRNE param_0_2_4
   CALL Display_3
   param_0_2_4:
   CPI TMP1, 0x04
   BRNE param_0_2_5
   CALL Display_4
   param_0_2_5:
   CPI TMP1, 0x05
   BRNE param_0_2_6
   CALL Display_5
   param_0_2_6:
   CPI TMP1, 0x06
   BRNE param_0_2_7
   CALL Display_6
   param_0_2_7:
   CPI TMP1, 0x07
   BRNE param_0_2_8
   CALL Display_7
   param_0_2_8:
   CPI TMP1, 0x08
   BRNE param_0_2_9
   CALL Display_8
   param_0_2_9:
   CPI TMP1, 0x09
   BRNE param_0_2_a
   CALL Display_9
   param_0_2_a:
   CPI TMP1, 0x0A
   BRNE param_0_2_b
   CALL Display_a
   param_0_2_b:
   CPI TMP1, 0x0B
   BRNE param_0_2_c
   CALL Display_b
   param_0_2_c:
   CPI TMP1, 0x0C
   BRNE param_0_2_d
   CALL Display_c
   param_0_2_d:
   CPI TMP1, 0x0D
   BRNE param_0_2_e
   CALL Display_d
   param_0_2_e:
   CPI TMP1, 0x0E
   BRNE param_0_2_f
   CALL Display_e
   param_0_2_f:
   CPI TMP1, 0x0F
   BRNE param_0_2_0
   CALL Display_f
   param_0_2_0:
   RCALL DELAY
   
   RJMP loop

 
DISPLAY_PARAM_1:
   LDI TMP, 0x08
   OUT PORTA, TMP
   CALL Display_b
   CALL DELAY
   
   LDI TMP, 0x04
   OUT PORTA, TMP
   CALL Display_1d
   CALL DELAY
   
   LDI TMP, 0x02
   OUT PORTA, TMP
   MOV TMP1, B4
   ANDI TMP1, 0xF0
   
   CPI TMP1, 0x00
   BRNE param_1_1_1
   CALL Display_0
   param_1_1_1:
   CPI TMP1, 0x10
   BRNE param_1_1_2
   CALL Display_1
   param_1_1_2:
   CPI TMP1, 0x20
   BRNE param_1_1_3
   CALL Display_2
   param_1_1_3:
   CPI TMP1, 0x30
   BRNE param_1_1_4
   CALL Display_3
   param_1_1_4:
   CPI TMP1, 0x40
   BRNE param_1_1_5
   CALL Display_4
   param_1_1_5:
   CPI TMP1, 0x50
   BRNE param_1_1_6
   CALL Display_5
   param_1_1_6:
   CPI TMP1, 0x60
   BRNE param_1_1_7
   CALL Display_6
   param_1_1_7:
   CPI TMP1, 0x70
   BRNE param_1_1_8
   CALL Display_7
   param_1_1_8:
   CPI TMP1, 0x80
   BRNE param_1_1_9
   CALL Display_8
   param_1_1_9:
   CPI TMP1, 0x90
   BRNE param_1_1_a
   CALL Display_9
   param_1_1_a:
   CPI TMP1, 0xA0
   BRNE param_1_1_b
   CALL Display_a
   param_1_1_b:
   CPI TMP1, 0xB0
   BRNE param_1_1_c
   CALL Display_b
   param_1_1_c:
   CPI TMP1, 0xC0
   BRNE param_1_1_d
   CALL Display_c
   param_1_1_d:
   CPI TMP1, 0xD0
   BRNE param_1_1_e
   CALL Display_d
   param_1_1_e:
   CPI TMP1, 0xE0
   BRNE param_1_1_f
   CALL Display_e
   param_1_1_f:
   CPI TMP1, 0xF0
   BRNE param_1_1_0
   CALL Display_f
   param_1_1_0:
   RCALL DELAY
   
   LDI TMP, 0x01
   OUT PORTA, TMP
   MOV TMP1, B4
   ANDI TMP1, 0x0F
   
   CPI TMP1, 0x00
   BRNE param_1_2_1
   CALL Display_0
   param_1_2_1:
   CPI TMP1, 0x01
   BRNE param_1_2_2
   CALL Display_1
   param_1_2_2:
   CPI TMP1, 0x02
   BRNE param_1_2_3
   CALL Display_2
   param_1_2_3:
   CPI TMP1, 0x03
   BRNE param_1_2_4
   CALL Display_3
   param_1_2_4:
   CPI TMP1, 0x04
   BRNE param_1_2_5
   CALL Display_4
   param_1_2_5:
   CPI TMP1, 0x05
   BRNE param_1_2_6
   CALL Display_5
   param_1_2_6:
   CPI TMP1, 0x06
   BRNE param_1_2_7
   CALL Display_6
   param_1_2_7:
   CPI TMP1, 0x07
   BRNE param_1_2_8
   CALL Display_7
   param_1_2_8:
   CPI TMP1, 0x08
   BRNE param_1_2_9
   CALL Display_8
   param_1_2_9:
   CPI TMP1, 0x09
   BRNE param_1_2_a
   CALL Display_9
   param_1_2_a:
   CPI TMP1, 0x0A
   BRNE param_1_2_b
   CALL Display_a
   param_1_2_b:
   CPI TMP1, 0x0B
   BRNE param_1_2_c
   CALL Display_b
   param_1_2_c:
   CPI TMP1, 0x0C
   BRNE param_1_2_d
   CALL Display_c
   param_1_2_d:
   CPI TMP1, 0x0D
   BRNE param_1_2_e
   CALL Display_d
   param_1_2_e:
   CPI TMP1, 0x0E
   BRNE param_1_2_f
   CALL Display_e
   param_1_2_f:
   CPI TMP1, 0x0F
   BRNE param_1_2_0
   CALL Display_f
   param_1_2_0:
   RCALL DELAY
   
   RJMP loop
   
DISPLAY_PARAM_2:
   LDI TMP, 0x08
   OUT PORTA, TMP
   CALL Display_b
   CALL DELAY
   
   LDI TMP, 0x04
   OUT PORTA, TMP
   CALL Display_2d
   CALL DELAY
   
   LDI TMP, 0x02
   OUT PORTA, TMP
   MOV TMP1, B5
   ANDI TMP1, 0xF0
   
   CPI TMP1, 0x00
   BRNE param_2_1_1
   CALL Display_0
   param_2_1_1:
   CPI TMP1, 0x10
   BRNE param_2_1_2
   CALL Display_1
   param_2_1_2:
   CPI TMP1, 0x20
   BRNE param_2_1_3
   CALL Display_2
   param_2_1_3:
   CPI TMP1, 0x30
   BRNE param_2_1_4
   CALL Display_3
   param_2_1_4:
   CPI TMP1, 0x40
   BRNE param_2_1_5
   CALL Display_4
   param_2_1_5:
   CPI TMP1, 0x50
   BRNE param_2_1_6
   CALL Display_5
   param_2_1_6:
   CPI TMP1, 0x60
   BRNE param_2_1_7
   CALL Display_6
   param_2_1_7:
   CPI TMP1, 0x70
   BRNE param_2_1_8
   CALL Display_7
   param_2_1_8:
   CPI TMP1, 0x80
   BRNE param_2_1_9
   CALL Display_8
   param_2_1_9:
   CPI TMP1, 0x90
   BRNE param_2_1_a
   CALL Display_9
   param_2_1_a:
   CPI TMP1, 0xA0
   BRNE param_2_1_b
   CALL Display_a
   param_2_1_b:
   CPI TMP1, 0xB0
   BRNE param_2_1_c
   CALL Display_b
   param_2_1_c:
   CPI TMP1, 0xC0
   BRNE param_2_1_d
   CALL Display_c
   param_2_1_d:
   CPI TMP1, 0xD0
   BRNE param_2_1_e
   CALL Display_d
   param_2_1_e:
   CPI TMP1, 0xE0
   BRNE param_2_1_f
   CALL Display_e
   param_2_1_f:
   CPI TMP1, 0xF0
   BRNE param_2_1_0
   CALL Display_f
   param_2_1_0:
   RCALL DELAY
   
   LDI TMP, 0x01
   OUT PORTA, TMP
   MOV TMP1, B5
   ANDI TMP1, 0x0F
   
   CPI TMP1, 0x00
   BRNE param_2_2_1
   CALL Display_0
   param_2_2_1:
   CPI TMP1, 0x01
   BRNE param_2_2_2
   CALL Display_1
   param_2_2_2:
   CPI TMP1, 0x02
   BRNE param_2_2_3
   CALL Display_2
   param_2_2_3:
   CPI TMP1, 0x03
   BRNE param_2_2_4
   CALL Display_3
   param_2_2_4:
   CPI TMP1, 0x04
   BRNE param_2_2_5
   CALL Display_4
   param_2_2_5:
   CPI TMP1, 0x05
   BRNE param_2_2_6
   CALL Display_5
   param_2_2_6:
   CPI TMP1, 0x06
   BRNE param_2_2_7
   CALL Display_6
   param_2_2_7:
   CPI TMP1, 0x07
   BRNE param_2_2_8
   CALL Display_7
   param_2_2_8:
   CPI TMP1, 0x08
   BRNE param_2_2_9
   CALL Display_8
   param_2_2_9:
   CPI TMP1, 0x09
   BRNE param_2_2_a
   CALL Display_9
   param_2_2_a:
   CPI TMP1, 0x0A
   BRNE param_2_2_b
   CALL Display_a
   param_2_2_b:
   CPI TMP1, 0x0B
   BRNE param_2_2_c
   CALL Display_b
   param_2_2_c:
   CPI TMP1, 0x0C
   BRNE param_2_2_d
   CALL Display_c
   param_2_2_d:
   CPI TMP1, 0x0D
   BRNE param_2_2_e
   CALL Display_d
   param_2_2_e:
   CPI TMP1, 0x0E
   BRNE param_2_2_f
   CALL Display_e
   param_2_2_f:
   CPI TMP1, 0x0F
   BRNE param_2_2_0
   CALL Display_f
   param_2_2_0:
   RCALL DELAY
   
   RJMP loop

DISPLAY_PARAM_3:
   LDI TMP, 0x08
   OUT PORTA, TMP
   CALL Display_h
   CALL DELAY
   
   CPI H, 4
   BRSH no_m
   LDI TMP, 0x02
   OUT PORTA, TMP
   CALL Display_m
   CALL Delay
   
   LDI TMP, 0x01
   OUT PORTA, TMP
   CPI H, 3
   BRNE param_3_2
   CALL Display_1
   param_3_2:
   CPI H, 2
   BRNE param_3_1
   CALL Display_2
   param_3_1:
   CPI H, 1
   BRNE param_3_0
   CALL Display_3
   param_3_0:
   CPI H, 0
   BRNE param_3_00
   CALL Display_4
   param_3_00:
   CALL DELAY
   
   RJMP loop
   
   no_m:
   LDI TMP, 0x01
   OUT PORTA, TMP
   CPI H, 4
   BRNE param_3_5
   CALL Display_0
   param_3_5:
   CPI H, 5
   BRNE param_3_6
   CALL Display_1
   param_3_6:
   CPI H, 6
   BRNE param_3_7
   CALL Display_2
   param_3_7:
   CPI H, 7
   BRNE param_3_8
   CALL Display_3
   param_3_8:
   CPI H, 8
   BRNE param_3_9
   CALL Display_4
   param_3_9:
   CALL DELAY
   
   RJMP loop

DISPLAY_PARAM_4:
   LDI TMP, 0x08
   OUT PORTA, TMP
   CALL Display_p
   CALL DELAY
   
   LDI TMP, 0x01
   OUT PORTA, TMP
   CPI P, 1
   BRNE param_4_2
   CALL Display_1
   param_4_2:
   CPI P, 2
   BRNE param_4_3
   CALL Display_2
   param_4_3:
   CPI P, 3
   BRNE param_4_4
   CALL Display_3
   param_4_4:
   CPI P, 4
   BRNE param_4_5
   CALL Display_4
   param_4_5:
   CPI P, 5
   BRNE param_4_6
   CALL Display_5
   param_4_6:
   CPI P, 6
   BRNE param_4_7
   CALL Display_6

   param_4_7:
   CPI P, 7
   BRNE param_4_8
   CALL Display_7
   
   param_4_8:
   CPI P, 8
   BRNE param_4_9
   CALL Display_8

   param_4_9:
   CPI P, 9
   BRNE param_4_end
   CALL Display_9
   param_4_end:


   CALL DELAY
   
   RJMP loop

DISPLAY_PARAM_5:
   LDI TMP, 0x08
   OUT PORTA, TMP
   CALL Display_dd
   CALL DELAY
   
   LDI TMP, 0x02
   OUT PORTA, TMP
   MOV TMP1, D
   ANDI TMP1, 0xF0
   
   CPI TMP1, 0x00
   BRNE param_5_1_1
   CALL Display_0
   param_5_1_1:
   CPI TMP1, 0x10
   BRNE param_5_1_2
   CALL Display_1
   param_5_1_2:
   CPI TMP1, 0x20
   BRNE param_5_1_3
   CALL Display_2
   param_5_1_3:
   CPI TMP1, 0x30
   BRNE param_5_1_4
   CALL Display_3
   param_5_1_4:
   CPI TMP1, 0x40
   BRNE param_5_1_5
   CALL Display_4
   param_5_1_5:
   CPI TMP1, 0x50
   BRNE param_5_1_6
   CALL Display_5
   param_5_1_6:
   CPI TMP1, 0x60
   BRNE param_5_1_7
   CALL Display_6
   param_5_1_7:
   CPI TMP1, 0x70
   BRNE param_5_1_8
   CALL Display_7
   param_5_1_8:
   CPI TMP1, 0x80
   BRNE param_5_1_9
   CALL Display_8
   param_5_1_9:
   CPI TMP1, 0x90
   BRNE param_5_1_a
   CALL Display_9
   param_5_1_a:
   CPI TMP1, 0xA0
   BRNE param_5_1_b
   CALL Display_a
   param_5_1_b:
   CPI TMP1, 0xB0
   BRNE param_5_1_c
   CALL Display_b
   param_5_1_c:
   CPI TMP1, 0xC0
   BRNE param_5_1_d
   CALL Display_c
   param_5_1_d:
   CPI TMP1, 0xD0
   BRNE param_5_1_e
   CALL Display_d
   param_5_1_e:
   CPI TMP1, 0xE0
   BRNE param_5_1_f
   CALL Display_e
   param_5_1_f:
   CPI TMP1, 0xF0
   BRNE param_5_1_0
   CALL Display_f
   param_5_1_0:
   RCALL DELAY
   
   LDI TMP, 0x01
   OUT PORTA, TMP
   MOV TMP1, D
   ANDI TMP1, 0x0F
   
   CPI TMP1, 0x00
   BRNE param_5_2_1
   CALL Display_0
   param_5_2_1:
   CPI TMP1, 0x01
   BRNE param_5_2_2
   CALL Display_1
   param_5_2_2:
   CPI TMP1, 0x02
   BRNE param_5_2_3
   CALL Display_2
   param_5_2_3:
   CPI TMP1, 0x03
   BRNE param_5_2_4
   CALL Display_3
   param_5_2_4:
   CPI TMP1, 0x04
   BRNE param_5_2_5
   CALL Display_4
   param_5_2_5:
   CPI TMP1, 0x05
   BRNE param_5_2_6
   CALL Display_5
   param_5_2_6:
   CPI TMP1, 0x06
   BRNE param_5_2_7
   CALL Display_6
   param_5_2_7:
   CPI TMP1, 0x07
   BRNE param_5_2_8
   CALL Display_7
   param_5_2_8:
   CPI TMP1, 0x08
   BRNE param_5_2_9
   CALL Display_8
   param_5_2_9:
   CPI TMP1, 0x09
   BRNE param_5_2_a
   CALL Display_9
   param_5_2_a:
   CPI TMP1, 0x0A
   BRNE param_5_2_b
   CALL Display_a
   param_5_2_b:
   CPI TMP1, 0x0B
   BRNE param_5_2_c
   CALL Display_b
   param_5_2_c:
   CPI TMP1, 0x0C
   BRNE param_5_2_d
   CALL Display_c
   param_5_2_d:
   CPI TMP1, 0x0D
   BRNE param_5_2_e
   CALL Display_d
   param_5_2_e:
   CPI TMP1, 0x0E
   BRNE param_5_2_f
   CALL Display_e
   param_5_2_f:
   CPI TMP1, 0x0F
   BRNE param_5_2_0
   CALL Display_f
   param_5_2_0:
   RCALL DELAY
   
   RJMP loop
   
DELAY_10MS:
    PUSH R24        
    PUSH r25
    
DELAY_10MS_OUTER:
    LDI R25, HIGH(16000)  
    LDI R24, LOW(16000)   
    
DELAY_10MS_INNER:
    SBIW R24, 1     
    NOP             
    BRNE DELAY_10MS_INNER 
    
    DEC TMP        
    BRNE DELAY_10MS_OUTER 
    
    POP R25         
    POP R24
    RET



	;===========================================================================
	;				Семейство функций вывода конкртеного значения
	;===========================================================================
Display_1:
   LDI R16, 0b11111001
   LDI R31, 0xFF
   EOR R16, R31
   OUT  PORTC, R16
   RET
   
Display_2:
   LDI R16, 0b10100100
   LDI R31, 0xFF
   EOR R16, R31
   OUT  PORTC, R16
   RET
Display_3:
   LDI R16, 0b10110000
   LDI R31, 0xFF
   EOR R16, R31
   OUT  PORTC, R16
   RET

Display_4:
   LDI R16, 0b10011001
   LDI R31, 0xFF
   EOR R16, R31
   OUT  PORTC, R16
   RET

Display_5:
   LDI R16, 0b10010010
   LDI R31, 0xFF
   EOR R16, R31
   OUT  PORTC, R16
   RET

Display_6:
   LDI R16, 0b10000010
   LDI R31, 0xFF
   EOR R16, R31
   OUT  PORTC, R16
   RET

Display_7:
   LDI R16, 0b11111000
   LDI R31, 0xFF
   EOR R16, R31
   OUT  PORTC, R16
   RET

Display_8:
   LDI R16, 0b10000000
   LDI R31, 0xFF
   EOR R16, R31
   OUT  PORTC, R16
   RET

Display_9:
   LDI R16, 0b10010000
   LDI R31, 0xFF
   EOR R16, R31
   OUT  PORTC, R16
   RET

Display_0:
   LDI R16, 0b11000000
   LDI R31, 0xFF
   EOR R16, R31
   OUT  PORTC, R16
   RET
 
Display_1d:
   LDI R16, 0b01111001
   LDI R31, 0xFF
   EOR R16, R31
   OUT  PORTC, R16
   RET
   
Display_2d:
   LDI R16, 0b00100100
   LDI R31, 0xFF
   EOR R16, R31
   OUT  PORTC, R16
   RET
 
Display_0d:
   LDI R16, 0b01000000
   LDI R31, 0xFF
   EOR R16, R31
   OUT  PORTC, R16
   RET

Display_a:
   LDI R16, 0b10001000
   LDI R31, 0xFF
   EOR R16, R31
   OUT  PORTC, R16
   RET


Display_b:
   LDI R16, 0b10000011
   LDI R31, 0xFF
   EOR R16, R31
   OUT  PORTC, R16
   RET

Display_c:
   LDI R16, 0b11000110
   LDI R31, 0xFF
   EOR R16, R31
   OUT  PORTC, R16
   RET

Display_d:
   LDI R16, 0b10100001
   LDI R31, 0xFF
   EOR R16, R31
   OUT  PORTC, R16
   RET

Display_e:
   LDI R16, 0b10000110
   LDI R31, 0xFF
   EOR R16, R31
   OUT  PORTC, R16
   RET

Display_f:
   LDI R16, 0b10001110
   LDI R31, 0xFF
   EOR R16, R31
   OUT  PORTC, R16
   RET

Display_h:
   LDI R16, 0b00001011
   LDI R31, 0xFF
   EOR R16, R31
   OUT  PORTC, R16
   RET


Display_p:
   LDI R16, 0b00001100
   LDI R31, 0xFF
   EOR R16, R31
   OUT  PORTC, R16
   RET

Display_dd:
   LDI R16, 0b00100001
   LDI R31, 0xFF
   EOR R16, R31
   OUT  PORTC, R16
   RET

Display_m:
   LDI R16, 0b10111111
   LDI R31, 0xFF
   EOR R16, R31
   OUT  PORTC, R16
   RET

DELAY:
	LDI  R16, 100	; x
	LDI  R17, 5
DELAY_SUB:
	DEC  R16
	BRNE DELAY_SUB
	DEC  R17
	BRNE DELAY_SUB
	RET
  
EXT_INT0: ; Обработчик прерывания int0
   PUSH R16 
   IN R16, SREG
   PUSH R16 
   
   SBRC FLAG, 0
   JMP flag_1
   
   LDI FLAG, 0x01
   
   
   JMP re_1
   
   flag_1:
   CLR FLAG
   
   re_1:
   POP R16
   OUT SREG, R16 
   POP R16 
   RETI 

EXT_INT1: ; Обработчик прерывания int1
   PUSH R16 
   IN R16, SREG
   PUSH R16 
   
   
   CPI FLAG, 0
   BREQ no_ovf
   CPI PARAM, 5
   BREQ ovf
   INC PARAM
   JMP no_ovf
   ovf:
   LDI PARAM, 0
   no_ovf:
   OUT PORTB, PARAM
   POP R16
   OUT SREG, R16 
   POP R16 
   RETI

ADC_:
    ; Пролог ISR
    PUSH R16
    IN   R16, SREG
    PUSH R16

    ; Считать результат АЦП (сначала ADCL, затем ADCH)
    IN   R29, ADCL
    IN   ADC_VAL, ADCH      ; старшие 8 бит (ADLAR=1)

    ; Обновление PWM
    CLI
    OUT  OCR2,   ADC_VAL
    CLR  TMP                  ; TMP = R16
    OUT  OCR1BH, TMP
    OUT  OCR1BL, ADC_VAL
    SEI

    ; ---------------- b0 (только выбор/индикация) ----------------
    ; Условие: в конфигурации (FLAG.bit0=1) и выбран PARAM=0
    SBRC FLAG, 0             ; если бит0=1 -> перейти к проверке PARAM
    RJMP __b0_show_chk
    RJMP __adc_exit

__b0_show_chk:
    CPI  PARAM, 0
    BRNE __adc_exit

    ; Масштабируем 0..255 → 0..127 и пишем ТОЛЬКО в B3 (зеркало для индикатора)
    MOV  TMP, ADC_VAL
    LSR  TMP
	MOV  B0,  TMP			; При отладке коммент ================================================
    MOV  B3,  TMP           ; B3 обновляется, B0 (R1) НЕ трогаем
	RJMP __adc_exit			; При отладке коммент ================================================

__adc_exit:
    ; Эпилог ISR
    POP  R16
    OUT  SREG, R16
    POP  R16
    RETI
