# -*- coding: utf-8 -*-
from setuptools import setup, Extension
import pybind11
import sys
import os

# Определяем, под какой компилятор собираем
extra_compile_args = []
if os.name == "nt":  # Windows + MSVC
    extra_compile_args = ["/std:c++17", "/O2", "/EHsc"]
else:                # Linux / macOS
    extra_compile_args = ["-std=c++17", "-O3"]

ext_modules = [
    Extension(
        "mystring",  # Имя итогового Python-модуля (.pyd / .so)
        ["MyString.cpp", "pybind11_wrapper.cpp"],  # Исходники
        include_dirs=[
            pybind11.get_include(),
        ],
        language="c++",
        extra_compile_args=extra_compile_args,
    ),
]

setup(
    name="mystring",
    version="1.0",
    author="Alex Tsebro",
    description="Python bindings for MyString C++ class (pybind11 wrapper)",
    ext_modules=ext_modules,
)
