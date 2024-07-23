#ifndef POT_READING_H
#define POT_READING_H

#include <stdint.h>
#include <tm4c123gh6pm.h>

// Function declarations
void ADC0_Init(void);
void sample_sequencer_init(void);
void pot_read(uint32_t *data);
#endif // POT_READING.H
