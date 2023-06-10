# CS143_cool_complier
FDU2023 complier course project that is required to implement a complier for cool(Classroom Object Oriented Language designed by MIT).

本项目沿用了CS143_Stanford的编译实践项目，为cool语言编写一个编译器。
cool主要具有如下特点：
    1.抽象
    2.静态类型
    3.重载（继承）
    4.自动化内存管理
原则上是一门面向对象的程序语言，该项目中选择了使用C++为其编写一个编译器。

实践过程主要包含四个阶段：词法分析，句法分析，句意检查，汇编代码生成。

cool语言的描述可以在handout中的cool手册中进行参考。
每个部分的具体要求在handouts中，实践过程中主要参照了这部分描述的功能进行设计。
每个部分的实现在assignments中，每个部分的工作量基本上呈倍增趋势，后两个部分的内容实在是太多，期末迫近可能实现得比较粗糙，但是具体的要求都已经基本实现了。

PA1主要是对cool语言的一个熟悉，简单地运用cool语言编写一个程序。

PA2是进行词法分析，利用正则表达式进行分词，并存储在符号表中。

PA3是进行语法分析，对上一部分的分词结果进行句法分析，利用bison编写生成规则并生成抽象语法树。

PA4是进行句意检查，遍历抽象语法树并对其做好标记，并且检查一些编译过程中可能出现的错误，

PA5是生成汇编代码，遍历抽象语法树并且转汇编。