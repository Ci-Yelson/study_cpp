# GDB 调试指南

GDB (GNU Debugger) 是一个强大的程序调试工具。本指南将介绍 GDB 的基本用法和高级特性。

## 基本用法

### 启动 GDB

```bash
# 启动 GDB 并加载程序
gdb ./vtable

# 启动时附加到正在运行的进程
gdb -p <pid>

# 启动并加载核心转储文件
gdb ./vtable core
```

### 运行程序

```bash
# 运行程序
(gdb) run
# 简写形式
(gdb) r

# 运行程序并在 main 函数处停止
(gdb) start

# 带参数运行
(gdb) run arg1 arg2

# 重新运行
(gdb) run
```

### 程序启动和入口点

```bash
# 运行程序并在 main 函数处停止
(gdb) start

# 在程序入口点设置断点（比 main 更早）
(gdb) b _start

# 在 main 函数设置断点
(gdb) b main

# 运行到下一个断点
(gdb) continue
```

## 断点操作

### 设置断点

```bash
# 在函数处设置断点
(gdb) break Base01::func01
(gdb) b Base01::func01

# 在行号处设置断点
(gdb) break vtable.cc:10

# 在条件满足时断点
(gdb) break vtable.cc:10 if i == 100

# 在地址处设置断点
(gdb) break *0x400123
```

### 管理断点

```bash
# 列出所有断点
(gdb) info breakpoints
(gdb) info break

# 删除断点
(gdb) delete 1    # 删除断点号为1的断点
(gdb) clear       # 删除当前行的断点
(gdb) delete      # 删除所有断点

# 禁用/启用断点
(gdb) disable 1
(gdb) enable 1
```

## 程序执行控制

```bash
# 单步执行（进入函数）
(gdb) step
(gdb) s

# 单步执行（不进入函数）
(gdb) next
(gdb) n

# 继续执行直到当前函数返回
(gdb) finish

# 继续执行
(gdb) continue
(gdb) c
```

## 查看程序状态

### 查看变量和内存

```bash
# 打印变量值
(gdb) print var_name
(gdb) p var_name

# 以特定格式打印
(gdb) p/x var_name    # 十六进制
(gdb) p/t var_name    # 二进制
(gdb) p/c var_name    # 字符

# 查看内存内容
(gdb) x/nfu addr      # n:数量 f:格式 u:单位
(gdb) x/10xb 0x400123 # 查看10个字节，以十六进制显示
```

### 查看调用栈

```bash
# 显示调用栈
(gdb) backtrace
(gdb) bt

# 切换栈帧
(gdb) frame n
(gdb) f n

# 查看当前栈帧的局部变量
(gdb) info locals
```

## 监视点

```bash
# 设置监视点（变量被写入时停止）
(gdb) watch var_name

# 设置读监视点（变量被读取时停止）
(gdb) rwatch var_name

# 设置读写监视点
(gdb) awatch var_name
```

## 多线程调试

```bash
# 查看所有线程
(gdb) info threads

# 切换到指定线程
(gdb) thread n

# 在所有线程设置断点
(gdb) break file:line thread all
```

## 调试 C++ 特性

### 虚函数和对象

```bash
# 打印对象的虚函数表
(gdb) p *((void ***)obj_ptr)

# 查看完整的对象布局
(gdb) ptype /o obj

# 查看虚函数表内容
(gdb) x/10a *((void ***)obj_ptr)
```

## 常用技巧

### 1. 自动显示变量值

```bash
# 添加自动显示
(gdb) display var_name

# 查看自动显示列表
(gdb) info display

# 删除自动显示
(gdb) undisplay num
```

### 2. 保存断点

```bash
# 保存断点到文件
(gdb) save breakpoints file.txt

# 从文件加载断点
(gdb) source file.txt
```

### 3. 条件断点和命令

```bash
# 设置条件断点
(gdb) break foo if count > 10

# 断点触发时执行命令
(gdb) commands 1
> print var_name
> continue
> end
```

## 调试实例

### 调试虚函数调用

```bash
# 编译带调试信息的程序
g++ -g vtable.cc -o vtable

# 启动 GDB
gdb ./vtable

# 在虚函数处设置断点
(gdb) b Base01::func01

# 运行程序
(gdb) run

# 查看虚函数表和对象布局
(gdb) ptype /o Base01
```

## 注意事项

1. 确保程序编译时使用 `-g` 选项包含调试信息
2. 使用 `-O0` 关闭优化以获得更好的调试体验
3. 对于 C++ 程序，使用 `-fno-inline` 可以更容易地调试内联函数
4. 记得在调试完成后删除或禁用断点
5. 使用 `set print pretty on` 可以更好地显示复杂数据结构

## 常见问题解决

1. 找不到源代码
   ```bash
   (gdb) directory /path/to/source
   ```

2. 符号被优化
   ```bash
   # 重新编译时使用
   g++ -g -O0 vtable.cc -o vtable
   ```

3. 调试信息不完整
   ```bash
   # 使用 -g3 获取更多调试信息
   g++ -g3 vtable.cc -o vtable
   ```

## 界面操作

### TUI 模式（文本用户界面）

```bash
# 进入 TUI 模式
(gdb) layout

# 显示源代码窗口
(gdb) layout src

# 显示汇编窗口
(gdb) layout asm

# 同时显示源代码和汇编窗口
(gdb) layout split

# 显示寄存器窗口
(gdb) layout regs

# 切换窗口焦点
(gdb) focus next
(gdb) focus prev

# 刷新界面（当显示混乱时）
(gdb) refresh

# 退出 TUI 模式
(gdb) tui disable
```

### TUI 快捷键

```bash
# Ctrl + X + A: 切换 TUI 模式
# Ctrl + X + 1: 显示一个窗口
# Ctrl + X + 2: 显示两个窗口
# Ctrl + L: 刷新屏幕
# Ctrl + P/N: 上/下一条命令历史
```

### 窗口调整

```bash
# 改变活动窗口大小
(gdb) winheight <win_name> [+ | -]count

# 示例：增加源代码窗口高度
(gdb) winheight src +5

# 示例：减少寄存器窗口高度
(gdb) winheight regs -3
``` 