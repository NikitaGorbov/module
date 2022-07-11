#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/sort.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nikita Gorbov <nikita.gorbov.2001@gmail.com>");
MODULE_VERSION("0.01");

static int my_sys;
static char *this_module_name;
int read_count;

static int my_compare(const void* a, const void* b)
{
    return strcmp(*(const char**)a, *(const char**)b);
}

static ssize_t attr_read(struct kobject *kobj, 
        struct kobj_attribute *attr,
        char *buf)
{
    struct module *mod;
    char *module_names[255];
    int i, num_of_modules = 0, bytes = 0;

    list_for_each_entry(mod, &THIS_MODULE->list, list) {
        module_names[num_of_modules] = mod->name;
        num_of_modules++;
    }
    module_names[num_of_modules - 1] = this_module_name;
    sort(module_names, num_of_modules, sizeof(const char *), my_compare, NULL);
    for (i = 0; i < num_of_modules; i++) {
        bytes += sprintf(buf + bytes, "%s\n", module_names[i]);
    }
    return bytes;
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

    this_module_name = THIS_MODULE->name;
    printk(KERN_INFO "this: %s\n", this_module_name);
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
