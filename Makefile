#g++ test2.cpp myDB.cpp -g -o test2 -w `mysql_config --cflags --libs`
Cserver: test2.o myDB.o
	g++ test2.o myDB.o -o Cserver -w `mysql_config --cflags --libs`

test2.o: test2.cpp
	g++ -c test2.cpp -o test2.o -w

myDB.o: myDB.cpp
	g++ -c myDB.cpp -o myDB.o -w `mysql_config --cflags --libs`

.PHONY:
clean:
	rm myDB.o test2.o

 
