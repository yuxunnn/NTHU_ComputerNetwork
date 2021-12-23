#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(){
    FILE *fd;
    FILE *fw;

    fd = fopen("video.mp4", "rb");
    fw = fopen("new.mp4", "wb");

    fseek(fd, 0, SEEK_END);
    int fileSize = ftell(fd);
    fseek(fd, 0, SEEK_SET);
    fseek(fw, 0, SEEK_SET);

    char *buffer = calloc(1024, sizeof(char));

    int readSize = fread(buffer, 1, 1024, fd);
    while(readSize > 0){
        fwrite(buffer, 1, readSize, fw);
        readSize = fread(buffer, 1, 1024, fd);
    }


    fclose(fw);
    fclose(fd);

    return 0;

}