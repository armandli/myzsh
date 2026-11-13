# Thin wrapper around CMake. All real build logic lives in CMakeLists.txt.
BUILD_DIR  ?= build
BUILD_TYPE ?= Debug
CMAKE      ?= cmake
CTEST      ?= ctest
JOBS       ?= $(shell sysctl -n hw.ncpu 2>/dev/null || nproc 2>/dev/null || echo 4)
GENERATOR  ?= $(if $(shell command -v ninja 2>/dev/null),Ninja,Unix Makefiles)
SANITIZE   ?= OFF
CMAKE_ARGS ?=
# reference shell used by `make reference` to validate the integration cases
REFERENCE_ZSH ?= $(shell command -v zsh)

CONFIGURE_STAMP := $(BUILD_DIR)/CMakeCache.txt

.PHONY: all configure build release test unit integration reference run clean distclean help

all: build

$(CONFIGURE_STAMP):
	$(CMAKE) -S . -B $(BUILD_DIR) -G "$(GENERATOR)" \
	  -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) \
	  -DMYZSH_SANITIZE=$(SANITIZE) \
	  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
	  $(CMAKE_ARGS)

configure:
	$(CMAKE) -S . -B $(BUILD_DIR) -G "$(GENERATOR)" \
	  -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) \
	  -DMYZSH_SANITIZE=$(SANITIZE) \
	  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
	  $(CMAKE_ARGS)

build: $(CONFIGURE_STAMP)
	$(CMAKE) --build $(BUILD_DIR) -j $(JOBS)

release:
	$(MAKE) BUILD_DIR=build-release BUILD_TYPE=Release build

test: build
	$(CTEST) --test-dir $(BUILD_DIR) --output-on-failure -j $(JOBS)

# FILTER narrows to a gtest pattern, e.g. `make unit FILTER='Lexer*'`
FILTER ?= *
unit: build
	$(BUILD_DIR)/test/unit_tests --gtest_filter='$(FILTER)'

integration: build
	$(BUILD_DIR)/test/integration_tests --gtest_filter='$(FILTER)'

# Runs the integration cases against the real zsh to validate expectations.
reference: build
	MYZSH_BIN=$(REFERENCE_ZSH) $(BUILD_DIR)/test/integration_tests --gtest_filter='$(FILTER)'

run: build
	$(BUILD_DIR)/src/myzsh

clean:
	-$(CMAKE) --build $(BUILD_DIR) --target clean

distclean:
	rm -rf $(BUILD_DIR) build-release

help:
	@echo "targets: build release test unit integration reference run clean distclean"
	@echo "vars:    BUILD_TYPE=Debug|Release SANITIZE=ON FILTER='Suite.*' CMAKE_ARGS=..."
