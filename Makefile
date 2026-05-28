CC = clang
CFLAGS ?= -MMD -MP -I include/ -g -Wall
install_prefix ?= ~/.local/bin

build_dir := build
sources_dir := src
include_dir := include
objects_dir := $(build_dir)/objects

headers := $(shell find $(include_dir)/ -name '*.h')
sources := $(shell find $(sources_dir)/ -name '*.c')
objects := $(addprefix $(objects_dir)/,$(sources:.c=.o))
dependencies := $(objects:.o=.d)
target := coderep
commands := compile_commands.json

.PHONY: all analyse bear clean install

all: $(build_dir)/$(target)

# Change "general" to "c" later
analyse:
	$(build_dir)/$(target) --lang general --input $(sources) $(headers)

bear:
	bear -o $(build_dir)/$(commands) -- make all

$(build_dir)/$(target): $(objects)
	$(CC) $(CFLAGS) $(objects) -o $@

$(objects_dir)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(build_dir)/*

install:
	cp $(build_dir)/$(target) $(install_prefix)/$(target)

-include $(dependencies)

