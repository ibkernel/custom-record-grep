all:
	g++ -std=c++11 main.cpp load_data.cpp search.cpp book_formatter.cpp -I./
	./a.out "書本" "../data/formattedData/"
#	./a.out "random query" "../data/dir/"
#	./a.out "random query" "tclap"
compile:
	g++ -std=c++11 main.cpp load_data.cpp search.cpp book_formatter.cpp -I./

format:
	g++ -std=c++11 main.cpp load_data.cpp search.cpp book_formatter.cpp -I./
	./a.out "書本" "../data/formattedData/" -f "../data/downloadedData/" -f  "../data/formattedData/" -f "../data/dwlist.txt" -f "../data/mergedlist.txt" -f "../data/StopWords/stop-golden.txt"
	

debug:
	g++ -g -std=c++11 main.cpp load_data.cpp search.cpp book_formatter.cpp -I./
	gdb ./a.out
clean:
	rm a.out


# /Users/wayne/Desktop/projects/crgrep/src/load_data.cpp:61