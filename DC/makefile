# FINAL BINARY TARGET
./bin/DC : ./obj/dataConsumer.o
	cc ./obj/dataConsumer.o -o ./bin/DC

# DEPENDENCIES
./obj/dataConsumer.o : ./src/dataConsumer.c ./inc/dataConsumer.h
	cc -c ./src/dataConsumer.c -o ./obj/dataConsumer.o

# OTHER TARGETS
all : ./bin/DC

clean:
	rm -f ./bin/*
	rm -f ./obj/*.o
	
