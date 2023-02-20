extern	unsigned int rub_10;
extern	unsigned int rub_50;
extern	unsigned int rub_100;
extern	unsigned int rub_200;
extern	unsigned int rub_500;
extern	unsigned int rub_1000;
extern	unsigned int rub_2000;
extern	unsigned int rub_5000;
extern	unsigned int rub_total;
extern unsigned char allow_notes[8];

extern unsigned char dma_buf_rec[100];

#define MINIMUM_CASH_BAG_DISTANCE 80 		// minimal free distance inside the cash bag

#define ASTRA_STATE_READY 				0
#define ASTRA_STATE_NO_INIT				(1<<1)
#define ASTRA_STATE_PATH_OPEN 		(1<<2)
#define ASTRA_STATE_BAG_FULL			(1<<3)
#define ASTRA_STATE_INSERT_JAM		(1<<4)
#define ASTRA_STATE_EXIT_JAM			(1<<5)
#define ASTRA_STATE_SCANNER_JAM		(1<<6)
#define ASTRA_STATE_CAPTURING			(1<<7)
#define ASTRA_STATE_PROCESSING		(1<<8)
#define ASTRA_STATE_STACKING			(1<<9)
#define ASTRA_STATE_USER_MENU			(1<<10)
#define ASTRA_STATE_HOST_TIMEOUT	(1<<11)

extern unsigned int currentAstraState;

void StartReceiveCommand (void);
void ReceiveCommand (void);
void SendCommand (unsigned int command);
void SaveNotesCounter(void);
void CheckUartDMA (void);
