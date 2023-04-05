# FINAL BINARY TARGET
./bin/DP-2 : ./obj/dataProducer-2.o
	cc ./obj/dataProducer-2.o -o ./bin/DP-2

# DEPENDENCIES
./obj/dataProducer-2.o : ./src/dataProducer-2.c ./inc/dataProducer-2.h
	cc -c ./src/dataProducer-2.c -o ./obj/dataProducer-2.o

# OTHER TARGETS
all : ./bin/DP-2

clean:
	rm -f ./bin/*
	rm -f ./obj/*.o
	
