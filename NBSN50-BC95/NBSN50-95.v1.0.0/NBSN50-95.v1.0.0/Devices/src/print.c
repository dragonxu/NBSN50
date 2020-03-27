#include "print.h"

#ifdef __GNUC__
/* With GCC, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

//这个变量是其他.c文件定义的
extern UART_HandleTypeDef huart2;
uint8_t ch;
uint8_t ch_r;
//重写这个函数,重定向printf函数到串口，意思就是说printf直接输出到串口，其默认输出到控制台的
/*fputc*/
//int fputc(int c, FILE * f)
//{
//    ch=c;
//    HAL_UART_Transmit(&hlpuart1,&ch,1,1000);//发送串口
//    return c;
//}

PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART2 and Loop until the end of transmission */
  HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF);
	__HAL_UART_CLEAR_IDLEFLAG(&huart2);//清除标志位
  return ch;
}

//重定向scanf函数到串口 意思就是说接受串口发过来的数据，其默认是接受控制台的数据
/*fgetc*/
int fgetc(FILE * F)    
{
    HAL_UART_Receive (&huart2,&ch_r,1,0xffff);//接收
    return ch_r;
}



