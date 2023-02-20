
extern unsigned int avgMg0,avgMg1,avgMg2,avgMg3,avgMg4,avgMg5,mgCnt,mgScanState;
extern unsigned int mg0,mg1,mg2,mg3,mg4,mg5;

void StartMG(void);
void PrepareMG (void);
void CleanMgValues(void) ;
void ScanMG(unsigned char *buf);

