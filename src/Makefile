CRGREP_SOURCES=main.cpp record.cpp ranking.cpp utils.cpp search.cpp result.cpp formatter.cpp

CLD_LIB_CFLAGS=-DCLD_WINDOWS -Ithird_party/cld -L. -lcld -lstdc++

TCLAP_CLFAGS=-Ithird_party/

CRGREP_CLFLAGS=-std=c++11 -O3 -Wall

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
	g++ $(CRGREP_CLFLAGS) $(TCLAP_CLFAGS) $(CRGREP_SOURCES) $(CLD_LIB_CFLAGS)
#	./a.out -q 書本 -q "+火球" -p ../data/formattedData2/ -o 10

interactive:
	g++ $(CRGREP_CLFLAGS) -O3 $(TCLAP_CLFAGS) $(CRGREP_SOURCES) $(CLD_LIB_CFLAGS)
	./a.out -p ../data/testD/ -i

ascending-order:
#	g++ $(CRGREP_CLFLAGS) $(TCLAP_CLFAGS) $(CRGREP_SOURCES) $(CLD_LIB_CFLAGS)
	./a.out -a -q "書本" -p "../data/formattedData/"

fuzzy:
#	g++ $(CRGREP_CLFLAGS) $(TCLAP_CLFAGS) $(CRGREP_SOURCES) $(CLD_LIB_CFLAGS)
	./a.out -d 3 -p "../data/formattedData/" -i

compile:
	g++ $(CRGREP_CLFLAGS) $(TCLAP_CLFAGS) $(CRGREP_SOURCES) $(CLD_LIB_CFLAGS)

format:
	g++ $(CRGREP_CLFLAGS) $(TCLAP_CLFAGS) $(CRGREP_SOURCES) $(CLD_LIB_CFLAGS)
	# ./a.out -f ../../downloadedData/ -f  ../../formattedData/

debug:
	g++ -g $(CRGREP_CLFLAGS) $(TCLAP_CLFAGS) $(CRGREP_SOURCES) $(CLD_LIB_CFLAGS)
	gdb ./a.out


clean:
	rm a.out
	rm *.o
	rm libcld.a

EXAMPLE_SRC_PATH=../example/custom-search-engine/src/
EXAMPLE_SRC_PATH2=../example/blind-test/src/

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

	cp third_party_headers.h $(EXAMPLE_SRC_PATH)third_party_headers.h

	cp ranking.cpp $(EXAMPLE_SRC_PATH2)ranking.cc
	cp ranking.h $(EXAMPLE_SRC_PATH2)ranking.h

	cp result.cpp $(EXAMPLE_SRC_PATH2)result.cc
	cp result.h $(EXAMPLE_SRC_PATH2)result.h

	cp search.cpp $(EXAMPLE_SRC_PATH2)search.cc
	cp search.h $(EXAMPLE_SRC_PATH2)search.h

	cp utils.cpp $(EXAMPLE_SRC_PATH2)utils.cc
	cp utils.h $(EXAMPLE_SRC_PATH2)utils.h

	cp record.cpp $(EXAMPLE_SRC_PATH2)record.cc
	cp record.h $(EXAMPLE_SRC_PATH2)record.h

	cp third_party_headers.h $(EXAMPLE_SRC_PATH2)third_party_headers.h

# build library cld (compact language detector)
build-libcld:
	g++ -c $(CLD_CFLAGS) $(CLD_SOURCES)
	$(AR) rcs libcld.a *.o
	rm *.o
