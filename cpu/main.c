#include <sys/stat.h>
#include <getopt.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <oisc.h>

static uint32_t memsz    = 0x2000; // Default to 8KiB of memory
char           *filename = NULL;
static uint32_t dmemst   = 0; // Where debug printing starts in memory
static uint32_t dmemen   = 3; // Where debug printing stops in memory
static int      debugmem = 0; // Wether or not to print memory info

static char     ser_buff[512]; // Serial input buffer
static int      ser_buff_loc = 0; // Serial input buffere location

void handle_opts(int, char **);
void usage(int);
void buff_write(char);
char buff_read();

int main(int argc, char **argv)
{
	// Check the options in `argv`
	handle_opts(argc, argv);

	// Get the program file size
	struct stat st;
	int stret = stat(filename, &st);
	if(stret) // Does the file exist?
	{
		printf("File %s not found!\n", filename);
		usage(1);
	}
	uint32_t fsize = st.st_size;

	if((fsize + ENTRYPOINT) > memsz) // Is the program too big to fit into the memory?
	{
		printf("File is too large to fit in allocated memory. (%u + %u > %u)", fsize, ENTRYPOINT, memsz);
		usage(1);
	}

	// Create the CPU structure, and initialize it
	struct oisc_cpu cpu;
	cpu.pc       = ENTRYPOINT;
	cpu.mem      = malloc(memsz);
	cpu.mem_size = memsz;

	// Copy the program into the CPU's memory
	FILE *file = fopen(filename, "r");
	fread(cpu.mem + ENTRYPOINT, 1, fsize, file);
	fclose(file);

	// Emulate the CPU
	while(cpu_cycle(&cpu) != 1)
	{
		if(cpu.mem[SERIAL_OUT])
		{
			printf("%c", (char)cpu.mem[SERIAL_OUT]);
			cpu.mem[SERIAL_OUT] = 0;
		}

		cpu.mem[ZERO_MEM] = 0; // This must ALWAYS be zero
		
		if(debugmem)
		{
			printf("{%X}: ", cpu.pc);
			uint32_t p = dmemst;
			while(p <= dmemen && p < cpu.mem_size)
			{
				printf("[%d]:%d ", p, cpu.mem[p]);
				p++;
			}
			printf("\n");
		}
	}

	free(cpu.mem);

	return 0;
}


void buff_write(char v)
{
	ser_buff[ser_buff_loc++] = v;
}

char buff_read()
{
	char r = ser_buff[0];
	memcpy(ser_buff, ser_buff + sizeof(char), ser_buff_loc);
	ser_buff_loc--;
	return r;
}

#define DEBUG_MEM       0x100
#define DEBUG_MEM_START 0x101
#define DEBUG_MEM_END   0x102

static struct option long_options[] =
{
	{ "help",      no_argument,       0, 'h' },
	{ "file",      required_argument, 0, 'f' },
	{ "mem",       required_argument, 0, 'm' },
	{ "dmemst",    required_argument, 0, DEBUG_MEM_START },
	{ "dmemen",    required_argument, 0, DEBUG_MEM_END },
	{ "debug-mem", no_argument,       0, DEBUG_MEM },
};

void handle_opts(int argc, char **argv)
{
	int opt;
	while((opt = getopt_long(argc, argv, "hfm", long_options, NULL)) != -1)
	{
		switch(opt)
		{
			case 'h':
				usage(0);
				break;

			case 'f':
				filename = optarg;
				break;

			case 'm':
				memsz = atoi(optarg);
				break;

			case DEBUG_MEM_START:
				dmemst = atoi(optarg);
				break;

			case DEBUG_MEM_END:
				dmemen = atoi(optarg);
				break;

			case DEBUG_MEM:
				debugmem = 1;
				break;

			default:
				usage(1);
				break;
		}
	}
}

void usage(int retval)
{
	printf("USAGE: osic-cpu [OPTIONS] -f file\n");
	printf("  OPTIONS:\n");
	printf("    -h  --help:  Show this help message.\n");
	printf("    -f  --file:  File to load into the CPU's memory.\n");
	printf("    -m  --mem:   Set the amount of memory given to the virtual CPU in bytes.\n");
	printf("                   (default 4096)\n");
	exit(retval);
}
