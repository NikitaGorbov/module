#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <asm/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nikita Gorbov <nikita.gorbov.2001@gmail.com>");
MODULE_VERSION("0.01");
#define DEVICE_NAME "solution_node"
#define KBUF_SIZE 256

static loff_t device_llseek(struct file *file, loff_t offset, int orig);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);
static int major_num = 240;

struct data {
    char kbuf[KBUF_SIZE];
    int end_position;
};

static struct file_operations file_ops = {
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release,
    .llseek = device_llseek
};

static loff_t device_llseek(struct file *file, loff_t offset, int orig)
{
    loff_t testpos;
    struct data *priv = file->private_data;

    printk(KERN_INFO "kernel_mooc LLSEEK offset=%lld", offset);
    switch (orig) {
        case SEEK_SET:
            testpos = offset;
            break;
        case SEEK_CUR:
            testpos = file->f_pos + offset;
            break;
        case SEEK_END:
            testpos = priv->end_position + offset;
            break;
        default:
            return -EINVAL;
    }
    testpos = testpos < KBUF_SIZE ? testpos : KBUF_SIZE;
    testpos = testpos >= 0 ? testpos : 0;
    file->f_pos = testpos;
    return testpos;
}

static ssize_t device_read(struct file *flip, char __user *buffer, size_t len, loff_t *offset)
{
    // char *kbuf;
    int nbytes, mylen;
    struct data *priv;

    priv = flip->private_data;
    mylen = priv->end_position - *offset;
    if (!mylen)
        return 0;
    if (len > mylen)
        len = mylen;
    nbytes = len - copy_to_user(buffer, priv->kbuf + *offset, len);
    *offset += nbytes;
    return mylen;
}

static ssize_t device_write(struct file *flip, const char *buffer, size_t len, loff_t *offset)
{
    int nbytes, str_len;
    struct data *priv;

    priv = flip->private_data;
    str_len = priv->end_position;
    nbytes = len - copy_from_user(priv->kbuf + *offset, buffer, len);
    if (str_len < *offset + len) {
        /* set new end of string */
        priv->end_position = *offset + len;
    }
    *offset += nbytes;
    return nbytes;
}

static int device_open(struct inode *inode, struct file *file)
{
    static int counter = 0;
    struct data *priv = NULL;

    priv = (struct data*)kmalloc(sizeof(struct data), GFP_KERNEL);
    if (!priv) {
        return -ENOMEM;
    }
    priv->end_position = sprintf(priv->kbuf, "%d", counter);
    file->private_data = priv;
    counter++;
    return 0;
}

static int device_release(struct inode *inode, struct file *file)
{
    struct data *priv;

    priv = file->private_data;
    if (priv)
        kfree(priv);
    priv = NULL;
    file->private_data = NULL;
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