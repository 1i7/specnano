#include <syniopsis.h>

extern  GPIO_InitTypeDef  GPIO_InitStructure;

const unsigned char RotStepTable[4] = {0x08, 0x02, 0x04, 0x01};
char RotorStateX = 0, RotorStateY = 0, RotorStateZ = 0;
unsigned char RotorX_offset = 7, RotorY_offset = 11, RotorZ_offset = 9;

void StepMotorX(char Direction);
void StepMotorY(char Direction);
void StepMotorZ(char Direction);
void StepMotorDelay(void);
//********************************************************************************************
// ������������� ������� ���������
//********************************************************************************************
void MotorInit(void){
  int i;
  // �������� ������������ ��� ����� E (������ X � Y)
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
  // �������� ������������ ��� ����� A (����� Z)
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

  GPIO_InitStructure.GPIO_Pin = (0x000f<<RotorX_offset)|(0x000f<<RotorY_offset);
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOE, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = (0x000f<<RotorZ_offset);
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOD, &GPIO_InitStructure);  
  
  for(i=0;i<50;i++){
    StepMotorX(1);
    StepMotorY(1);
    StepMotorZ(1);
    StepMotorDelay();
  }
  for(i=0;i<50;i++){
    StepMotorX(-1);
    StepMotorY(-1);
    StepMotorZ(-1);
    StepMotorDelay();
  }
}
//********************************************************************************************
// �������� ���������� � ������������ � ����������� ������� �����
//********************************************************************************************
void Track(void){
  int Counter;

  for(Counter=0;Counter<MovP.NumOfPositions;Counter++){
      // ��� �� X-���
    if(MovP.NewPointOffsetX[Counter]!=0)StepMotorX(MovP.NewPointOffsetX[Counter]);
      // ��� �� Y-���
    if(MovP.NewPointOffsetY[Counter]!=0)StepMotorY(MovP.NewPointOffsetY[Counter]);
      // ��� �� Z-���
    if(MovP.NewPointOffsetZ[Counter]!=0)StepMotorZ(MovP.NewPointOffsetZ[Counter]);
    StepMotorDelay();
  }  
}
//********************************************************************************************
// ������� ��������� X �� ���� ���
//********************************************************************************************
void StepMotorX(char Direction){
  unsigned int Pins;
  RotorStateX +=Direction;
  if(RotorStateX<0)RotorStateX = 3;
  if(RotorStateX>3)RotorStateX = 0;
  // ���������� ����������� ���� � ����
  Pins = (0x0f&(~RotStepTable[RotorStateX]))<<RotorX_offset;
  GPIO_ResetBits(GPIOE, Pins);
  // � ����������� ��������� � �������
  Pins = (RotStepTable[RotorStateX])<<RotorX_offset;
  GPIO_SetBits(GPIOE, Pins);
}
//********************************************************************************************
// ������� ��������� Y �� ���� ���
//********************************************************************************************
void StepMotorY(char Direction){
  unsigned int Pins;
  RotorStateY +=Direction;
  if(RotorStateY<0)RotorStateY = 3;
  if(RotorStateY>3)RotorStateY = 0;
  // ���������� ����������� ���� � ����
  Pins = (0x0f&(~RotStepTable[RotorStateY]))<<RotorY_offset;
  GPIO_ResetBits(GPIOE, Pins);
  // � ����������� ��������� � �������
  Pins = (RotStepTable[RotorStateY])<<RotorY_offset;
  GPIO_SetBits(GPIOE, Pins);
}
//********************************************************************************************
// ������� ��������� Z �� ���� ���
//********************************************************************************************
void StepMotorZ(char Direction){
  unsigned int Pins;
  RotorStateZ +=Direction;
  if(RotorStateZ<0)RotorStateZ = 3;
  if(RotorStateZ>3)RotorStateZ = 0;
  // ���������� ����������� ���� � ����
  Pins = (0x0f&(~RotStepTable[RotorStateZ]))<<RotorZ_offset;
  GPIO_ResetBits(GPIOD, Pins);
  // � ����������� ��������� � �������
  Pins = (RotStepTable[RotorStateZ])<<RotorZ_offset;
  GPIO_SetBits(GPIOD, Pins);
}
//********************************************************************************************
// �������� �������� ������
//********************************************************************************************
void StepMotorDelay(void){
  Delay(100000);
}