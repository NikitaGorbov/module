#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nikita Gorbov <nikita.gorbov.2001@gmail.com>");
MODULE_VERSION("0.01");

static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

static int __init solution_init(void)
{
	return 0;
}

static void __exit solution_exit(void)
{
}

module_init(solution_init);
module_exit(solution_exit);
