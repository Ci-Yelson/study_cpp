# objdump 使用指南

objdump 是一个用于显示目标文件信息的工具。以下是一些常用的命令选项:

## 基本用法

```bash
objdump [选项] <目标文件>
```

## 常用选项

- `-d, --disassemble`: 反汇编代码段
- `-S, --source`: 同时显示源代码（需要有调试信息 -g）
- `-t, --syms`: 显示符号表
- `-C, --demangle`: 将低级符号名解码为用户级名称
- `-r, --reloc`: 显示文件的重定位条目
- `-h, --headers`: 显示目标文件各个段的头部摘要信息
- `-x, --all-headers`: 显示所有可用的头信息
- `-D, --disassemble-all`: 反汇编所有段

## 实际示例

### 1. 查看虚函数表

编译带调试信息的程序：

```bash
g++ vtable.cc -o vtable -g
```

反汇编并查看源码：

```bash
objdump -C -S vtable
```

### 2. 查看符号表

```bash
objdump -t vtable
```

### 3. 查看段信息

```bash
objdump -h vtable
```

## 分析输出

### 虚函数表分析

使用 `objdump -C -S` 时，你可能会看到类似这样的输出：

```cpp
vtable for Base01:
    .quad 0x0
    .quad Base01::func01()
    .quad Base01::cool01()
```

这表示：
- 第一个 quad 通常是 type_info 指针
- 后续的 quad 是虚函数指针

### 符号表分析

使用 `objdump -t` 时，输出格式为：

```
SYMBOL TABLE:
地址        标志  段    大小   符号名
```

常见标志含义：
- `g`: 全局符号
- `l`: 本地符号
- `F`: 函数符号
- `O`: 目标文件符号

## 高级用法

### 1. 仅反汇编特定函数

```bash
objdump -C -d vtable | grep -A20 "<Base01::func01()>"
```

### 2. 显示所有段的完整内容

```bash
objdump -s vtable
```

### 3. 显示动态符号表

```bash
objdump -T vtable
```

## 调试技巧

1. 结合 GDB 使用：
   - 使用 objdump 查看汇编代码
   - 在 GDB 中设置断点
   - 使用 GDB 的 `disas` 命令验证

2. 分析虚函数调用：
   - 使用 `-C -S` 查看虚函数表布局
   - 观察 vtable 指针的偏移量
   - 分析虚函数调用的汇编代码

3. 内存布局分析：
   - 使用 `-h` 查看段的大小和位置
   - 分析类的内存布局
   - 确认虚函数表的位置

## 注意事项

1. 确保编译时使用 `-g` 选项以包含调试信息
2. C++ 代码需要使用 `-C` 选项来正确显示符号名
3. 大型文件的反汇编可能需要较长时间
4. 建议使用管道和 grep 来过滤需要的信息

这个指南涵盖了：
1. objdump 的基本用法
2. 常用选项的详细说明
3. 实际示例和分析方法
4. 高级用法和调试技巧
5. 注意事项

你可以根据这个指南来学习如何使用 objdump 分析程序，特别是在研究虚函数表和类的内存布局时非常有用。
