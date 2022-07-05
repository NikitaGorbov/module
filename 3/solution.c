#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/random.h>
#include "checker.h"

#define MAX_ARR_SIZE 10

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nikita Gorbov <nikita.gorbov.2001@gmail.com>");
MODULE_VERSION("0.01");

/* Return array size */
size_t fill_array(short *arr)
{
    size_t size;

    get_random_bytes(&size, sizeof(int));
    if (size < 0)
        size = -size;
    size %= MAX_ARR_SIZE;
    if (!size) 
        size = MAX_ARR_SIZE;
    get_random_bytes(arr, sizeof(short) * size);
    return size;
}

int my_array_sum(short *array, size_t n)
{
    int i, sum = 0;

    for (i = 0; i < n; i++) {
        sum += array[i];
    }
    return sum;
}

int __init solution_init(void)
{
    CHECKER_MACRO;
    int i, j, result;
    size_t arr_size;
    short array[MAX_ARR_SIZE];
    char buf[256];
    ssize_t buf_size;

    for (j = 0; j < 10; j++) {
        arr_size = fill_array(array);
        for (i = 0; i < arr_size; i++) {
            printk(KERN_INFO "%d\n", array[i]);
        }
        result = array_sum(array, arr_size);
        buf_size = generate_output(result, array, arr_size, buf);
        if (result == my_array_sum(array, arr_size)) {
            printk(KERN_INFO "%s", buf);
        } else {
            printk(KERN_ERR "%s", buf);
        }
    }
    return 0;
}

void __exit solution_exit(void)
{
    CHECKER_MACRO;
}

module_init(solution_init);
module_exit(solution_exit);
