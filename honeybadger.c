#include <fcntl.h>
#include <stdio.h>

int main() {
        int fd;
        char buf[] = "Honey Badger";
        char out[1];
        fd = fopen("/dev/memory", "w");
        fseek(fd, 1, SEEK_SET);
        fwrite(buf, 1, 12, fd);
        fclose(fd);
        fd = fopen("/dev/memory", "r");
        fread(out, 1, 1, fd);
        fclose(fd);
        printf(&out);
        return 0;
}
