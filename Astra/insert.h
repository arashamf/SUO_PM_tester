
extern unsigned int doubleCnt;  
extern unsigned int doubleSum; 

typedef struct {
	int ins0;
	int ins1;
} insert_t;


insert_t ReadInsertSensors(void);
unsigned int ReadAFSensor(void);
void ShowDoubleSensors(void);
void SaveDoubleSensorInfo(void);
int IsNoteHere(void);


