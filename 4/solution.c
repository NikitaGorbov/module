#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include "checker.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nikita Gorbov <nikita.gorbov.2001@gmail.com>");
MODULE_VERSION("0.01");

void *void_ptr;
int *int_ptr;
struct device *device_ptr;

static int __init solution_init(void)
{
    void_ptr = kmalloc(get_void_size(), GFP_KERNEL);
    int_ptr = kmalloc(get_int_array_size() * sizeof(int), GFP_KERNEL);
    device_ptr = kmalloc(sizeof(struct device), GFP_KERNEL);
    submit_void_ptr(void_ptr);
    submit_int_array_ptr(int_ptr);
    submit_struct_ptr(device_ptr);

    return 0;
}

static void __exit solution_exit(void)
{
    checker_kfree(void_ptr);
    checker_kfree(int_ptr);
    checker_kfree(device_ptr);
}

module_init(solution_init);
module_exit(solution_exit);
