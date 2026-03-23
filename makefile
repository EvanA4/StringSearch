bmh:
	g++ src/bmh.cpp -o bin/bmh -Wextra -Wall

bmo:
	g++ src/bmo.cpp -o bin/bmo -Wextra -Wall

kmp:
	g++ src/kmp.cpp -o bin/kmp -Wextra -Wall

kra:
	g++ src/kra.cpp -o bin/kra -Wextra -Wall

dmb:
	g++ src/dmb.cpp -o bin/dmb -Wextra -Wall

tgen:
	g++ src/tgen.cpp -o bin/tgen -Wextra -Wall

times:
	g++ src/times.cpp -o bin/times -Wextra -Wall

clean:
	rm bin/*