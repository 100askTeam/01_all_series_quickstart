
objs := main.o sub.o

test : $(objs)
	gcc -o test $^


# 需要判断是否存在依赖文件
# .main.o.d .sub.o.d
dep_files := $(foreach f, $(objs), .$(f).d)
dep_files := $(wildcard $(dep_files))

# 把依赖文件包含进来
ifneq ($(dep_files),)
  include $(dep_files)
endif


%.o : %.c
	gcc -Wp,-MD,.$@.d  -c -o $@  $<
	

clean:
	rm *.o test -f

distclean:
	rm  $(dep_files) *.o test -f