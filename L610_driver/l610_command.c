#include "l610_command.h"

uint8_t state1;
uint8_t recv_way_state;

#define USART_MAX_SEND_LEN		800					//最大发送缓存字节数
//串口发送缓存区 	
__align(8) uint8_t USART_TX_BUF[USART_MAX_SEND_LEN]; 	//发送缓冲,最大USART_MAX_SEND_LEN字节  	

//串口1,printf 函数
//确保一次发送数据不超过USART_MAX_SEND_LEN字节
void u1_printf(char* fmt,...)  
{  
	uint16_t i,j; 
	va_list ap; 
	va_start(ap,fmt);
	vsprintf((char*)USART_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART_TX_BUF);		//此次发送数据的长度
	for(j=0;j<i;j++)							//循环发送数据
	{
	while((USART1->ISR&0X40)==0);//循环发送,直到发送完毕   
	USART1->TDR = (uint8_t) USART_TX_BUF[j]; 
	} 
}

void l610_send(char *txaddr)
{
	USART_RX_STA=0;
	u1_printf("%s\r\n",txaddr);	//发送命令
	USART_RX_STA=0;
}



//L610发送命令后,检测接收到的应答
//str:期待的应答结果
//返回值:0,没有得到期待的应答结果
//    其他,期待应答结果的位置(str的位置)
uint8_t* l610_check_cmd(uint8_t *str)
{
	
	char *strx=0;
	if(USART_RX_STA&0X8000)		//接收到一次数据了
	{ 
		USART_RX_BUF[USART_RX_STA&0X7FFF]=0;//添加结束符
		strx=strstr((const char*)USART_RX_BUF,(const char*)str);
	} 
	return (uint8_t*)strx;
}
//S
//cmd:发送的命令字符串
//ack:期待的应答结果,如果为空,则表示不需要等待应答
//waittime:等待时间(单位:10ms)
//返回值:0,发送成功(得到了期待的应答结果)
//       1,发送失败
uint8_t l610_send_cmd(uint8_t *cmd,uint8_t *ack,uint16_t waittime)
{
	uint8_t res=0;
	USART_RX_STA=0;
	u1_printf("%s\r\n",cmd);	//发送命令
	if(ack&&waittime)		//需要等待应答
	{
		while(--waittime)	//等待倒计时
		{
			HAL_Delay(10);
			if(USART_RX_STA&0X8000)//接收到期待的应答结果
			{
				if(l610_check_cmd(ack))
				{
					printf("ack:%s\r\n",(uint8_t*)ack);
					break;//得到有效数据 
				}
					USART_RX_STA=0;
			} 
		}
		if(waittime==0)res=1; 
	}
	return res;
} 



