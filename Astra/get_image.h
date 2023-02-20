
#define MIN_LEVEL 50					// minimum IR scanner level to detect the banknote edge 
#define NOTE_INSERT_LEVEL 150	// minimum insert (double detection) sensor level

#define ERR_CHAIN 10
#define ERR_FEEDING 11
#define ERR_LONG_NOTE 12
#define ERR_ENC 13
#define ERR_FISHING 14
#define IMG_OK 100

#define BANKNOTE_FREE_RUN 65	// distance the banknote will run after the TOP scanner

#define BOT_SCAN_OFFSET 18		// offset for the bottom scanner (the origin is the top scanner)
#define MG_OFFSET 52					// offset for the MR sensor (the origin is the top scanner)
#define UV_OFFSET 67					// offset for the UV sensor (the origin is the top scanner)
#define AS_OFFSET 74					// offset for the AS sensor (the origin is the top scanner)
#define AF_OFFSET 130					// value = maximum banknote length - offset between AF sensor and top scanner
#define AF_THRESHOLD_MV 500		// maximum AF sensor value in (mV) which is not fishing attempt	

extern unsigned int encCnt,imgCnt,edgeCnt;
extern volatile unsigned int getImgState;
extern volatile unsigned int minIdleTime;
extern volatile unsigned int idleTime;
extern volatile unsigned int suspectAF;

extern volatile unsigned int error_enc;

void GetImage (void);
unsigned int CheckPath(void);
void DisableImageInterruptions(void);
void AreScannersClean(void);


