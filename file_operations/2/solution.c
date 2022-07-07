#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <linux/sysfs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nikita Gorbov <nikita.gorbov.2001@gmail.com>");
MODULE_VERSION("0.01");

#define ARRAY_PARAM_SIZE 5

static int my_sys;
static int a, b;
static int c[ARRAY_PARAM_SIZE];
module_param(b, int, 0660);
module_param(a, int, 0660);
module_param_array(c, int, NULL, 0660);

static int array_sum (int *array, int n)
{
    int i, res = 0;

    for (i = 0; i < n; i++) {
        res += array[i];
    }
    return res;
}

static ssize_t attr_read(struct kobject *kobj, 
                      struct kobj_attribute *attr,
                     char *buf)
{
    return sprintf(buf, "%d\n", a + b + array_sum(c, ARRAY_PARAM_SIZE));
}

static struct kobj_attribute my_sys_attribute =
    __ATTR(my_sys, 0664, attr_read, NULL);

static struct attribute *attrs[] = {
    &my_sys_attribute.attr,
    NULL,   
      /* list has to be null-terminated */
};

static struct attribute_group attr_group = {
    .attrs = attrs,
};

static struct kobject *Driver_Module_kobj;


static int __init solution_init(void)
{
    int retval;

    Driver_Module_kobj = kobject_create_and_add("my_kobject", 
                                                    kernel_kobj);
    if (!Driver_Module_kobj)
        return -ENOMEM;

    retval = sysfs_create_group(Driver_Module_kobj,
                                    &attr_group);
    if (retval)
        kobject_put(Driver_Module_kobj);
    return retval;
}

static void __exit solution_exit(void)
{
    kobject_put(Driver_Module_kobj);
}

module_init(solution_init);
module_exit(solution_exit);