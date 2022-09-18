.PHONY: all
all:
	maike2 --configfiles=maikeconfig.json --target-dir=__targets

clean:
	rm -rf __targets

DESTDIR?=""
PREFIX?="/usr"
.PHONY: install
install: make_pkgconfig.sh
	mkdir -p $(DESTDIR)$(PREFIX)/include/playback
	mkdir -p $(DESTDIR)$(PREFIX)/lib
	cp __targets/api/libpb_prod.a $(DESTDIR)$(PREFIX)/lib/libpb_prod.a
	find api -name '*.hpp' \
	    | while read in; do grep -v '^//@' "$$in" \
	    > $(DESTDIR)$(PREFIX)/include/playback/$$(basename $$in); done
	mkdir -p $(DESTDIR)$(PREFIX)/lib/pkgconfig
	./make_pkgconfig.sh $(PREFIX) $(DESTDIR)$(PREFIX)/lib/pkgconfig/pb_prod.pc
