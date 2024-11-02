#include <stdio.h>
#include <stdlib.h>

int main(void)
{
  int i;
  
  for (i = 0; i < 200; i++)
  {
    /* gerando valores aleatórios entre zero e 100 */
    if(i == 199){
        printf("%d", rand() % 100);
        break;
    }
    printf("%d\n", rand() % 100);
  }
  
  return 0;
}