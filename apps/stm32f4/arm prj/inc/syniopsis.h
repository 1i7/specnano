#include <stm32f4xx.h>


typedef struct {
  float X_offset;
  float Y_offset;
  float Z_offset;
  int   NumOfPositions;
  char  NewPointOffsetX[2000];
  char  NewPointOffsetY[2000];
  char  NewPointOffsetZ[2000];
} MovementPosition;



extern  void            Delay(__IO uint32_t nCount);
// ������� ������������� �������
extern  void            init_SPI1(void);
extern  void            MotorInit(void);

extern  uint8_t         MySPI_Send(uint8_t data);
extern  uint8_t         MySPI_Read();
extern  uint8_t         SD_sendCommand(uint8_t cmd, uint32_t arg);
extern  uint8_t         SD_init(void);
extern  uint8_t         SD_ReadSector(uint32_t BlockNumb,uint8_t *buff);
extern  uint8_t         SD_WriteSector(uint32_t BlockNumb,uint8_t *buff);
extern  uint8_t         SD_WaitFree(void);

// ������� ������ �� ������� �������
extern  char            fgetc(void);
extern  void            ungetc(void);


extern  void            Parser_Init(void);
extern  unsigned char   Parse_Command(void);
extern  void            Track(void);
extern  MovementPosition        MovP;

extern  uint8_t         SDHC;
extern  uint8_t         SD_Buff[512];
extern  char            MyBuff[100];            // ����� ��� ��������� ������ � ��������
extern  char*           MyBuffAddr;             // ��������� �� ��������� ����������� ������
extern  char            MyBuffPointer;          // ��������� �� ������� ������ ������