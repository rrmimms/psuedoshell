EXEC=pseudo-shell
FLAGS=-Wall -ggdb3 -g -o


all: $(EXEC)

$(EXEC): main.o command.o string_parser.o
	gcc $(FLAGS) $@ $^	

main.o: main.c
	gcc -c $^

%.o: %.c %.h
	gcc -c $<

clean:
	rm -f *.o $(EXEC)
	rm -r test
	rm valOut.txt
