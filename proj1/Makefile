EXEC=pseudo-shell
FLAGS=-Wall -g


all: $(EXEC)

$(EXEC): main.o command.o string_parser.o
	gcc $(FLAGS) -o $@ $^	

main.o: main.c
	gcc $(FLAGS) -c $^

%.o: %.c %.h		
	gcc $(FLAGS) -c $<

clean:
	rm -f *.o $(EXEC)
	rm -r output.txt
	rm -r test
	rm valOut.txt

test:
	bash test_script.sh

run:
	./pseudo-shell
