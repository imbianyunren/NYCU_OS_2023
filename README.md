# OS HW1

312555008 許庭涵

HW link : https://hackmd.io/@Bmch4MS0Rz-VZWB74huCvw/B1b2S_Kl6

# A. For the kernel compilation part
### Paste the screenshot of the results of executing uname -a and cat /etc/os-release commands as the example shows.

![image](https://github.com/imbianyunren/OS_HW1/assets/60705979/56de28c5-ba22-4981-a177-373971881981)


# B. For the system call part

* ###  Describe how you implemented the two system calls in detail.
    * Which kernel sources did you modifiy? What do they do?
* ### For each system call you implemented:
    * Paste the well-formatted source code into the document.
    * Paste the screenshot of the messages the system call printed.


## 1. sys_hello
### 1-1 Define the system call content
In **/kernel_linux/linux-5.19.12/** `mkdir` hello folder and `touch` hello.c to write system call content.

> ~/kernel_linux/linux-5.19.12/hello/hello.c
```c
#include <linux/kernel.h>
#include <linux/syscalls.h>
SYSCALL_DEFINE0(hello)
{
    printk("Hello, world!\n");
    printk("312555008\n");
    return 0;
}
```
### 1-2 Write Makefile for compiling
In same dir, create the Makefile with the following command and Write the following code for compiling.

The variable `obj-y` represents a collection of target file names that need to be compiled into the kernel.

> ~/kernel_linux/linux-5.19.12/hello/Makefile
```
obj-y := hello.o
```
### 1-3 Add the system call to the kernel's system call table.
Open the syscall table.

> arch/x86/entry/syscalls/syscall_64.tbl

Add the following code after the last system call before x32 system calls.

I defined the system call number after the last system call.
```
449     ...
450     ...
-> 451     common  hello  sys_hello
```
![image](https://github.com/imbianyunren/OS_HW1/assets/60705979/fb5ba65f-d155-4ac0-a28a-48b20b6dc495)


### 1-4 Add the directory name in kernel Makefile

***Let the compiler knows that new system call can found in `/hello` directory.***

Open Makefile in kernel directory

> ~/kernel_linux/linux-5.19.12

Search for `core-y`. In the second result, there is a series of directories.

For linux kernel 5.19.12, it's in line `1103`, and add directory `hello/`  at the end

![image](https://github.com/imbianyunren/OS_HW1/assets/60705979/ca349fe8-5285-4904-bae9-5c87558c90fa)


### 1-5 Define the system call function.
Open system call header file.

> nano include/linux/syscalls.h

Edit syscalls.h, put `sys_hello` function to the last of the file (before #endif).

```c
asmlinkage long sys_hello(void);
```

![image](https://github.com/imbianyunren/OS_HW1/assets/60705979/aa55378a-a695-4757-9fd5-5f1fe0c96d39)


### 1-6 Compile kernel again
**1. Compile the kernel's source code.**
    
    make -j6

**2. Prepare the installer of the kernel.**

    sudo make modules_install -j6

**3. Install the kernel.**
    
    sudo make install -j6

**4. Reboot**

### 1-7 Write test file
Write a file to test `sys_hello` function
Create the directory for testing file and `touch` hello.c.
> /home/lunna/hw1/hello.c
```c
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
```
### 1-8 Compile the file and run.
```
gcc -o hello hello.c
./hello.c
```
If there are no assert error`(ret != 0)`, it should succeed.

**Lets check kernel msg!**

```
sudo dmesg
```

And here is the result, sys_hello print the string successfully! 

![image](https://github.com/imbianyunren/OS_HW1/assets/60705979/ff87de98-e952-459e-85c3-e8089b16b3b3)



## 2. sys_revstr
### 2-1 Define the system call content
In **/kernel_linux/linux-5.19.12/** `mkdir` revstr folder and `touch` revstr.c to write system call content.

> /home/lunna/kernel_linux/linux-5.19.12/revstr/revstr.c
```c
#include <linux/kernel.h>
#include <linux/syscalls.h>
int reverse(char *str){
        int i, j;
        char temp;
        j=strlen(str) - 1;
        for(i=0; i<j; i++){
                temp = str[i];
                str[i] = str[j];
                str[j] = temp;
                j--;
        }
        return 0;
}

SYSCALL_DEFINE2(revstr, unsigned int, num, char *, msg)
{
    char buf[256];
    long copied = strncpy_from_user(buf, msg, sizeof(buf));
    if (copied < 0 || copied == sizeof(buf))
        return -EFAULT;
    printk(KERN_INFO "The origin string: %s\n", buf);
    reverse(buf);
    printk(KERN_INFO "The reversed string: %s\n", buf);
    return 0;
}
```
### 2-2 Write Makefile for compiling
In same dir, create the Makefile with the following command and Write the following code for compiling.

The variable `obj-y` represents a collection of target file names that need to be compiled into the kernel.

> ~/kernel_linux/linux-5.19.12/revstr/Makefile
```
obj-y := revstr.o
```
### 2-3 Add the system call to the kernel's system call table.
Open the syscall table.

> arch/x86/entry/syscalls/syscall_64.tbl

Add the following code after the last system call before x32 system calls.

I defined the system call number after the last system call.
```
449     ...
450     ...
451     common  hello  sys_hello
-> 452     common  revstr  sys_revstr
```
![image](https://github.com/imbianyunren/OS_HW1/assets/60705979/aadfb64d-b753-4ce5-aeef-07ae0af1c3f2)


### 2-4 Add the directory name in kernel Makefile

***Let the compiler knows that new system call can found in `/revstr` directory.***

Open Makefile in kernel directory

> ~/kernel_linux/linux-5.19.12

Search for `core-y`. In the second result, there is a series of directories.

For linux kernel 5.19.12, it's in line `1103`, and add directory `revstr/`  at the end

![image](https://github.com/imbianyunren/OS_HW1/assets/60705979/7e753a26-5cd8-4f1a-b1b0-da1a972c043c)


### 2-5 Define the system call function.
Open system call header file.

> nano include/linux/syscalls.h

Edit syscalls.h, put `sys_revstr` function to the last of the file (before #endif).

```c
asmlinkage long sys_revstr(unsigned int num, char *msg);
```

![image](https://github.com/imbianyunren/OS_HW1/assets/60705979/66501edd-db37-4156-8ebd-7a9684134353)


### 2-6 Compile kernel again
**1. Compile the kernel's source code.**
    
    make -j6

**2. Prepare the installer of the kernel.**

    sudo make modules_install -j6

**3. Install the kernel.**
    
    sudo make install -j6

**4. Reboot**

### 2-7 Write test file
Write a file to test `sys_revstr` function
Create the directory for testing file and `touch` revstr.c.
> /home/lunna/hw1/revstr.c
```c
#include <assert.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>
#define __NR_revstr 452
int main(int argc, char *argv[]) {
    int ret1 = syscall(__NR_revstr, 5, "hello");
    assert(ret1 == 0);
    int ret2 = syscall(__NR_revstr, 11, "5Y573M C411");
    assert(ret2 == 0);
    return 0;
}
```
### 2-8 Compile the file and run.
```
gcc -o revstr revstr.c
./revstr.c
```
If there are no assert error`(ret# != 0)`, it should succeed.

**Lets check kernel msg!**

```
sudo dmesg
```

And here is the result, sys_revstr print the string successfully! 

![image](https://github.com/imbianyunren/OS_HW1/assets/60705979/cbae48d3-db5c-46d8-b1eb-6fa9116721e5)



# Reference:
### A part :
https://kernelnewbies.org/KernelBuild

https://wiki.archlinux.org/title/Kernel/Traditional_compilation

https://stackoverflow.com/questions/28684811/how-to-change-version-string-of-the-kernel

https://magiclen.org/grub-menu/
### B part :
https://dev.to/jasper/adding-a-system-call-to-the-linux-kernel-5-8-1-in-ubuntu-20-04-lts-2ga8

https://member.adl.tw/ernieshu/syscall_2_6_35.htm

https://brennan.io/2016/11/14/kernel-dev-ep3/

https://wenyuangg.github.io/posts/linux/linux-add-system-call.html
