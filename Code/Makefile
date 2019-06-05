sock : main.o networkAssistant.o utilityFunctions.o
	gcc -Wall -lm -o $@ main.o networkAssistant.o utilityFunctions.o -lwebsockets

main.o: main.c
	gcc -Wall -std=gnu99 -g -c $^

networkAssistant.o : networkAssistant.c
	gcc -Wall -std=gnu99 -g -c $^

utilityFunctions.o : utilityFunctions.c
		gcc -Wall -std=gnu99 -g -c $^

test: sock
	gnome-terminal -e "./sock -p 2001 -o agar.io 192.168.130.114"


clean:
	rm -r *.o
	rm -r sock
