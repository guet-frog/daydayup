test: clouder1.o clouder2.o clouder3.o clouder4.o
	gcc -o test clouder1.o clouder2.o clouder3.o clouder4.o
	
clouder1.o: src/clouder1.c
	gcc -c src/clouder1.c
clouder2.o:src/clouder2.c
	gcc -c src/clouder2.c
clouder3.o:src/clouder3.c
	gcc -c src/clouder3.c
clouder4.o:src/clouder4.c
	gcc -c src/clouder4.c
	
clean:
	rm -f test; rm -f clouder*.o;


#注意编译顺序
#gcc -c src/clouder1.c
#gcc -c src/clouder2.c
#gcc -c src/clouder3.c
#gcc -c src/clouder4.c
#gcc -o test clouder1.o clouder2.o clouder3.o clouder4.o

#执行OK
#clouder4.o:src/clouder4.c
#clouder4.o:
#	gcc -c src/clouder4.c

#执行error
#clouder4.o:src/clouder4.c
#clouder4.o:clouder4.c			#找不到依赖clouder4.c, 将clouder4.c作为target -> no rule to make target
#	gcc -c src/clouder4.c
#make: *** No rule to make target `clouder4.c', needed by `clouder4.o'.  Stop.

#执行error
#clouder4.o:src/clouder4.c
#	gcc -c src/clouder4.c
#make: *** No rule to make target `clouder4.o', needed by `test'.  Stop.