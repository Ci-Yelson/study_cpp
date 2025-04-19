
all: pc_cv pc_sem join_sem philosophers

CFLAGS := -O2

pc_cv: pc_cv.cpp
	g++ $(CFLAGS) -o $@.out $<

pc_sem: pc_sem.cpp
	g++ $(CFLAGS) -o $@.out $<
	# $@: 目标文件名
	# $<: 第一个依赖文件名

join_sem: join_sem.cpp
	g++ $(CFLAGS) -o $@.out $<

philosophers: philosophers.cpp
	g++ $(CFLAGS) -o $@.out $<

clean:
	rm -f *.out
