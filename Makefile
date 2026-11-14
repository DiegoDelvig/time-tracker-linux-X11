# ============================================================
#  timetracker : Makefile
#    make        compile le daemon  -> build/tracker
#    make test   compile et lance les tests sous valgrind
#    make run    compile puis lance le daemon
#    make clean  supprime build/
# ============================================================

# La cible par défaut est explicite : l'ordre des règles ne compte plus.
.DEFAULT_GOAL := all

# --- 1. Configuration -------------------------------------------------

CC      := gcc
CFLAGS  := -Wall -Wextra -Wpedantic -g -std=c11 \
           -D_POSIX_C_SOURCE=200809L -MMD -MP -Isrc
LDLIBS  := -lX11
BUILD   := build

# --- 2. Fichiers ------------------------------------------------------

SRCS  := $(wildcard src/*.c)
OBJS  := $(SRCS:src/%.c=$(BUILD)/%.o)
DEPS  := $(OBJS:.o=.d)

TESTS := $(BUILD)/test_stats \
         $(BUILD)/test_storage \
         $(BUILD)/test_paths

# --- 3. Programme -----------------------------------------------------

all: $(BUILD)/tracker

$(BUILD)/tracker: $(OBJS)
	$(CC) $^ -o $@ $(LDLIBS)

# Un .c de src/ -> un .o (régénéré si un header inclus change, grâce aux .d)
$(BUILD)/%.o: src/%.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD):
	mkdir -p $@

run: all
	./$(BUILD)/tracker

# --- 4. Tests ---------------------------------------------------------
# Règle commune : tests/test_X.c + les .o listés ci-dessous -> build/test_X
# Pour ajouter un test : créer tests/test_X.c, ajouter sa ligne de
# dépendances ici et son nom dans TESTS.

$(BUILD)/test_%: tests/test_%.c | $(BUILD)
	$(CC) $(CFLAGS) $^ -o $@

$(BUILD)/test_stats:   $(BUILD)/stats.o
$(BUILD)/test_storage: $(BUILD)/storage.o $(BUILD)/stats.o
$(BUILD)/test_paths:   $(BUILD)/paths.o

test: $(TESTS)
	@for t in $(TESTS); do \
		valgrind --leak-check=full --error-exitcode=1 -q ./$$t || exit 1; \
	done
	@echo "Tous les tests passent."

# --- 5. Divers --------------------------------------------------------

clean:
	rm -rf $(BUILD)

# Dépendances générées par gcc (-MMD) : doit rester en fin de fichier.
-include $(DEPS)

.PHONY: all run test clean
