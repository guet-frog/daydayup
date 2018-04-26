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


#gcc -c src/clouder1.c
#gcc -c src/clouder2.c
#gcc -c src/clouder3.c
#gcc -c src/clouder4.c
#gcc -o test clouder1.o clouder2.o clouder3.o clouder4.o