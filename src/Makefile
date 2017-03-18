CRGREP_SOURCES=main.cpp record.cpp ranking.cpp utils.cpp search.cpp result.cpp formatter.cpp

CLD_LIB_CFLAGS=-DCLD_WINDOWS -I./cld -L. -lcld -lstdc++

CLD_CFLAGS=-fPIC -I./cld -O2 -DCLD_WINDOWS
LDFLAGS=-L.
CLD_AR=ar
CLD_SOURCES=./cld/encodings/compact_lang_det/cldutil.cc \
        ./cld/encodings/compact_lang_det/cldutil_dbg_empty.cc \
        ./cld/encodings/compact_lang_det/compact_lang_det.cc \
        ./cld/encodings/compact_lang_det/compact_lang_det_impl.cc \
        ./cld/encodings/compact_lang_det/ext_lang_enc.cc \
        ./cld/encodings/compact_lang_det/getonescriptspan.cc \
        ./cld/encodings/compact_lang_det/letterscript_enum.cc \
        ./cld/encodings/compact_lang_det/tote.cc \
        ./cld/encodings/compact_lang_det/generated/cld_generated_score_quadchrome_0406.cc \
        ./cld/encodings/compact_lang_det/generated/compact_lang_det_generated_cjkbis_0.cc \
        ./cld/encodings/compact_lang_det/generated/compact_lang_det_generated_ctjkvz.cc \
        ./cld/encodings/compact_lang_det/generated/compact_lang_det_generated_deltaoctachrome.cc \
        ./cld/encodings/compact_lang_det/generated/compact_lang_det_generated_quadschrome.cc \
        ./cld/encodings/compact_lang_det/win/cld_htmlutils_windows.cc \
        ./cld/encodings/compact_lang_det/win/cld_unilib_windows.cc \
        ./cld/encodings/compact_lang_det/win/cld_utf8statetable.cc \
        ./cld/encodings/compact_lang_det/win/cld_utf8utils_windows.cc \
        ./cld/encodings/internal/encodings.cc \
        ./cld/languages/internal/languages.cc

all:
	g++ -std=c++11 -O3 -I./ $(CRGREP_SOURCES) $(CLD_LIB_CFLAGS)
	./a.out -q 書本 -p ../data/formattedData/

interactive:
	g++ -std=c++11 -O3 -I./ $(CRGREP_SOURCES) $(CLD_LIB_CFLAGS)
	./a.out -p ../data/formattedData/ -i

# run-all: build-libcld
# 	g++ -std=c++11 -I./ $(CRGREP_SOURCES) $(CLD_LIB_CFLAGS)
# 	./a.out "書本" "../data/formattedData/"

ascending-order:
	g++ -std=c++11 -I./ $(CRGREP_SOURCES) $(CLD_LIB_CFLAGS)
	./a.out -a -q "書本" -p "../data/formattedData/"

fuzzy:
	g++ -std=c++11 -I./ $(CRGREP_SOURCES) $(CLD_LIB_CFLAGS)
	./a.out -d 3 -p "../data/formattedData/" -i

compile:
	g++ -o crgrepe -std=c++11 -I./ $(CRGREP_SOURCES) $(CLD_LIB_CFLAGS)

test:
	g++ -std=c++11 -I./ -O3 $(CRGREP_SOURCES) $(CLD_LIB_CFLAGS)
#	./a.out -f ../data/downloadedData/ -f ../data/formattedData/
	./a.out -q "書本" -p ../data/formattedData/testing.txt -i

format:
	g++ -std=c++11 -O3 -I./ $(CRGREP_SOURCES) $(CLD_LIB_CFLAGS)
#	./a.out -q "書本" -p ../data/formattedData/ -f ../data/downloadedData/天域蒼穹/1612-第一百五十六章_黑風手.txt -f  ../data/formattedData/ -f ../data/StopWords/stop-golden.txt
	./a.out -q "書本" -p ../data/formattedData/ -f ../data/downloadedData/ -f  ../data/formattedData/ -f ../data/StopWords/stop-golden.txt
#	./a.out -q "書本" -p "../data/formattedData/" -f "../data/downloadedData/" -f  "../data/formattedData/" -f "../data/dwlist.txt" -f "../data/mergedlist.txt" -f "../data/StopWords/stop-golden.txt"

debug:
	g++ -g -std=c++11 -I./ $(CRGREP_SOURCES) $(CLD_LIB_CFLAGS)
	./a.out -p ../data/formattedData/
clean:
	rm a.out
	rm *.o
	rm libcld.a

EXAMPLE_SRC_PATH=../example/custom-search-engine/src/

update-example-src:
	cp ranking.cpp $(EXAMPLE_SRC_PATH)ranking.cc
	cp ranking.h $(EXAMPLE_SRC_PATH)ranking.h

	cp result.cpp $(EXAMPLE_SRC_PATH)result.cc
	cp result.h $(EXAMPLE_SRC_PATH)result.h

	cp search.cpp $(EXAMPLE_SRC_PATH)search.cc
	cp search.h $(EXAMPLE_SRC_PATH)search.h

	cp utils.cpp $(EXAMPLE_SRC_PATH)utils.cc
	cp utils.h $(EXAMPLE_SRC_PATH)utils.h

	cp record.cpp $(EXAMPLE_SRC_PATH)record.cc
	cp record.h $(EXAMPLE_SRC_PATH)record.h

# build library cld (compact language detector)


build-libcld:
	g++ -c $(CLD_CFLAGS) $(CLD_SOURCES)
	$(AR) rcs libcld.a *.o
	rm *.o

compile-cld-file:
	g++ -DCLD_WINDOWS -I./cld -L. -o languageDetect languageDetect.cpp -lcld -lstdc++
