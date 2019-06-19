sock : main.o brain-duel.o brain-2.o networkAssistant.o utilityFunctions.o
	gcc -Wall -lm -std=gnu99 -o $@  main.o brain-duel.o brain-2.o networkAssistant.o utilityFunctions.o -lwebsockets

main.o: main.c
	gcc -Wall -std=gnu99 -g -c $^

networkAssistant.o : networkAssistant.c
	gcc -Wall -std=gnu99 -g -c $^

utilityFunctions.o : utilityFunctions.c
	gcc -Wall -std=gnu99 -g -c $^

brain-duel.o: brain-duel.c
	gcc -Wall -std=gnu99 -g -c $^

brain-2.o : brain-2.c
	gcc -Wall -std=gnu99 -g -c $^

affiche: spectacle
	gnome-terminal -e "./spectacle -s -p 2001 192.168.130.151"

test: sock
	./sock -p 2002 -o agar.io 192.168.130.151

multi : sock
	gnome-terminal --tab-with-profile="sheep" -e "./sock -p 2001 -o agar.io 192.168.130.151"
	gnome-terminal --tab-with-profile="sheep" -e "./sock -p 2001 -o agar.io 192.168.130.151"
	gnome-terminal --tab-with-profile="sheep" -e "./sock -p 2001 -o agar.io 192.168.130.151"


duo : sock
	gnome-terminal --tab-with-profile="sheep" -e "./sock -p 2001 -o agar.io 192.168.130.151"
	gnome-terminal --tab-with-profile="sheep" -e "./sock -p 2001 -o agar.io 192.168.130.151"
	gnome-terminal --tab-with-profile="sheep" -e "./duo -s -p 2001 192.168.130.151"

local: sock
	./sock -V -p 1443 -o agar.io 127.0.0.1
	
affiche-local: spectacle
	./spectacle -s -p 1443 127.0.0.1

multi-local: sock
	./sock -p 1443 -o agar.io 127.0.0.1
	./sock -p 1443 -o agar.io 127.0.0.1
	./sock -p 1443 -o agar.io 127.0.0.1
	./sock -p 1443 -o agar.io 127.0.0.1
	./sock -V 1443 -o agar.io 127.0.0.1
	./sock -V 1443 -o agar.io 127.0.0.1

clean:
	rm -r *.o
	rm -r sock
