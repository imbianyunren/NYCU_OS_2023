# Assignment 3: System Information Fetching Kernel Module

### 印象中有一些錯誤未修補
312555008 Hsu Ting-Han

## File intro:
1. kfetch_mod_312555008.c: kernel module file
2. kfetch.c: user test to conmunicate and fetch information from kernel
3. kfetch.h: header define arg numbers and some parameters
4. Makefile: to compile/insert/remove kernel module

## User guide:
1. `unzip` the compress file
2. `make` to compile kernel module
3. `make load` insert
4. `cc kfetch.c -o kfetch` make the user test execute file
5. `sudo ./kfetch (-arg1) (-arg2)...` execute it to fetch system information
```
    which may look like when sudo ./kfetch -a
                        lunna
        ^--^            -----
       (･ω･ |           Kernel: 5.19.12-os-312555008
       /    |           CPU:    13th Gen Intel(R) Core(TM) i5-13600K
      / ---  \          CPUs:   8 / 8
     ( |   |  |         Mem:    10910 MB/ 16005 MB
   |\_)___/\)/\         Procs:  267
  <__)------(__/        Uptime: 111 mins
```
```
    which may look like when sudo ./kfetch -c -m -p -u
                        lunna
        ^--^            -----
       (･ω･ |           CPU:    13th Gen Intel(R) Core(TM) i5-13600K
       /    |           Mem:    10889 MB/ 16005 MB
      / ---  \          Procs:  273
     ( |   |  |         Uptime: 106mins
   |\_)___/\)/\   
  <__)------(__/   
```
6. `make unload` remove kernel module
7. `make clean` clean files generated before

## Some words 
All I want for Christmas is All pass(ノ｀Д)ノ, **PADORU PADORU**
