#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/sort.h>
#include <linux/interrupt.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nikita Gorbov <nikita.gorbov.2001@gmail.com>");
MODULE_VERSION("0.01");

static int my_sys;
static int ir=0;

static ssize_t attr_read(struct kobject *kobj, 
                      struct kobj_attribute *attr,
                     char *buf)
{
    return sprintf(buf, "%d\n", ir);
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

static irq_handler_t my_handler(int irq_no, void *dev_id, struct pt_regs *regs)
{
    ir++;
    return (irq_handler_t) IRQ_HANDLED;
}

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

    retval = request_irq(8, (irq_handler_t) my_handler, IRQF_SHARED, "rtc0", (void *)my_handler);
    return retval;
}

static void __exit solution_exit(void)
{
    free_irq(8, NULL);
}

module_init(solution_init);
module_exit(solution_exit);
