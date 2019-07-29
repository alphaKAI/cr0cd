#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/uaccess.h>

MODULE_LICENSE("Dual MIT/GPL");

#define DRIVER_NAME "cr0cd"
#define PROC_NAME "cr0cd"

static int cr0cd_proc_open(struct inode *inode, struct file *file) {
  printk("%s IS OPEND\n", DRIVER_NAME);

  return 0;
}

volatile static int CR0;

void int_to_bits(int n, char *bits) {
  int i;
  for(i = 0; i < 32; i++) {
    bits[i] = (n >> (31 - i)) & 1 ? '1' : '0';
  }
}

static ssize_t cr0cd_proc_read(struct file *file, char __user *buf,
                               size_t count, loff_t *f_pos) {
  char bits[33];

  printk("%s IS READ\n", DRIVER_NAME);

  asm("mov %%cr0, %%rax; mov %%rax, CR0" ::: "rax"); // read
  int_to_bits(CR0, bits);
  bits[32] = '\0';

  copy_to_user(buf, bits, 33);

  return 33;
}

static ssize_t cr0cd_proc_write(struct file *file, const char __user *buf,
                                size_t count, loff_t *f_pos) {
  char bits[33];
  char mode[10];

  printk("%s IS WRITTEN\n", DRIVER_NAME);
  asm("mov %%cr0, %%rax; mov %%rax, CR0" ::: "rax"); // read

  bits[32] = '\0';

  int_to_bits(CR0, bits);

  printk("<BEFORE> CURRENT CR0: %s", bits);

  copy_from_user(mode, buf, sizeof(mode));

  if(mode[0] == '1') { // disable
    printk("[DISABLE] CPU Cache\n");

    CR0 |= 1 << 30;
  } else { // enable
    printk("[ENABLE] CPU Cache\n");

    CR0 &= ~(1 << 30);
  }

  asm("mov CR0, %%rax; mov %%rax, %%cr0" ::: "rax"); // write

  int_to_bits(CR0, bits);

  printk("<AFTER> CURRENT CR0: %s", bits);

  return count;
}

// handler table for proc_fs
static struct file_operations cr0cd_proc_fops = {
    .owner = THIS_MODULE,
    .open = cr0cd_proc_open,
    .read = cr0cd_proc_read,
    .write = cr0cd_proc_write,
};

static int cr0cd_init(void) {
  struct proc_dir_entry *entry;

  printk("INTIALIZE %s\n", DRIVER_NAME);

  entry = proc_create(PROC_NAME, S_IRUGO | S_IWUGO, NULL, &cr0cd_proc_fops);

  if(entry == NULL) {
    printk(KERN_ERR " failed to create device %s\n", DRIVER_NAME);
    return -ENOMEM;
  }

  return 0;
}

static void cr0cd_exit(void) {
  printk("EXIT %s\n", DRIVER_NAME);

  remove_proc_entry(PROC_NAME, NULL);
}

module_init(cr0cd_init);
module_exit(cr0cd_exit);
