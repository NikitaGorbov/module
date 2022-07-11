#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <linux/ioctl.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nikita Gorbov <nikita.gorbov.2001@gmail.com>");
MODULE_VERSION("0.01");
#define DEVICE_NAME "solution_node"
#define IOC_MAGIC 'k'

#define SUM_LENGTH _IOWR(IOC_MAGIC, 1, char*)
#define SUM_CONTENT _IOWR(IOC_MAGIC, 2, char*)

static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);
static int major_num = 240;
static int sum_len, sum_content;
static char kernel_buf[16];
static char buf[20];

static int cur_open_count, overall_open_count, write_count;

static long my_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    int num;
    switch(cmd) {
        case SUM_LENGTH:
            copy_from_user(buf, (char *)arg, 10);
            sum_len += strlen(buf);
            return sum_len;
        case SUM_CONTENT:
            copy_from_user(buf, (char *)arg, 20);
            kstrtoint(buf, 10, &num);
            sum_content += num;
            return sum_content;
        default:
            printk(KERN_ERR "Unknown command\n");
            break;
    }
    return 0;
}

static struct file_operations file_ops = {
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release,
    .unlocked_ioctl = my_ioctl
};

static ssize_t device_read(struct file *flip, char __user *buffer, size_t len, loff_t *offset)
{
    int size;
    /* Read from file only once */
    if (*offset > 0)
        return 0;
    size = sprintf(kernel_buf, "%d %d\n", sum_len, sum_content);
    if (copy_to_user(buffer, kernel_buf, size))
        return -EFAULT;
    *offset += size;
    return size;
}

static ssize_t device_write(struct file *flip, const char *buffer, size_t len, loff_t *offset)
{
    return 0;
}

static int device_open(struct inode *inode, struct file *file)
{
    if (cur_open_count)
        return -EBUSY;
    cur_open_count++;
    overall_open_count++;
    try_module_get(THIS_MODULE);
    return 0;
}

static int device_release(struct inode *inode, struct file *file)
{
    cur_open_count--;
    module_put(THIS_MODULE);
    return 0;
}

static int __init solution_init(void)
{
    int res;

    res = register_chrdev(major_num, DEVICE_NAME, &file_ops);
    if (res < 0)
        printk(KERN_ALERT "Could not register device, error: %d\n", res);
    return res;
}

static void __exit solution_exit(void)
{
    unregister_chrdev(major_num, DEVICE_NAME);
}

module_init(solution_init);
module_exit(solution_exit);
