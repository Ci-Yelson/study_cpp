
all: pc_cv pc_sem join_sem philosophers pc_sem_20

CFLAGS := -O2

pc_cv: pc_cv.cpp
	g++ $(CFLAGS) -o $@.out $<

pc_sem: pc_sem.cpp
	g++ $(CFLAGS) -o $@.out $<
	# $@: 目标文件名
	# $<: 第一个依赖文件名

pc_sem_20: pc_sem_20.cpp
	g++-13 -std=c++20 $(CFLAGS) -o $@.out $<

join_sem: join_sem.cpp
	g++ $(CFLAGS) -o $@.out $<

philosophers: philosophers.cpp
	g++ $(CFLAGS) -o $@.out $<

clean:
	rm -f *.out
