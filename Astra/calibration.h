
#define MAX_EXPOSURE_TIME 60		// maximum light exposure time for both scanners

#define AVG_SCAN_LEVEL 250 			// Calibration mode only: the normal averaged level of the resultBuf
#define AVG_SCAN_TOLERANCE 15 	// Calibration mode only: maximum deviation 

#define MAX_UV 800
#define IDEAL_UV 150
#define TOLERANCE_UV 15

#define DD_TARGET_LEVEL 1000    //1000mV level for one banknote
#define AF_TARGET_LEVEL 500     //500mV


#define MG_MIN_LEVEL 70 
#define MG_MAX_LEVEL 150

// calibration parameters
#define CAL_LENGTH 366   // length in 32 bit words
union calUnion{
	struct {
		unsigned int topRedW[22];	// white level
		unsigned int topRedB[22];	// black level
		unsigned int topRtime;		// exposure time
		unsigned int topGreenW[22];
		unsigned int topGreenB[22];
		unsigned int topGtime;
		unsigned int topBlueW[22];
		unsigned int topBlueB[22];
		unsigned int topBtime;
		unsigned int topIRW[22];
		unsigned int topIRB[22];
		unsigned int topItime;
		
		unsigned int botRedW[22];
		unsigned int botRedB[22];
		unsigned int botRtime;
		unsigned int botGreenW[22];
		unsigned int botGreenB[22];
		unsigned int botGtime;
		unsigned int botBlueW[22];
		unsigned int botBlueB[22];
		unsigned int botBtime;
		unsigned int botIRW[22];
		unsigned int botIRB[22];
		unsigned int botItime;
		
		unsigned int uvW;					// white level
		unsigned int uvB;					// black level	
		unsigned int uvTime;			// exposure time
		
		unsigned int asTime;			// anti Stokes charge time
		
		unsigned int ddDuty;			// double detection PWM duty value
		unsigned int afDuty;			// anti fishing PWM duty value
	}table;
	
	unsigned int buf[CAL_LENGTH];	// should be 128Kbytes maximum
};
extern union calUnion cal;

void Calibration(void);
void WaitButton2(void);
unsigned int SetupWhite(unsigned char lightSource);
void SetupBlack(unsigned char lightSource, unsigned int chargeTime);
void SaveWhiteCal(unsigned int *buf);
void SaveBlackCal(unsigned int *bufBlack, unsigned int *bufWhite);
void ApplyCal(unsigned int *bufWhite, unsigned int *bufBlack);
unsigned int SetupUvW(void);
void SetupUvB(unsigned int tmpChargeTime);
void SaveUvCal(void);
void ShowUv(unsigned int chargeTime);
void ShowSensors(void);
void SetupDD (void);
void SetupAF (void);
unsigned int SelfTest(void);
void CheckMg(void); 

