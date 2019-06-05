sock : main.o networkAssistant.o
	gcc -Wall -lm -o $@ main.o networkAssistant.o -lwebsockets

main.o: main.c
	gcc -Wall -std=gnu99 -g -c $^

networkAssistant.o : networkAssistant.c
	gcc -Wall -std=gnu99 -g -c $^


clean:
	rm -r *.o
	rm -r sock
