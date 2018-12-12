.PHONY: final

final:
	mkdir Build
	cd Build; cmake ..; make
	cp Build/final/final ./final
