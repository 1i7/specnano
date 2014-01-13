#define CS_HIGH GPIOA->BSRRL |= GPIO_Pin_4;
#define CS_LOW  GPIOA->BSRRH |= GPIO_Pin_4;

#define GO_IDLE_STATE            0              //����������� ������������   
#define SEND_IF_COND             8              //��� SDC V2 - �������� ��������� ����������   
#define READ_SINGLE_BLOCK        17             //������ ���������� ����� ������  
#define WRITE_SINGLE_BLOCK       24             //������ ���������� ����� ������
#define SD_SEND_OP_COND	         41             //������ �������� �������������  
#define APP_CMD			 55             //������� ������� �� ACMD <n> ������
#define READ_OCR		 58             //������ �������� OCR