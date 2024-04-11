#include "rc522.h"

//uint8_t Card_KEY[6] = {0x22,0x22,0x22,0x22,0x22,0x22}; //{0x11,0x11,0x11,0x11,0x11,0x11};   //����

uint8_t Card_KEY[6] = {0x11,0x11,0x11,0x11,0x11,0x11};
//MF522������
#define PCD_IDLE              0x00               //ȡ����ǰ����
#define PCD_AUTHENT           0x0E               //��֤��Կ
#define PCD_RECEIVE           0x08               //��������
#define PCD_TRANSMIT          0x04               //��������
#define PCD_TRANSCEIVE        0x0C               //���Ͳ���������
#define PCD_RESETPHASE        0x0F               //��λ
#define PCD_CALCCRC           0x03               //CRC����

//Mifare_One��Ƭ������
#define PICC_REQIDL           0x26               //Ѱ��������δ��������״̬
#define PICC_REQALL           0x52               //Ѱ��������ȫ����
#define PICC_ANTICOLL1        0x93               //����ײ
#define PICC_ANTICOLL2        0x95               //����ײ
#define PICC_AUTHENT1A        0x60               //��֤A��Կ
#define PICC_AUTHENT1B        0x61               //��֤B��Կ
#define PICC_READ             0x30               //����
#define PICC_WRITE            0xA0               //д��
#define PICC_DECREMENT        0xC0               //�ۿ�
#define PICC_INCREMENT        0xC1               //��ֵ
#define PICC_RESTORE          0xC2               //�������ݵ�������
#define PICC_TRANSFER         0xB0               //���滺����������
#define PICC_HALT             0x50               //����

//MF522 FIFO���ȶ���
#define DEF_FIFO_LENGTH       64                 //FIFO size=64byte
#define MAXRLEN  18

//MF522�Ĵ�������
// PAGE 0
#define     RFU00                 0x00
#define     CommandReg            0x01
#define     ComIEnReg             0x02
#define     DivlEnReg             0x03
#define     ComIrqReg             0x04
#define     DivIrqReg             0x05
#define     ErrorReg              0x06
#define     Status1Reg            0x07
#define     Status2Reg            0x08
#define     FIFODataReg           0x09
#define     FIFOLevelReg          0x0A
#define     WaterLevelReg         0x0B
#define     ControlReg            0x0C
#define     BitFramingReg         0x0D
#define     CollReg               0x0E
#define     RFU0F                 0x0F
//PAGE 1
#define     RFU10                 0x10
#define     ModeReg               0x11
#define     TxModeReg             0x12
#define     RxModeReg             0x13
#define     TxControlReg          0x14
#define     TxAutoReg             0x15
#define     TxSelReg              0x16
#define     RxSelReg              0x17
#define     RxThresholdReg        0x18
#define     DemodReg              0x19
#define     RFU1A                 0x1A
#define     RFU1B                 0x1B
#define     MifareReg             0x1C
#define     RFU1D                 0x1D
#define     RFU1E                 0x1E
#define     SerialSpeedReg        0x1F
//PAGE 2
#define     RFU20                 0x20
#define     CRCResultRegM         0x21
#define     CRCResultRegL         0x22
#define     RFU23                 0x23
#define     ModWidthReg           0x24
#define     RFU25                 0x25
#define     RFCfgReg              0x26
#define     GsNReg                0x27
#define     CWGsCfgReg            0x28
#define     ModGsCfgReg           0x29
#define     TModeReg              0x2A
#define     TPrescalerReg         0x2B
#define     TReloadRegH           0x2C
#define     TReloadRegL           0x2D
#define     TCounterValueRegH     0x2E
#define     TCounterValueRegL     0x2F
//PAGE 3
#define     RFU30                 0x30
#define     TestSel1Reg           0x31
#define     TestSel2Reg           0x32
#define     TestPinEnReg          0x33
#define     TestPinValueReg       0x34
#define     TestBusReg            0x35
#define     AutoTestReg           0x36
#define     VersionReg            0x37
#define     AnalogTestReg         0x38
#define     TestDAC1Reg           0x39
#define     TestDAC2Reg           0x3A
#define     TestADCReg            0x3B
#define     RFU3C                 0x3C
#define     RFU3D                 0x3D
#define     RFU3E                 0x3E
#define     RFU3F                    0x3F

//��MF522ͨѶʱ���صĴ������
#define     MI_OK                 0
#define     MI_NOTAGERR           (1)
#define     MI_ERR                (2)

#define    SHAQU1    		0X01
#define    KUAI4    		0X04
#define    KUAI7    		0X07
#define    REGCARD    	0xa1
#define    CONSUME    	0xa2
#define 	 READCARD    	0xa3
#define		 ADDMONEY    	0xa4


void _RC522_delay_ns(uint32_t ns)
{
    uint32_t i;
    for(i=0; i<ns; i++)
    {
        __nop();
        __nop();
        __nop();
    }
}

uint8_t RC522_ReadWriteByte(uint8_t txData);

void RC522_GPIOInit(void)
{
    GPIO_InitType GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(	RC522_SCK_RCC, ENABLE );
    GPIO_InitStructure.GPIO_Pins = RC522_SCK_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP ;  //�������
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
    GPIO_Init(RC522_SCK_PORT, &GPIO_InitStructure);

    RCC_APB2PeriphClockCmd(	RC522_MOSI_RCC, ENABLE );
    GPIO_InitStructure.GPIO_Pins = RC522_MOSI_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP ;  //�������
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
    GPIO_Init(RC522_MOSI_PORT, &GPIO_InitStructure);

    RCC_APB2PeriphClockCmd(	RC522_MISO_RCC, ENABLE );
    GPIO_InitStructure.GPIO_Pins = RC522_MISO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_PU  ;  //��������
    GPIO_Init(RC522_MISO_PORT, &GPIO_InitStructure);

    RCC_APB2PeriphClockCmd(	RC522_CS_RCC, ENABLE );
    GPIO_InitStructure.GPIO_Pins = RC522_CS_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP ;  //�������
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
    GPIO_Init(RC522_CS_PORT, &GPIO_InitStructure);
    RC522_CS_1;	 //SPI ��ѡ��

    RCC_APB2PeriphClockCmd(	RC522_RST_RCC, ENABLE );
    GPIO_InitStructure.GPIO_Pins = RC522_RST_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP ;  //�������
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
    GPIO_Init(RC522_RST_PORT, &GPIO_InitStructure);
    RC522_RST_0;

    RC522_ReadWriteByte(0XFF);
}

uint8_t RC522_ReadWriteByte(uint8_t txData)
{
    uint8_t i;
    uint8_t rxData = 0;
    for(i = 0; i < 8; i++)
    {
        RC522_SCK_0;
        RC522_delay_us(1);
        //���ݷ���
        if(txData & 0x80) {
            RC522_MOSI_1;
        } else {
            RC522_MOSI_0;
        }
        txData <<= 1;
        RC522_delay_us(1);
        RC522_SCK_1;
        RC522_delay_us(1);
        //���ݽ���
        rxData <<= 1;
        if(RC522_READ_MISO) {
            rxData |= 0x01;
        }
        RC522_delay_us(1);
    }
    RC522_SCK_0;
    return rxData;
}



void RC522_Reset(void)
{
    RC522_PcdReset();
    RC522_PcdAntennaOff();
    OSTimeDly(2, OS_OPT_TIME_PERIODIC, &timeerr);
    RC522_PcdAntennaOn();
}

void RC522_Init(void)
{
    uint8_t ver;
    RC522_GPIOInit();
    RC522_Reset();
    RC522_WriteRawRC(0x01,0x0F);  // ---Add
    /* Get RC522 version code */
    ver = RC522_ReadRawRC(VersionReg);

    //while(ver!= 0x82);    // ---Add A32NQ32C3 Ϊ0x82
    printf("Verison:%2X\r\n",ver);
}


//��    �ܣ�Ѱ��
//����˵��: req_code[IN]:Ѱ����ʽ
//                0x52 = Ѱ��Ӧ�������з���14443A��׼�Ŀ�
//                0x26 = Ѱδ��������״̬�Ŀ�
//          pTagType[OUT]����Ƭ���ʹ���
//                0x4400 = Mifare_UltraLight
//                0x0400 = Mifare_One(S50)
//                0x0200 = Mifare_One(S70)
//                0x0800 = Mifare_Pro(X)
//                0x4403 = Mifare_DESFire
//��    ��: �ɹ�����MI_OK
char RC522_PcdRequest(uint8_t req_code,uint8_t *pTagType)
{
    char status;
    uint8_t   unLen;
    uint8_t   ucComMF522Buf[MAXRLEN];

    RC522_ClearBitMask(Status2Reg,0x08);
    RC522_WriteRawRC(BitFramingReg,0x07);
    RC522_SetBitMask(TxControlReg,0x03);

    ucComMF522Buf[0] = req_code;
    status = RC522_PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,1,ucComMF522Buf,&unLen);

    if ((status == MI_OK) && (unLen == 0x10)) {
        *pTagType     = ucComMF522Buf[0];
        *(pTagType+1) = ucComMF522Buf[1];
        status = MI_OK;
    }
    else {
        status = MI_ERR;
    }

    return status;
}

//��    �ܣ�����ײ
//����˵��: pSnr[OUT]:��Ƭ���кţ�4�ֽ�
//��    ��: �ɹ�����MI_OK
char RC522_PcdAnticoll(uint8_t *pSnr)
{
    char   status;
    uint8_t	i,snr_check=0;
    uint8_t	unLen;
    uint8_t	ucComMF522Buf[MAXRLEN];

    RC522_ClearBitMask(Status2Reg,0x08);
    RC522_WriteRawRC(BitFramingReg,0x00);
    RC522_ClearBitMask(CollReg,0x80);

    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x20;

    status = RC522_PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,2,ucComMF522Buf,&unLen);

    if (status == MI_OK) {
        for (i=0; i<4; i++) {
            *(pSnr+i)  = ucComMF522Buf[i];
            snr_check ^= ucComMF522Buf[i];
        }
        if (snr_check != ucComMF522Buf[i]) {
            status = MI_ERR;
        }
    }

    RC522_SetBitMask(CollReg,0x80);
    return status;
}

//��    �ܣ�ѡ����Ƭ
//����˵��: pSnr[IN]:��Ƭ���кţ�4�ֽ�
//��    ��: �ɹ�����MI_OK
char RC522_PcdSelect(uint8_t *pSnr)
{
    char   		status;
    uint8_t   i;
    uint8_t   unLen;
    uint8_t   ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x70;
    ucComMF522Buf[6] = 0;
    for (i=0; i<4; i++) {
        ucComMF522Buf[i+2] = *(pSnr+i);
        ucComMF522Buf[6]  ^= *(pSnr+i);
    }

    RC522_CalulateCRC(ucComMF522Buf,7,&ucComMF522Buf[7]);
    RC522_ClearBitMask(Status2Reg,0x08);

    status = RC522_PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,9,ucComMF522Buf,&unLen);

    if ((status == MI_OK) && (unLen == 0x18)) {
        status = MI_OK;
    }
    else {
        status = MI_ERR;
    }
    return status;
}

//��    �ܣ���֤��Ƭ����
//����˵��: auth_mode[IN]: ������֤ģʽ
//          0x60 = ��֤A��Կ
//          0x61 = ��֤B��Կ
//          addr[IN]�����ַ
//          pKey[IN]������
//          pSnr[IN]����Ƭ���кţ�4�ֽ�
//��    ��: �ɹ�����MI_OK
char RC522_PcdAuthState(uint8_t auth_mode,uint8_t addr,uint8_t *pKey,uint8_t *pSnr)
{
    char   status;
    uint8_t   unLen;
    uint8_t   i,ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = auth_mode;
    ucComMF522Buf[1] = addr;
    for (i=0; i<6; i++) {
        ucComMF522Buf[i+2] = *(pKey+i);
    }
    for (i=0; i<6; i++) {
        ucComMF522Buf[i+8] = *(pSnr+i);
    }

    status = RC522_PcdComMF522(PCD_AUTHENT,ucComMF522Buf,12,ucComMF522Buf,&unLen);
    if ((status != MI_OK) || (!(RC522_ReadRawRC(Status2Reg) & 0x08))) {
        status = MI_ERR;
    }

    return status;
}

//��    �ܣ���ȡM1��һ������
//����˵��: addr[IN]�����ַ
//          p [OUT]�����������ݣ�16�ֽ�
//��    ��: �ɹ�����MI_OK
char RC522_PcdRead(uint8_t addr,uint8_t *p)
{
    char   status;
    uint8_t   unLen;
    uint8_t   i,ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = PICC_READ;
    ucComMF522Buf[1] = addr;
    RC522_CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);

    status = RC522_PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
    if ((status == MI_OK) && (unLen == 0x90)) {
        for (i=0; i<16; i++) {
            *(p +i) = ucComMF522Buf[i];
        }
    }
    else {
        status = MI_ERR;
    }
    return status;
}

//��    �ܣ�д���ݵ�M1��һ��
//����˵��: addr[IN]�����ַ
//          p [IN]��д������ݣ�16�ֽ�
//��    ��: �ɹ�����MI_OK
char RC522_PcdWrite(uint8_t addr,uint8_t *p)
{
    char   status;
    uint8_t   unLen;
    uint8_t   i,ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = PICC_WRITE;
    ucComMF522Buf[1] = addr;
    RC522_CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);

    status = RC522_PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A)) {
        status = MI_ERR;
    }
    if (status == MI_OK) {
        for (i=0; i<16; i++) {
            ucComMF522Buf[i] = *(p +i);
        }
        RC522_CalulateCRC(ucComMF522Buf,16,&ucComMF522Buf[16]);
        status = RC522_PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,18,ucComMF522Buf,&unLen);
        if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A)) {
            status = MI_ERR;
        }
    }
    return status;
}

//��    �ܣ����Ƭ��������״̬
//��    ��: �ɹ�����MI_OK
char RC522_PcdHalt(void)
{
    uint8_t   unLen;
    uint8_t   ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = PICC_HALT;
    ucComMF522Buf[1] = 0;
    RC522_CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);

    RC522_PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
    return MI_OK;
}

//��MF522����CRC16����
void RC522_CalulateCRC(uint8_t *pIn ,uint8_t len,uint8_t *pOut)
{
    uint8_t i,n;
    RC522_ClearBitMask(DivIrqReg,0x04);
    RC522_WriteRawRC(CommandReg,PCD_IDLE);
    RC522_SetBitMask(FIFOLevelReg,0x80);
    for (i=0; i<len; i++) {
        RC522_WriteRawRC(FIFODataReg, *(pIn +i));
    }
    RC522_WriteRawRC(CommandReg, PCD_CALCCRC);
    i = 0xFF;
    do {
        n = RC522_ReadRawRC(DivIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x04));
    pOut [0] = RC522_ReadRawRC(CRCResultRegL);
    pOut [1] = RC522_ReadRawRC(CRCResultRegM);
}

//��    �ܣ���λRC522
//��    ��: �ɹ�����MI_OK
char RC522_PcdReset(void)
{
    RC522_RST_1;
    OSTimeDly(1, OS_OPT_TIME_PERIODIC, &timeerr);
    RC522_RST_0;
    OSTimeDly(1, OS_OPT_TIME_PERIODIC, &timeerr);
    RC522_RST_1;
    OSTimeDly(1, OS_OPT_TIME_PERIODIC, &timeerr);
    RC522_WriteRawRC(CommandReg,PCD_RESETPHASE);
    OSTimeDly(1, OS_OPT_TIME_PERIODIC, &timeerr);

    RC522_WriteRawRC(ModeReg,0x3D);            //��Mifare��ͨѶ��CRC��ʼֵ0x6363
    RC522_WriteRawRC(TReloadRegL,30);
    RC522_WriteRawRC(TReloadRegH,0);
    RC522_WriteRawRC(TModeReg,0x8D);
    RC522_WriteRawRC(TPrescalerReg,0x3E);
    RC522_WriteRawRC(TxAutoReg,0x40);//����Ҫ
    return MI_OK;
}

//����RC632�Ĺ�����ʽ
char RC522_M500PcdConfigISOType(uint8_t type)
{
    if (type == 'A') {                    //ISO14443_A
        RC522_WriteRawRC(ControlReg, 0x10);        //����
        RC522_ClearBitMask(Status2Reg,0x08);
        RC522_WriteRawRC(ModeReg,0x3D);//3F
        RC522_WriteRawRC(RxSelReg,0x86);//84
        RC522_WriteRawRC(RFCfgReg,0x7F);   //4F
        RC522_WriteRawRC(TReloadRegL,30);//tmoLength);// TReloadVal = 'h6a =tmoLength(dec)
        RC522_WriteRawRC(TReloadRegH,0);
        RC522_WriteRawRC(TModeReg,0x8D);
        RC522_WriteRawRC(TPrescalerReg,0x3E);
        _RC522_delay_ns(1000);
        RC522_PcdAntennaOn();
    }
    else {
        return 1;
    }
    return MI_OK;
}

//��    �ܣ���RC632�Ĵ���
//����˵����Address[IN]:�Ĵ�����ַ
//��    �أ�������ֵ
uint8_t RC522_ReadRawRC(uint8_t Address)
{
    uint8_t   ucAddr;
    uint8_t   ucResult=0;
    RC522_CS_0;
    ucAddr = ((Address<<1)&0x7E)|0x80;
    RC522_ReadWriteByte(ucAddr);
    ucResult=RC522_ReadWriteByte(0);
    RC522_CS_1;
    return ucResult;
}

//��    �ܣ�дRC522�Ĵ���
//����˵����Address[IN]:�Ĵ�����ַ
//          value[IN]:д���ֵ
void RC522_WriteRawRC(uint8_t Address, uint8_t value)
{
    uint8_t   ucAddr;

    RC522_CS_0;
    ucAddr = ((Address<<1)&0x7E);
    RC522_ReadWriteByte(ucAddr);
    RC522_ReadWriteByte(value);
    RC522_CS_1;
}

//��    �ܣ���RC522�Ĵ���λ
//����˵����reg[IN]:�Ĵ�����ַ
//          mask[IN]:��λֵ
void RC522_SetBitMask(uint8_t reg,uint8_t mask)
{
    char   tmp = 0x00;
    tmp = RC522_ReadRawRC(reg);
    RC522_WriteRawRC(reg,tmp | mask);  // set bit mask
}

//��    �ܣ���RC522�Ĵ���λ
//����˵����reg[IN]:�Ĵ�����ַ
//          mask[IN]:��λֵ
void RC522_ClearBitMask(uint8_t reg, uint8_t mask)
{
    char tmp = 0x00;
    tmp = RC522_ReadRawRC(reg);
    RC522_WriteRawRC(reg, tmp & ~mask);  // clear bit mask
}

//��    �ܣ�ͨ��RC522��ISO14443��ͨѶ
//����˵����Command[IN]:RC522������
//          pIn [IN]:ͨ��RC522���͵���Ƭ������
//          InLenByte[IN]:�������ݵ��ֽڳ���
//          pOut [OUT]:���յ��Ŀ�Ƭ��������
//          *pOutLenBit[OUT]:�������ݵ�λ����
char RC522_PcdComMF522(uint8_t Command,uint8_t *pIn,uint8_t InLenByte,uint8_t *pOut,uint8_t *pOutLenBit)
{
    char status = MI_ERR;
    uint8_t   irqEn   = 0x00;
    uint8_t   waitFor = 0x00;
    uint8_t   lastBits;
    uint8_t   n;
    uint16_t  i;
    switch (Command) {
    case PCD_AUTHENT:
        irqEn   = 0x12;
        waitFor = 0x10;
        break;
    case PCD_TRANSCEIVE:
        irqEn   = 0x77;
        waitFor = 0x30;
        break;
    default:
        break;
    }

    RC522_WriteRawRC(ComIEnReg,irqEn|0x80);
    RC522_ClearBitMask(ComIrqReg,0x80);    //�������ж�λ
    RC522_WriteRawRC(CommandReg,PCD_IDLE);
    RC522_SetBitMask(FIFOLevelReg,0x80);         //��FIFO����

    for (i=0; i<InLenByte; i++) {
        RC522_WriteRawRC(FIFODataReg, pIn [i]);
    }
    RC522_WriteRawRC(CommandReg, Command);

    if (Command == PCD_TRANSCEIVE) {
        RC522_SetBitMask(BitFramingReg,0x80);
    }     //��ʼ����

    i = 20000;//����ʱ��Ƶ�ʵ���������M1�����ȴ�ʱ��25ms
    do {
        n = RC522_ReadRawRC(ComIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x01) && !(n&waitFor));
    RC522_ClearBitMask(BitFramingReg,0x80);

    if (i!=0) {
        if(!(RC522_ReadRawRC(ErrorReg)&0x1B)) {             //��������
            status = MI_OK;
            if (n & irqEn & 0x01) {
                status = MI_NOTAGERR;
            }
            if (Command == PCD_TRANSCEIVE) {
                n = RC522_ReadRawRC(FIFOLevelReg);
                lastBits = RC522_ReadRawRC(ControlReg) & 0x07;
                if(lastBits) {
                    *pOutLenBit = (n-1)*8 + lastBits;
                }
                else {
                    *pOutLenBit = n*8;
                }
                if (n == 0) {
                    n = 1;
                }
                if (n > MAXRLEN) {
                    n = MAXRLEN;
                }
                for (i=0; i<n; i++)
                {
                    pOut [i] = RC522_ReadRawRC(FIFODataReg);
                    //printf("pOut=%x\r\n",pOut[i]);
                }
            }
        }
        else {
            status = MI_ERR;
        }
    }
    RC522_SetBitMask(ControlReg,0x80);           // stop timer now
    RC522_WriteRawRC(CommandReg,PCD_IDLE);
    return status;
}


//��������
//ÿ��������ر����շ���֮��Ӧ������1ms�ļ��
void RC522_PcdAntennaOn(void)
{
    uint8_t   i;
    i = RC522_ReadRawRC(TxControlReg);
    if (!(i & 0x03)) {
        RC522_SetBitMask(TxControlReg, 0x03);
    }
}

//�ر�����
void RC522_PcdAntennaOff(void)
{
    RC522_ClearBitMask(TxControlReg, 0x03);
}


//��    �ܣ��ۿ�ͳ�ֵ
//����˵��: dd_mode[IN]��������
//               0xC0 = �ۿ�
//               0xC1 = ��ֵ
//          addr[IN]��Ǯ����ַ
//          pValue[IN]��4�ֽ���(��)ֵ����λ��ǰ
//��    ��: �ɹ�����MI_OK
char RC522_SetValue(uint8_t addr,uint32_t Value)
{
    uint8_t buffer[18];

    // Translate the int32_t into 4 bytes; repeated 2x in value block
    buffer[0] = buffer[ 8] = (Value & 0xFF);
    buffer[1] = buffer[ 9] = (Value & 0xFF00) >> 8;
    buffer[2] = buffer[10] = (Value & 0xFF0000) >> 16;
    buffer[3] = buffer[11] = (Value & 0xFF000000) >> 24;
    // Inverse 4 bytes also found in value block
    buffer[4] = ~buffer[0];
    buffer[5] = ~buffer[1];
    buffer[6] = ~buffer[2];
    buffer[7] = ~buffer[3];
    // Address 2x with inverse address 2x
    buffer[12] = buffer[14] = addr;
    buffer[13] = buffer[15] = ~addr;

    // Write the whole data block
    return RC522_WriteCard(addr, buffer);
}

//��    �ܣ��ۿ�ͳ�ֵ
//����˵��: dd_mode[IN]��������
//               0xC0 = �ۿ�
//               0xC1 = ��ֵ
//          addr[IN]��Ǯ����ַ
//          pValue[IN]��4�ֽ���(��)ֵ����λ��ǰ
//��    ��: �ɹ�����MI_OK
char RC522_PcdValue(uint8_t dd_mode,uint8_t addr,uint8_t *pValue)
{
    char status;
    uint8_t  unLen;
    uint8_t ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = dd_mode;
    ucComMF522Buf[1] = addr;
    RC522_CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);

    status = RC522_PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A)) {
        status = MI_ERR;
    }
    if (status == MI_OK) {
        ucComMF522Buf[0] = pValue[0];
        ucComMF522Buf[1] = pValue[1];
        ucComMF522Buf[2] = pValue[2];
        ucComMF522Buf[3] = pValue[3];
        RC522_CalulateCRC(ucComMF522Buf,4,&ucComMF522Buf[4]);
        unLen = 0;
        status = RC522_PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,6,ucComMF522Buf,&unLen);
        if (status != MI_ERR) {
            status = MI_OK;
        }
    }
    if (status == MI_OK) {
        ucComMF522Buf[0] = PICC_TRANSFER;
        ucComMF522Buf[1] = addr;
        RC522_CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
        status = RC522_PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
        if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A)) {
            status = MI_ERR;
        }
    }
    return status;
}

//��    �ܣ�����Ǯ��
//����˵��: sourceaddr[IN]��Դ��ַ
//          goaladdr[IN]��Ŀ���ַ
//��    ��: �ɹ�����MI_OK
char RC522_PcdBakValue(uint8_t sourceaddr, uint8_t goaladdr)
{
    char status;
    uint8_t  unLen;
    uint8_t ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = PICC_RESTORE;
    ucComMF522Buf[1] = sourceaddr;
    RC522_CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
    status = RC522_PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A)) {
        status = MI_ERR;
    }

    if (status == MI_OK) {
        ucComMF522Buf[0] = 0;
        ucComMF522Buf[1] = 0;
        ucComMF522Buf[2] = 0;
        ucComMF522Buf[3] = 0;
        RC522_CalulateCRC(ucComMF522Buf,4,&ucComMF522Buf[4]);
        status = RC522_PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,6,ucComMF522Buf,&unLen);
        if (status != MI_ERR) {
            status = MI_OK;
        }
    }
    if (status != MI_OK) {
        return MI_ERR;
    }
    ucComMF522Buf[0] = PICC_TRANSFER;
    ucComMF522Buf[1] = goaladdr;
    RC522_CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A)) {
        status = MI_ERR;
    }
    return status;
}

//��������
//Card_ID ����ID
//Card_addr ���ĵ�ַ��16λ���ݣ�1,2,4,5,6,8,9,10��ѡ
//Card_Data ������ַ������
uint8_t RC522_ReadCard(uint8_t Card_addr,uint8_t *Card_Data)
{

    uint8_t CT[2]; //������
    uint8_t SN[4]; //����
    uint8_t status2;
    uint8_t i = 0;
    for(i=0; i<3; i++) {
        status2 = RC522_PcdRequest(0x52,CT);//Ѱ��,���Ϊ������
        if(status2==MI_OK) { //Ѱ���ɹ�
            status2 = RC522_PcdAnticoll(SN);  //����ײ���������Ƭ���кţ�4�ֽ�--��0������0��ǰ4���ֽ���UID�����кţ�
            break;
        }
    }
    if(i == 3) return 1;
    if(status2==MI_OK) { //����ײ�ɹ�
        status2 = RC522_PcdSelect(SN);  //ѡ��
    }
    else
        return 2;
    if(status2==MI_OK) { //ѡ���ɹ�
        status2 = RC522_PcdAuthState(0x60, Card_addr, Card_KEY, SN);//��֤��
    }
    else
        return 3;
    if(status2==MI_OK) { //��֤���ɹ�
        status2 = RC522_PcdRead(Card_addr,Card_Data);
        if(status2==MI_OK) return 0;
        else return 4;
    }
    else
        return 5;
}

//д������
//Card_addr ���ĵ�ַ��16λ���ݣ�1,2,4,5,6,8,9,10��ѡ
//Card_Data д��������
uint8_t RC522_WriteCard(uint8_t Card_addr, uint8_t *Card_Data)
{
    uint8_t status2;
    uint8_t CT[2]; //������
    uint8_t SN[4]; //����
    uint8_t i = 0;
    for(i=0; i<3; i++) {
        status2 = RC522_PcdRequest(0x52,CT);//Ѱ��,���Ϊ������
        if(status2==MI_OK) { //Ѱ���ɹ�
            status2 = RC522_PcdAnticoll(SN);  //����ײ���������Ƭ���кţ�4�ֽ�--��0������0��ǰ4���ֽ���UID�����кţ�
            break;
        }
    }
    if(i == 3) return 1;
    if(status2==MI_OK) { //����ײ�ɹ�
        status2 = RC522_PcdSelect(SN);  //ѡ��
    }
    else
        return 2;
    if(status2==MI_OK) { //ѡ���ɹ�
        status2 = RC522_PcdAuthState(0x60, Card_addr, Card_KEY, SN);//��֤��
    }
    else
        return 3;
    if(status2==MI_OK) { //��֤���ɹ�
        status2 = RC522_PcdWrite(Card_addr, Card_Data);
        if(status2==MI_OK) return 0;
        else return 4;
    }
    else
        return 5;
}
