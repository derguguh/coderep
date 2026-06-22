CC = clang

base_flags := -MMD -MP -I include/ -Wall -Wextra
DEBUG_CFLAGS := $(base_flags) -DDEBUG -g
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

.PHONY: all debug release analyse bear clean install

all: $(build_dir)/$(target)

debug:
	$(MAKE) CFLAGS="$(DEBUG_CFLAGS)" build_dir="$(debug_build_dir)"

release:
	$(MAKE) CFLAGS="$(RELEASE_CFLAGS)" build_dir="$(release_build_dir)"

# Change "general" to "c" later
analyse:
	$(build_dir)/$(target) --lang general --input $(sources) $(headers)

bear:
	bear -o build/$(commands) -- make all

$(build_dir)/$(target): $(objects)
	$(CC) $(CFLAGS) $(objects) -o $@

$(objects_dir)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf build/*

install:
	mkdir -p $(install_prefix)
	cp $(release_build_dir)/$(target) $(install_prefix)/$(target)

-include $(dependencies)

