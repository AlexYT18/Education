#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "MyString.h"

namespace py = pybind11;
using namespace my_string_std;

PYBIND11_MODULE(mystring, m) {
    m.doc() = "PyBind11 wrapper for MyString C++ class";

    py::class_<MyString>(m, "MyString")
        // --- Конструкторы ---
        .def(py::init<>())
        .def(py::init<const char*>(), py::arg("source"))
        .def(py::init<const char*, size_t>(), py::arg("source"), py::arg("count"))
        .def(py::init<const std::string&>(), py::arg("source"))
        .def(py::init<size_t, char>(), py::arg("count"), py::arg("char"))
        .def(py::init<MyString&>(), py::arg("other"))

        // --- Методы управления и свойства ---
        .def("clear", &MyString::clear)
        .def("shrink_to_fit", &MyString::shrink_to_fit)
        .def("size", &MyString::size)
        .def("capacity", &MyString::capacity)
        .def("empty", &MyString::empty)
        .def("c_str", [](const MyString& s) { return std::string(s.c_str()); })

        // --- Методы модификации ---
        .def("insert", py::overload_cast<size_t, const char*>(&my_string_std::MyString::insert),
            py::arg("index"), py::arg("text"))
        .def("append", py::overload_cast<const char*>(&my_string_std::MyString::append),
            py::arg("text"))
        .def("erase", py::overload_cast<size_t, size_t>(&my_string_std::MyString::erase),
            py::arg("index"), py::arg("count"))
        .def("replace", py::overload_cast<size_t, size_t, const char*>(&my_string_std::MyString::replace),
            py::arg("index"), py::arg("count"), py::arg("text"))
        .def("replace_char", py::overload_cast<size_t, size_t, char const&>(&my_string_std::MyString::replace),
            py::arg("index"), py::arg("count"), py::arg("char"))
        .def("replace_obj", py::overload_cast<size_t, size_t, MyString&>(&my_string_std::MyString::replace),
            py::arg("index"), py::arg("count"), py::arg("other"))

        // --- Извлечение подстрок ---
        .def("substr", py::overload_cast<size_t, size_t>(&my_string_std::MyString::substr),
            py::arg("index"), py::arg("count"))
        .def("substr_from", py::overload_cast<size_t>(&my_string_std::MyString::substr),
            py::arg("index"))

        // --- Методы объединения ---
        .def("plus", py::overload_cast<const char*, size_t>(&my_string_std::MyString::plus),
            py::arg("source"), py::arg("count"))
        .def("plus_str", py::overload_cast<const std::string&, size_t>(&my_string_std::MyString::plus),
            py::arg("source"), py::arg("count"))
        .def("plus_char", py::overload_cast<size_t, char>(&my_string_std::MyString::plus),
            py::arg("count"), py::arg("char"))

        // --- Поиск ---
        .def("find", py::overload_cast<const char*, size_t>(&my_string_std::MyString::find),
            py::arg("substr"), py::arg("start") = 0)
        .def("find_char", py::overload_cast<char const&, size_t>(&my_string_std::MyString::find),
            py::arg("char"), py::arg("start") = 0)
        .def("find_obj", py::overload_cast<MyString&, size_t>(&my_string_std::MyString::find),
            py::arg("other"), py::arg("start") = 0)

        // --- Python-интерфейс ---
        .def("__str__", [](const MyString& s) { return std::string(s.c_str()); })
        .def("__repr__", [](const MyString& s) {
        return "<MyString '" + std::string(s.c_str()) + "'>";
            })
        .def("__len__", &MyString::size)
        .def("__getitem__", [](const MyString& s, size_t i) {
        if (i >= s.size()) throw py::index_error("index out of range");
        return s.c_str()[i];
            })
        .def("__setitem__", [](MyString& s, size_t i, char c) {
        if (i >= s.size()) throw py::index_error("index out of range");
        s[i] = c;
            })
        .def("__add__", [](const MyString& a, const MyString& b) {
        MyString tmp(const_cast<MyString&>(a));
        tmp.append(const_cast<MyString&>(b));
        return tmp;
            })
        .def("__iadd__", [](MyString& a, const MyString& b) -> MyString& {
        a.append(const_cast<MyString&>(b));
        return a;
            })
        .def("__eq__", [](const MyString& a, const MyString& b) {
        return a.compare(const_cast<MyString&>(b)) == 0;
            })
        .def("__ne__", [](const MyString& a, const MyString& b) {
        return a.compare(const_cast<MyString&>(b)) != 0;
            });
}
