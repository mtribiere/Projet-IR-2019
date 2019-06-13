sock : main.o brain.o networkAssistant.o utilityFunctions.o
	gcc -Wall -lm -std=gnu99 -o $@  main.o brain.o networkAssistant.o utilityFunctions.o -lwebsockets

main.o: main.c
	gcc -Wall -std=gnu99 -g -c $^

networkAssistant.o : networkAssistant.c
	gcc -Wall -std=gnu99 -g -c $^

utilityFunctions.o : utilityFunctions.c
	gcc -Wall -std=gnu99 -g -c $^

brain.o: brain.c
	gcc -Wall -std=gnu99 -g -c $^

affiche: spectacle
	gnome-terminal -e "./spectacle -s -p 2001 192.168.130.150"

test: sock
	./sock -p 2001 -o agar.io 192.168.130.150

multi : sock
	gnome-terminal --tab-with-profile="sheep" -e "./sock -p 2001 -o agar.io 192.168.130.150"
	gnome-terminal --tab-with-profile="sheep" -e "./sock -p 2001 -o agar.io 192.168.130.150"
	gnome-terminal --tab-with-profile="sheep" -e "./sock -V -p 2001 -o agar.io 192.168.130.150"
	gnome-terminal --tab-with-profile="sheep" -e "./sock -V -p 2001 -o agar.io 192.168.130.150"
	gnome-terminal --tab-with-profile="sheep" -e "./sock -V -p 2001 -o agar.io 192.168.130.150"
	gnome-terminal --tab-with-profile="sheep" -e "./sock -V -p 2001 -o agar.io 192.168.130.150"

local: sock
	./sock -p 1443 -o agar.io 127.0.0.1

clean:
	rm -r *.o
	rm -r sock
