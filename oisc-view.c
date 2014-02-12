#include <stdint.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	FILE *bin = NULL;

	if(argc < 2) return 1;

	bin = fopen(argv[1], "r");

	if(!bin) return 1;

	uint32_t t = 0;

	uint32_t addr = 0x1000; // Entrypoint
	
	while(1)
	{
		printf("%08X: ", addr);
		addr += 3;

		t = 0;
		if(!fread(&t, 4, 1, bin)) break;
		printf("%08X ", t);

		t = 0;
		if(!fread(&t, 4, 1, bin)) break;
		printf("%08X ", t);

		t = 0;
		if(!fread(&t, 4, 1, bin)) break;
		printf("%08X\n", t);
	}

	return 0;
}
