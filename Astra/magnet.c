// MG data capture 

#include "magnet.h"
#include "timer.h"
#include "pins.h"
#include "scanner.h"
#include "my_adc.h"	

unsigned int avgMg0,avgMg1,avgMg2,avgMg3,avgMg4,avgMg5,mgCnt,mgScanState;
unsigned int mg0,mg1,mg2,mg3,mg4,mg5;
float mgOut0=0,mgOut1=0,mgOut2=0,mgOut3=0,mgOut4=0,mgOut5=0;

void StartMG(void)
{
	avgMg0=avgMg1=avgMg2=avgMg3=avgMg4=avgMg5=mgCnt=mgScanState=0;
	EnableTimer5(100);	// Timer5 samples the MR sensor each 100us 
}

void PrepareMG (void)
{
	avgMg0 = avgMg0/mgCnt;
	avgMg1 = avgMg1/mgCnt;
	avgMg2 = avgMg2/mgCnt;
	avgMg3 = avgMg3/mgCnt;
	avgMg4 = avgMg4/mgCnt;
	avgMg5 = avgMg5/mgCnt;
	CleanMgValues();
	mgScanState=1;
}


void CleanMgValues(void) 
{
//	mg0=mg1=mg2=mg3=mg4=mg5=0; 
}


void ScanMG(unsigned char *buf)
{
	buf[0] = mgOut0;//mg0;
	buf[1] = mgOut1;// mg1;
	buf[2] = mgOut2;//mg2;
	buf[3] = mgOut3;//mg3;
	buf[4] = mgOut4;//mg4;
	buf[5] = mgOut5;//mg5;
}


void TIM5_IRQHandler(void) // ~5us
{
	static float mag0=0,mag1=0,mag2=0,mag3=0,mag4=0,mag5=0;
	static float mgi0=0,mgi1=0,mgi2=0,mgi3=0,mgi4=0,mgi5=0;
	#define k1 (float)12.0
	#define k2   (float)3.0
	#define k3  (float)0.93 
	TIM5->SR &= ~TIM_SR_UIF;

	ADC1_ChannelSelect(ADC_MR_0);
	avgMg0 = ADC1_getValue();
	ADC1_ChannelSelect(ADC_MR_1);
	avgMg1 = ADC1_getValue();
	ADC1_ChannelSelect(ADC_MR_2);
	avgMg2 = ADC1_getValue();
	ADC1_ChannelSelect(ADC_MR_3);
	avgMg3 = ADC1_getValue();
	ADC1_ChannelSelect(ADC_MR_4);
	avgMg4 = ADC1_getValue();
	ADC1_ChannelSelect(ADC_MR_5);
	avgMg5 = ADC1_getValue();
	mag0+=((float)avgMg0-mag0)/k1;
	mgi0+=(mag0-(float)avgMg0-mgi0)/k2;
	if(mgi0>=0)
		mgOut0=mgOut0*k3+mgi0;
	else
		mgOut0=mgOut0*k3-mgi0;

	mag1+=((float)avgMg1-mag1)/k1;
	mgi1+=(mag1-(float)avgMg1-mgi1)/k2;
	if(mgi1>=0)
		mgOut1=mgOut1*k3+mgi1;
	else
		mgOut1=mgOut1*k3-mgi1;

	mag2+=((float)avgMg2-mag2)/k1;
	mgi2+=(mag2-(float)avgMg2-mgi2)/k2;
	if(mgi2>=0)
		mgOut2=mgOut2*k3+mgi2;
	else
		mgOut2=mgOut2*k3-mgi2;
	
	mag3+=((float)avgMg3-mag3)/k1;
	mgi3+=(mag3-(float)avgMg3-mgi3)/k2;
	if(mgi3>=0)
		mgOut3=mgOut3*k3+mgi3;
	else
		mgOut3=mgOut3*k3-mgi3;

	mag4+=((float)avgMg4-mag4)/k1;
	mgi4+=(mag4-(float)avgMg4-mgi4)/k2;
	if(mgi4>=0)
		mgOut4=mgOut4*k3+mgi4;
	else
		mgOut4=mgOut4*k3-mgi4;

	mag5+=((float)avgMg5-mag5)/k1;
	mgi5+=(mag5-(float)avgMg5-mgi5)/k2;
	if(mgi5>=0)
		mgOut5=mgOut5*k3+mgi5;
	else
		mgOut5=mgOut5*k3-mgi5;
}

