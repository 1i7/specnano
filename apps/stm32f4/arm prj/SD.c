#include <stm32f4xx.h>
#include <stm32f4xx_spi.h>
#include <syniopsis.h>
#include <SD.h>

uint8_t  SDHC;
uint8_t  SD_Buff[512];

//********************************************************************************************
//function	 ������ ��������� �� ���� ������ � �������� ������
//return	 ����������� ������
//********************************************************************************************
void ungetc(void){
  // ������������ ���������
  MyBuffPointer--;
}
//********************************************************************************************
//function	 ������ ���������� ������� �� �������� ������
//return	 ����������� ������
//********************************************************************************************
char fgetc(void){
  char  ReadedChar;
  ReadedChar = MyBuff[MyBuffPointer];
  // ������������� ���������
  MyBuffPointer++;
  return ReadedChar;
}
//********************************************************************************************
//function	 �������� ������������ SD �����
//return	 0 - �� ������
//********************************************************************************************
uint8_t SD_WaitFree(){
  uint16_t    wait=0;

  CS_LOW;
  while(!MySPI_Read())               //���� ����� ������,��� ������ ����
    if(wait++ > 0xfffe){CS_HIGH; return 0xff;}
  CS_HIGH;
  return 0;
}
//********************************************************************************************
//function	 ������ ���������� ������� SD
//�rguments	 ����� �������, ��������� �� ������ ��� ������
//return	 0 - ������ ������� �������
//********************************************************************************************
uint8_t SD_WriteSector(uint32_t BlockNumb,uint8_t *buff){
  uint8_t     response;
  uint16_t    i,wait=0;
 
  //������� ������� "������ ������ �����" � ��������� ��� ������
  if( SD_sendCommand(WRITE_SINGLE_BLOCK, BlockNumb)) return 1;
 
  CS_LOW;
  MySPI_Send(0xfe);    
 
  //�������� ����� ������� � �����
  for(i=0; i<512; i++) MySPI_Send(*buff++);
 
  MySPI_Send(0xff);                //������ 2 ����� CRC ��� ��� ��������
  MySPI_Send(0xff);
 
  response = MySPI_Read();
 
  if( (response & 0x1f) != 0x05) //���� ������ ��� ������ ������ ������
  { CS_HIGH; return 1; }
 
  //������� ��������� ������ ����� ������
  while(!MySPI_Read())             //���� ����� ������,��� ������ ����
  if(wait++ > 0xfffe){CS_HIGH; return 1;}
 
  CS_HIGH;
  MySPI_Send(0xff);   
  CS_LOW;         
 
  while(!MySPI_Read())               //���� ����� ������,��� ������ ����
  if(wait++ > 0xfffe){CS_HIGH; return 1;}
  CS_HIGH;
 
  return 0;
}
//********************************************************************************************
//function	 ������ ���������� ������� SD
//�rguments	 ����� �������,��������� �� ����� �������� 512 ����
//return	 0 - ������ �������� �������
//********************************************************************************************
uint8_t SD_ReadSector(uint32_t BlockNumb,uint8_t *buff){ 
  uint16_t i=0;
 
  //������� ������� "������ ������ �����" � ��������� ��� ������
  if(SD_sendCommand(READ_SINGLE_BLOCK, BlockNumb)) return 1;  
  CS_LOW;
  //��������  ������� ������
  while(MySPI_Read() != 0xfe)                
  if(i++ > 0xfffe) {CS_HIGH; return 1;}       
 
  //������ 512 ����	���������� �������
  for(i=0; i<512; i++) *buff++ = MySPI_Read();
 
  MySPI_Read(); 
  MySPI_Read(); 
  MySPI_Read(); 
 
  CS_HIGH;
 
  return 0;
}
//********************************************************************************************
//function	 ������������� ����� ������
//return	 0 - ����� ����������������
//********************************************************************************************
uint8_t SD_init(void){
  uint8_t   i;
  uint8_t   response;
  uint8_t   SD_version = 2;	          //�� ��������� ������ SD = 2
  uint16_t  retry = 0 ;
 
  for(i=0;i<10;i++) MySPI_Read();      //������� ����� 74 ������   
 
  //�������� ����������� ����� �����
  while(SD_sendCommand(GO_IDLE_STATE, 0)!=0x01)
    if(retry++>0x20)  return 1;
  MySPI_Read();
  MySPI_Read();
 
  retry = 0;                                     
  while(SD_sendCommand(SEND_IF_COND,0x000001AA)!=0x01)
  { 
    if(retry++>0xfe) 
    { 
      SD_version = 1;
      break;
    } 
  }
 
  retry = 0;                                     
  do
  {
    response = SD_sendCommand(APP_CMD,0); 
    response = SD_sendCommand(SD_SEND_OP_COND,0x40000000);
    retry++;
    if(retry>0xffe) return 1;                     
  }while(response != 0x00);                      
 
 
  //������ ������� OCR, ����� ���������� ��� �����
  retry = 0;
  SDHC = 0;
  if (SD_version == 2)
  { 
    while(SD_sendCommand(READ_OCR,0)!=0x00)
	 if(retry++>0xfe)  break;
  }
 
  return 0; 
}
//********************************************************************************************
//function	 ������� ������� � SD
//Arguments	 ������� � �� ��������
//return	 0xff - ��� ������
//********************************************************************************************
uint8_t SD_sendCommand(uint8_t cmd, uint32_t arg){
  uint8_t response, wait=0, tmp;     
 
  //��� ���� ������ SD ��������� �������� ������, �.�. ��� ��� ��������� ��������� 
  if(SDHC == 0){
    if(cmd == READ_SINGLE_BLOCK || cmd == WRITE_SINGLE_BLOCK )arg = arg << 9;
  }
  //��� SDHC �������� ������ ����� ��������� �� �����(������������ ���������)	
 
  CS_LOW;
 
  //�������� ��� ������� � �� ��������
  MySPI_Send(cmd | 0x40);
  MySPI_Send(arg>>24);
  MySPI_Send(arg>>16);
  MySPI_Send(arg>>8);
  MySPI_Send(arg);
 
  //�������� CRC (��������� ������ ��� ���� ������)
  if(cmd == SEND_IF_COND){
    MySPI_Send(0x87);
  }else{
    MySPI_Send(0x95);
  }
 
  //������� �����
  while((response = MySPI_Read()) == 0xff)
  {
    if(wait++ > 0xfe) break;                //�������, �� �������� ����� �� �������
  }
 
  //�������� ������ ���� ���������� ������� READ_OCR
  if(response == 0x00 && cmd == 58)     
  {
    tmp = MySPI_Read();                      //�������� ���� ���� �������� OCR            
    if(tmp & 0x40) SDHC = 1;               //���������� ����� SDHC 
    else           SDHC = 0;               //���������� ����� SD
    //��������� ��� ���������� ����� �������� OCR
    MySPI_Read(); 
    MySPI_Read(); 
    MySPI_Read(); 
  }
 
  MySPI_Read();
 
  CS_HIGH; 
 
  return response;
}
//********************************************************************************************
// ���� ������ ����� ������
//********************************************************************************************
uint8_t MySPI_Read(){
  return MySPI_Send(0xff);
}
//********************************************************************************************
// ��������/���� ������ ����� ������
//********************************************************************************************
uint8_t MySPI_Send(uint8_t data){
  SPI1->DR = data; // ������� ������������ ������ � ������� ������ SPI
  while( !(SPI1->SR & SPI_I2S_FLAG_TXE) ); // ��� ���������� ��������
  while( !(SPI1->SR & SPI_I2S_FLAG_RXNE) ); // ��� ���������� �����
  while( SPI1->SR & SPI_I2S_FLAG_BSY ); // ��� ������������ SPI
  return SPI1->DR; // ���������� �������� ������ �� �������� ������ SPI
}
//********************************************************************************************
// ������������� SPI
//********************************************************************************************
void init_SPI1(void){
  GPIO_InitTypeDef GPIO_InitStruct;
  SPI_InitTypeDef SPI_InitStruct;

  // �������� ������������ ��� ����� A
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

  /* ����, ������������ SPI1
  * PA5 = SCK
  * PA6 = MISO
  * PA7 = MOSI
  */
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_6 | GPIO_Pin_5;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStruct);

  // ��������� ������ SPI1 � �������������� ������� �����
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);

  /* ��� ������� �������� (Chip Select) ���������� ��� 4 ����� A (PA4)*/
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOA, &GPIO_InitStruct);

  CS_HIGH;      // ��������� CS � 1

  // ������� ������������ ������ SPI
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

  /* �������������� SPI1 � Mode 0 
  * CPOL = 0 --> ���� ������������ � ��������� 0 � ������ �����
  * CPHA = 0 --> ������� ������ �� ��������� ������
  */
  SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex; // ��������������� �����, ���������� ����� MOSI � MISO
  SPI_InitStruct.SPI_Mode = SPI_Mode_Master;     // ������ � ������ �������, ����� NSS ������ 1
  SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b; // 8-� ������ ����� ������ (����� ���� 16-� ������)
  SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;        // ���� ������������ � ��������� 0 � ������ �����
  SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;      // ������� ������ �� ��������� ������
  SPI_InitStruct.SPI_NSS = SPI_NSS_Soft | SPI_NSSInternalSoft_Set; // ���������� ���������� NSS, ��������� ������ � 1
  SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128; // ������� SPI = ������� APB2 / 4
  SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;// ������� ��� ������
  SPI_Init(SPI1, &SPI_InitStruct); 

  SPI_Cmd(SPI1, ENABLE); // �������� SPI1
}