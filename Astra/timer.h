
void delay(unsigned int x);
void EnableTimer7(unsigned int value);
unsigned int ReadTimer7(void);
void DisableTimer7(void);
unsigned int CheckTimer7(unsigned int max_time);
void delay_ms(unsigned int delay);
void delay_us(unsigned int delay);
void EnableTimer6(unsigned int value);
void DisableTimer6(void);
void EnableTimer5(unsigned int value);
void DisableTimer5(void);
void EnableIdleScan(void);
void DisableIdleScan(void);

void init_timer7(unsigned int value);
void ReLoadTimer7(unsigned int value);
void deInit_timer7(void);
unsigned int GetTim7Value(void);
void Timer7_reset(void);

void init_timer6(unsigned int value);
void ReLoadTimer6(unsigned int value);
void deInit_timer6(void);
unsigned int GetTim6Value(void);
void Timer6_reset(void);

void init_timer2(unsigned int value);
void ReLoadTimer2(unsigned int value);
void deInit_timer2(void);
unsigned int GetTim2Value(void);
void Timer2_reset(void);

void EnablePeriodicalAdc(void);
void DisablePeriodicalAdc(void);


