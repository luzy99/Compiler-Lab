# Syntax Parser

> c++实现，语法分析器

## 编译环境

vs2015

## 目录结构

```sh
D:.
└─SyntaxParser
    ├─Release
    │      input1.txt	# 测试输入
    │      input2.txt
    │      out1.txt		# 测试输出
    │      SyntaxParser.exe	# 可执行程序
    │
    └─SyntaxParser
        │  Edge.h
        │  Item.h
        │  parser.cpp	# 主程序
        │  Utils.h
```

## 运行方法

**进入release目录**

```sh
cd .\SyntaxParser\Release\
```

`SyntaxParser.exe`为可执行程序，可直接双击启动。

**分别输入**

> 终结符集，非终结符集，所有产生式
> （以上可直接复制粘贴 input.txt 中的内容，一次性输入）
>
> 示例如下：

```
请输入终结符集：
i + * ( )
请输入非终结符集：
S E T F
输入所有产生式（空字用‘@’表示）,以‘end’结束:
S->E
E->E + T
E->T
T->T * F
T->F
F->( E )
F->i
end
```

此时将显示LR(1)预测分析表，可继续输入token序列进行分析。