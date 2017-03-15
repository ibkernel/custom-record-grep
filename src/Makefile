all:
	g++ -std=c++11 main.cpp record.cpp ranking.cpp book_formatter.cpp utils.cpp search.cpp result.cpp -I./
	./a.out "書本" "../data/formattedData/"

one:
	g++ -std=c++11 main.cpp record.cpp ranking.cpp book_formatter.cpp utils.cpp search.cpp -I./
	./a.out "書本" "../data/formattedData/全職法師.txt"

test:
	g++ -std=c++11 main.cpp record.cpp ranking.cpp book_formatter.cpp utils.cpp search.cpp -I./
	./a.out "書本" "../data/debugData/"

compile:
	g++ -std=c++11 main.cpp record.cpp ranking.cpp book_formatter.cpp utils.cpp search.cpp -I./

format:
	g++ -std=c++11 -O3 main.cpp record.cpp ranking.cpp book_formatter.cpp utils.cpp search.cpp -I./
	./a.out "書本" "../data/formattedData/" -f "../data/downloadedData/" -f  "../data/formattedData/" -f "../data/dwlist.txt" -f "../data/mergedlist.txt" -f "../data/StopWords/stop-golden.txt"

debug:
	g++ -g -std=c++11 main.cpp record.cpp ranking.cpp book_formatter.cpp utils.cpp search.cpp -I./
	gdb ./a.out
clean:
	rm a.out

