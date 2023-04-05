# FINAL BINARY TARGET
./bin/DP-1 : ./obj/dataProducer-1.o
	cc ./obj/dataProducer-1.o -o ./bin/DP-1

# DEPENDENCIES
./obj/dataProducer-1.o : ./src/dataProducer-1.c ./inc/dataProducer-1.h
	cc -c ./src/dataProducer-1.c -o ./obj/dataProducer-1.o

# OTHER TARGETS
all : ./bin/DP-1

clean:
	rm -f ./bin/*
	rm -f ./obj/*.o
	
