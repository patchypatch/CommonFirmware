#include "usart.h"

void init_NewLib_helpers(void) {
  usart_init(1U, 'A', 9U, 99U, 7U); // for printf
}