all:
	g++ main.cpp load_data.cpp search.cpp -I./
	./a.out "書本" "../data/formattedData/test.txt"
#	./a.out "random query" "../data/dir/"
#	./a.out "random query" "tclap"
compile:
	g++ main.cpp -I./

debug:
	g++ -g main.cpp load_data.cpp search.cpp -I./
	./a.out "書本" "../data/formattedData/test.txt"
clean:
	rm a.out