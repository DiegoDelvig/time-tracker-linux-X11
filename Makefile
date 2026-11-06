CC      = gcc
CFLAGS  = -Wall -Wextra -Wpedantic -g -std=c11 -MMD -MP -Isrc
BUILD   = build
LDLIBS  = -lX11

SRCS := $(wildcard src/*.c)
OBJS := $(SRCS:src/%.c=$(BUILD)/%.o)
DEPS := $(OBJS:.o=.d)

all: $(BUILD)/tracker

$(BUILD)/test_stats: tests/test_stats.c $(BUILD)/stats.o | $(BUILD)
	$(CC) $(CFLAGS) $^ -o $@

test: $(BUILD)/test_stats
	valgrind --leak-check=full --error-exitcode=1 ./$(BUILD)/test_stats

.PHONY: all clean test

$(BUILD)/tracker: $(OBJS)
	$(CC) $^ -o $@ $(LDLIBS)

$(BUILD)/%.o: src/%.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD):
	mkdir -p $(BUILD)

clean:
	rm -rf $(BUILD)

-include $(DEPS)

.PHONY: all clean
