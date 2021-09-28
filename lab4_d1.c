#include <stdio.h>

int main() {
        int fd;
        char high = '1';
        char low = '0';
  
        fd = fopen("/sys/class/gpio/gpio23/value", "w");
       
        // toggle the gpio pin
        while(1) {
          
          // don't know if we should be writing an int instead?
          
          fwrite(&high, sizeof(high), 1, fd);
          fwrite(&low, sizeof(low), 1, fd);
        }
        return 0;
}
