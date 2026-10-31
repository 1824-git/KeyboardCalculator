cmake_minimum_required(VERSION 3.16)

project(CalculatorApp VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

# Qt 6.5 及以上均可（本项目在 6.9.2 上开发，6.10 / 6.11 也能直接编译）
find_package(Qt6 6.5 REQUIRED COMPONENTS Widgets)

qt_standard_project_setup()

# 纯代码界面：只有主程序 + Calculator 类，没有任何 .ui 文件
qt_add_executable(CalculatorApp
    main.cpp
    calculator.h
    calculator.cpp
)

target_link_libraries(CalculatorApp PRIVATE Qt6::Widgets)

# Windows 下运行时不弹出控制台窗口
set_target_properties(CalculatorApp PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

# MSVC 需要 /utf-8 才能正确解析源文件里的中文（源文件同时保存为 UTF-8 with BOM）
if (MSVC)
    target_compile_options(CalculatorApp PRIVATE /utf-8)
endif()
