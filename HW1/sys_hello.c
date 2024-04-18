#include <assert.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>

#define __NR_hello 451

int main(int argc, char *argv[]) {
    long ret = syscall(__NR_hello);
    assert(ret == 0);

    return 0;
}
