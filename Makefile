CFLAGS=-g -Wall -MMD -D_POSIX_C_SOURCE=200809L

SRCS=$(wildcard src/*.c)
OBJS=$(addprefix build/,$(addsuffix .o,$(basename $(SRCS))))
DEPS=$(addprefix build/,$(addsuffix .d,$(basename $(SRCS))))

.PHONY: all clean

all: aoc

aoc: $(OBJS)
	@mkdir -p $(dir $@)
	@echo "link and compile aoc"
	@clang $(CFLAGS) -o aoc $(OBJS)

build/src/%.o: src/%.c
	@mkdir -p $(dir $@)
	@echo "compile $<"
	@clang $(CFLAGS) -c $< -o $@

clean:
	rm -rf build

-include $(DEPS)
