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

build-libcld:
	g++ -c $(CLD_CFLAGS) $(CLD_SOURCES)
	$(AR) rcs libcld.a *.o
	rm *.o