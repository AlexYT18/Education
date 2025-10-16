__asm("jmp kmain"); 
#define VDA (0xb8000)
#define IDT_INT (0x0E)
#define CUR_PORT (0x3D4)
#define VIDEO_WIDTH (80)
#define STR_LEN (40)
typedef unsigned long long uint64_t;

static inline unsigned char inb (unsigned short port); 
typedef void (*intr_handler)(); 
void intr_reg_handler(int num, unsigned short segm_sel, unsigned short type_attr, intr_handler hndlr); 
void keyb_process_keys(); 
void clear(); 
void keyb_handler(); 
void out_char (unsigned char symbol); 
void date(unsigned int number, unsigned int year); 
void nsconv(int i); 
void szconv(int i); 
unsigned int atoi(char* num); 
void wintime(int i); 
void posixtime(int i); 
void out_hash(); 
void parsing_command(); 
void info(); 
void clear(); 
void helping(); 
void error(int error_num); 

const char* symbols_shift = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const char* symbols = "0123456789abcdefghijklmnopqrstuvwxyz";
const char *codes = "001234567890-=0\tqwertyuiop[]00asdfghjkl;\'00\\zxcvbnm,./000 ";
const char *shift_codes = "00!@#$%^&*()_+00QWERTYUIOP{}00ASDFGHJKL:\"00|ZXCVBNM<>?000 ";

char system_color, str_inputing[STR_LEN + 1] = { 0 };
int cur_strnum, cur_position,string_count;
bool lshift,rshift,caps;

void int_char_initing()
{
    cur_strnum = 0;
    cur_position = 0;
    string_count = 0;
    system_color = 0;
    lshift = false;
    rshift = false;
    caps = false;
}
struct idt_classic
{
    unsigned short offset_low; 
    unsigned short segm_sel; 
    unsigned char zero; 
    unsigned char type_attr;
    unsigned short offset_high;
} __attribute__((packed));  

struct idt_ptr{
    unsigned short size;
    unsigned int address;
} __attribute__((packed));  

struct idt_classic idt_based[256];
struct idt_ptr idtp_temp;

// ========================================================
// ========================================================

unsigned int atoi(char* s)
{
    unsigned int n = 0;
    while (*s && (*s >= '0' && *s <= '9'))
        n = (n * 10u) + (unsigned int)(*s++ - '0');
    return (*s == '\0' || *s == ' ') ? n : (unsigned int)-1;
}

unsigned int strlen(char* s)
{
    unsigned int count = 0;
    char* p = s;
    while (*p && *p != '\n' &&count < STR_LEN) {
        ++count;
        ++p;
    }
    return count;
}

unsigned int strlen_const(const char* s)
{
    unsigned int count = 0;
    const char* p = s;
    while (*p && *p != '\n' &&count < STR_LEN) {
        ++count;
        ++p;
    }
    return count;
}

bool strcmp(char* a, const char* b)
{
    if (!a || !b) return false;

    unsigned int i = 0;
    while (a[i] && b[i] && i < STR_LEN) {
        if (a[i] != b[i]) return false;
        ++i;
    }
    return (a[i] == '\0' && b[i] == '\0');
}

char* strcpy(char* dest, const char* src)
{
    char* r = dest;
    while ((*r++ = *src++));
    return dest;
}

void memset(char* s)
{
    if (!s) return;
    char* p = s;
    unsigned int left = STR_LEN;
    while (left--) *p++ = '\0';
}

// ========================================================
// ========================================================

void scrolling_screen()
{
    if (cur_strnum != 23) {
        ++cur_strnum;
        return;
    }

    unsigned char* video_buf = (unsigned char*)VDA;
    unsigned int bytes_per_line = 2 * VIDEO_WIDTH;

    for (unsigned int i = 0; i < 23 * bytes_per_line; ++i)
        video_buf[i] = video_buf[i + bytes_per_line];

    for (unsigned int j = 0; j < bytes_per_line; j += 2)
        video_buf[23 * bytes_per_line + j] = 0;

    cur_position = 0;
}

void output_string(const void* data, unsigned int strnum)
{
    unsigned char* ptr = (unsigned char*)data;
    unsigned char* video_buf = (unsigned char*)VDA;
    video_buf += 2 * strnum * VIDEO_WIDTH;

    while (*ptr) {
        *video_buf = *ptr;
        video_buf[1] = system_color;
        video_buf += 2;
        ++ptr;
    }
    scrolling_screen();
}

void cursor_set_position(unsigned int strnum, unsigned int pos)
{
    unsigned short position_update = (strnum * VIDEO_WIDTH) + pos;
    asm volatile ("out %b0, %w1" : : "a" (0x0F), "Nd" (CUR_PORT)); 
    asm volatile ("out %b0, %w1" : : "a" ((unsigned char)(position_update & 0xFF)), "Nd" (CUR_PORT + 1)); 
    asm volatile ("out %b0, %w1" : : "a" (0x0E), "Nd" (CUR_PORT)); 
    asm volatile ("out %b0, %w1" : : "a" ((unsigned char)( (position_update >> 8) & 0xFF)), "Nd" (CUR_PORT + 1)); 
}

void default_intr_handler()
{
    asm volatile("pusha; popa; iret"); 
}

void output_symbol(char c, unsigned int color, unsigned int x, unsigned int y)
{
    unsigned char* video_buf = (unsigned char*)VDA;
    unsigned int offset = 2 * (y * VIDEO_WIDTH + x);

    video_buf[offset] = c;
    video_buf[offset + 1] = (unsigned char)color;
}

// ========================================================
// ========================================================

void win_time(char* com)
{
    int i = 0;
    unsigned int num = 0;
    unsigned int seconds_cou = 0;
    for(; i < STR_LEN; i++) 
    {
        if(com[i] == '\0') 
        break;
    }
    for(int j= i-7;j <= i; j++) com[j]='\0';
    if(com[i-8]!='\0') seconds_cou=com[i-8]-'0';
    com[i-8]='\0';
    num = atoi(com); 
    
    date(num, 1601); 
}

void date(unsigned int number, unsigned int year)
{
    unsigned int sec = number % 60;
    number /= 60;
    unsigned int min = number % 60;
    number /= 60;
    unsigned int hour = number % 24;
    number /= 24;

    while (1) {
        unsigned int leap = (year % 400 == 0) || ((year % 4 == 0) && (year % 100 != 0));
        unsigned int year_days = leap ? 366 : 365;
        if (number < year_days) break;
        number -= year_days;
        year++;
    }

    unsigned int leap = (year % 400 == 0) || ((year % 4 == 0) && (year % 100 != 0));
    unsigned int mdays[12] = {31, 28 + leap, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    unsigned int month = 0;
    while (month < 12 && number >= mdays[month]) {
        number -= mdays[month];
        month++;
    }

    unsigned int day = number + 1;
    unsigned int m = month + 1;

    unsigned char str[32];
    unsigned int pos = 0;

    str[pos++] = (day / 10) + '0';
    str[pos++] = (day % 10) + '0';
    str[pos++] = '.';
    str[pos++] = (m / 10) + '0';
    str[pos++] = (m % 10) + '0';
    str[pos++] = '.';
    str[pos++] = ((year / 1000) % 10) + '0';
    str[pos++] = ((year / 100) % 10) + '0';
    str[pos++] = ((year / 10) % 10) + '0';
    str[pos++] = (year % 10) + '0';
    str[pos++] = ' ';
    str[pos++] = (hour / 10) + '0';
    str[pos++] = (hour % 10) + '0';
    str[pos++] = ':';
    str[pos++] = (min / 10) + '0';
    str[pos++] = (min % 10) + '0';
    str[pos++] = ':';
    str[pos++] = (sec / 10) + '0';
    str[pos++] = (sec % 10) + '0';
    str[pos] = '\0';

    output_string(str, cur_strnum);
    cur_position = 0;
    cursor_set_position(cur_strnum, cur_position);
}

void number_to_string(unsigned int integer, int arg1, int arg2) {
    unsigned int number = integer, i = 0;
    char str[23] = { 0 };
    for(; i<19 ; i++){
        number /= 10;
        if (number == 0)break;
    }
    for (int j = i; j >= 0; j--) {
        str[j] = integer % 10 + '0';
        integer /= 10;
    }
    str[i + 1] = '.';
    str[i + 2] = arg1 + '0';
    str[i + 3] = arg2 + '0';
    str[i + 4] = '\0';

    output_string(str, cur_strnum); 
    cur_position=0;
    cursor_set_position(cur_strnum, cur_position); 
}
void division(unsigned int number, int divider) 
{
    unsigned int divisor = 1;

    if (divider == 1) divisor = 1024;//KB
    else if (divider == 2) divisor = 1024 * 1024;//MB
    else if (divider == 3) divisor = 1024 * 1024 * 1024;//GB

    unsigned int integer = number / divisor;
    unsigned int remainder = number % divisor; 

    unsigned int ost1 = (remainder * 10) / divisor;    
    unsigned int ost2 = ((remainder * 100) / divisor) % 10;  

    number_to_string(integer, ost1, ost2);
}
void in_kbyte(char* from, unsigned int number) {
    unsigned long long kbyte = 0;

    if (strcmp(from, "b")) {
        kbyte = number / 1024ULL;
    }
    else if (strcmp(from, "kb")) {
        kbyte = number;
    }
    else if (strcmp(from, "mb")) {
        if (number > 4194303) { 
            error(2);
            return;
        }
        kbyte = number * 1024ULL;
    }
    else if (strcmp(from, "gb")) {
        if (number > 4095) { 
            error(2);
            return;
        }
        kbyte = number * 1024ULL * 1024ULL;
    }
    else if (strcmp(from, "tb")) {
        if (number > 3) { 
            error(2);
            return;
        }
        kbyte = number * 1024ULL * 1024ULL * 1024ULL;
    }
    else {
        error(6); 
        return;
    }

    number_to_string((unsigned int)kbyte, 0, 0);
}

void in_byte(char* from, unsigned int number) {
    unsigned long long byte = 0;
    if (strcmp(from, "b")) {
        byte = number;
    }
    else if (strcmp(from, "kb")) {
        byte = (unsigned long long)number * 1024ULL;
    }
    else if (strcmp(from, "mb")) {
        byte = (unsigned long long)number * 1024ULL * 1024ULL;
    }
    else if (strcmp(from, "gb")) {
        byte = (unsigned long long)number * 1024ULL * 1024ULL * 1024ULL;
    }
    else {
        error(6);
        return;
    }

    number_to_string((unsigned int)byte, 0, 0);
}
void in_mbyte(char* from, unsigned int number) {
    unsigned long long mbyte = 0;

    if (strcmp(from, "b")) {
        mbyte = number / (1024ULL * 1024ULL);
    }
    else if (strcmp(from, "kb")) {
        mbyte = number / 1024ULL;
    }
    else if (strcmp(from, "mb")) {
        mbyte = number;
    }
    else if (strcmp(from, "gb")) {
        if (number > 4194303) { 
            error(2);
            return;
        }
        mbyte = number * 1024ULL;
    }
    else if (strcmp(from, "tb")) {
        if (number > 4095) { 
            error(2);
            return;
        }
        mbyte = number * 1024ULL * 1024ULL;
    }
    else {
        error(6); 
        return;
    }

    number_to_string((unsigned int)mbyte, 0, 0);
}

void in_gbyte(char* from, unsigned int number) 
{
    unsigned long long gbyte = 0;

    if (strcmp(from, "b")) {
        gbyte = number / (1024ULL * 1024ULL * 1024ULL);
    }
    else if (strcmp(from, "kb")) {
        gbyte = number / (1024ULL * 1024ULL);
    }
    else if (strcmp(from, "mb")) {
        gbyte = number / 1024ULL;
    }
    else if (strcmp(from, "gb")) {
        gbyte = number;
    }
    else if (strcmp(from, "tb")) {
        if (number > 4095) { 
            error(2);
            return;
        }
        gbyte = number * 1024ULL;
    }
    else {
        error(6); 
        return;
    }

    number_to_string((unsigned int)gbyte, 0, 0);
}

void in_tbyte(char* from, unsigned int number) {
    unsigned long long byte = 0;
    if (strcmp(from, "b")){
        byte = 0;
        number_to_string(byte, 0, 0);  
    } 
    else if (strcmp(from, "kb"))division(number, 3); 
    else if (strcmp(from, "mb"))division(number, 2); 
    else if (strcmp(from, "gb"))division(number, 1); 
    else if(strcmp(from, "tb")){
        byte=number;
        number_to_string(byte, 0, 0);  
    }
}
void determine_function(unsigned int number, char* from, char* to) {
    if(strcmp(to, "b"))in_byte(from, number); 
    else if (strcmp(to, "kb"))in_kbyte(from, number); 
    else if (strcmp(to, "mb"))in_mbyte(from, number); 
    else if (strcmp(to, "gb"))in_gbyte(from, number); 
    else if (strcmp(to, "tb"))in_tbyte(from, number); 
}

bool overflow(char* number)
{
    const char max_val[] = "4294967295";
    unsigned int len = 0;
    while (number[len] != '\0' && len < STR_LEN) {
        if (number[len] < '0' || number[len] > '9')
            return false;
        len++;
    }

    if (len == 0) return false;
    if (len < 10) return true;
    if (len > 10) return false;

    for (int i = 0; i < 10; i++) {
        if (number[i] < max_val[i]) return true;
        if (number[i] > max_val[i]) return false;
    }
    return true;
}


bool correct_nsconv(char* number, int from_cc, int to_cc)
{
    memset(str_inputing);
    const char* error_in = 0;

    if (number[0] == '\0') {
        error_in = "error: invalid input";
        output_string(error_in, cur_strnum);
        cur_position = 0;
        cursor_set_position(cur_strnum, cur_position);
        return false;
    }

    if (from_cc < 2 || to_cc < 2 || from_cc > 36 || to_cc > 36) {
        error_in = "error: incorrect number system (possible number systems from 2 to 36)";
        output_string(error_in, cur_strnum);
        cur_position = 0;
        cursor_set_position(cur_strnum, cur_position);
        return false;
    }

    if (from_cc == 10 && !overflow(number)) {
        error_in = "error: overflow";
        output_string(error_in, cur_strnum);
        cur_position = 0;
        cursor_set_position(cur_strnum, cur_position);
        return false;
    }

    for (int i = 0; number[i] != '\0' && i < STR_LEN; i++) {
        char c = number[i];
        int value;
        if (c >= '0' && c <= '9') value = c - '0';
        else if (c >= 'A' && c <= 'Z') value = c - 'A' + 10;
        else if (c >= 'a' && c <= 'z') value = c - 'a' + 10;
        else {
            error_in = "error: invalid character in number";
            output_string(error_in, cur_strnum);
            cur_position = 0;
            cursor_set_position(cur_strnum, cur_position);
            return false;
        }
        if (value >= from_cc) {
            error_in = "error: the source number system does not support the entered number";
            output_string(error_in, cur_strnum);
            cur_position = 0;
            cursor_set_position(cur_strnum, cur_position);
            return false;
        }
    }

    return true;
}

bool correct_szconv(unsigned int number, char* byte1, char* byte2)
{
    if (number == (unsigned int)-1) {
        error(5);
        return false;
    }
    if (!byte1[0] || !byte2[0]) {
        error(7);
        return false;
    }
    for (int idx = 0; idx < 2; idx++) {
        char* s = (idx == 0) ? byte1 : byte2;

        if (!s || !s[0]) {
            error(6); 
            return false;
        }
        char c0 = (s[0] >= 'A' && s[0] <= 'Z') ? (char)(s[0] + 32) : s[0];
        char c1 = (s[1] >= 'A' && s[1] <= 'Z') ? (char)(s[1] + 32) : s[1];

        bool valid = false;
        switch (c0) {
            case 'b':
                valid = (s[1] == '\0');
                break;
            case 'k':
            case 'm':
            case 'g':
            case 't':
                valid = (c1 == 'b' && s[2] == '\0');
                break;
            default:
                valid = false;
        }

        if (!valid) {
            error(6); 
            return false;
        }
    }
    return true;
}

static inline unsigned char key_pressing_asm (unsigned short port)
{
 unsigned char data;
 asm volatile ("inb %w1, %b0" : "=a" (data) : "Nd" (port)); 
 return data;
}

void intr_reg_handler(int num, unsigned short segm_sel, unsigned short type_attr, intr_handler hndlr)
{
    unsigned int hndlr_addr = (unsigned int) hndlr;
    idt_based[num].offset_low = (unsigned short) (hndlr_addr & 0xFFFF); 
    idt_based[num].segm_sel = segm_sel;
    idt_based[num].zero = 0;
    idt_based[num].type_attr = type_attr;
    idt_based[num].offset_high = (unsigned short) (hndlr_addr >> 16); 
}

void wintime(int i)
{
    char str_command[STR_LEN] = { 0 };
    for (unsigned int count = 0; i < STR_LEN; i++, count++) 
    {
        if (str_inputing[i] == ' ' || str_inputing[i] == '\0' || str_inputing[i] == '\n') 
        {
            str_command[count] = '\0';
            break;
        }
        str_command[count] = str_inputing[i];
    }
    if(str_command[0] == '\0')
    {
        memset(str_inputing); 
        error(7); 
        return;
    }
    win_time(str_command); 
    output_string("bag catch",0); 
}
void posixtime(int i)
{   
    char str_command[STR_LEN] = { 0 };
    for (unsigned int count = 0; i < STR_LEN; i++, count++) 
    {
        if (str_inputing[i] == ' ' || str_inputing[i] == '\0' || str_inputing[i] == '\n') 
        {
            str_command[count] = '\0';
            break;
        }
        str_command[count] = str_inputing[i];
    }
    if(str_command[0] == '\0')
    {
        memset(str_inputing); 
        error(7); 
        return;
    }
    if (atoi(str_command) == -1) 
    {
        memset(str_inputing); 
        error(5); 
        return;
    }
    if(!overflow(str_command))
    {
        memset(str_inputing); 
        error(2); 
        return;
    }
    unsigned int ntmp = atoi(str_command); 
    date(ntmp, 1970); 
}
void nsconv(int i)
{
    char number[STR_LEN] = {0};
    char arg1[STR_LEN] = {0};
    char arg2[STR_LEN] = {0};
    int from_cc = 0, to_cc = 0;
    int stage = 0, n = 0;

    for (; i < STR_LEN && str_inputing[i] != '\0'; i++) {
        char c = str_inputing[i];
        if (c == ' ') {
            stage++;
            n = 0;
            continue;
        }
        if (stage == 0) number[n++] = c;
        else if (stage == 1) arg1[n++] = c;
        else if (stage == 2) arg2[n++] = c;
    }

    from_cc = atoi(arg1);
    to_cc = atoi(arg2);

    if (!correct_nsconv(number, from_cc, to_cc))
        return;

    unsigned int value = 0;
    int len = strlen_const(number);

    for (int j = 0; j < len; j++) {
        char c = number[j];
        int digit;
        if (c >= '0' && c <= '9') digit = c - '0';
        else if (c >= 'A' && c <= 'Z') digit = c - 'A' + 10;
        else if (c >= 'a' && c <= 'z') digit = c - 'a' + 10;
        else return;
        value = value * from_cc + digit;
        if (value > 4294967295U) {
            output_string("error: overflow", cur_strnum);
            cur_position = 0;
            cursor_set_position(cur_strnum, cur_position);
            return;
        }
    }

    if (value == 0) {
        output_string("0", cur_strnum);
        cur_position = 0;
        cursor_set_position(cur_strnum, cur_position);
        return;
    }

    unsigned char result[64];
    int idx = 0;
    while (value > 0 && idx < 63) {
        int rem = value % to_cc;
        result[idx++] = (rem < 10) ? ('0' + rem) : ('A' + rem - 10);
        value /= to_cc;
    }

    unsigned char output[64];
    for (int j = 0; j < idx; j++)
        output[j] = result[idx - j - 1];
    output[idx] = '\0';

    output_string(output, cur_strnum);
    cur_position = 0;
    cursor_set_position(cur_strnum, cur_position);
}

void szconv(int i) 
{
    char str_command[STR_LEN] = { 0 };
    int count = 0;
    unsigned int num1 = 0, flag = 1;
    char byte1[3] = { 0 }, byte2[3] = { 0 };
    for (; i < STR_LEN; i++, count++) {
        if (str_inputing[i] == ' ' || str_inputing[i] == '\0') {
            if(flag==2)byte1[count] = '\0';
            else if(flag==3)byte2[count] = '\0';
            else {
                str_command[count] = '\0';
                num1 = atoi(str_command); 
                if(!overflow(str_command)){
                error(2); 
                return;
                }
            }
            flag++;
            count = -1;
            if (flag == 4) break;
            else continue;
        }
        if(flag==2)byte1[count] = str_inputing[i];
        else if(flag==3)byte2[count] = str_inputing[i];
        else str_command[count] = str_inputing[i];
    }
    if(!correct_szconv(num1, byte1, byte2)){
        memset(str_inputing);     
        return;
    }
    determine_function(num1, byte1, byte2); 
}
void parsing_command()
{
    char str_command[STR_LEN] = { 0 };
    int i = 0;
    for (; i < STR_LEN; i++) 
    {
        if (str_inputing[i] == ' ' || str_inputing[i] == '\0') 
        {
            str_command[i] = '\0';
            break;
        }
        str_command[i] = str_inputing[i];
    }
    i++;//first arg

    if (strcmp(str_command, "info")) info(); 
    else if (strcmp(str_command, "clear")) clear(); 
    else if (strcmp(str_command, "shutdown") || strcmp(str_command, "exit"))
    {
        asm volatile ("outw %w0, %w1" : : "a" (0x2000), "Nd" (0x604)); 
    } 
    else if (strcmp(str_command, "help")) helping(); 
    else if (strcmp(str_command, "nsconv")) 
    {
        memset(str_command); 
        nsconv(i); 
    } 
    else if (strcmp(str_command, "posixtime")) 
    {
        memset(str_command); 
        posixtime(i); 
    } 
    else if (strcmp(str_command, "wintime"))
    {
        memset(str_command); 
        wintime(i); 
    }
    else if (strcmp(str_command, "szconv")) 
    {
        memset(str_command); 
        szconv(i); 
    } 
    
    else {
        memset(str_inputing); 
        output_string("error: invalid command. Use \"help\" to see all commands", cur_strnum); 
        cur_position = 0;
        cursor_set_position(cur_strnum, cur_position); 
    }
}
void input_string(unsigned char button_input)
{
    char symbol=0;
    if((lshift==false 
         && rshift==false
         && caps==false) 
        ||(caps==true
         &&  (lshift==true || rshift==true)))
        symbol=codes[(int)button_input];
    else if(lshift==true||rshift==true||caps==true)
        symbol=shift_codes[(int)button_input];
    if(cur_position!=STR_LEN)out_char(symbol); 
    lshift=false;
    rshift=false;
}
void keyb_process_keys()
{ 
    if (key_pressing_asm(0x64) & 0x01) 
    {
        unsigned char button_input = key_pressing_asm(0x60); 
        if (button_input < 128) 
        {
            if(button_input == 54  &&  rshift == false)
            {
                rshift = true;
                return;
            }
            if (button_input == 42  &&  lshift == false)
            {
                lshift = true;
                return;
            }
            
            if(button_input == 58  &&  caps==false)
            {
                caps = true;
                return;
            }
            if(button_input == 58  &&  caps==true)
            {
                caps = false;
                return;
            }
            if (button_input == 14) 
            {
                unsigned char* video_buf = (unsigned char*) VDA;
                video_buf += 2*(cur_strnum * VIDEO_WIDTH + cur_position - 1); 
                video_buf[0] = '\0';
                if (cur_position > 2) cur_position--;
                cursor_set_position(cur_strnum, cur_position); 
                if(string_count>0) string_count--;
                str_inputing[string_count]='\0';
                return;
            }
            if (button_input == 15  &&  lshift == false)
            {
                unsigned char* video_buf = (unsigned char*) VDA;
                cur_position+=4;
                video_buf+=cur_strnum*VIDEO_WIDTH + cur_position;
                cursor_set_position(cur_strnum, cur_position); 
                return;
            }
            if (button_input == 28)
            {
                str_inputing[string_count]='\0';
                scrolling_screen(); 
                parsing_command(); 
                cur_position=0;
                cursor_set_position(cur_strnum, cur_position); 
                memset(str_inputing); 
                string_count=0;
                out_hash(); 
                return;
            }
            if(button_input >= 2  &&  button_input <= 13 ||
                button_input >= 16  &&  button_input <= 27||
                button_input >= 30  &&  button_input <= 41||
                button_input >= 43  &&  button_input <= 53||
                button_input == 57) input_string(button_input); 
        }
    }
}

void keyb_handler()
{ 
    asm("pusha"); 
    keyb_process_keys(); 
    asm volatile ("out %b0, %w1" : : "a" (0x20), "Nd" (0x20)); 
    asm("popa; leave; iret"); 
}



void out_hash(){
    unsigned char* video_buf = (unsigned char*) VDA;
    video_buf += 2*(cur_strnum * VIDEO_WIDTH + cur_position); 
    video_buf[0] = '#';
    video_buf[1] = system_color;
    video_buf[2] = ' ';
    video_buf[3] = system_color;
    cur_position += 2;
    cursor_set_position(cur_strnum, cur_position); 
}
void out_char(unsigned char symbol)
{
    unsigned char* video_buf = (unsigned char*) VDA;
    video_buf += 2*(cur_strnum * VIDEO_WIDTH + cur_position); 
    video_buf[0] = symbol;
    video_buf[1] = system_color;
    if(cur_position < 42)
    {
        str_inputing[string_count] = symbol;
        cur_position++;
        string_count++;
        cursor_set_position(cur_strnum, cur_position); 
    }
}

void info()
{
    const char* pre[] = {
        "ConvertOS:",
        "Developer: Tsebro Alex",
        "Group: 5151001/30001, KB-1",
        "Assembler translator: YASM",
        "Syntax: INTEL",
        "Compiler: gcc",
        "Font: CP437",
        0
    };
    for (int i = 0; pre[i]; ++i) output_string(pre[i], cur_strnum);

    const unsigned char colors[] = {0x07, 0x0F, 0x0E, 0x01, 0x04, 0x02};
    const char* labels[] = {
        "OS color: gray",
        "OS color: white",
        "OS color: yellow",
        "OS color: blue",
        "OS color: red",
        "OS color: green"
    };
    int idx = -1;
    for (int j = 0; j < 6; ++j) {
        if (system_color == colors[j]) { idx = j; break; }
    }
    if (idx >= 0) output_string(labels[idx], cur_strnum);
    cursor_set_position(cur_strnum, cur_position);
}

void clear()
{
    unsigned char* vmem = (unsigned char*)VDA;
    unsigned int total = VIDEO_WIDTH * 25 * 2;
    for (unsigned int i = 0; i < total; i += 2) {
        vmem[i] = 0;
        vmem[i + 1] = 0;
    }
    cur_strnum = 0;
    cur_position = 0;
    cursor_set_position(0, 0);
}


void error(int error_num)
{
    memset(str_inputing); 
    const char* error_in = { 0 };
    if (error_num == 2)error_in = "error: overflow";
    else if (error_num == 3)error_in = "error: the source number system does not support the entered number";
    else if (error_num == 4)error_in = "error: incorrect number system (possible number systems from 2 to 36)";
    else if (error_num == 5)error_in = "error: invalid number input ";
    else if (error_num == 6)error_in = "error: invalid byte name";
    else if (error_num == 7)error_in = "error: invalid input";
    output_string(error_in, cur_strnum); 
    cur_position = 0;
    cursor_set_position(cur_strnum, cur_position); 
}
void helping()
{
    output_string("info", cur_strnum); 
    output_string("clear", cur_strnum); 
    output_string("shutdown", cur_strnum); 
    output_string("nsconv <number> <from CC> <to CC>", cur_strnum); 
    output_string("posixtime <number>", cur_strnum); 
    output_string("wintime <number>", cur_strnum); 
    output_string("szconv <number> <from size> <to size> (size: b,kb,mb,tb)", cur_strnum); 
    cursor_set_position(cur_strnum, cur_position); 
}

void start()
{
    int_char_initing(); 
    const char* hello = "Welcome to ConvertOS!";
    // === Loading color === 
    char color= (*(char*)(0x600)); 
    if(color== 0) system_color=0x07;
    else if(color== 1) system_color=0x0F;
    else if(color== 2) system_color=0x0E;
    else if(color== 3) system_color=0x01;
    else if(color== 4) system_color=0x04;
    else if(color== 5) system_color=0x02;

    unsigned char* video_buf = (unsigned char*) VDA;
    for(int i = 0; i < 80 * 42; i++)
    {
        video_buf[i*2] = ' ';     
        video_buf[i*2+1] = system_color;     
    }
    output_string(hello, 0); 
    // IDT - Interrupt Descriptor Table
    int idt_count = sizeof(idt_based) / sizeof(idt_based[0]); 
    for(int i = 0; i < idt_count; i++) intr_reg_handler(i, 0x8, 0x80 | IDT_INT,default_intr_handler); 

    intr_reg_handler(0x09, 0x8, 0x80 | IDT_INT, keyb_handler);  
    asm volatile ("out %b0, %w1" : : "a" (0xFF ^ 0x02), "Nd" (0x20 + 1)); 

    idt_count = sizeof(idt_based) / sizeof(idt_based[0]); 
    idtp_temp.address = (unsigned int) (&idt_based[0]); 
    idtp_temp.size = (sizeof (struct idt_classic) * idt_count) - 1;
    asm("lidt %0" : : "m" (idtp_temp) ); 

    asm("sti"); 
    cursor_set_position(cur_strnum, cur_position); 
    out_hash(); 
}

void debuging()
{

}


extern "C" int kmain() 
{
    start(); 
    debuging();
    while(true) asm("hlt"); 
    return 0;
}
