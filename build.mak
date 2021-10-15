SCRIPT_DIRECTORY := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

.PHONY: build
build:
	cd "$(SCRIPT_DIRECTORY)src" && make -f build.mak build