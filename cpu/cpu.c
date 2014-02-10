#include <stdint.h>

#include <oisc.h>

int cpu_cycle(struct oisc_cpu *cpu)
{
	int32_t a = cpu->mem[cpu->pc + 0];
	int32_t b = cpu->mem[cpu->pc + 1];
	int32_t c = cpu->mem[cpu->pc + 2];

	if(a < 0 || b < 0)
		return 1; // End of execution

	cpu->mem[b] -= cpu->mem[a];
	if(cpu->mem[b] > 0)
		cpu->pc += 3;
	else
	{
		if(c < 0)
			return 1; // End of execution
		cpu->pc = c;
	}
	return 0;
}
