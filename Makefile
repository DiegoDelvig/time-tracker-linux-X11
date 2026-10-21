CC      = gcc
CFLAGS  = -Wall -Wextra -g -std=c11
BUILD   = build

all: $(BUILD)/tracker

$(BUILD)/tracker: src/main.c | $(BUILD)
	$(CC) $(CFLAGS) $< -o $@

$(BUILD):
	mkdir -p $(BUILD)

clean:
	rm -rf $(BUILD)

.PHONY: all clean
