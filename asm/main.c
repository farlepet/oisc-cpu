#include <stdint.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

#include <oisc.h>

static char *infile  = NULL;
static char *outfile = NULL;

static FILE *in = NULL;
static FILE *out = NULL;

static int max_labels = 1024; // Maximum number of lables allowed

struct label
{
	char     name[32];
	uint32_t hash;
	uint32_t addr;
	uint32_t line;
};

struct label *labels;

#define WR(v) do { uint32_t T = (uint32_t)v; fwrite(&T, 4, 1, out); } while(0);
#define WRITE(a, b, c) do { fwrite(&a, 4, 1, out); fwrite(&b, 4, 1, out); fwrite(&c, 4, 1, out); } while(0);

void handle_opts(int, char **);
void usage(int);
int32_t get_number(char *);
void find_labels();

int main(int argc, char **argv)
{
	handle_opts(argc, argv);

	if(!infile || !outfile) usage(1);

	in  = fopen(infile, "r");
	out = fopen(outfile, "w");

	if(!in)
	{
		printf("Input file %s could not be opened!", infile);
		return 1;
	}
	if(!out)
	{
		printf("Output file %s could not be opened/created!", outfile);
		fclose(in);
		return 1;
	}

	find_labels();

	fseek(in, 0, SEEK_SET);

	char str[1024];

	while(fscanf(in, "%[^\n]\n", str) != EOF)
	{
		printf("Assembling line: %s\n", str);
		// Convert entire line to uppercase
		unsigned i = 0;
		int not_white = 0, is_comment = 0, colon = 0;
		for(; i < strlen(str); i++)
		{
			str[i] = toupper(str[i]);
			if(str[i] == ':') colon = 1;
			if(!not_white && str[i] == '#') is_comment = 1;
			if(!isspace(str[i])) not_white = 1;
		}

		if(colon) continue;

		if(str[0] == '#' || strlen(str) == 0 ||
			!not_white || is_comment) continue; // This line is a comment, or whitespace

		if(str[0] == '.') // Data
		{
			if(str[2] == '"') // There MUST be a space inbewteen
			{
				int i = 3;
				while(str[i] != '"')
					WR(str[i++]);
			}
			else WR(get_number(str + 2));
			continue;
		}

		char *a;
		char *b;
		char *c;

		int32_t A;
		int32_t B;
		int32_t C;

		a = strtok(str,  ", ");
		b = strtok(NULL, ", ");
		c = strtok(NULL, ", ");

		A = get_number(a);
		B = get_number(b);
		if(!c) C = (ftell(out) / 4) + 3 + ENTRYPOINT;
		else   C = get_number(c);

		WRITE(A, B, C);
	}

	fclose(in);
	fclose(out);

	return 0;
}


uint32_t strhash(char *str)
{
	uint32_t hash = 0;
	while(*str)
	{
		uint8_t end = ((hash >> 24) & 0xFF);
		hash = (hash << 8) | (*str ^ end);
		str++;
	}
	return hash;
}

void find_labels()
{
	labels = malloc(sizeof(struct label) * max_labels);

	uint32_t line = 1;
	uint32_t addr = ENTRYPOINT; // Place it above the device-mapped memory
	int c_idx = 0;

	char str[1024];

	while(fscanf(in, "%[^\n]\n", str) != EOF)
	{
		// Convert entire line to uppercase
		unsigned i = 0;
		int not_white = 0, is_comment = 0, colon = 0;
		for(; i < strlen(str); i++)
		{
			str[i] = toupper(str[i]);
			if(str[i] == ':') colon = i; // If it is the first char on the line, ignore it
			if(!not_white && str[i] == '#') is_comment = 1;
			if(!isspace(str[i])) not_white = 1;
		}

		if(colon) // We found a comment
		{
			memcpy(labels[c_idx].name, str, colon);
			labels[c_idx].hash = strhash(labels[c_idx].name);
			labels[c_idx].line = line;
			labels[c_idx].addr = addr;
			printf("Found label on line %d at addr %08X with hash %08X: %s\n", line, addr, labels[c_idx].hash, labels[c_idx].name);
			c_idx++;
			continue;
		}

		line++;

		if(str[0] == '#' || strlen(str) == 0 ||
			!not_white || is_comment) continue; // This line is a comment, or whitespace

		if(str[0] == '.') // Data
		{
			if(str[2] == '"')
			{
				int i = 3;
				while(str[i++] != '"') addr++;
			}
			else addr++; // Only one integer may be supplied per line right now
			continue;
		}

		addr += 3;
	}

}

int find_label(char *name)
{
	int i = 0;
	
	uint32_t hash = strhash(name);

	for(; i < max_labels; i++)
	{
		if(labels[i].hash == hash)
			if(!strcmp(labels[i].name, name)) // Just incase two labels have the same hash
				return i;
	}
	return -1; // Not found...
}


int32_t get_number(char *str)
{
	int base;

	char *test = str;
	if(*test == '-' || *test == '+') test++; // ignore sign
	if(*test == '0')
	{
		if(*(test + 1) == 'x') base = 16;
		else base = 8;
	}
	else if(isalpha(*test))
	{
		int idx = find_label(str);
		if(idx == -1)
		{
			printf("ERROR: Could not find label: %s", str);
			exit(1);
		}
		printf("DEBUG: Found label: H:%08X A:%08X L:%d N:%s\n", labels[idx].hash, labels[idx].addr, labels[idx].line, labels[idx].name);
		return labels[idx].addr;
	}
	else base = 10;

	return strtol(str, NULL, base);
}

static struct option long_options[] =
{
	{ "help",      no_argument,       0, 'h' },
	{ "in",        required_argument, 0, 'i' },
	{ "out",       required_argument, 0, 'o' },
};

void handle_opts(int argc, char **argv)
{
	int opt;
	while((opt = getopt_long(argc, argv, "hio", long_options, NULL)) != -1)
	{
		switch(opt)
		{
			case 'h':
				usage(0);
				break;

			case 'i':
				infile = optarg;
				break;

			case 'o':
				outfile = optarg;
				break;

			default:
				usage(1);
				break;
		}
	}
}

void usage(int retval)
{
	printf("USAGE: osic-asm [OPTIONS] -i infile -o outfile\n");
	printf("  OPTIONS:\n");
	printf("    -h  --help:  Show this help message.\n");
	printf("    -i  --in:    Source file to assemble.\n");
	printf("    -m  --out:   Output binary file.\n");
	exit(retval);
}
