#include "l610_command.h"

uint8_t state1;
uint8_t recv_way_state;

#define USART_MAX_SEND_LEN		800					//����ͻ����ֽ���
//���ڷ��ͻ����� 	
__align(8) uint8_t USART_TX_BUF[USART_MAX_SEND_LEN]; 	//���ͻ���,���USART_MAX_SEND_LEN�ֽ�  	

//����1,printf ����
//ȷ��һ�η������ݲ�����USART_MAX_SEND_LEN�ֽ�
void u1_printf(char* fmt,...)  
{  
	uint16_t i,j; 
	va_list ap; 
	va_start(ap,fmt);
	vsprintf((char*)USART_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART_TX_BUF);		//�˴η������ݵĳ���
	for(j=0;j<i;j++)							//ѭ����������
	{
	while((USART1->ISR&0X40)==0);//ѭ������,ֱ���������   
	USART1->TDR = (uint8_t) USART_TX_BUF[j]; 
	} 
}

void l610_send(char *txaddr)
{
	USART_RX_STA=0;
	u1_printf("%s\r\n",txaddr);	//��������
	USART_RX_STA=0;
}



//L610���������,�����յ���Ӧ��
//str:�ڴ���Ӧ����
//����ֵ:0,û�еõ��ڴ���Ӧ����
//    ����,�ڴ�Ӧ������λ��(str��λ��)
uint8_t* l610_check_cmd(uint8_t *str)
{
	
	char *strx=0;
	if(USART_RX_STA&0X8000)		//���յ�һ��������
	{ 
		USART_RX_BUF[USART_RX_STA&0X7FFF]=0;//��ӽ�����
		strx=strstr((const char*)USART_RX_BUF,(const char*)str);
	} 
	return (uint8_t*)strx;
}
//S
//cmd:���͵������ַ���
//ack:�ڴ���Ӧ����,���Ϊ��,���ʾ����Ҫ�ȴ�Ӧ��
//waittime:�ȴ�ʱ��(��λ:10ms)
//����ֵ:0,���ͳɹ�(�õ����ڴ���Ӧ����)
//       1,����ʧ��
uint8_t l610_send_cmd(uint8_t *cmd,uint8_t *ack,uint16_t waittime)
{
	uint8_t res=0;
	USART_RX_STA=0;
	u1_printf("%s\r\n",cmd);	//��������
	if(ack&&waittime)		//��Ҫ�ȴ�Ӧ��
	{
		while(--waittime)	//�ȴ�����ʱ
		{
			HAL_Delay(10);
			if(USART_RX_STA&0X8000)//���յ��ڴ���Ӧ����
			{
				if(l610_check_cmd(ack))
				{
					printf("ack:%s\r\n",(uint8_t*)ack);
					break;//�õ���Ч���� 
				}
					USART_RX_STA=0;
			} 
		}
		if(waittime==0)res=1; 
	}
	return res;
} 



