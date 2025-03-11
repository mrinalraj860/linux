#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/sched/signal.h>
#include <linux/sched.h>

#define PROC_NAME "pg_stats"

// Show function for seq_file
static int pg_stats_show(struct seq_file *m, void *v)
{
	struct task_struct *task;

	// Lock to ensure consistency while reading the task list
	rcu_read_lock();

	// Iterate through all tasks
	for_each_process(task) {
		seq_printf(
			m,
			"[%d]: [[%llu],[%llu],[%llu]], [[%llu],[%llu],[%llu]], [[%llu],[%llu],[%llu]], [[%llu],[%llu],[%llu]]\n",
			task->pid, task->pg_stats.pgd_alloc_count,
			task->pg_stats.pgd_free_count,
			task->pg_stats.pgd_set_count,
			task->pg_stats.pud_alloc_count,
			task->pg_stats.pud_free_count,
			task->pg_stats.pud_set_count,
			task->pg_stats.pmd_alloc_count,
			task->pg_stats.pmd_free_count,
			task->pg_stats.pmd_set_count,
			task->pg_stats.pte_alloc_count,
			task->pg_stats.pte_free_count,
			task->pg_stats.pte_set_count);
	}

	// Unlock after done reading task list
	rcu_read_unlock();

	return 0;
}

// Open function for seq_file
static int pg_stats_open(struct inode *inode, struct file *file)
{
	return single_open(file, pg_stats_show, NULL);
}

// Define file operations
static const struct proc_ops pg_stats_fops = {
	.proc_open = pg_stats_open,
	.proc_read = seq_read,
	.proc_lseek = seq_lseek,
	.proc_release = single_release,
};

// Module initialization function
static int __init pg_stats_init(void)
{
	// Create proc entry
	proc_create(PROC_NAME, 0, NULL, &pg_stats_fops);
	pr_info("pg_stats: module loaded\n");
	return 0;
}

// Module cleanup function
static void __exit pg_stats_exit(void)
{
	// Remove proc entry
	remove_proc_entry(PROC_NAME, NULL);
	pr_info("pg_stats: module unloaded\n");
}

// Register module init and exit functions
module_init(pg_stats_init);
module_exit(pg_stats_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mrinal Raj");
MODULE_DESCRIPTION("Page Table Operation Counter");