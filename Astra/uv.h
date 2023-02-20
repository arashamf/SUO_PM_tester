
#define UV_AVG_INTERVAL 30

extern unsigned int uvResult,uv1,uv2;
extern unsigned int uvBackground,uvCnt;

void GetUv(unsigned int chargeTime);
void PrepareUV (void);
void GetUvBackground(void);
unsigned int ApplyUvCal(void);
void GetAvgUv(unsigned int tmpChargeTime);
