#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: xargs <params>\n");
        exit(1);
    }

    char* child_argv[MAXARG];
    char buf[512] = {'\0'};
    int index = 0;
    for (int i = 1; i < argc; i++) {
        child_argv[index++] = argv[i];
    }

    sleep(10);

    // 从标准输入中读取命令到buf中
    // 若执行 echo 1 2 3，则标准输入为 0 1 2
    int n;
    while ((n = read(0, buf, sizeof(buf))) > 0) {
        char* p = buf;
        for (int i = 0; i < n; i++) {
            if (buf[i] != '\n')
                continue;
            if (fork() == 0) {
                buf[i] = '\0';
                child_argv[index] = p;
                exec(child_argv[0], child_argv);
                exit(0);
            } else {
                p = &buf[i + 1];
                wait(0);
            }
        }
    }

    exit(0);
}