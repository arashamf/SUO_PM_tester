extern unsigned int flag_receive;
extern char buf_rec[100];
void CDC_SendData(unsigned char *buf, int len);
void CDC_SendImage(void);
void CDC_ClearRecBuf (void);
unsigned int CDC_RecData (unsigned char data);
unsigned int CDC_CheckMessage (unsigned int len);
void CDC_ParseMessage (void);

