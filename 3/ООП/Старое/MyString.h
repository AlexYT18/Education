#ifndef MYSTRING_H
#define MYSTRING_H

#include <iostream>
#include <cstring>
#include <algorithm>
#include <stdexcept>
#include <string>

namespace my_string_std
{
    class MyString
    {
    private:
        char* my_ptr;
        size_t my_size;
        size_t my_capacity;

    public:
        // --- Конструкторы и деструктор ---
        MyString();
        MyString(const char* SOURCE_STR);
        MyString(const char* SOURCE_STR, size_t COUNT);
        MyString(const std::initializer_list<char>& LIST);
        MyString(const std::string& SOURCE_STR);
        MyString(size_t COUNT, char cur_char);
        MyString(MyString& another);
        ~MyString();

        // --- Методы управления памятью ---
        void clear();
        void shrink_to_fit();

        // --- Операторы присваивания ---
        void operator=(const char* SOURCE_STR);
        void operator=(char const& cur_char);
        void operator=(MyString& another);

        // --- Методы получения свойств ---
        size_t size() const;
        size_t capacity() const;
        const char* c_str() const;
        bool empty() const;

        // --- Вставки ---
        void insert(size_t INDEX, const char* SOURCE_STR);
        void insert(size_t INDEX, char const& cur_char);
        void insert(size_t INDEX, MyString& another);
        void insert(size_t INDEX, size_t COUNT, char cur_char);
        void insert(size_t INDEX, const char* SOURCE_STR, size_t COUNT);
        void insert(size_t INDEX, const std::string& SOURCE_STR, size_t COUNT);

        // --- Добавления (append) ---
        void append(const char* SOURCE_STR);
        void append(char const& cur_char);
        void append(MyString& another);
        void append(size_t COUNT, char cur_char);
        void append(const char* SOURCE_STR, size_t COUNT);
        void append(const std::string& SOURCE_STR, size_t COUNT);

        // --- Удаления ---
        void erase(size_t INDEX, size_t COUNT);
        void erase(size_t INDEX);

        // --- Замены ---
        void replace(size_t INDEX, size_t COUNT, const char* SOURCE_STR);
        void replace(size_t INDEX, size_t COUNT, char const& cur_char);
        void replace(size_t INDEX, size_t COUNT, MyString& another);
        void replace(size_t INDEX, size_t COUNT, size_t NEW_COUNT, char cur_char);
        void replace(size_t INDEX, size_t COUNT, const char* SOURCE_STR, size_t NEW_COUNT);
        void replace(size_t INDEX, size_t COUNT, const std::string& SOURCE_STR, size_t NEW_COUNT);

        // --- Извлечение ---
        MyString substr(size_t INDEX, size_t COUNT);
        MyString substr(size_t INDEX);

        // --- Конкатенация ---
        MyString operator+(const char* SOURCE_STR);
        MyString operator+(char const& cur_char);
        MyString operator+(MyString& another);

        // --- Расширение ---
        void operator+=(const char* SOURCE_STR);
        void operator+=(char const& cur_char);
        void operator+=(MyString& another);

        // --- Альтернативные методы объединения с ограничением ---
        MyString plus(const char* SOURCE_STR, size_t COUNT);
        MyString plus(const std::string& SOURCE_STR, size_t COUNT);
        MyString plus(size_t COUNT, char cur_char);

        // --- Индексация ---
        char& operator[](size_t INDEX);
        const char& operator[](size_t INDEX) const;

        // --- Сравнение ---
        int compare(const char* SOURCE_STR) const;
        int compare(char const& cur_char) const;
        int compare(MyString& another) const;

        bool operator==(const char* SOURCE_STR) const;
        bool operator==(char const& cur_char) const;
        bool operator==(MyString& another) const;

        bool operator!=(const char* SOURCE_STR) const;
        bool operator!=(char const& cur_char) const;
        bool operator!=(MyString& another) const;

        bool operator<(const char* SOURCE_STR) const;
        bool operator<(char const& cur_char) const;
        bool operator<(MyString& another) const;

        bool operator<=(const char* SOURCE_STR) const;
        bool operator<=(char const& cur_char) const;
        bool operator<=(MyString& another) const;

        bool operator>(const char* SOURCE_STR) const;
        bool operator>(char const& cur_char) const;
        bool operator>(MyString& another) const;

        bool operator>=(const char* SOURCE_STR) const;
        bool operator>=(char const& cur_char) const;
        bool operator>=(MyString& another) const;

        // --- Поиск ---
        int find(const char* SOURCE_STR, size_t START_INDEX = 0) const;
        int find(char const& cur_char, size_t START_INDEX = 0) const;
        int find(MyString& another, size_t START_INDEX = 0) const;
    };
}

#endif // MYSTRING_H
