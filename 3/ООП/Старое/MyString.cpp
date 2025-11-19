#include "MyString.h"
#include <cstring>
#include <algorithm>
#include <stdexcept>
#include <string>

namespace my_string_std 
{

    // =======================
    // Внутренние хелперы
    // =======================

    // Безопасная инициализация пустой строки
    static inline void make_empty(char*& ptr, size_t& sz, size_t& cap) {
        cap = 1;
        ptr = new char[cap];
        ptr[0] = '\0';
        sz = 0;
    }

    // Подсчёт следующей достаточной ёмкости (геометрический рост)
    inline size_t next_capacity_ge(size_t current, size_t required) {
        if (current == 0) current = 1;
        while (current < required) current *= 2;
        return current;
    }

    // =======================
    // Класс MyString
    // =======================

    // --- Приватные вспомогательные методы (как статические/инлайн в .cpp) ---

    // Обеспечить ёмкость >= required
    inline void ensure_capacity(char*& ptr, size_t& sz, size_t& cap, size_t required) {
        if (cap >= required) return;
        size_t new_cap = next_capacity_ge(cap, required);
        char* np = new char[new_cap];
        if (sz > 0) std::memcpy(np, ptr, sz);
        np[sz] = '\0';
        delete[] ptr;
        ptr = np;
        cap = new_cap;
    }

    // Скопировать из data длиной len в новый буфер (инициализация объекта)
    inline void init_from_buffer(char*& ptr, size_t& sz, size_t& cap, const char* data, size_t len) {
        cap = std::max<size_t>(1, len + 1);
        ptr = new char[cap];
        if (len) std::memcpy(ptr, data, len);
        ptr[len] = '\0';
        sz = len;
    }

    // Скопировать из C-строки (nullptr трактуем как пустую строку)
    inline void init_from_cstr(char*& ptr, size_t& sz, size_t& cap, const char* s) {
        if (!s) { make_empty(ptr, sz, cap); return; }
        init_from_buffer(ptr, sz, cap, s, std::strlen(s));
    }

    // Безопасный обратный сдвиг вправо на len элементов, начиная с index..sz включительно (копируем хвост вместе с '\0')
    inline void shift_right(char* buf, size_t index, size_t sz, size_t len) {
        // копируем с конца, включая нулевой терминатор
        for (size_t i = sz + 1; i-- > index; ) {
            buf[i + len] = buf[i];
        }
    }

    // Безопасный сдвиг влево на len элементов, начиная с index..конец
    inline void shift_left(char* buf, size_t index, size_t sz, size_t len) {
        // переносим [index+len .. sz] на [index ..]
        for (size_t i = index; i <= sz; ++i) {
            buf[i] = buf[i + len];
        }
    }

    // Внутренняя вставка блока [data,len] по индексу
    inline void insert_internal(char*& ptr, size_t& sz, size_t& cap, size_t index, const char* data, size_t len) {
        if (!data || len == 0) return;
        if (index > sz) return;
        ensure_capacity(ptr, sz, cap, sz + len + 1);
        shift_right(ptr, index, sz, len);
        std::memcpy(ptr + index, data, len);
        sz += len;
    }

    // Внутреннее добавление блока [data,len] в конец
    inline void append_internal(char*& ptr, size_t& sz, size_t& cap, const char* data, size_t len) {
        if (!data || len == 0) return;
        ensure_capacity(ptr, sz, cap, sz + len + 1);
        std::memcpy(ptr + sz, data, len);
        sz += len;
        ptr[sz] = '\0';
    }

    // Внутренняя замена COUNT символов, начиная с INDEX, на блок [data,len]
    inline void replace_internal(char*& ptr, size_t& sz, size_t& cap, size_t index, size_t count, const char* data, size_t len) {
        if (index > sz) return;
        size_t actual = std::min<size_t>(count, sz - index);
        if (!data) { // если данных нет — это эквивалент удаления
            if (actual == 0) return;
            shift_left(ptr, index, sz, actual);
            sz -= actual;
            ptr[sz] = '\0';
            return;
        }
        // сценарии: len == actual (прямая перезапись), len > actual (надо расширить), len < actual (надо сжать)
        if (len == actual) {
            if (len) std::memcpy(ptr + index, data, len);
            return;
        }
        else if (len > actual) {
            size_t grow = len - actual;
            ensure_capacity(ptr, sz, cap, sz + grow + 1);
            shift_right(ptr, index + actual, sz, grow);
            if (len) std::memcpy(ptr + index, data, len);
            sz += grow;
        }
        else { // len < actual
            // сначала пишем len байт, затем сдвигаем влево лишнее
            if (len) std::memcpy(ptr + index, data, len);
            size_t shrink = actual - len;
            // сдвигаем остаток вправо-лево: фактически сжимаем
            for (size_t i = index + len; i <= sz; ++i) {
                ptr[i] = ptr[i + shrink];
            }
            sz -= shrink;
        }
        ptr[sz] = '\0';
    }

    // Сравнение лексикографически двух буферов фиксированной длины
    inline int cmp_lex(const char* a, size_t a_len, const char* b, size_t b_len) {
        size_t m = std::min<size_t>(a_len, b_len);
        int r = std::memcmp(a, b, m);
        if (r != 0) return (r < 0) ? -1 : 1;
        if (a_len == b_len) return 0;
        return (a_len < b_len) ? -1 : 1;
    }

    // Наивный поиск подстроки [pat, pat_len] в [txt, txt_len] начиная с start; вернуть int индекс или -1
    inline int find_naive(const char* txt, size_t txt_len, const char* pat, size_t pat_len, size_t start) {
        if (pat_len == 0) return static_cast<int>(start <= txt_len ? start : txt_len);
        if (start > txt_len) return -1;
        if (pat_len > txt_len) return -1;
        for (size_t i = start; i + pat_len <= txt_len; ++i) {
            if (std::memcmp(txt + i, pat, pat_len) == 0) return static_cast<int>(i);
        }
        return -1;
    }

    // =======================
    // Реализация MyString
    // =======================

    // --- Конструкторы / деструктор ---

    MyString::MyString() : my_ptr(nullptr), my_size(0), my_capacity(0) {
        make_empty(my_ptr, my_size, my_capacity);
    }

    MyString::MyString(const char* SOURCE_STR) : my_ptr(nullptr), my_size(0), my_capacity(0) {
        init_from_cstr(my_ptr, my_size, my_capacity, SOURCE_STR);
    }

    MyString::MyString(const char* SOURCE_STR, size_t COUNT) : my_ptr(nullptr), my_size(0), my_capacity(0) {
        if (!SOURCE_STR || COUNT == 0) {
            make_empty(my_ptr, my_size, my_capacity);
        }
        else {
            size_t len = std::min<size_t>(COUNT, std::strlen(SOURCE_STR));
            init_from_buffer(my_ptr, my_size, my_capacity, SOURCE_STR, len);
        }
    }

    MyString::MyString(const std::initializer_list<char>& LIST) : my_ptr(nullptr), my_size(0), my_capacity(0) {
        init_from_buffer(my_ptr, my_size, my_capacity, LIST.size() ? LIST.begin() : "", LIST.size());
    }

    MyString::MyString(const std::string& SOURCE_STR) : my_ptr(nullptr), my_size(0), my_capacity(0) {
        init_from_buffer(my_ptr, my_size, my_capacity, SOURCE_STR.data(), SOURCE_STR.size());
    }

    MyString::MyString(size_t COUNT, char cur_char) : my_ptr(nullptr), my_size(0), my_capacity(0) {
        if (COUNT == 0) { make_empty(my_ptr, my_size, my_capacity); return; }
        my_capacity = COUNT + 1;
        my_ptr = new char[my_capacity];
        std::memset(my_ptr, static_cast<unsigned char>(cur_char), COUNT);
        my_ptr[COUNT] = '\0';
        my_size = COUNT;
    }

    MyString::MyString(MyString& another) : my_ptr(nullptr), my_size(0), my_capacity(0) {
        init_from_buffer(my_ptr, my_size, my_capacity, another.my_ptr, another.my_size);
    }

    MyString::~MyString() {
        delete[] my_ptr;
        my_ptr = nullptr;
        my_size = 0;
        my_capacity = 0;
    }

    // --- Управление памятью / содержимым ---

    void MyString::clear() {
        delete[] my_ptr;
        make_empty(my_ptr, my_size, my_capacity);
    }

    void MyString::shrink_to_fit() {
        if (my_capacity == my_size + 1) return;
        char* np = new char[my_size + 1];
        if (my_size) std::memcpy(np, my_ptr, my_size);
        np[my_size] = '\0';
        delete[] my_ptr;
        my_ptr = np;
        my_capacity = my_size + 1;
    }

    // --- Присваивания ---

    void MyString::operator=(const char* SOURCE_STR) {
        if (!SOURCE_STR) {
            clear();
            return;
        }
        size_t len = std::strlen(SOURCE_STR);
        if (len + 1 > my_capacity) {
            delete[] my_ptr;
            my_capacity = std::max<size_t>(1, len + 1);
            my_ptr = new char[my_capacity];
        }
        if (len) std::memcpy(my_ptr, SOURCE_STR, len);
        my_ptr[len] = '\0';
        my_size = len;
    }

    void MyString::operator=(char const& cur_char) {
        if (my_capacity < 2) {
            delete[] my_ptr;
            my_capacity = 2;
            my_ptr = new char[my_capacity];
        }
        my_ptr[0] = cur_char;
        my_ptr[1] = '\0';
        my_size = 1;
    }

    void MyString::operator=(MyString& another) {
        if (this == &another) return;
        if (another.my_size + 1 > my_capacity) {
            delete[] my_ptr;
            my_capacity = another.my_size + 1;
            my_ptr = new char[my_capacity];
        }
        if (another.my_size) std::memcpy(my_ptr, another.my_ptr, another.my_size);
        my_ptr[another.my_size] = '\0';
        my_size = another.my_size;
    }

    // --- Getters ---

    size_t MyString::size() const { return my_size; }
    size_t MyString::capacity() const { return my_capacity; }
    const char* MyString::c_str() const { return my_ptr; }
    bool MyString::empty() const { return my_size == 0; }

    // --- Вставки по индексу ---

    void MyString::insert(size_t INDEX, const char* SOURCE_STR) {
        if (!SOURCE_STR) return;
        insert_internal(my_ptr, my_size, my_capacity, INDEX, SOURCE_STR, std::strlen(SOURCE_STR));
    }

    void MyString::insert(size_t INDEX, char const& cur_char) {
        insert_internal(my_ptr, my_size, my_capacity, INDEX, &cur_char, 1);
    }

    void MyString::insert(size_t INDEX, MyString& another) {
        insert_internal(my_ptr, my_size, my_capacity, INDEX, another.my_ptr, another.my_size);
    }

    void MyString::insert(size_t INDEX, size_t COUNT, char cur_char) {
        if (COUNT == 0) return;
        ensure_capacity(my_ptr, my_size, my_capacity, my_size + COUNT + 1);
        if (INDEX > my_size) return;
        shift_right(my_ptr, INDEX, my_size, COUNT);
        std::memset(my_ptr + INDEX, static_cast<unsigned char>(cur_char), COUNT);
        my_size += COUNT;
        my_ptr[my_size] = '\0';
    }

    void MyString::insert(size_t INDEX, const char* SOURCE_STR, size_t COUNT) {
        if (!SOURCE_STR || COUNT == 0) return;
        size_t len = std::min<size_t>(COUNT, std::strlen(SOURCE_STR));
        insert_internal(my_ptr, my_size, my_capacity, INDEX, SOURCE_STR, len);
    }

    void MyString::insert(size_t INDEX, const std::string& SOURCE_STR, size_t COUNT) {
        if (COUNT == 0) return;
        size_t len = std::min<size_t>(COUNT, SOURCE_STR.size());
        insert_internal(my_ptr, my_size, my_capacity, INDEX, SOURCE_STR.data(), len);
    }

    // --- Добавление в конец (append) ---

    void MyString::append(const char* SOURCE_STR) {
        if (!SOURCE_STR) return;
        append_internal(my_ptr, my_size, my_capacity, SOURCE_STR, std::strlen(SOURCE_STR));
    }

    void MyString::append(char const& cur_char) {
        append_internal(my_ptr, my_size, my_capacity, &cur_char, 1);
    }

    void MyString::append(MyString& another) {
        append_internal(my_ptr, my_size, my_capacity, another.my_ptr, another.my_size);
    }

    void MyString::append(size_t COUNT, char cur_char) {
        if (COUNT == 0) return;
        ensure_capacity(my_ptr, my_size, my_capacity, my_size + COUNT + 1);
        std::memset(my_ptr + my_size, static_cast<unsigned char>(cur_char), COUNT);
        my_size += COUNT;
        my_ptr[my_size] = '\0';
    }

    void MyString::append(const char* SOURCE_STR, size_t COUNT) {
        if (!SOURCE_STR || COUNT == 0) return;
        size_t len = std::min<size_t>(COUNT, std::strlen(SOURCE_STR));
        append_internal(my_ptr, my_size, my_capacity, SOURCE_STR, len);
    }

    void MyString::append(const std::string& SOURCE_STR, size_t COUNT) {
        if (COUNT == 0) return;
        size_t len = std::min<size_t>(COUNT, SOURCE_STR.size());
        append_internal(my_ptr, my_size, my_capacity, SOURCE_STR.data(), len);
    }

    // --- Удаление ---

    void MyString::erase(size_t INDEX, size_t COUNT) {
        if (INDEX >= my_size || COUNT == 0) return;
        size_t actual = std::min<size_t>(COUNT, my_size - INDEX);
        shift_left(my_ptr, INDEX, my_size, actual);
        my_size -= actual;
        my_ptr[my_size] = '\0';
    }

    void MyString::erase(size_t INDEX) {
        if (INDEX >= my_size) return;
        shift_left(my_ptr, INDEX, my_size, 1);
        --my_size;
        my_ptr[my_size] = '\0';
    }

    // --- Замены ---

    void MyString::replace(size_t INDEX, size_t COUNT, const char* SOURCE_STR) {
        if (!SOURCE_STR) { erase(INDEX, COUNT); return; }
        replace_internal(my_ptr, my_size, my_capacity, INDEX, COUNT, SOURCE_STR, std::strlen(SOURCE_STR));
    }

    void MyString::replace(size_t INDEX, size_t COUNT, char const& cur_char) {
        if (INDEX > my_size) return;
        size_t actual = std::min<size_t>(COUNT, (INDEX <= my_size) ? (my_size - INDEX) : 0ULL);
        // заменяем COUNT на 1 символ cur_char
        replace_internal(my_ptr, my_size, my_capacity, INDEX, actual, &cur_char, 1);
    }

    void MyString::replace(size_t INDEX, size_t COUNT, MyString& another) {
        replace_internal(my_ptr, my_size, my_capacity, INDEX, COUNT, another.my_ptr, another.my_size);
    }

    void MyString::replace(size_t INDEX, size_t COUNT, size_t NEW_COUNT, char cur_char) {
        // заменить COUNT на NEW_COUNT копий cur_char
        if (INDEX > my_size) return;
        size_t actual = std::min<size_t>(COUNT, my_size - INDEX);
        if (NEW_COUNT == 0) { erase(INDEX, actual); return; }
        // создаём временный буфер NEW_COUNT одинаковых символов
        // (без выделения отдельного массива можно сделать двумя ступенями, но временный буфер проще)
        MyString tmp(NEW_COUNT, cur_char);
        replace_internal(my_ptr, my_size, my_capacity, INDEX, actual, tmp.my_ptr, tmp.my_size);
    }

    void MyString::replace(size_t INDEX, size_t COUNT, const char* SOURCE_STR, size_t NEW_COUNT) {
        if (!SOURCE_STR || NEW_COUNT == 0) { erase(INDEX, COUNT); return; }
        size_t len = std::min<size_t>(NEW_COUNT, std::strlen(SOURCE_STR));
        replace_internal(my_ptr, my_size, my_capacity, INDEX, COUNT, SOURCE_STR, len);
    }

    void MyString::replace(size_t INDEX, size_t COUNT, const std::string& SOURCE_STR, size_t NEW_COUNT) {
        if (NEW_COUNT == 0) { erase(INDEX, COUNT); return; }
        size_t len = std::min<size_t>(NEW_COUNT, SOURCE_STR.size());
        replace_internal(my_ptr, my_size, my_capacity, INDEX, COUNT, SOURCE_STR.data(), len);
    }

    // --- Извлечение подстроки ---

    MyString MyString::substr(size_t INDEX, size_t COUNT) {
        if (INDEX >= my_size || COUNT == 0) {
            // возвращаем пустую строку
            MyString empty;
            return empty;
        }

        size_t len = std::min<size_t>(COUNT, my_size - INDEX);
        MyString r;
        delete[] r.my_ptr;
        r.my_capacity = len + 1;
        r.my_ptr = new char[r.my_capacity];
        std::memcpy(r.my_ptr, my_ptr + INDEX, len);
        r.my_ptr[len] = '\0';
        r.my_size = len;
        return r;
    }

    MyString MyString::substr(size_t INDEX) {
        if (INDEX >= my_size) {
            MyString empty;
            return empty;
        }

        size_t len = my_size - INDEX;
        MyString r;
        delete[] r.my_ptr;
        r.my_capacity = len + 1;
        r.my_ptr = new char[r.my_capacity];
        std::memcpy(r.my_ptr, my_ptr + INDEX, len);
        r.my_ptr[len] = '\0';
        r.my_size = len;
        return r;
    }

    // --- Конкатенация (operator+) ---

    MyString MyString::operator+(const char* SOURCE_STR) {
        MyString r;
        if (!SOURCE_STR || SOURCE_STR[0] == '\0') {
            // просто копируем текущее содержимое
            delete[] r.my_ptr;
            r.my_capacity = my_size + 1;
            r.my_ptr = new char[r.my_capacity];
            if (my_size) std::memcpy(r.my_ptr, my_ptr, my_size);
            r.my_ptr[my_size] = '\0';
            r.my_size = my_size;
            return r;
        }

        size_t add = std::strlen(SOURCE_STR);
        delete[] r.my_ptr;
        r.my_capacity = my_size + add + 1;
        r.my_ptr = new char[r.my_capacity];
        if (my_size) std::memcpy(r.my_ptr, my_ptr, my_size);
        std::memcpy(r.my_ptr + my_size, SOURCE_STR, add);
        r.my_size = my_size + add;
        r.my_ptr[r.my_size] = '\0';
        return r;
    }

    MyString MyString::operator+(char const& cur_char) {
        MyString r;
        delete[] r.my_ptr;
        r.my_capacity = my_size + 2;
        r.my_ptr = new char[r.my_capacity];
        if (my_size) std::memcpy(r.my_ptr, my_ptr, my_size);
        r.my_ptr[my_size] = cur_char;
        r.my_ptr[my_size + 1] = '\0';
        r.my_size = my_size + 1;
        return r;
    }

    MyString MyString::operator+(MyString& another) {
        MyString r;
        delete[] r.my_ptr;

        r.my_capacity = my_size + another.my_size + 1;
        r.my_ptr = new char[r.my_capacity];
        if (my_size) std::memcpy(r.my_ptr, my_ptr, my_size);
        if (another.my_size) std::memcpy(r.my_ptr + my_size, another.my_ptr, another.my_size);
        r.my_size = my_size + another.my_size;
        r.my_ptr[r.my_size] = '\0';
        return r;
    }

    // --- Альтернативные методы объединения (plus) ---

    MyString MyString::plus(const char* SOURCE_STR, size_t COUNT) {
        MyString r;
        if (!SOURCE_STR || COUNT == 0) {
            delete[] r.my_ptr;
            r.my_capacity = my_size + 1;
            r.my_ptr = new char[r.my_capacity];
            if (my_size) std::memcpy(r.my_ptr, my_ptr, my_size);
            r.my_ptr[my_size] = '\0';
            r.my_size = my_size;
            return r;
        }

        size_t add = std::min<size_t>(COUNT, std::strlen(SOURCE_STR));
        delete[] r.my_ptr;
        r.my_capacity = my_size + add + 1;
        r.my_ptr = new char[r.my_capacity];
        if (my_size) std::memcpy(r.my_ptr, my_ptr, my_size);
        if (add) std::memcpy(r.my_ptr + my_size, SOURCE_STR, add);
        r.my_size = my_size + add;
        r.my_ptr[r.my_size] = '\0';
        return r;
    }

    MyString MyString::plus(const std::string& SOURCE_STR, size_t COUNT) {
        MyString r;
        if (COUNT == 0) {
            delete[] r.my_ptr;
            r.my_capacity = my_size + 1;
            r.my_ptr = new char[r.my_capacity];
            if (my_size) std::memcpy(r.my_ptr, my_ptr, my_size);
            r.my_ptr[my_size] = '\0';
            r.my_size = my_size;
            return r;
        }

        size_t add = std::min<size_t>(COUNT, SOURCE_STR.size());
        delete[] r.my_ptr;
        r.my_capacity = my_size + add + 1;
        r.my_ptr = new char[r.my_capacity];
        if (my_size) std::memcpy(r.my_ptr, my_ptr, my_size);
        if (add) std::memcpy(r.my_ptr + my_size, SOURCE_STR.data(), add);
        r.my_size = my_size + add;
        r.my_ptr[r.my_size] = '\0';
        return r;
    }

    MyString MyString::plus(size_t COUNT, char cur_char) {
        MyString r;
        if (COUNT == 0) {
            delete[] r.my_ptr;
            r.my_capacity = my_size + 1;
            r.my_ptr = new char[r.my_capacity];
            if (my_size) std::memcpy(r.my_ptr, my_ptr, my_size);
            r.my_ptr[my_size] = '\0';
            r.my_size = my_size;
            return r;
        }

        delete[] r.my_ptr;
        r.my_capacity = my_size + COUNT + 1;
        r.my_ptr = new char[r.my_capacity];
        if (my_size) std::memcpy(r.my_ptr, my_ptr, my_size);
        std::memset(r.my_ptr + my_size, static_cast<unsigned char>(cur_char), COUNT);
        r.my_size = my_size + COUNT;
        r.my_ptr[r.my_size] = '\0';
        return r;
    }

    // --- Расширение (operator+=) ---

    void MyString::operator+=(const char* SOURCE_STR) {
        if (!SOURCE_STR) return;
        append_internal(my_ptr, my_size, my_capacity, SOURCE_STR, std::strlen(SOURCE_STR));
    }

    void MyString::operator+=(char const& cur_char) {
        append_internal(my_ptr, my_size, my_capacity, &cur_char, 1);
    }

    void MyString::operator+=(MyString& another) {
        append_internal(my_ptr, my_size, my_capacity, another.my_ptr, another.my_size);
    }

    // --- Индексация ---

    char& MyString::operator[](size_t INDEX) {
        if (INDEX >= my_size) throw std::out_of_range("Index out of range");
        return my_ptr[INDEX];
    }

    const char& MyString::operator[](size_t INDEX) const {
        if (INDEX >= my_size) throw std::out_of_range("Index out of range");
        return my_ptr[INDEX];
    }

    // --- Сравнение и операторы отношений ---

    int MyString::compare(const char* SOURCE_STR) const {
        if (!SOURCE_STR) {
            // сравнение с пустой строкой
            return (my_size == 0) ? 0 : 1;
        }
        size_t rhs_len = std::strlen(SOURCE_STR);
        return cmp_lex(my_ptr, my_size, SOURCE_STR, rhs_len);
    }

    int MyString::compare(char const& cur_char) const {
        if (my_size == 0) return -1;      // "" < "X"
        if (my_size > 1) return 1;        // "ab" > 'a'
        if (my_ptr[0] == cur_char) return 0;
        return (static_cast<unsigned char>(my_ptr[0]) < static_cast<unsigned char>(cur_char)) ? -1 : 1;
    }

    int MyString::compare(MyString& another) const {
        return cmp_lex(my_ptr, my_size, another.my_ptr, another.my_size);
    }

    bool MyString::operator==(const char* SOURCE_STR) const { return compare(SOURCE_STR) == 0; }
    bool MyString::operator==(char const& cur_char) const { return compare(cur_char) == 0; }
    bool MyString::operator==(MyString& another) const { return compare(another) == 0; }

    bool MyString::operator!=(const char* SOURCE_STR) const { return !(*this == SOURCE_STR); }
    bool MyString::operator!=(char const& cur_char) const { return !(*this == cur_char); }
    bool MyString::operator!=(MyString& another) const { return !(*this == another); }

    bool MyString::operator<(const char* SOURCE_STR) const { return compare(SOURCE_STR) < 0; }
    bool MyString::operator<(char const& cur_char) const { return compare(cur_char) < 0; }
    bool MyString::operator<(MyString& another) const { return compare(another) < 0; }

    bool MyString::operator<=(const char* SOURCE_STR) const { return compare(SOURCE_STR) <= 0; }
    bool MyString::operator<=(char const& cur_char) const { return compare(cur_char) <= 0; }
    bool MyString::operator<=(MyString& another) const { return compare(another) <= 0; }

    bool MyString::operator>(const char* SOURCE_STR) const { return compare(SOURCE_STR) > 0; }
    bool MyString::operator>(char const& cur_char) const { return compare(cur_char) > 0; }
    bool MyString::operator>(MyString& another) const { return compare(another) > 0; }

    bool MyString::operator>=(const char* SOURCE_STR) const { return compare(SOURCE_STR) >= 0; }
    bool MyString::operator>=(char const& cur_char) const { return compare(cur_char) >= 0; }
    bool MyString::operator>=(MyString& another) const { return compare(another) >= 0; }

    // --- Поиск ---

    int MyString::find(const char* SOURCE_STR, size_t START_INDEX) const {
        if (!SOURCE_STR) return -1;
        size_t pat_len = std::strlen(SOURCE_STR);
        return find_naive(my_ptr, my_size, SOURCE_STR, pat_len, START_INDEX);
    }

    int MyString::find(char const& cur_char, size_t START_INDEX) const {
        if (START_INDEX >= my_size) return -1;
        for (size_t i = START_INDEX; i < my_size; ++i) {
            if (my_ptr[i] == cur_char) return static_cast<int>(i);
        }
        return -1;
    }

    int MyString::find(MyString& another, size_t START_INDEX) const {
        return find_naive(my_ptr, my_size, another.my_ptr, another.my_size, START_INDEX);
    }

} // namespace my_string_std
