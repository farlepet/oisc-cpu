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

#define WRITE(a, b, c) do { fwrite(&a, 4, 1, out); fwrite(&b, 4, 1, out); fwrite(&c, 4, 1, out); } while(0);

void handle_opts(int, char **);
void usage(int);
int32_t get_number(char *);

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

	char str[1024];

	while(fscanf(in, "%[^\n]\n", str) != EOF)
	{
		// Convert entire line to uppercase
		unsigned i = 0;
		for(; i < strlen(str); i++) str[i] = toupper(str[i]);

		if(str[0] == '#') continue; // This line is a comment

		char *a;
		char *b;
		char *c;

		int32_t A;
		int32_t B;
		int32_t C;

		a = strtok(str,  " ");
		b = strtok(NULL, " ");
		c = strtok(NULL, " ");

		if(!c) c = "0";

		A = get_number(a);
		B = get_number(b);
		C = get_number(c);

		WRITE(A, B, C);
	}

	fclose(in);
	fclose(out);

	return 0;
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
	printf("    -f  --in:    Source file to assemble.\n");
	printf("    -m  --out:   Output binary file.\n");
	exit(retval);
}
