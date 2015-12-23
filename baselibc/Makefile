# You can override the CFLAGS and C compiler externally,
# e.g. make PLATFORM=cortex-m3
CFLAGS += -I./include
CC=lcc
ifeq ($(PLATFORM),cortex-m3)
  CC      = arm-none-eabi-gcc
  AR      = arm-none-eabi-ar
  CFLAGS += -mcpu=cortex-m3 -mthumb
  CFLAGS += -fno-common -Os
  CFLAGS += -ffunction-sections -fdata-sections
endif

# With this, the makefile should work on Windows also.
ifdef windir
  RM = del
endif

# Just include all the source files in the build.
CSRC = $(wildcard src/*.c)
OBJS = $(CSRC:.c=.o)

# And the files for the test suite
TESTS_CSRC = $(wildcard tests/*_tests.c)
TESTS_OBJS = $(TESTS_CSRC:.c=)

# Some of the files uses "templates", i.e. common pieces
# of code included from multiple files.
CFLAGS += -Isrc/templates

all: libc.a

clean:
	$(RM) $(OBJS) $(TESTS_OBJS) libc.a

libc.a: $(OBJS)
	$(RM) $@
	$(CC) -o $@ dummy_main.c $^

run_tests: $(TESTS_OBJS)
	$(foreach f,$^,$f)

tests/%: tests/%.c tests/tests_glue.c libc.a
	$(CC) $(CFLAGS) -o $@ dummy_main.c $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<
