本程序的Makefile分为3类:
1. 顶层目录的Makefile
2. 顶层目录的Makefile.build
3. 各级子目录的Makefile

一、各级子目录的Makefile：
   它最简单，形式如下：

EXTRA_CFLAGS  := 
CFLAGS_file.o := 

obj-y += file.o
obj-y += subdir/
   
   "obj-y += file.o"  表示把当前目录下的file.c编进程序里，
   "obj-y += subdir/" 表示要进入subdir这个子目录下去寻找文件来编进程序里，是哪些文件由subdir目录下的Makefile决定。
   "EXTRA_CFLAGS",    它给当前目录下的所有文件(不含其下的子目录)设置额外的编译选项, 可以不设置
   "CFLAGS_xxx.o",    它给当前目录下的xxx.c设置它自己的编译选项, 可以不设置

注意: 
1. "subdir/"中的斜杠"/"不可省略
2. 顶层Makefile中的CFLAGS在编译任意一个.c文件时都会使用
3. CFLAGS  EXTRA_CFLAGS  CFLAGS_xxx.o 三者组成xxx.c的编译选项

二、顶层目录的Makefile：
   它除了定义obj-y来指定根目录下要编进程序去的文件、子目录外，
   主要是定义工具链前缀CROSS_COMPILE,
   定义编译参数CFLAGS,
   定义链接参数LDFLAGS,
   这些参数就是文件中用export导出的各变量。

三、顶层目录的Makefile.build：
   这是最复杂的部分，它的功能就是把某个目录及它的所有子目录中、需要编进程序去的文件都编译出来，打包为built-in.o
   详细的讲解请看视频。

四、怎么使用这套Makefile：
1．把顶层Makefile, Makefile.build放入程序的顶层目录
   在各自子目录创建一个空白的Makefile

2．确定编译哪些源文件
   修改顶层目录和各自子目录Makefile的obj-y : 
    obj-y += xxx.o
	obj-y += yyy/
	这表示要编译当前目录下的xxx.c, 要编译当前目录下的yyy子目录	

3. 确定编译选项、链接选项
   修改顶层目录Makefile的CFLAGS，这是编译所有.c文件时都要用的编译选项;
   修改顶层目录Makefile的LDFLAGS，这是链接最后的应用程序时的链接选项;
   
   修改各自子目录下的Makefile：
   "EXTRA_CFLAGS",    它给当前目录下的所有文件(不含其下的子目录)设置额外的编译选项, 可以不设置
   "CFLAGS_xxx.o",    它给当前目录下的xxx.c设置它自己的编译选项, 可以不设置
   
4. 使用哪个编译器？
   修改顶层目录Makefile的CROSS_COMPILE, 用来指定工具链的前缀(比如arm-linux-)
   
5. 确定应用程序的名字：
   修改顶层目录Makefile的TARGET, 这是用来指定编译出来的程序的名字

6. 执行"make"来编译，执行"make clean"来清除，执行"make distclean"来彻底清除
   
   