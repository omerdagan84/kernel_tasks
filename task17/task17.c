/*
 *   task6.c - Task 6 kerneltask.
 *   create a loadable kernel module and makefile
 *   module will open a misc device /dev/kerneltask
 *
 *		Author: Omer Dagan
 *		Date: 21.4.16
 *		mail: omer.dagan@tandemg.com
 *		based on: http://linux.die.net/lkmpg/x121.html
*/

#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/wait.h>
#include <linux/kthread.h>

#define MAX_INPUT_SIZE 20

struct task_context {
	struct task_struct *task_thread;
	wait_queue_head_t	wee_wait;
	int					f_wait;
};
static struct task_context task_ctx;

static ssize_t task17_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
	char input[MAX_INPUT_SIZE];
	int ret;

	if (count > MAX_INPUT_SIZE)
		return -EINVAL;
	ret = copy_from_user(input, buf, count);
	pr_info("copied from the user %s count=%lu", input, count);
	return count + 1;

}

static const struct file_operations task17_fops = {
	.owner	= THIS_MODULE,
	.write	= task17_write,
};

static struct miscdevice kerneltask_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "kerneltask",
	.fops = &task17_fops,
	.mode = S_IWUGO
};

static int run_thread(void *arg)
{
	int rc;

	pr_debug("thread - starting wait on \"wee-wait\"\n");
	wait_event_killable(task_ctx.wee_wait, task_ctx.f_wait);
	pr_debug("thread - done wait on \"wee-wait\"\n");
	return 0;
}

static int __init task17_init(void)
{
	int ret;

	/* Initialize the wait queue */
	init_waitqueue_head(&task_ctx.wee_wait);

	/* Setup the thread */
	task_ctx.f_wait = 0; /*condition for wait on queue*/
	task_ctx.task_thread = kthread_run(run_thread, NULL, "kerneltask"); /*thread is stopped*/
	if (IS_ERR(task_ctx.task_thread)) {
		ret = PTR_ERR(task_ctx.task_thread);
		return ret;
	}

	ret = misc_register(&kerneltask_dev); /* register the misc device*/
	if (ret) {
		pr_debug("Unable to register kerneltask misc device\n");
		return ret;
	} else {
		pr_debug("Registered kerneltask misc device\n");
	}

	return ret;
}

static void __exit task17_exit(void)
{
	pr_debug("De-registered kerneltask misc device\n");
	task_ctx.f_wait = 1;
	wake_up_all(&task_ctx.wee_wait);

	misc_deregister(&kerneltask_dev);
}

module_init(task17_init);
module_exit(task17_exit);
