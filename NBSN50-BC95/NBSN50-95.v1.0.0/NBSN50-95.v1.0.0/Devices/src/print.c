#include "print.h"

#ifdef __GNUC__
/* With GCC, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

//�������������.c�ļ������
extern UART_HandleTypeDef huart2;
uint8_t ch;
uint8_t ch_r;
//��д�������,�ض���printf���������ڣ���˼����˵printfֱ����������ڣ���Ĭ�����������̨��
/*fputc*/
//int fputc(int c, FILE * f)
//{
//    ch=c;
//    HAL_UART_Transmit(&hlpuart1,&ch,1,1000);//���ʹ���
//    return c;
//}

PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART2 and Loop until the end of transmission */
  HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF);
	__HAL_UART_CLEAR_IDLEFLAG(&huart2);//�����־λ
  return ch;
}

//�ض���scanf���������� ��˼����˵���ܴ��ڷ����������ݣ���Ĭ���ǽ��ܿ���̨������
/*fgetc*/
int fgetc(FILE * F)    
{
    HAL_UART_Receive (&huart2,&ch_r,1,0xffff);//����
    return ch_r;
}



