/* WORK IN PROGRESS */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/random.h>

#define MAX_ARR_SIZE 10

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nikita Gorbov <nikita.gorbov.2001@gmail.com>");
MODULE_VERSION("0.01");

/* Return array size */
static size_t fill_array(short *arr) {
	size_t size;

	get_random_bytes(&size, sizeof(int));
	if (size < 0)
		size = -size;
	size %= MAX_ARR_SIZE;
	if (!size) 
		size = MAX_ARR_SIZE;
	printk(KERN_ERR "size = %zu\n", size);
	get_random_bytes(arr, sizeof(short) * size);
	return size;
}

static int __init solution_init(void)
{
	int i;
	size_t arr_size;
	short array[MAX_ARR_SIZE];

	arr_size = fill_array(array);
	for (i = 0; i < arr_size; i++) {
		printk(KERN_INFO "%d\n", array[i]);
	}
	return 0;
}

static void __exit solution_exit(void)
{
}

module_init(solution_init);
module_exit(solution_exit);
