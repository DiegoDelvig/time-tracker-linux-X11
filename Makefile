CC      = gcc
CFLAGS  = -Wall -Wextra -Wpedantic -g -std=c11 -MMD -MP -Isrc
LDLIBS  = -lX11
BUILD   = build

SRCS := $(wildcard src/*.c)
OBJS := $(SRCS:src/%.c=$(BUILD)/%.o)
DEPS := $(OBJS:.o=.d)

TESTS = $(BUILD)/test_stats $(BUILD)/test_storage

all: $(BUILD)/tracker

$(BUILD)/tracker: $(OBJS)
	$(CC) $^ -o $@ $(LDLIBS)

$(BUILD)/%.o: src/%.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD):
	mkdir -p $(BUILD)

# --- Tests : code hors de src/, un exécutable par module ---

$(BUILD)/test_stats: tests/test_stats.c $(BUILD)/stats.o | $(BUILD)
	$(CC) $(CFLAGS) $^ -o $@

$(BUILD)/test_storage: tests/test_storage.c $(BUILD)/storage.o $(BUILD)/stats.o | $(BUILD)
	$(CC) $(CFLAGS) $^ -o $@

test: $(TESTS)
	@for t in $(TESTS); do \
		valgrind --leak-check=full --error-exitcode=1 -q ./$$t || exit 1; \
	done
	@echo "Tous les tests passent."

clean:
	rm -rf $(BUILD)

-include $(DEPS)

.PHONY: all clean test
