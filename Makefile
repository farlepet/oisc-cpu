# Makefile for oisc-cpu

MAINDIR = $(CURDIR)
CPUSRC	= $(MAINDIR)/cpu
ASMSRC  = $(MAINDIR)/asm
INC     = $(MAINDIR)/inc

CCPUSRC = $(wildcard $(CPUSRC)/*.c)
CPUOBJ  = $(patsubst %.c,%.o,$(CCPUSRC))
CPUEXEC = $(MAINDIR)/oisc-cpu

CASMSRC = $(wildcard $(ASMSRC)/*.c)
ASMOBJ  = $(patsubst %.c,%.o,$(CASMSRC))
ASMEXEC = $(MAINDIR)/oisc-asm

CFLAGS  = -Wall -Wextra -Werror -I $(INC)

all: $(CPUOBJ) $(ASMOBJ)
	@echo " LD $(CPUEXEC)"
	@$(CC) $(CPUOBJ) -o $(CPUEXEC)
	@echo " LD $(ASMEXEC)"
	@$(CC) $(ASMOBJ) -o $(ASMEXEC)

clean:
	@echo "Cleaning oisc-cpu"
	@rm -f $(CPUOBJ) $(ASMOBJ) $(CPUEXEC) $(ASMEXEC)

%.o: %.c
	@echo " CC $<"
	@$(CC) $(CFLAGS) -c -o $@ $<
