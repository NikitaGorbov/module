#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nikita Gorbov <nikita.gorbov.2001@gmail.com>");
MODULE_VERSION("0.01");

static int __init solution_init(void)
{
	printk(KERN_INFO "test \nkernel_mooc\n");
	return 0;
}

static void __exit solution_exit(void)
{
}

module_init(solution_init);
module_exit(solution_exit);
