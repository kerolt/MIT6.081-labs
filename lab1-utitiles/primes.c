// Target:
// Write a concurrent version of prime sieve using pipes. This idea is due to
// Doug McIlroy, inventor of Unix pipes. The picture halfway down this page and
// the surrounding text explain how to do it. Your solution should be in the
// file user/primes.c.
// ------------------------
// Hints:
// Be careful to close file descriptors that a process doesn't need, because
// otherwise your program will run xv6 out of resources before the first process
// reaches 35. Once the first process reaches 35, it should wait until the
// entire pipeline terminates, including all children, grandchildren, &c. Thus
// the main primes process should only exit after all the output has been
// printed, and after all the other primes processes have exited. Hint: read
// returns zero when the write-side of a pipe is closed. It's simplest to
// directly write 32-bit (4-byte) ints to the pipes, rather than using formatted
// ASCII I/O. You should create the processes in the pipeline only as they are
// needed. Add the program to UPROGS in Makefile.
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void Filter(int pipe_fd[2]) {
    close(pipe_fd[1]);
    int prime;
    read(pipe_fd[0], &prime, 4);
    printf("prime %d\n", prime);

    int num;
    if (read(pipe_fd[0], &num, 4) == 0) {
        exit(0);
    }

    int new_pipe_fd[2];
    pipe(new_pipe_fd);
    int pid = fork();
    if (pid == -1) {
        printf("Fork error!\n");
        exit(1);
    } else if (pid == 0) {
        Filter(new_pipe_fd);
    } else {
        close(new_pipe_fd[0]);
        if (num % prime != 0) {
            write(new_pipe_fd[1], &num, 4);
        }
        while (read(pipe_fd[0], &num, 4) > 0) {
            if (num % prime != 0) {
                write(new_pipe_fd[1], &num, 4);
            }
        }
        close(new_pipe_fd[1]);
        close(pipe_fd[0]);
        wait(0);
    }
}

int main() {
    int pipe_fd[2];
    pipe(pipe_fd);

    int pid = fork();
    if (pid == -1) {
        printf("Fork error!\n");
        exit(1);
    } else if (pid == 0) {
        Filter(pipe_fd);
    } else {
        close(pipe_fd[0]);
        for (int i = 2; i <= 35; i++) {
            write(pipe_fd[1], &i, 4); // 一个int为4 byte
        }
        close(pipe_fd[1]);
        wait(0);
    }
    exit(0);
}