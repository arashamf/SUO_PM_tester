#include "stm32f4xx.h"

void SaveCalValues(unsigned int *buf, unsigned int nWords);
int LoadCalValues(unsigned int *buf, unsigned int nWords);
void WriteGlobalNoteCounter (void);
unsigned int ReadNoteCounter (void);
void ResetNoteCounter(void);
