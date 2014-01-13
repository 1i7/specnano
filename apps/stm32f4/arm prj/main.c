#include <stm32f4xx.h>
//#include <stm32f4xx_spi.h>
#include <syniopsis.h>
#include <ff.h>
#include <diskio.h>

#include <stdio.h>
#include "file_processing_method.h"
#include "gcode.h"
#include "G_commands.h"
#include "g01.h"
//#include "g02.h"
//#include "g03.h"

// ��� �����, �� �������� ������ �������
#define FileName        "0:proba.txt"
// ��� ����� ��� ����
#define FileNameLog     "0:log.txt"

GPIO_InitTypeDef  GPIO_InitStructure;
TIM_TimeBaseInitTypeDef TimerStructure;
FATFS         FileSystem;
FIL           MyFile;
FIL           MyFileLog;
char          MyBuff[100];            // ����� ��� ��������� ������ � ��������
char          MyBuffPointer;          // ��������� �� ������� ������ ������
char*         MyBuffAddr;             // ��������� �� ��������� ����������� ������
uint8_t       GoodCommands = 0;       // ������� ����� � "�����������" ���������
uint8_t       BadCommands = 0;        // ������� "�����" �����
MovementPosition        MovP;

int handler_result_svg(double dx, double dy, double dz, double f) {
  //printf("%f,%f ", dx, dy);
}

//********************************************************************************************
// �������� ������ - ���� � ���������
//********************************************************************************************
void main(void)
{
  DWORD EOF_Identeficator;
  // ���������� ����������
  unsigned long FileSize;               // ������ �����
  unsigned int  LineCouter = 0;         // ������� �����, ����������� �� �����
  DSTATUS       res;                    // ��������� ������ ������� FatFS

  handler_g01 = &gcode_g01;
  //handler_g02 = &gcode_g02;
  //handler_g03 = &gcode_g03;
  handler_gcommand_result = &handler_result_svg;
    
  // ��������� ������������� �������
  //Parser_Init();
  
  MotorInit();
  
  __enable_irq(); //���������� ��������� ����������
  
  // ������������� SPI
  init_SPI1();
  
  // ������������� �����
  res = disk_initialize(0);
  res = f_mount(0, &FileSystem);        // ���������
  res = f_open (&MyFile, FileName, FA_READ);       //��������� ����
  res = f_open (&MyFileLog, FileNameLog, FA_WRITE | FA_CREATE_ALWAYS);       //��������� ���� ����
  FileSize = f_size(&MyFile);           // �������� ������� ����� (�� �����, ������ ��� �������)
  res = f_printf( &MyFileLog, "������ G-�����.\n");
  res = f_printf( &MyFileLog, "���������: STM32\n");
  res = f_printf( &MyFileLog, "�����������: ��������� �������\n");
  res = f_printf( &MyFileLog, "������: 0.1.0.0 \n\n\n");
  res = f_printf( &MyFileLog, "���: \n\n");
  while(1){
    // ������ ��������� ������
    MyBuffAddr = (char*)(f_gets((TCHAR*)&MyBuff, (int)sizeof(MyBuff), &MyFile));
    MyBuffPointer = 0;
    EOF_Identeficator = f_eof(&MyFile);
    //if(EOF_Identeficator) break;  // ��� ������ - �������
    if(MyBuffAddr==0) break;
    LineCouter++;
    // ���� ������ ��������� - ����������� � �������
    if(parse_start()){
      // ����� ���� �������... )
      BadCommands++;
      // �� �������� ������������� � ����
      res = f_printf( &MyFileLog, "������ �%d", LineCouter);
      res = f_printf( &MyFileLog, " �������� ������!\n");
    }else{
      // ������ � �������� ������� ����������
      GoodCommands++;
      // �� �������� ������������� � ����
      res = f_printf( &MyFileLog, "������ �%d", LineCouter);
      res = f_printf( &MyFileLog, " ������� ����������.\n");
    }
  }
  res = f_close (&MyFile);      // ��������� ����
  res = f_close (&MyFileLog);      // ��������� ���� ����
  res = f_mount(0, 0);          // ��������� ����
  
  // �������
  while(1);
}
//********************************************************************************************
// ������� ��������
//********************************************************************************************
void Delay(__IO uint32_t nCount){
  while(nCount--);
}