#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#define BUF_SIZE 4096
#define LINE_SIZE 512

int main(int argc, char *argv[]) {
    char buf[BUF_SIZE] = {0};
    char line[LINE_SIZE] = {0};

    if (fgets(line, LINE_SIZE, stdin) == NULL) {
        printf("Error reading input.\n");
        return -1;
    }

    if (line[strlen(line) - 1] == '\n')
        line[strlen(line) - 1] = '\0';


    FILE *fp = fopen("/dev/var8", "r+");
    if (fp == NULL) {
        printf("Can not open /dev/var8.\n");
        return -1;
    }

    fwrite(line, strlen(line), 1, fp);
    fseek(fp, 0, SEEK_SET);
    fread(buf, sizeof(buf), 1, fp);
    printf("%s\n", buf);


    fclose(fp);

    return 0;
}
