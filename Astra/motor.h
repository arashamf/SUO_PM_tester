
//#define MOTOR_DEBUG_ENABLED						// creates data buffer (PI regulator data) and sends it by UART after motor is stopped

#define MAIN_MOTOR_KI 3								// PI controller: intergration factor [%]
#define MAIN_MOTOR_KP 100							// PI controller: proportional factor [%]
#define MAIN_MOTOR_TARGET_SPEED 500		// main motor target speed [mm per second]
#define ENCODER_SAVE_TIME 750000/MAIN_MOTOR_TARGET_SPEED

#define NOTE_DROPPED_OK				0
#define NOTE_ABORTED					1


extern unsigned int motorCnt;

extern signed int motor_integr;
extern signed int motor_proport;
extern signed int motor_pi;

extern volatile unsigned int error_motor;
extern volatile unsigned int test_motor;

void StartMotorControl(void);
void MotorSpeedControl(void);
void SendMotorReport(void);
unsigned char CleanPath(void);
int NoteDestiny(void);
void CheckMotor (void);
unsigned int CheckMotorFirst (void);
int RejectNote(void);
int AcceptNote(void);


