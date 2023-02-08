#Use GNU compiler
CC = g++ -g

all:producer consumer 

producer.o: producer.cc
	$(CC) -c producer.cc

producer: producer.o
	$(CC) -o producer producer.o	

consumer.o: consumer.cc
	$(CC) -c consumer.cc

consumer: consumer.o
	$(CC) -o consumer consumer.o	

clean:
	rm -f producer *.o consumer *.o
	
