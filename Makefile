.PHONY: final

final:
	mkdir -p Build
	cd Build; cmake -DCMAKE_BUILD_TYPE=Debug ..; make
	cp Build/final/final ./final
