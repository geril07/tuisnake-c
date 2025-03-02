builddir=build
target=main

setup:
	meson setup $(builddir)

compile:
	meson compile -C $(builddir)

run:
	./$(builddir)/$(target)

compile-run: 
	$(MAKE) compile
	$(MAKE) run

cleanup:
	rm -rf $(builddir) .cache log.txt
