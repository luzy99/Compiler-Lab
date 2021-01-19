# Lexical Analyzer
> c++实现，词法分析器

## 编译环境

vs2015

## 目录结构

```sh
D:.
│  trans.csv	# 手动推导出的状态转换表
│  transTable.xlsx
│
├─MyLex
  │
  ├─Debug
  │      example1.cpp	# 测试输入
  │      example2.cpp
  │      example3.cpp
  │      example4.cpp
  │      MyLex.exe	# 可执行程序
  │      output1.txt	# 测试输出
  │      output2.txt
  │      output3.txt
  │      output4.txt
  │
  └─MyLex
      │  lex.cpp	# 主程序
      └─  out.txt
```

## 运行方法

进入debug目录

```sh
cd ./MyLex/Debug/
```

`MyLex.exe`为可执行程序，启动方式如下：

```sh
# 参数为待分析源程序
.\MyLex.exe .\example1.cpp
# 若要输出到文件请使用管道命令
.\MyLex.exe .\example1.cpp > out.txt
```

