

#include <linux/atomic.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h> /* Specifically, a module */
#include <linux/printk.h>
#include <linux/types.h>
#include <linux/uaccess.h> /* for get_user and put_user */
#include <linux/version.h>
#include <linux/utsname.h>
#include <linux/string.h>

#include <linux/mm.h>
#include <asm/errno.h>
#include <linux/jiffies.h>
#include "kfetch.h"
#define SUCCESS 0
#define DEVICE_NAME "kfetch"
#define MAJOR_NUM 101 

// #define BUF_LEN 1024
// #define NR_CPUS CONFIG_NR_CPUS
enum
{
    CDEV_NOT_USED = 0,
    CDEV_EXCLUSIVE_OPEN = 1,
};


static char logo1[23] = "        ^--^        \t";
static char dash[50] ;
static char logo2[30] = "       (･ω･ |       \t";
static char logo3[23] = "       /    |       \t";
static char logo4[23] = "      / ---  \\     \t";
static char logo5[23] = "     ( |   |  |     \t";
static char logo6[23] = "   |\\_)___/\\)/\\   \t";
static char logo7[23] = "  <__)------(__/   \t";
static char tmp[50] = "\0";

/* Is the device open right now? Used to prevent concurrent access into
 * the same device
 */
static atomic_t already_open = ATOMIC_INIT(CDEV_NOT_USED);

/* The message the device will give when asked */
static char message[KFETCH_BUF_SIZE] = "\0";
static struct class *cls;
static int mask_info=-1;

static char *cpu_info_print(void)
{
    unsigned int cpu = 0;
    struct cpuinfo_x86 *c;
    c = &cpu_data(cpu);
    if (c->x86_model_id[0])
        pr_cont("%s", c->x86_model_id);

    return c->x86_model_id;
}
static long unsigned int memory_free(void)
{
    struct sysinfo si;
    long unsigned int e = 0;
    uint32_t unit_kb;

    si_meminfo(&si);
    unit_kb = si.mem_unit / 1024;
    e = (si.freeram * unit_kb) / 1024;
    // printk(KERN_INFO "Memory Info Module: Free Memory = %lu MB / Total Memory = %lu MB\n",
    //        (si.freeram * si.mem_unit) / 1024 / 1024,
    //        (si.totalram * si.mem_unit) / 1024 / 1024);

    return e;
}
static long unsigned int memory_total(void)
{
    struct sysinfo si;
    long unsigned int e = 0;
    uint32_t unit_kb;

    si_meminfo(&si);
    unit_kb = si.mem_unit / 1024;
    e = (si.totalram * unit_kb) / 1024;
    // e = (si.totalram * si.mem_unit) / 1048576;
    // printk(KERN_INFO "Memory Info Module: Free Memory = %lu MB / Total Memory = %lu MB\n",
    //        (si.freeram * si.mem_unit) / 1024 / 1024,
    //        (si.totalram * si.mem_unit) / 1024 / 1024);

    return e;
}
static int process_count(void)
{
    struct task_struct *task;
    int process_count = 0;
    int thread_count = 0;
    // Traverse the process list
    for_each_process(task)
    {
        process_count++;
    }
    for_each_process(task)
    {
        thread_count += get_nr_threads(task);
    }

    // pr_info("Thread Count Module: Number of threads = %d\n", thread_count);
    // pr_info("Process Count Module: Number of processes = %d\n", process_count);

    return process_count;
}
static long unsigned int uptime(void)
{
    unsigned long uptime_in_seconds = jiffies_to_msecs(jiffies) / 1000;
    unsigned long uptime_in_minutes = uptime_in_seconds / 60;

    pr_info("Uptime: System has been running for %lu minutes\n", uptime_in_minutes);

    return uptime_in_minutes;
}
static char *kernel_version(void)
{
    struct new_utsname *uts;

    uts = utsname();
    pr_info("Kernel: %s\n", uts->release);

    return uts->release;
}

/* This is called whenever a process attempts to open the device file */
static int kfetch_open(struct inode *inode, struct file *file)
{
    
    // static int counter = 0; 
 
    if (atomic_cmpxchg(&already_open, CDEV_NOT_USED, CDEV_EXCLUSIVE_OPEN)) 
        return -EBUSY;

    try_module_get(THIS_MODULE);
    pr_info("kf device_open(%p)\n", file);

    return SUCCESS;
}

static int kfetch_release(struct inode *inode, struct file *file)
{
    atomic_set(&already_open, CDEV_NOT_USED); 
    pr_info("kf device_release(%p,%p)\n", inode, file);

    module_put(THIS_MODULE);
    return SUCCESS;
}

/* This function is called whenever a process which has already opened the
 * device file attempts to read from it.
 */
static ssize_t kfetch_read(struct file *file,   /* see include/linux/fs.h   */
                           char __user *buffer, /* buffer to be filled  */
                           size_t length,       /* length of the buffer     */
                           loff_t *offset)
{
    int i = 3;
    int mask_tmp = 0;
    // char maskinfo[10];
    mask_tmp = mask_info;
    
    // int bytes_read = 0;
    snprintf(message, KFETCH_BUF_SIZE, "                   \t%s\n", utsname()->nodename);
    for(int n = 0 ; n < strlen(utsname()->nodename);n++){
        strcat(dash,"-");
    }
    strcat(dash,"\n");

    // snprintf(maskinfo, KFETCH_BUF_SIZE, "%d\n", mask_info);
    // strcat(message, maskinfo);
    strcat(message, logo1);
    strcat(message, dash);

    strcat(message, logo2);

    if (mask_info == KFETCH_FULL_INFO || mask_info == -1)
    {
        snprintf(tmp, 50, "Kernel:\t%s\n", kernel_version());
        strcat(message, tmp);
        strcat(message, logo3);
        snprintf(tmp, 50, "CPU:\t%s\n", cpu_info_print());
        strcat(message, tmp);
        strcat(message, logo4);
        snprintf(tmp, 50, "CPUs:\t%d / %d\n", num_possible_cpus(), num_online_cpus());
        strcat(message, tmp);
        strcat(message, logo5);
        snprintf(tmp, 50, "Mem:\t%lu MB/ %lu MB\n", memory_free(), memory_total());
        strcat(message, tmp);
        strcat(message, logo6);
        snprintf(tmp, 50, "Procs:\t%d\n", process_count());
        strcat(message, tmp);
        strcat(message, logo7);
        snprintf(tmp, 50, "Uptime:\t%lu mins\n", uptime());
        strcat(message, tmp);
    }
    else
    {
// Kernel: The kernel release
// CPU: The CPU model name
// CPUs: The number of CPU cores, in the format <# of online CPUs> / <# of total CPUs>
// Mem: The memory information, in the format<free memory> / <total memory> (in MB)
// Procs: The number of processes
// Uptime: How long the system has been running, in minutes.
        while (mask_tmp > 0)
        {
            if (mask_tmp & KFETCH_RELEASE)
            {
                mask_tmp -= KFETCH_RELEASE;
                snprintf(tmp, 50, "Kernel:\t%s\n", kernel_version());
                strcat(message, tmp);
            }
            else if (mask_tmp & KFETCH_CPU_MODEL)
            {
                mask_tmp -= KFETCH_CPU_MODEL;
                snprintf(tmp, 50, "CPU:\t%s\n", cpu_info_print());
                strcat(message, tmp);
            }
            else if (mask_tmp & KFETCH_NUM_CPUS)
            {
                mask_tmp -= KFETCH_NUM_CPUS;
                snprintf(tmp, 50, "CPUs:\t%d / %d\n", num_online_cpus(), num_possible_cpus());
                strcat(message, tmp);
            }
            else if (mask_tmp & KFETCH_MEM)
            {
                mask_tmp -= KFETCH_MEM;
                snprintf(tmp, 50, "Mem:\t%lu MB/ %lu MB\n", memory_free(), memory_total());
                strcat(message, tmp);
            }
            else if (mask_tmp & KFETCH_NUM_PROCS)
            {
                mask_tmp -= KFETCH_NUM_PROCS;
                snprintf(tmp, 50, "Procs:\t%d\n", process_count());
                strcat(message, tmp);
            }
            else if (mask_tmp & KFETCH_UPTIME)
            {
                mask_tmp -= KFETCH_UPTIME;
                snprintf(tmp, 50, "Uptime:\t%lumins\n", uptime());
                strcat(message, tmp);
            }
            if(mask_tmp == 0) {
                    break;
            }
            switch (i)
            {
            case 3:
                strcat(message, logo3);
                break;
            case 4:
                strcat(message, logo4);
                break;
            case 5:
                strcat(message, logo5);
                break;
            case 6:
                strcat(message, logo6);
                break;
            case 7:
                strcat(message, logo7);
                break;
            default:
                break;
            }
            i++;
        }
        while (i < 8)
        {
            switch (i)
            {
            case 3:
                // if(mask_info == -1) strcat(message, "\n");
                strcat(message, logo3);
                strcat(message, "\n");

                break;
            case 4:
                strcat(message, logo4);
                strcat(message, "\n");

                break;
            case 5:
                strcat(message, logo5);
                strcat(message, "\n");

                break;
            case 6:
                strcat(message, logo6);
                strcat(message, "\n");

                break;
            case 7:
                strcat(message, logo7);
                strcat(message, "\n");

                break;
            default:
                break;
            }
            i++;
        }
    }

    message[strlen(message)] = '\0';
    length = sizeof(message);

    if (copy_to_user(buffer, &message, length))
    {
        pr_alert("Failed to copy data to user");
        return 0;
    }
    return length;
}

/* called when somebody tries to write into our device file. */
static ssize_t kfetch_write(struct file *file, const char __user *buffer,
                            size_t length, loff_t *offset)
{

    if (copy_from_user(&mask_info, buffer, length))
    {
        pr_info("mask info %d", mask_info);

        pr_alert("Failed to copy data from user");
        return 0;
    }

    pr_info("kf device_write(%p,%d,%ld)", file, mask_info, length);
    return mask_info;
}

const static struct file_operations kfetch_ops = {
    .owner = THIS_MODULE,
    .read = kfetch_read,
    .write = kfetch_write,
    .open = kfetch_open,
    .release = kfetch_release,
};

/* Initialize the module - Register the character device */
static int __init kfetch_init(void)
{
    int ret_val = register_chrdev(MAJOR_NUM, DEVICE_NAME, &kfetch_ops);

    /* Negative values signify an error */
    if (ret_val < 0)
    {
        pr_alert("%s failed with %d\n",
                 "Sorry, registering the character device ", ret_val);
        return ret_val;
    }

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 4, 0)
    cls = class_create(DKFETCH_DEV_NAME);
#else
    cls = class_create(THIS_MODULE, KFETCH_DEV_NAME);
#endif
    device_create(cls, NULL, MKDEV(MAJOR_NUM, 0), NULL, KFETCH_DEV_NAME);

    pr_info("kf Device created on /dev/%s\n", KFETCH_DEV_NAME);

    return 0;
}

/* Cleanup - unregister the appropriate file from /proc */
static void __exit kfetch_exit(void)
{
    device_destroy(cls, MKDEV(MAJOR_NUM, 0));
    class_destroy(cls);

    /* Unregister the device */
    unregister_chrdev(MAJOR_NUM, DEVICE_NAME);
}

module_init(kfetch_init);
module_exit(kfetch_exit);

MODULE_LICENSE("GPL");