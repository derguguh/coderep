CC = clang
CFLAGS = -MMD -MP -I include/ -g -Wall

build_dir := build/
sources_dir := src/
objects_dir := $(build_dir)objects/

sources := $(shell find $(sources_dir) -name '*.c')
objects := $(addprefix $(objects_dir),$(sources:.c=.o))
dependencies := $(objects:.o=.d)
target := coderep
commands := compile_commands.json

.PHONY: all bear clean

all: $(build_dir)$(target)

bear:
	bear -o $(build_dir)$(commands) -- make all

$(build_dir)$(target): $(objects)
	$(CC) $(CFLAGS) $(objects) -o $@

$(objects_dir)%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(build_dir)*

-include $(dependencies)

