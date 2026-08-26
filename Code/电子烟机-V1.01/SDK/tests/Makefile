# Host-side unit tests for the firmware's hardware-independent logic.
#
#   make          build and run every suite
#   make coverage build/run with gcov instrumentation and print a line/branch
#                 coverage report for the modules under test
#   make clean

CC      ?= gcc
SDK     := ..
LIBS    := $(SDK)/libs
BUILD   := build

INCLUDES := \
  -I framework \
  -I fakes \
  -I fakes/include \
  -I $(LIBS) \
  -I $(LIBS)/Tools \
  -I $(LIBS)/AT_Module \
  -I $(LIBS)/drv

CFLAGS  ?= -std=c99 -g -O0 -Wall -Wno-unused-parameter $(INCLUDES)
LDFLAGS ?=
LDLIBS  ?= -lm

COVERAGE_FLAGS := --coverage -fprofile-arcs -ftest-coverage

# Firmware sources under test, plus the harness and the hardware fakes.
COMMON_SRC := framework/test_framework.c fakes/fakes.c $(LIBS)/sys_manager.c

SUITES := tools drv_load app_motor at

tools_SRC    := $(LIBS)/Tools/tools.c
drv_load_SRC := $(LIBS)/drv/drv_load.c $(LIBS)/drv/drv_charger.c $(LIBS)/drv/drv_battery.c
app_motor_SRC:= $(LIBS)/drv/app_motor.c
at_SRC       := $(LIBS)/AT_Module/AT.c $(LIBS)/Tools/tools.c $(LIBS)/drv/app_motor.c \
                $(LIBS)/drv/drv_charger.c $(LIBS)/drv/drv_load.c

BINS := $(addprefix $(BUILD)/test_,$(SUITES))

.PHONY: all test coverage clean

all: test

$(BUILD):
	mkdir -p $(BUILD)

define SUITE_rule
$(BUILD)/test_$(1): test_$(1).c $$($(1)_SRC) $$(COMMON_SRC) | $(BUILD)
	$$(CC) $$(CFLAGS) $$(EXTRA_CFLAGS) $$^ -o $$@ $$(LDFLAGS) $$(EXTRA_LDFLAGS) $$(LDLIBS)
endef
$(foreach s,$(SUITES),$(eval $(call SUITE_rule,$(s))))

test: $(BINS)
	@rc=0; for t in $(BINS); do echo "== $$t"; ./$$t || rc=1; done; exit $$rc

coverage: clean
	$(MAKE) test EXTRA_CFLAGS="$(COVERAGE_FLAGS)" EXTRA_LDFLAGS="$(COVERAGE_FLAGS)"
	gcovr --root .. --filter '.*/libs/' --exclude-unreachable-branches \
	  --print-summary --txt --branches --decisions 2>/dev/null || \
	  gcov -b $(BUILD)/*.gcda

clean:
	rm -rf $(BUILD) *.gcov *.gcda *.gcno
