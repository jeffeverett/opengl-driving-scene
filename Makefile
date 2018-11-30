.PHONY: final

final:
	git submodule update --init --recursive
	mkdir Build
	cd Build; cmake ..; make
	cp Build/final ./final
