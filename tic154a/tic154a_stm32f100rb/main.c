#include<stm32f10x_gpio.h>
#include<stm32f10x_rcc.h>
#include<stm32f10x_i2c.h>

typedef struct {
	const void * pvWBuf;
	unsigned long ulWLen;
	unsigned long ulWCount;
	void * pvRBuf;
	unsigned long ulRLen;
	unsigned long ulRCount;
} tI2CSlaveTransferCfg;

typedef struct {
	unsigned long ulSlave;
	const void * pvWBuf;
	unsigned long ulWLen;
	unsigned long ulWCount;
	void * pvRBuf;
	unsigned long ulRLen;
	unsigned long ulRCount;
} tI2CMasterTransferCfg;

#define _I2C	I2C1

#define I2C1_DISPLAY_ADDRESS    0x70

void Delay(volatile uint32_t nCount) {
	for (; nCount != 0; nCount--)
		;
}
void delay(uint32_t ms) {
	volatile uint32_t nCount;
	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);
	nCount = (RCC_Clocks.HCLK_Frequency / 10000) * ms;
	for (; nCount != 0; nCount--)
		;
}

void setupRST() {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOC, &GPIO_InitStructure);

}

void showDebug(int val) {
	if (val) {
		GPIO_SetBits(GPIOC, GPIO_Pin_9);
	} else {
		GPIO_ResetBits(GPIOC, GPIO_Pin_9);
	}
}

void setRST(int val) {
	if (val) {
		GPIO_SetBits(GPIOC, GPIO_Pin_8);
	} else {
		GPIO_ResetBits(GPIOC, GPIO_Pin_8);
	}
}

void writeI2C(uint8_t b) {
	I2C_GenerateSTART(_I2C, ENABLE);
	while (!I2C_CheckEvent(_I2C, I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress(_I2C, 2, I2C_Direction_Transmitter);
	while (!I2C_CheckEvent(_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	I2C_SendData(_I2C, b);
	while (!I2C_CheckEvent(_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	showDebug(0); //GPIOC->ODR = 0;
	I2C_GenerateSTOP(_I2C, ENABLE);
	delay(5);
}

uint8_t readI2C() {
	I2C_GenerateSTART(_I2C, DISABLE);
	while (!I2C_CheckEvent(_I2C, I2C_EVENT_SLAVE_RECEIVER_ADDRESS_MATCHED));
	showDebug(0); //GPIOC->ODR = 0;
	uint8_t byte = I2C_ReceiveData(_I2C);
	while (!I2C_CheckEvent(_I2C, I2C_EVENT_SLAVE_BYTE_RECEIVED));
	I2C_AcknowledgeConfig(_I2C, DISABLE);

	return byte;
}

void testI2CIO() {
	int mode_write = 1;
	if (mode_write) {
		writeI2C(177);
	} else {
		uint8_t answer;
		answer = readI2C();
		if (answer == 177) {
			showDebug(0);
		};
	}
}

//
void int_LCD(void) {
	delay(1000);
	GPIO_SetBits(GPIOC, GPIO_Pin_8);

	delay(100);

	I2C_GenerateSTART(_I2C, ENABLE);
	while (!I2C_CheckEvent(_I2C, I2C_EVENT_MASTER_MODE_SELECT));
	//showDebug(0);
	I2C_Send7bitAddress(_I2C, I2C1_DISPLAY_ADDRESS, I2C_Direction_Transmitter);
	while (!I2C_CheckEvent(_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	//showDebug(0);
	I2C_SendData(_I2C, 0b11100010);
	while (!I2C_CheckEvent(_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	//showDebug(0);
	//I2C_GenerateSTART(_I2C, DISABLE);
	I2C_GenerateSTOP(_I2C, ENABLE);
	//I2C_GenerateSTOP(_I2C, DISABLE);

	delay(10);

	I2C_GenerateSTART(_I2C, ENABLE);
	while (!I2C_CheckEvent(_I2C, I2C_EVENT_MASTER_MODE_SELECT));
	//showDebug(0);
	I2C_Send7bitAddress(_I2C, I2C1_DISPLAY_ADDRESS, I2C_Direction_Transmitter);
	while (!I2C_CheckEvent(_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	//showDebug(0);
	I2C_SendData(_I2C, 0b11101011);
	while (!I2C_CheckEvent(_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	//showDebug(0);
	I2C_SendData(_I2C, 0b10000001);
	while (!I2C_CheckEvent(_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	I2C_SendData(_I2C, 120);
	while (!I2C_CheckEvent(_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	I2C_SendData(_I2C, 0b11000110);
	while (!I2C_CheckEvent(_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	I2C_SendData(_I2C, 0b10101111);
	while (!I2C_CheckEvent(_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	//showDebug(0);
	//I2C_GenerateSTART(_I2C, DISABLE);
	I2C_GenerateSTOP(_I2C, ENABLE);
	//I2C_GenerateSTOP(_I2C, DISABLE);

//	RSETLCD=1;				// ��������� ������� �� ������
//	_delay(160000);			// 16 000 = 1�� - 10��
//	i2c_start(0x70,0,0);	// 0x70 ����� ����������, 0-�������, 0-������
//	i2c_write(0b11100010); 	//
//	i2c_stop();
//	_delay(160000);			// 16 000 = 1�� - 10��
//	i2c_start(0x70,0,0);	// 0x70 ����� ����������, 0-�������, 0-������
//	i2c_write(0b11101011); 	// BIAS 6
//	i2c_write(0b10000001);  // ��������� Vbias
//	i2c_write(120);			// 124
//	i2c_write(0b11000110);  // ��������� ���� �������� ����� � ���, � ����� �� �����
//	i2c_write(0b10101111);	// �������� �������
//	i2c_stop();
//
	//clear_LCD(0); // ������� �������
} //

//
void clear_LCD(char tip) {
	int a;

	I2C_GenerateSTART(_I2C, ENABLE);
	while (!I2C_CheckEvent(_I2C, I2C_EVENT_MASTER_MODE_SELECT));
	//showDebug(0);
	I2C_Send7bitAddress(_I2C, I2C1_DISPLAY_ADDRESS, I2C_Direction_Transmitter);
	while (!I2C_CheckEvent(_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	showDebug(0);
	I2C_SendData(_I2C, 0b10110000);
	while (!I2C_CheckEvent(_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	//showDebug(0);
	I2C_SendData(_I2C, 0b00000000);
	while (!I2C_CheckEvent(_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	//showDebug(0);
	I2C_SendData(_I2C, 0b00010000);
	while (!I2C_CheckEvent(_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	//showDebug(0);
	//I2C_GenerateSTART(_I2C, DISABLE);
	I2C_GenerateSTOP(_I2C, ENABLE);
	//I2C_GenerateSTOP(_I2C, DISABLE);

	I2C_GenerateSTART(_I2C, ENABLE);
	while (!I2C_CheckEvent(_I2C, I2C_EVENT_MASTER_MODE_SELECT));
	showDebug(0);
	I2C_Send7bitAddress(_I2C, I2C1_DISPLAY_ADDRESS,
			(uint8_t) 0x02/*I2C_Direction_Transmitter*/);
	while (!I2C_CheckEvent(_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	showDebug(0);

	if (tip == 0) {
		for (a = 0; a < 1056; a++) {
			I2C_SendData(_I2C, 0b00000000);
			while (!I2C_CheckEvent(_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
		}
		//showDebug(0);
	} else if (tip == 1) {
		for (a = 0; a < 1056; a++) {
			I2C_SendData(_I2C, 0b11111111);
			while (!I2C_CheckEvent(_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
		}
		//showDebug(0);
	} else {
//		I2C_SendData(_I2C, 0b11111111);
//					while (!I2C_CheckEvent(_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
//						;
//					showDebug(0);
		for (a = 0; a < 528; a++) {
			I2C_SendData(_I2C, 0xAA);
			while (!I2C_CheckEvent(_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
			I2C_SendData(_I2C, 0x55);
			while (!I2C_CheckEvent(_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
		}
		//showDebug(0);
	}
	//I2C_GenerateSTART(_I2C, DISABLE);
	I2C_GenerateSTOP(_I2C, ENABLE);
	//I2C_GenerateSTOP(_I2C, DISABLE);

//int a;
//		i2c_start(0x70,0,0);	// 0x70 ����� ����������, �������, ������
//		i2c_write(0b10110000); 	// �������� 0
//		i2c_write(0b00000000); 	// ������� 0
//		i2c_write(0b00010000); 	//
//		i2c_stop();
//
//		i2c_start(0x70,1,0);	// 0x70 ����� ����������, ������, ������
//		if (tip==0)	for (a=0;a<1056;a++)i2c_write(0b00000000);
//		else 	if(tip==1) for (a=0;a<1056;a++)i2c_write(0b11111111);
//		else 	for (a=0;a<528;a++)	{i2c_write(0xAA);i2c_write(0x55);}
//		i2c_stop();
}

// ��������� �����
//[���]0-����� �����,1-������
//[���������� �� X] 0-131
//[���������� �� Y] 0-63
void point(int tip, int X, int Y) {
//#define bitset(var,bitno) ((var) |= 1<<(bitno)); 	// ����������
//#define bitclr(var,bitno) ((var) &= ~(1<<(bitno)));	// ��������
//int fon,Ya;
//// ����������� ��������� ���������� ����� � ��������� �������
//		Ya=Y%8; 							// ��������� ����� � �����.
//		if ((Y/8)==0)Y=(Y/8);				//
//		else Y=(Y/8);						//
//
//// ��������� �������
//		i2c_start(0x70,0,0);				// 0x70 ����� ����������, �������, ������
//	    i2c_write(0b10110000 | Y);			// ��������� ��������
//	    i2c_write(X & 0b00001111);			// ��������� �������
//	    i2c_write((X >> 4) | 0b00010000);	//
//		i2c_stop();
////������ ����� � ����������
//		i2c_start(0x70,1,1);		// 0x70 ����� ����������, ������, ������
//		i2c_read_ack();
//		fon=i2c_read_noack();
//		i2c_stop();
//// ����������� �����
//		if (tip) {bitset(fon,Ya);}
//		else {bitclr(fon,Ya);}
//// ��������� ������� �� X
//		i2c_start(0x70,0,0);				// 0x70 ����� ����������, �������, ������
//	    i2c_write(0b10110000 | Y);			// ��������� ��������
//	    i2c_write(X & 0b00001111);			// ��������� �������
//	    i2c_write((X >> 4) | 0b00010000);	//
//		i2c_stop();
//// ������ ����������������� �����
//		i2c_start(0x70,1,0);		// 0x70 ����� ����������, ������, ������
//		i2c_write(fon);
//		i2c_stop();

} //

int main(void) {
	I2C_DeInit(_I2C);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);

	I2C_InitTypeDef myI2C;
	myI2C.I2C_Ack = I2C_Ack_Enable;
	myI2C.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	myI2C.I2C_DutyCycle = I2C_DutyCycle_16_9;
	myI2C.I2C_ClockSpeed = 100000;
	myI2C.I2C_Mode = I2C_Mode_I2C;
	myI2C.I2C_OwnAddress1 = 1;

	GPIO_InitTypeDef myGPIO;
	myGPIO.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	myGPIO.GPIO_Speed = GPIO_Speed_50MHz;
	myGPIO.GPIO_Mode = GPIO_Mode_AF_OD;

	GPIO_Init(GPIOB, &myGPIO);

	I2C_Cmd(_I2C, ENABLE);
	I2C_Init(_I2C, &myI2C);
	I2C_AcknowledgeConfig(_I2C, ENABLE);

	// init output pins
	setupRST();
	showDebug(1);

	//testI2CIO();
	// Start display
	int_LCD();
	//delay(1000);
	//showDebug(0);
	clear_LCD(0); // �������
	showDebug(0);

	//while (1) {

	delay(1000);
	//clear_LCD(1);
	//showDebug(0);
	//Delay(160000);
	//clear_LCD(0); // �������
	//Delay(160000);
	delay(1000);
	//}

}
