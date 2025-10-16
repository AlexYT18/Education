[BITS 16]
[ORG 0x7C00]
start:
    mov ax, cs
    mov ds, ax
    mov ss, ax
    mov sp, start
    mov ah, 0x0e
    mov ax, 0x1000
    mov es, ax
    mov bx, 0x00
    mov ah, 0x02
    mov dl, 1
    mov dh, 0x00
    mov cl, 0x01
    mov ch, 0x00
    mov al, 40
    int 0x13
    call menu

menu:
    mov ax, 3
    int 0x10
    mov si, color_title
    mov edi, 0xb8000
    mov edx, 0x00
    mov si, color_title
    call draw_color_list

input:
    mov ah, 0x00
    int 0x16
    cmp ah, 0x48
    je move_up
    cmp ah, 0x50
    je move_down
    cmp ah, 0x1C
    je load_kernel
    jmp input

move_up:
    dec byte [selected_color]
    cmp byte [selected_color], 0xFF
    jne menu
    mov byte [selected_color], 5
    jmp menu

move_down:
    inc byte [selected_color]
    cmp byte [selected_color], 6
    jne menu
    mov byte [selected_color], 0
    jmp menu

draw_color_list:
    mov si, color_title
    call print_string
    call new_line
    mov si, color_list
    xor bx, bx
.draw_loop:
    lodsw
    or ax, ax
    jz .done
    push si
    mov si, ax
    cmp bl, [selected_color]
    jne .no_arrow
    mov ah, 0x0E
    mov al, ' '
    int 0x10
    mov al, '>'
    int 0x10
    jmp .print
.no_arrow:
    mov ah, 0x0E
    mov al, ' '
    int 0x10
    mov al, ' '
    int 0x10
    mov al, ' '
    int 0x10
.print:
    call print_string
    call new_line
    pop si
    inc bl
    jmp .draw_loop
.done:
    ret

print_string:
    lodsb
    or al, al
    jz .done
    mov ah, 0x0E
    int 0x10
    jmp print_string
.done:
    ret

new_line:
    mov ah, 0x0E
    mov al, 0x0D
    int 0x10
    mov al, 0x0A
    int 0x10
    ret

selected_color: db 0
color_title: db 'Choose color:', 0
gray: db 'gray', 0
white: db 'white', 0
yellow: db 'yellow', 0
blue: db 'blue', 0
red: db 'red', 0
green: db 'green', 0

color_list:
    dw gray
    dw white
    dw yellow
    dw blue
    dw red
    dw green
    dw 0

color_codes:
    db 0x07; 0
    db 0x0F; 1
    db 0x0E; 2
    db 0x01; 3
    db 0x04; 4
    db 0x02; 5

choise_color:
    xor bx, bx
    mov bx, [selected_color]
    mov [0x600], bx
    ret
load_kernel:
    mov ax, 3
    int 0x10
    call choise_color
    cli
    lgdt [gdt_info]
    in al, 0x92
    or al, 2
    out 0x92, al
    mov eax, cr0
    or al, 1
    mov cr0, eax
    jmp 0x8:protected_mode

; === Descriptor ===
gdt:
    db 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    db 0xff, 0xff, 0x00, 0x00, 0x00, 0x9A, 0xCF, 0x00
    db 0xff, 0xff, 0x00, 0x00, 0x00, 0x92, 0xCF, 0x00

gdt_info:
    dw gdt_info - gdt
    dw gdt, 0

[BITS 32]
protected_mode:
    mov ax, 0x10
    mov es, ax
    mov ds, ax
    mov ss, ax
    call 0x10000
    times (512 - ($ - start) - 2) db 0
    db 0x55, 0xAA
