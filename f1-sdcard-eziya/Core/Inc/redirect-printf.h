#ifndef __redirect_printf_h__
#define __redirect_printf_h__

#ifndef USE_HUART_PRINTF
#define USE_HUART_PRINTF 0
#endif // USE_HUART_PRINTF

#if USE_HUART_PRINTF
#ifndef HUART_PRINTF
#error "Please define HUART_PRINTF as some huartx in main.h"
#endif // HUART_PRINTF
extern UART_HandleTypeDef HUART_PRINTF;
#endif // USE_HUART_PRINTF

#if defined(__GNUC__)
int _write(int fd, char *ptr, int len)
{
  #if USE_HUART_PRINTF
  HAL_UART_Transmit(&HUART_PRINTF, (uint8_t *)ptr, len, HAL_MAX_DELAY);
  #endif
  for (int i = 0; i < len; i++)
    ITM_SendChar((*ptr++));
  return len;
}
#elif defined(__ICCARM__)
#include "LowLevelIOInterface.h"
size_t __write(int handle, const unsigned char *buffer, size_t size)
{
  #if USE_HUART_PRINTF
  HAL_UART_Transmit(&HUART_PRINTF, (uint8_t *)buffer, size, HAL_MAX_DELAY);
  #endif
  // need to implement ITM_SendChar for __ICCARM__ here
  return size;
}
#elif defined(__CC_ARM)
int fputc(int ch, FILE *f)
{
  #if USE_HUART_PRINTF
  HAL_UART_Transmit(&HUART_PRINTF, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
  #endif
  ITM_SendChar(ch);
  return ch;
}
#endif

#endif // #ifndef __redirect_printf_h__
