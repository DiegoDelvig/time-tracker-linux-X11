CC      = gcc
CFLAGS  = -Wall -Wextra -Wpedantic -g -std=c11 -MMD -MP -Isrc
BUILD   = build

SRCS := $(wildcard src/*.c)
OBJS := $(SRCS:src/%.c=$(BUILD)/%.o)
DEPS := $(OBJS:.o=.d)

all: $(BUILD)/tracker

$(BUILD)/tracker: $(OBJS)
	$(CC) $^ -o $@

$(BUILD)/%.o: src/%.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD):
	mkdir -p $(BUILD)

clean:
	rm -rf $(BUILD)

-include $(DEPS)

.PHONY: all clean
