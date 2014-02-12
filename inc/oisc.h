#ifndef OSIC_H
#define OSIC_H

#include <stdint.h>

/*
 * Memory layout of this SUBLEQ VM:
 *   * 0x0000: Always 0
 *   * 0x0001 - 0x00FF: Scratch RAM -- Free for use
 *   * 0x0100 - 0x010F: Reserved for serial devices
 *       * 0x0100: Serial out -- Writes to the serial port when written to
 *       * 0x0101: Serial in -- A buffered serial input
 *       * 0x0102: Serial received -- Is 1 when data is waiting in Serial in: NOTE: MUST BE SET TO 0 AFTER USE!
 *       * 0x0103 - 0x010F: Currently unused
 *   * 0x0110 - 0x0111: Used for Memory Mapped I/O (MMIO)
 *       * 0x0110: MMIO inputs and outputs
 *       * 0x0111: MMIO directions (0 = in, 1 = out)
 *   * 0x0110 - 0x0FFF: Reserved for future use
 *   * 0x1000 - inf: Program memory
 */


#define ENTRYPOINT   0x1000 // We reserve some space for device memory


#define ZERO_MEM     0x000  // This location in memory is always 0

#define SERIAL_OUT   0x100  // If this memory address is writted to, that data is sent via serial (only the first 8 bits)
#define SERIAL_IN    0x101  // Is set to received data whenever data is received
#define SERIAL_RECVD 0x102  // Is 1 if there is data at SERIAL_RECVD

#define MMIO_VALUES  0x110  // Location of memory mapped io input/output values
#define MMIO_DIREC   0x111  // Directions of the MMIO lines

struct oisc_cpu
{
	uint32_t pc;
	int32_t *mem;
	uint32_t mem_size;
};

int cpu_cycle(struct oisc_cpu *);

#endif // OSIC
