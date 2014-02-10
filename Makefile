# Makefile for oisc-cpu

MAINDIR = $(CURDIR)
CPUSRC	= $(MAINDIR)/cpu
INC     = $(MAINDIR)/inc

CCPUSRC = $(wildcard $(CPUSRC)/*.c)
CPUOBJ  = $(patsubst %.c,%.o,$(CCPUSRC))
CPUEXEC = $(MAINDIR)/oisc-cpu

CFLAGS  = -Wall -Wextra -Werror -I $(INC)

all: $(CPUOBJ)
	@echo " LD $(CPUEXEC)"
	@$(CC) $(CPUOBJ) -o $(CPUEXEC)

clean:
	@echo "Cleaning oisc-cpu"
	@rm -f $(CPUOBJ)

%.o: %.c
	@echo " CC $<"
	@$(CC) $(CFLAGS) -c -o $@ $<
