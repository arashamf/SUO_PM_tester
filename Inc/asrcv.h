unsigned char StartBootStm (void);
unsigned char EraseStm(void);
unsigned char WriteStm(void);
unsigned int WaitACK(void);
unsigned char LockReadStm(void);
unsigned char UnLockReadStm(void);
void SendAdress (int adr);
void ClearBuf (void);
void SendBuf (void);
unsigned int AsFirmwareUpload (void);
void AsMcuBurning	(void);

#define LEN_BIN_ASRCV 10284

extern const unsigned char bin_asrcv[];
