SCRIPT_DIRECTORY := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
BIN_DIRECTORY := $(abspath $(SCRIPT_DIRECTORY)bin)/
SRC_DIRECTORY := $(abspath $(SCRIPT_DIRECTORY)src)/

.PHONY: build
build: \
	$(BIN_DIRECTORY) \
	$(BIN_DIRECTORY)image.js \
	$(BIN_DIRECTORY)index.html \
	$(BIN_DIRECTORY)default.png
	
$(BIN_DIRECTORY):
	mkdir "$(BIN_DIRECTORY)"

$(BIN_DIRECTORY)image.js: $(SRC_DIRECTORY)image.c $(SRC_DIRECTORY)wasm.c
	emcc -O3 --no-entry -s EXPORTED_RUNTIME_METHODS='["cwrap"]' -o "$(BIN_DIRECTORY)image.js" $(SRC_DIRECTORY)image.c $(SRC_DIRECTORY)wasm.c

$(BIN_DIRECTORY)index.html: $(SRC_DIRECTORY)index.html
	cp "$(SRC_DIRECTORY)index.html" "$(BIN_DIRECTORY)index.html"

$(BIN_DIRECTORY)default.png: $(SRC_DIRECTORY)default.png
	cp "$(SRC_DIRECTORY)default.png" "$(BIN_DIRECTORY)default.png"
