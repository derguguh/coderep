CC = clang

#
# Some backends or flags don't use some parameters
# Qualifiers in returns are more about helping with usage via definition than real usage
#
base_flags := -MMD -MP -I include/ -Wall -Wextra -Wno-unused-parameter -Wno-ignored-qualifiers
DEBUG_CFLAGS := $(base_flags) -DDEBUG -g -O0
RELEASE_CFLAGS := $(base_flags) -O2

CFLAGS ?= $(DEBUG_CFLAGS)
install_prefix ?= ~/.local/bin

debug_build_dir := build/debug
release_build_dir := build/release

build_dir ?= $(debug_build_dir)

sources_dir := src
include_dir := include
objects_dir := $(build_dir)/objects

headers := $(shell find $(include_dir)/ -name '*.h')
sources := $(shell find $(sources_dir)/ -name '*.c')
objects := $(addprefix $(objects_dir)/,$(sources:.c=.o))
dependencies := $(objects:.o=.d)
target := coderep
commands := compile_commands.json

.PHONY: all doall build debug release analyse bear clean install install-dev

all: $(build_dir)/$(target)

doall: clean bear release install analyse

build: debug release

debug:
	$(MAKE) CFLAGS="$(DEBUG_CFLAGS)" build_dir="$(debug_build_dir)"

release:
	$(MAKE) CFLAGS="$(RELEASE_CFLAGS)" build_dir="$(release_build_dir)"

# Change "text" to "c" later
analyse:
	$(build_dir)/$(target) -qc -l text -i $(sources) $(headers)

bear:
	bear -o build/$(commands) -- make debug

$(build_dir)/$(target): $(objects)
	$(CC) $(CFLAGS) $^ -o $@

$(objects_dir)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf build/*

install:
	mkdir -p $(install_prefix)
	cp $(release_build_dir)/$(target) $(install_prefix)/$(target)
	cp $(debug_build_dir)/$(target) $(install_prefix)/$(target)-debug

install-dev:
	mkdir -p $(install_prefix)
	cp $(release_build_dir)/$(target) $(install_prefix)/$(target)-dev
	cp $(debug_build_dir)/$(target) $(install_prefix)/$(target)-dev-debug

-include $(dependencies)

