# CalculatorApp

一个用 **Qt 6 + C++17** 写的桌面计算器，纯代码布局（不使用 `.ui` 文件）。

- 顶部两块显示区：表达式（`labelExpression`）+ 结果（`labelResult`）
- 4×5 网格按钮：`0-9`、`.`、`+ - * /`、`=`、`C`、`CE`、退格、`+/-`
- 鼠标点击与键盘输入复用同一套槽函数（`handleDigit` / `handleDot` / `handleOperator` / ...）
- 键盘支持：`0-9`、`+ - * /`、`.`、`Enter`、`Backspace`、`Esc`
- 容错：连续小数点只留一个、连续运算符只替换、`=` 后再输入数字重新开始、
  支持连续运算（`2+3+4`）、除数为 0 显示“除数为0”且下次输入自动恢复

## 目录结构

```
CalculatorApp/
├─ CMakeLists.txt     # 构建配置（Qt6::Widgets + AUTOMOC）
├─ main.cpp           # 入口：QApplication + Calculator 窗口
├─ calculator.h       # Calculator 类声明（继承 QWidget）
├─ calculator.cpp     # 布局、样式表、键盘事件、运算逻辑
└─ .gitignore         # 忽略 build/、*.user、moc_* 等生成物
```

## 环境要求

- Qt **6.5 及以上**（开发环境为 Qt 6.9.2）
- 编译器：MinGW 64-bit 或 MSVC 2019/2022
- CMake ≥ 3.16（Qt Creator 自带，无需单独安装）

## 用 Qt Creator 打开并运行

1. 启动 Qt Creator，`文件 → 打开文件或项目…`（`Ctrl+O`）。
2. 选择本目录下的 **`CMakeLists.txt`**，点“打开”。
3. 首次打开会进入“配置项目”页：勾选一个桌面套件，例如
   `Desktop Qt 6.9.2 MinGW 64-bit`，点 **`Configure Project`**。
4. 点左下角绿色三角（或按 `Ctrl+R`）构建并运行；只想编译按 `Ctrl+B`。
5. 窗口出现后，**先用鼠标点一下窗口区域**让窗口获得键盘焦点，然后就能直接用键盘输入
   （按钮设置为 `NoFocus`，不会抢走焦点）。

> 提示：Qt Creator 默认把构建产物放在项目同级的 `build-CalculatorApp-<套件>-<配置>/` 目录，
> 该目录已被 `.gitignore` 忽略，不会混入源码。

## 命令行构建（可选）

```powershell
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release `
      -DCMAKE_PREFIX_PATH=D:\Qt\6.9.2\mingw_64
cmake --build build
```

生成的可执行文件为 `build\CalculatorApp.exe`。

## 键盘快捷键

| 按键 | 作用 |
|---|---|
| `0`–`9` | 输入数字 |
| `+` `-` `*` `/` | 加 / 减 / 乘 / 除（连续按只替换运算符） |
| `.`（或 `,`） | 小数点 |
| `Enter` / `Return` / `=` | 求值 |
| `Backspace` | 退格删除一位 |
| `Esc` | 全部清除（等同 `C` 按钮） |
| `Delete` | 清除当前输入（等同 `CE` 按钮） |

## 备注

- 源码保存为 **UTF-8 with BOM**；`CMakeLists.txt` 也为 MSVC 加了 `/utf-8`，
  这样“除数为0”等中文在两种编译器下都不会乱码。
- 打包分发时先构建 Release，再把 exe 拷到空目录并执行：
  ```powershell
  D:\Qt\6.9.2\mingw_64\bin\windeployqt.exe --release --compiler-runtime `
      --no-translations --no-opengl-sw .\CalculatorApp.exe
  ```
  部署后 `platforms\`、`styles\` 等子目录不能拍平，整个目录一起分发。
