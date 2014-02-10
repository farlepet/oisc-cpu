#ifndef OSIC_H
#define OSIC_H

#include <stdint.h>

#define ENTRYPOINT   0x1000 // We reserve some space for device memory


#define ZERO_MEM     0x000  // This location in memory is always 0

#define SERIAL_OUT   0x100  // If this memory address is writted to, that data is sent via serial (only the first 8 bits)
#define SERIAL_IN    0x101  // Is set to received data whenever data is received
#define SERIAL_RECVD 0x102  // Is 1 if there is data at SERIAL_RECVD

#define MMIO_VALUES  0x108  // Location of memory mapped io input/output values
#define MMIO_DIREC   0x109  // Directions of the MMIO lines

struct oisc_cpu
{
	uint32_t pc;
	int32_t *mem;
	uint32_t mem_size;
};

int cpu_cycle(struct oisc_cpu *);

#endif // OSIC
