#include <linux/cdev.h>
#include <linux/device.h>
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

static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);
static char kernel_buf[8];
static int device_open_count;
static dev_t device;
static int major_num;
static struct cdev c_dev;
static struct class *cl;
static char *node_name;
module_param(node_name, charp, 0660);
static struct file_operations file_ops = {
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release
};

static int my_dev_uevent(struct device *dev, struct kobj_uevent_env *env)
{
    add_uevent_var(env, "DEVMODE=%#o", 0666);
    return 0;
}

static ssize_t device_read(struct file *flip, char __user *buffer, size_t len, loff_t *offset)
{
    int size;
    /* Read from file only once */
    if (*offset > 0)
        return 0;
    size = sprintf(kernel_buf, "%d", major_num);
    if (copy_to_user(buffer, kernel_buf, size))
        return -EFAULT;
    *offset += size;
    return size;
}

static ssize_t device_write(struct file *flip, const char *buffer, size_t len, loff_t *offset)
{
    printk(KERN_ALERT "Cannot write into device. Device is read-only.\n");
    return -EINVAL;
}

static int device_open(struct inode *inode, struct file *file)
{
    if (device_open_count)
        return -EBUSY;
    device_open_count++;
    try_module_get(THIS_MODULE);
    return 0;
}

static int device_release(struct inode *inode, struct file *file)
{
    device_open_count--;
    module_put(THIS_MODULE);
    return 0;
}

static int __init solution_init(void)
{
    int init_result;

    init_result = alloc_chrdev_region(&device, 0, 1, node_name);
    if (0 > init_result) {
        printk(KERN_ALERT "Device Registration failed\n");
        return -1;
    }
    major_num = MAJOR(device);
    if (!(cl = class_create(THIS_MODULE, "chardev"))) {
        printk(KERN_ALERT "Class creation failed\n");
        unregister_chrdev_region(device, 1);
        return -1;
    }
    cl -> dev_uevent = my_dev_uevent;
    if (!device_create(cl, NULL, device, NULL, node_name)) {
        printk(KERN_ALERT "Device creation failed\n");
        class_destroy(cl);
        unregister_chrdev_region(device, 1);
        return -1;
    }
    cdev_init(&c_dev, &file_ops);
    if (cdev_add(&c_dev, device, 1) == -1) {
        printk(KERN_ALERT "Device addition failed\n");
        device_destroy(cl, device);
        class_destroy(cl);
        unregister_chrdev_region(device, 1);
        return -1;
    }

    return 0;
}

static void __exit solution_exit(void)
{
    cdev_del(&c_dev);
    device_destroy(cl, device);
    class_destroy(cl);
    unregister_chrdev_region(device, 1);
}

module_init(solution_init);
module_exit(solution_exit);
