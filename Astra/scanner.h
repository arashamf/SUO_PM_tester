
#define TOP_RED 0
#define TOP_GREEN 1
#define TOP_BLUE 2
#define TOP_IR 3

#define BOTTOM_RED 10
#define BOTTOM_GREEN 11
#define BOTTOM_BLUE 12
#define BOTTOM_IR 13

extern unsigned int resultBuf[25];

void ScanTop(unsigned char lightSource, unsigned int chargeTime);
void ScanBottom(unsigned char lightSource, unsigned int chargeTime);
void ShowScan(unsigned char lightSource, unsigned int chargeTime);
unsigned int GetAvg(void);
	


