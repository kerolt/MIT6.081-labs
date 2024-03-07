// Use pipe to create a pipe.
// Use fork to create a child.
// Use read to read from the pipe, and write to write to the pipe.
// Use getpid to find the process ID of the calling process.
// Add the program to UPROGS in Makefile.
// User programs on xv6 have a limited set of library functions available to
// them. You can see the list in user/user.h; the source (other than for system
// calls) is in user/ulib.c, user/printf.c, and user/umalloc.c.
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define R 0
#define W 1

int main() {
    int p2c[2], c2p[2];
    pipe(p2c);
    pipe(c2p);

    int pid = fork();
    if (pid == 0) { // chile: read from the parent
        char buf[32] = {0};
        close(c2p[R]); // 不用从子进程读
        close(p2c[W]); // 不用从父进程写

        read(p2c[R], buf, sizeof(buf));
        close(p2c[R]);
        printf("%d: received ping\n", getpid());
        write(c2p[W], "pong", 4);
        close(c2p[W]);

        exit(0);
    } else { // parent: read from the child
        char buf[32] = {0};
        close(p2c[R]); // 不用从父进程读
        close(c2p[W]); // 不用从子进程写

        write(p2c[W], "ping", 4);
        close(p2c[W]);
        read(c2p[R], buf, sizeof(buf));
        printf("%d: received pong\n", getpid());
        close(c2p[R]);

        exit(0);
    }
}