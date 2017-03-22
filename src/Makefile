CRGREP_SOURCES=main.cpp record.cpp ranking.cpp utils.cpp search.cpp result.cpp formatter.cpp

CLD_LIB_CFLAGS=-DCLD_WINDOWS -Ithird_party/cld -L. -lcld -lstdc++

TCLAP_CLFAGS=-Ithird_party/

CLD_CFLAGS=-fPIC -Ithird_party/cld -O2 -DCLD_WINDOWS
LDFLAGS=-L.
CLD_AR=ar
CLD_SOURCES=third_party/cld/encodings/compact_lang_det/cldutil.cc \
        third_party/cld/encodings/compact_lang_det/cldutil_dbg_empty.cc \
        third_party/cld/encodings/compact_lang_det/compact_lang_det.cc \
        third_party/cld/encodings/compact_lang_det/compact_lang_det_impl.cc \
        third_party/cld/encodings/compact_lang_det/ext_lang_enc.cc \
        third_party/cld/encodings/compact_lang_det/getonescriptspan.cc \
        third_party/cld/encodings/compact_lang_det/letterscript_enum.cc \
        third_party/cld/encodings/compact_lang_det/tote.cc \
        third_party/cld/encodings/compact_lang_det/generated/cld_generated_score_quadchrome_0406.cc \
        third_party/cld/encodings/compact_lang_det/generated/compact_lang_det_generated_cjkbis_0.cc \
        third_party/cld/encodings/compact_lang_det/generated/compact_lang_det_generated_ctjkvz.cc \
        third_party/cld/encodings/compact_lang_det/generated/compact_lang_det_generated_deltaoctachrome.cc \
        third_party/cld/encodings/compact_lang_det/generated/compact_lang_det_generated_quadschrome.cc \
        third_party/cld/encodings/compact_lang_det/win/cld_htmlutils_windows.cc \
        third_party/cld/encodings/compact_lang_det/win/cld_unilib_windows.cc \
        third_party/cld/encodings/compact_lang_det/win/cld_utf8statetable.cc \
        third_party/cld/encodings/compact_lang_det/win/cld_utf8utils_windows.cc \
        third_party/cld/encodings/internal/encodings.cc \
        third_party/cld/languages/internal/languages.cc

all:
	g++ -std=c++11 -O3 $(TCLAP_CLFAGS) $(CRGREP_SOURCES) $(CLD_LIB_CFLAGS)
	./a.out -q 書本 -q "+火球" -p ../data/formattedData/

interactive:
	g++ -std=c++11 -O3 $(TCLAP_CLFAGS) $(CRGREP_SOURCES) $(CLD_LIB_CFLAGS)
	./a.out -p ../data/formattedData/ -i

# run-all: build-libcld
# 	g++ -std=c++11 -I./ $(CRGREP_SOURCES) $(CLD_LIB_CFLAGS)
# 	./a.out "書本" "../data/formattedData/"

ascending-order:
	g++ -std=c++11 $(TCLAP_CLFAGS) $(CRGREP_SOURCES) $(CLD_LIB_CFLAGS)
	./a.out -a -q "書本" -p "../data/formattedData/"

fuzzy:
	g++ -std=c++11 $(TCLAP_CLFAGS) $(CRGREP_SOURCES) $(CLD_LIB_CFLAGS)
	./a.out -d 3 -p "../data/formattedData/" -i

compile:
	g++ -o crgrep -std=c++11 $(TCLAP_CLFAGS) $(CRGREP_SOURCES) $(CLD_LIB_CFLAGS)

test:
	g++ -std=c++11 $(TCLAP_CLFAGS) -O3 $(CRGREP_SOURCES) $(CLD_LIB_CFLAGS)

#	./cr1 -q "火球" -p ../data/formattedData/001.txt
#	./cr2 -q "火球" -p ../data/formattedData2/001.txt


format:
	g++ -std=c++11 -O3 $(TCLAP_CLFAGS) $(CRGREP_SOURCES) $(CLD_LIB_CFLAGS)
#	./a.out -q "書本" -p ../data/formattedData/ -f ../data/downloadedData/天域蒼穹/1612-第一百五十六章_黑風手.txt -f  ../data/formattedData/ -f ../data/StopWords/stop-golden.txt
	./a.out -f ../data/downloadedData/ -f  ../data/formattedData2/ -f ../data/StopWords/stop-golden.txt
#	./a.out -q "書本" -p "../data/formattedData/" -f "../data/downloadedData/" -f  "../data/formattedData/" -f "../data/dwlist.txt" -f "../data/mergedlist.txt" -f "../data/StopWords/stop-golden.txt"

debug:
	g++ -g -std=c++11 $(TCLAP_CLFAGS) $(CRGREP_SOURCES) $(CLD_LIB_CFLAGS)
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
