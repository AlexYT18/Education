.org $000
 JMP RESET ; Указатель на начало программы
.org INT0addr
 JMP EXT_INT0 ; Указатель на обработчик прерывания int0
.org INT1addr
 JMP EXT_INT1 ; Указатель на обработчик прерывания int1

RESET: ; Начало основной программы
LDI R20, HIGH(RAMEND) ; Старшие разряды адреса
OUT SPH, R20 ; Установка вершины стека в конец ОЗУ
LDI R20, LOW(RAMEND) ; Младшие разряды адреса
OUT SPL, R20 ; Установка вершины стека в конец ОЗУ
SER R16
LDI R20, 0xFF
OUT DDRA, R20 ; Настройка PORTA на вывод
OUT DDRB, R20 ; Настройка PORTB на вывод
LDI R20, 0x00
OUT DDRC, R20
LDI R20, 0x73
OUT DDRD, R20
LDI R17, 0x02
LDI R18, 0xAA
LDI R19, 0x01
SET

LDI R16, (1<<ISC11)|(0<<ISC10)|(1<<ISC01)|(0<<ISC00)
OUT MCUCR, R16

LDI R16, (1<<INT1)|(1<<INT0)
OUT GICR, R16


;LDI R16, 0x0F
;OUT MCUCR, R16 ; Настройка прерываний int0 и int1 на условие 0/1


OUT GIFR, R16 ; Предотвращение срабатывания int0 и int1 при включении прерываний
SEI ; Включение прерываний
LDI R25, 0
LDI R23, 0
OUT EEARL, R25		
OUT  EEARH, R23 		
SBI EECR,EERE 		
IN R24, EEDR
CPI R24, 0
BREQ LOOP
MOV R19, R24

LOOP:
SBIS PIND, 7
JMP TICK
IN R18, PINC
CALL delay_120 ; ожидание нажатия комбинации кнопок
RJMP LOOP

TICK:

BRTC ZERO
   SBI PORTD, 6
   CPI R19, 0x02
   BREQ O_2
   CPI R19, 0x03
   BREQ O_3
   SBI PORTD, 4
   CBI PORTD, 5
   LDI R20, 0xFF
   OUT PORTA, R20
   OUT PORTB, R20
   CALL delay_x
   CLT
   RJMP LOOP
   O_2:
   CBI PORTD, 4
   SBI PORTD, 5
   LDI R20, 0xAA
   OUT PORTA, R20
   OUT PORTB, R20
   CALL delay_x
   CLT
   RJMP LOOP
	O_3:
   MOV R21, R18
   COM R21
   INC R21
   OUT PORTA, R18     ; прямой код Y
   OUT PORTB, R21     ; всегда отрицание (-Y)
   CALL delay_x
   CLT
   RJMP LOOP

ZERO:
   CBI PORTD, 6
   CPI R19, 0x02
   BREQ Z_2
   CPI R19, 0x03
   BREQ Z_3
   SBI PORTD, 4
   CBI PORTD, 5
   LDI R20, 0x00
   OUT PORTA, R20
   OUT PORTB, R20
   CALL delay_x
   SET
   RJMP LOOP
   Z_2:
   CBI PORTD, 4
   SBI PORTD, 5
   LDI R20, 0x55
   OUT PORTA, R20
   OUT PORTB, R20
   CALL delay_x
   SET
   RJMP LOOP
   Z_3:
   ; можешь оставить как есть, он уже выводит -Y
   MOV R21, R18
   COM R21
   INC R21
   OUT PORTA, R18
   OUT PORTB, R21
   CALL delay_x
   SET
   RJMP LOOP

EXT_INT0: ; Обработчик прерывания int0
IN R26, SREG
PUSH R26
CPI R19, 0x03
BREQ REF_1
INC R19
RCALL WRITE
RETI
REF_1:
LDI R19, 0x01
RCALL WRITE
POP R26
OUT SREG, R26
RETI ; Возврат из обработчика прерываний и разрешение прерываний

EXT_INT1: ; Обработчик прерывания int1
IN R26, SREG
PUSH R26
CPI R19, 0x01
BREQ REF_2
DEC R19
RCALL WRITE
RETI
REF_2:
LDI R19, 0x03
RCALL WRITE
POP R26
OUT SREG, R26
RETI

delay_x:
   LDI R31, 229
   LDI R30, 104
   LDI R29, 5
delay_x_sub:
   DEC R30
   BRNE delay_x_sub
   DEC R31
   BRNE delay_x_sub
   DEC R29
   BRNE delay_x_sub
   RET

delay_120: ; задержка 120 мс
 LDI R31, 5
 LDI R30, 223
 LDI R29, 188
delay_sub:
 DEC R29
 BRNE delay_sub
 DEC R30
 BRNE delay_sub
 DEC R31
 BRNE delay_sub
 NOP
 NOP
 RET
 
 WRITE:
   SBIC	EECR,EEWE		
   RJMP	WRITE 		
 
   CLI				
   OUT 	EEARL, R25 		
   OUT 	EEARH, R23		
   OUT 	EEDR, R19		
 
   SBI 	EECR,EEMWE		
   SBI 	EECR,EEWE		
 
   SEI 				
   RET
