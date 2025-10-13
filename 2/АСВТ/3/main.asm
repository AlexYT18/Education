.def Number_one = R20
.def Number_two = R21
.def Number_three = R22
.def Number_four = R23


.org $000
   JMP RESET	; Указатель на начало программы
.org INT0addr
   JMP EXT_INT0	; Указатель на обработчик прерывания int0
.org INT1addr
   JMP EXT_INT1	; Указатель на обработчик прерывания int1
.org OC1Aaddr
   JMP PLUS_SEC


   
RESET:	; Начало основной программы
	LDI  R20, HIGH(RAMEND) 
	OUT  SPH, R20 	; Установка вершины стека в конец ОЗУ (Старшие разряды адреса)
	LDI  R20, LOW(RAMEND)
	OUT  SPL, R20	; Установка вершины стека в конец ОЗУ (Младшие разряды адреса)
	SER  R16		; Установка всех битов регистра
	OUT  DDRC, R16	; Настройка PORTC на вывод 
	OUT  DDRB, R16	;
	LDI  R16, 0xCF
	OUT  DDRA, R16
	CLR R16			; Очистка регистра
	OUT DDRD, R16
	
	; timer init
	LDI R16, 0b00000000
	OUT TCNT1H, R16
	OUT TCNT1L, R16
	LDI R16, LOW(7812)
	OUT OCR1AL, R16
	LDI R16, HIGH(7812)
	OUT OCR1AH, R16
	LDI R16, 0b00000000
	OUT TCCR1A, R16
	LDI R16, 0b00010000
	OUT TiMSK, R16
	
	CLR Number_one
	CLR Number_two
	CLR Number_three
	CLR Number_four
	CLR R19
	CLR R26
	
	LDI R16, 0x0F
	OUT  MCUCR, R16	; Настройка прерываний int0 и int1 на условие 0/1
	LDI  R16, 0xC0	
	OUT  GICR, R16	; Разрешение прерываний int0 и int1
	OUT  GIFR, R16	; Предотвращение срабатывания int0 и int1 при включении прерываний
	SEI				; Включение прерываний

LOOP:	; Бесконечный цикл
	
	IN R25, PIND	; R <- P
	SBRC R25, 0		; Пропуск след программы если бит очищен
	CALL WRITE
	SBRC R25, 0
	JMP start
	CLT				; Очистка флага передачи

	IN R25, PIND	
	SBRC R25, 1
	CALL READ		
	SBRC R25, 1
	JMP start
	CLR R26
	
	start:
	CLR R16
	OUT PORTC, R16
	LDI R16, 0x01
	OUT PORTA, R16
	CPI Number_one, 0
	BRNE next_11
	CALL Display_0
	next_11:
	CPI Number_one, 1
	BRNE next_12
	CALL Display_1
	next_12:
	CPI Number_one, 2
	BRNE next_13
	CALL Display_2
	next_13:
	CPI Number_one, 3
	BRNE next_14
	CALL Display_3
	next_14:
	CPI Number_one, 4
	BRNE next_15
	CALL Display_4
	next_15:
	CPI Number_one, 5
	BRNE next_16
	CALL Display_5
	next_16:
	CPI Number_one, 6
	BRNE next_17
	CALL Display_6
	next_17:
	CPI Number_one, 7
	BRNE next_18
	CALL Display_7
	next_18:
	CPI Number_one, 8
	BRNE next_19
	CALL Display_8
	next_19:
	CPI Number_one, 9
	BRNE next_20
	CALL Display_9
	next_20:


	CALL DELAY
	CLR R16
	OUT PORTC, R16
	LDI R16, 0x02
	OUT PORTA, R16
	CPI Number_two, 0
	BRNE next_21
	CALL Display_0
	next_21:
	CPI Number_two, 1
	BRNE next_22
	CALL Display_1
	next_22:
	CPI Number_two, 2
	BRNE next_23
	CALL Display_2
	next_23:
	CPI Number_two, 3
	BRNE next_24
	CALL Display_3
	next_24:
	CPI Number_two, 4
	BRNE next_25
	CALL Display_4
	next_25:
	CPI Number_two, 5
	BRNE next_26
	CALL Display_5
	next_26:
	CPI Number_two, 6
	BRNE next_27
	CALL Display_6
	next_27:
	CPI Number_two, 7
	BRNE next_28
	CALL Display_7
	next_28:
	CPI Number_two, 8
	BRNE next_29
	CALL Display_8
	next_29:
	CPI Number_two, 9
	BRNE next_30
	CALL Display_9
	next_30:
	
	
	CALL DELAY
	CLR R16
	OUT PORTC, R16
	LDI R16, 0x04
	OUT PORTA, R16
	CPI Number_three, 0
	BRNE next_31
	CALL Display_0d
	next_31:
	CPI Number_three, 1
	BRNE next_32
	CALL Display_1d
	next_32:
	CPI Number_three, 2
	BRNE next_33
	CALL Display_2d
	next_33:
	CPI Number_three, 3
	BRNE next_34
	CALL Display_3d
	next_34:
	CPI Number_three, 4
	BRNE next_35
	CALL Display_4d
	next_35:
	CPI Number_three, 5
	BRNE next_36
	CALL Display_5d
	next_36:
	CPI Number_three, 6
	BRNE next_37
	CALL Display_6d
	next_37:
	CPI Number_three, 7
	BRNE next_38
	CALL Display_7d
	next_38:
	CPI Number_three, 8
	BRNE next_39
	CALL Display_8d
	next_39:
	CPI Number_three, 9
	BRNE next_40
	CALL Display_9d
	next_40:
	
	
	CALL DELAY
	CLR R16
	OUT PORTC, R16
	LDI R16, 0x08
	OUT PORTA, R16
	CPI Number_four, 0
	BRNE next_41
	CALL Display_0
	next_41:
	CPI Number_four, 1
	BRNE next_42
	CALL Display_1
	next_42:
	CPI Number_four, 2
	BRNE next_43
	CALL Display_2
	next_43:
	CPI Number_four, 3
	BRNE next_44
	CALL Display_3
	next_44:
	CPI Number_four, 4
	BRNE next_45
	CALL Display_4
	next_45:
	CPI Number_four, 5
	BRNE next_46
	CALL Display_5
	next_46:
	CPI Number_four, 6
	BRNE next_47
	CALL Display_6
	next_47:
	CPI Number_four, 7
	BRNE next_48
	CALL Display_7
	next_48:
	CPI Number_four, 8
	BRNE next_49
	CALL Display_8
	next_49:
	CPI Number_four, 9
	BRNE next
	CALL Display_9
	next:

	CALL DELAY
	RJMP LOOP	; Бесконечный цикл

EEWrite:	
	SBIC	EECR,EEWE		; Ждем готовности памяти к записи. Крутимся в цикле
	RJMP	EEWrite 		; до тех пор пока не очистится флаг EEWE
 
	CLI						; Затем запрещаем прерывания.
	OUT 	EEARL,R19 		; Загружаем адрес нужной ячейки
	OUT 	EEARH,R16  		; старший и младший байт адреса
	OUT 	EEDR,R18 		; и сами данные, которые нам нужно загрузить
 
	SBI 	EECR,EEMWE		; взводим предохранитель
	SBI 	EECR,EEWE		; записываем байт
 
	SEI 				; разрешаем прерывания
	RET 				; возврат из процедуры
 
 
EERead:	
	SBIC 	EECR,EEWE		; Ждем пока будет завершена прошлая запись.
	RJMP	EERead			; также крутимся в цикле.
	OUT 	EEARL, R24		; загружаем адрес нужной ячейки
	OUT  	EEARH, R16 		; его старшие и младшие байты
	SBI 	EECR,EERE 		; Выставляем бит чтения
	IN 	R18, EEDR 			; Забираем из регистра данных результат
	RET

WRITE:
   IN R16, TCCR1B
   SBRS R16, 0
   JMP Wend
   BRTS Wend
   SET
   CLR R16
   MOV R18, Number_one
   CALL EEWrite
   INC R19
   MOV R18, Number_two
   CALL EEWrite
   INC R19
   MOV R18, Number_three
   CALL EEWrite
   INC R19
   MOV R18, Number_four
   CALL EEWrite
   INC R19
   Wend:
   RET
   
READ:
   IN R16, TCCR1B
   SBRC R16, 0
   JMP Rend
   CPI R19, 0
   BREQ Rend
   CPI R26, 1
   BREQ Rend
   LDI R26, 1
   CLR R16
   Rloop:
   CP R24, R19
   BRNE no
   CLR R24
   no:
   CALL EEread
   MOV Number_one, R18
   INC R24
   CALL EEread
   MOV Number_two, R18
   INC R24
   CALL EEread
   MOV Number_three, R18
   INC R24
   CALL EEread
   MOV Number_four, R18
   INC R24
   Rend:
   RET
   

   ;=====================================================================================================================
   ;  для дисплея необходимо перевернуть значения, для это используем R31 и выводим значение R16 на портC
   ;=====================================================================================================================
Display_1:
   LDI  R16, 0b11111001
   LDI  R31, 0xFF
   EOR  R16, R31
   OUT  PORTC, R16
   RET
   
Display_2:
   LDI  R16, 0b10100100
   LDI  R31, 0xFF
   EOR  R16, R31
   OUT  PORTC, R16
   
   RET
Display_3:
   LDI  R16, 0b10110000
   LDI  R31, 0xFF
   EOR  R16, R31
   OUT  PORTC, R16
   RET

Display_4:
   LDI  R16, 0b10011001
   LDI  R31, 0xFF
   EOR  R16, R31
   OUT  PORTC, R16
   RET

Display_5:
   LDI  R16, 0b10010010
   LDI  R31, 0xFF
   EOR  R16, R31
   OUT  PORTC, R16
   RET

Display_6:
   LDI  R16, 0b10000010
   LDI  R31, 0xFF
   EOR  R16, R31
   OUT  PORTC, R16
   RET

Display_7:
   LDI  R16, 0b11111000
   LDI  R31, 0xFF
   EOR  R16, R31
   OUT  PORTC, R16
   RET

Display_8:
   LDI  R16, 0b10000000
   LDI  R31, 0xFF
   EOR  R16, R31
   OUT  PORTC, R16
   RET

Display_9:
   LDI  R16, 0b10010000
   LDI  R31, 0xFF
   EOR  R16, R31
   OUT  PORTC, R16
   RET

Display_0:
   LDI  R16, 0b11000000
   LDI  R31, 0xFF
   EOR  R16, R31
   OUT  PORTC, R16
   RET
 

	; для значений с точками
Display_1d:
   LDI  R16, 0b01111001
   LDI  R31, 0xFF
   EOR  R16, R31
   OUT  PORTC, R16
   RET
   
Display_2d:
   LDI  R16, 0b00100100
   LDI  R31, 0xFF
   EOR  R16, R31
   OUT  PORTC, R16
   RET
Display_3d:
   LDI  R16, 0b00110000
   LDI  R31, 0xFF
   EOR  R16, R31
   OUT  PORTC, R16
   RET

Display_4d:
   LDI  R16, 0b00011001
   LDI  R31, 0xFF
   EOR  R16, R31
   OUT  PORTC, R16
   RET

Display_5d:
   LDI  R16, 0b00010010
   LDI  R31, 0xFF
   EOR  R16, R31
   OUT  PORTC, R16
   RET

Display_6d:
   LDI  R16, 0b00000010
   LDI  R31, 0xFF
   EOR  R16, R31
   OUT  PORTC, R16
   RET

Display_7d:
   LDI  R16, 0b01111000
   LDI  R31, 0xFF
   EOR  R16, R31
   OUT  PORTC, R16
   RET

Display_8d:
   LDI  R16, 0b00000000
   LDI  R31, 0xFF
   EOR  R16, R31
   OUT  PORTC, R16
   RET

Display_9d:
   LDI  R16, 0b00010000
   LDI  R31, 0xFF
   EOR  R16, R31
   OUT  PORTC, R16
   RET

Display_0d:
   LDI  R16, 0b01000000
   LDI  R31, 0xFF
   EOR  R16, R31
   OUT  PORTC, R16
   RET
	
DELAY:
	LDI  R16, 100	; x
	LDI  R17, 1
DELAY_SUB:
	DEC  R16
	BRNE DELAY_SUB
	DEC  R17
	BRNE DELAY_SUB
	RET

EXT_INT0:				; Обработчик прерывания int0
	PUSH R16			; Сохранение текущего значения R16 в стеке
	IN   R16, SREG
	PUSH R16			; Сохранение текущего значения SREG в стеке
	
	IN R16, TCCR1B
	SBRC R16, 0
	JMP sk
	LDI R16, 0b00001101
	OUT TCCR1B, R16
	jmp skk
	sk:
	CLR R24
	LDI R16, 0b00001000
	OUT TCCR1B, R16
	skk:
	
	POP  R16
	OUT  SREG, R16		; Восстановление значения SREG из стека
	POP  R16			; Восстановление значения R16 из стека
	RETI				; Возврат из обработчика прерываний и разрешение прерываний

EXT_INT1:				; Обработчик прерывания int1
	PUSH R16			; Сохранение текущего значения R16 в стеке
	IN   R16, SREG
	PUSH R16			; Сохранение текущего значения SREG в стеке
	
	CLR Number_one
	CLR Number_two
	CLR Number_three
	CLR Number_four
	CLR R24
	CLR R19
	LDI R16, 0b00001000
	OUT TCCR1B, R16
	
	POP  R16
	OUT  SREG, R16		; Восстановление значения SREG из стека
	POP  R16			; Восстановление значения R16 из стека
	RETI				; Возврат из обработчика прерываний и разрешение прерываний

	; TCNT1  = OCCR1A, то прибавляем 1 к таймеру  
PLUS_SEC:
   PUSH R16				; Сохранение текущего значения R16 в стеке
   IN   R16, SREG
   PUSH R16				; Сохранение текущего значения SREG в стеке
   
   LDI R16, LOW(7812)
   OUT OCR1AL, R16
   LDI R16, HIGH(7812)
   OUT OCR1AH, R16
   
   INC Number_one
   CPI Number_one, 10
   BRNE fin
   LDI Number_one, 0
   INC Number_two
   CPI Number_two, 6
   BRNE fin
   LDI Number_two, 0
   INC Number_three
   CPI Number_three, 10
   BRNE fin
   LDI Number_three, 0
   INC Number_four
   CPI Number_four, 10
   BRNE fin
   LDI Number_four, 0
   fin:
   
   POP  R16
   OUT  SREG, R16	; Восстановление значения SREG из стека
   POP  R16			; Восстановление значения R16 из стека
  
   RETI