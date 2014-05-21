#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


int main()
{
    uint32_t num_in, samples = 0;
    uint16_t num0, num1;

    FILE* in_file = fopen("adc0.txt", "r"); //read only
    
    if (!in_file) { // equivalent to  if(in_file == NULL)
        printf("file can't be read\n");
        exit(-1); // required stdlib.h 
    }

    while (fscanf(in_file, "%d", &num_in) == 1) {
        samples++;
        num0 = (uint16_t) num_in;
        num1 = num0 & ~(1 << 0);
        printf("%d) %#04x    %#04x \n", samples, num0, num1);
    }
}
