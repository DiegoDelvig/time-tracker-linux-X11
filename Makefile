# ============================================================
#  timetracker : Makefile
#    make        compile le daemon (build/tracker) et le client (build/tracker-ui)
#    make test   compile et lance les tests sous valgrind
#    make run    compile puis lance le daemon au premier plan
#    make clean  supprime build/
# ============================================================

.DEFAULT_GOAL := all

# --- 1. Configuration -------------------------------------------------

CC      := gcc
CFLAGS  := -Wall -Wextra -Wpedantic -g -std=c11 \
           -D_POSIX_C_SOURCE=200809L -MMD -MP -Isrc
LDLIBS  := -lX11
BUILD   := build

# --- 2. Fichiers ------------------------------------------------------

SRCS := $(wildcard src/*.c)
OBJS := $(SRCS:src/%.c=$(BUILD)/%.o)
DEPS := $(OBJS:.o=.d)

# Chaque exécutable liste SES modules : un seul main par programme.
DAEMON_OBJS := $(addprefix $(BUILD)/, daemon_main.o paths.o proc_info.o \
               stats.o storage.o ticker.o tracker.o x11_probe.o)
UI_OBJS     := $(addprefix $(BUILD)/, ui_main.o paths.o stats.o \
               storage.o render.o)

TESTS := $(BUILD)/test_stats \
         $(BUILD)/test_storage \
         $(BUILD)/test_paths \
         $(BUILD)/test_render

# --- 3. Programmes ----------------------------------------------------

all: $(BUILD)/tracker $(BUILD)/tracker-ui

# Le daemon seul a besoin de X11.
$(BUILD)/tracker: $(DAEMON_OBJS)
	$(CC) $^ -o $@ $(LDLIBS)

# Le client n'a aucune dépendance X11.
$(BUILD)/tracker-ui: $(UI_OBJS)
	$(CC) $^ -o $@

# Un .c de src/ -> un .o (régénéré si un header inclus change, grâce aux .d)
$(BUILD)/%.o: src/%.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD):
	mkdir -p $@

run: $(BUILD)/tracker
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
$(BUILD)/test_render:  $(BUILD)/render.o $(BUILD)/stats.o

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
