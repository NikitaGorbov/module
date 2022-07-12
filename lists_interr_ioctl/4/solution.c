#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/moduleparam.h>
#include "checker.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nikita Gorbov <nikita.gorbov.2001@gmail.com>");
MODULE_VERSION("0.01");
 
static struct hrtimer my_hr_timer;
static unsigned long delays[10];
static int timers_array_size, timers_counter;

module_param_array(delays, ulong, &timers_array_size, 0660);
 
static int __init solution_init(void);
static void __exit solution_exit(void);

enum hrtimer_restart timer_callback(struct hrtimer *timer)
{
    ktime_t ktime;

    check_timer();
    timers_counter++;
    if (timers_counter >= timers_array_size)
        return HRTIMER_NORESTART;
    ktime = ktime_set(0, (delays[timers_counter] * 1000000L));
    hrtimer_forward_now(timer, ktime);
    return HRTIMER_RESTART;
}

static int __init solution_init(void)
{
    ktime_t ktime;

    if (timers_array_size > 0) {
        ktime = ktime_set(0, (delays[0] * 1000000L));
        check_timer();
        hrtimer_init(&my_hr_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
        my_hr_timer.function = &timer_callback;
        hrtimer_start(&my_hr_timer, ktime, HRTIMER_MODE_REL);
    }
    return 0;
}

static void __exit solution_exit(void)
{
    if (timers_array_size > 0) {
        hrtimer_cancel(&my_hr_timer);
    }
}
 
module_init(solution_init);
module_exit(solution_exit);
 