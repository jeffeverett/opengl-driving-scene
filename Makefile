.PHONY: opengl-driving-scene

opengl-driving-scene:
	mkdir -p Build
	cd Build; cmake -DCMAKE_BUILD_TYPE=Debug ..; make
	cp Build/opengl-driving-scene/opengl-driving-scene ./opengl-driving-scene
