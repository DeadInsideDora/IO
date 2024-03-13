#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>


#define BUF_SIZE 256

static dev_t first;
static struct cdev c_dev;
static struct class * cl;

char ibuf[BUF_SIZE];
char resultbuf[BUF_SIZE];


static int my_open(struct inode *i, struct file *f)
{
    printk(KERN_INFO "Driver: open()\n");
    return 0;
}

static int my_close(struct inode *i, struct file *f)
{
    printk(KERN_INFO "Driver: close()\n");
    return 0;
}

static ssize_t my_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{

int count = strlen(resultbuf);
printk(KERN_INFO "Driver: read()\n");

if (*off > 0 || len < count) {
return 0;
}

pr_info("%s\n", resultbuf);

if (copy_to_user(buf, resultbuf, count) != 0) {
return -EFAULT;
}

*off = count;

return count;
}

static int validate_char(const char *first) {
    return (*first >= '0' && *first <= '9') || *first == '+' || *first == '-';
}

static int read_number(char **first, const char *last)
{
    int integer = 0;
    int sign = 1;
    int state_sign = 0;
    while (*first != last && !validate_char(*first))
        ++*first;

    while (*first != last && validate_char(*first)) {
        if (**first == '+' || **first == '-') {
            if (state_sign) {
                break;
            }
            sign = **first == '+' ? 1 : -1;
        } else {
            state_sign = 1;
            integer = integer * 10 + (**first - '0');
        }
        ++*first;
    }
    if (state_sign) {
        return sign * integer;
    }
    return 1;
}

static int exist_digits(size_t len)
{
    size_t i = 0;
    while (i < len) {
        if (ibuf[i] >= '0' && ibuf[i] <= '9') {
            return 1;
        }
        ++i;
    }
    return 0;
}


static ssize_t my_write(struct file *f, const char __user *buf,  size_t len, loff_t *off)
{
char *first;
char *last;
int result = 1;
int pos;
char temporary[BUF_SIZE];

printk(KERN_INFO "Driver: write()\n");

if(len > BUF_SIZE)
return 0;

if (copy_from_user(ibuf, buf, len) != 0) {
return -EFAULT;
}

if (!exist_digits(len)) {
    return len;
}

first = ibuf;
last = first + len;
while (first != last) {
result *= read_number(&first, last);
}

memset(temporary, 0, BUF_SIZE);
if (resultbuf[0] != '\0') {
pos = snprintf(temporary, BUF_SIZE - 1, "%s\n%d", resultbuf, result);
} else {
pos = snprintf(temporary, BUF_SIZE - 1, "%d", result);
}
temporary[pos] = '\0';
memcpy(resultbuf, temporary, BUF_SIZE);
pr_info("%s\n", resultbuf);

return len;
}

static struct file_operations mychdev_fops =
        {
                .owner = THIS_MODULE,
                .open = my_open,
                .release = my_close,
                .read = my_read,
                .write = my_write
        };

static int __init ch_drv_init(void)
{
    printk(KERN_INFO "Module init!\n");
    memset(resultbuf, 0, BUF_SIZE);
    if (alloc_chrdev_region(&first, 0, 1, "ch_dev") < 0)
    {
        return -1;
    }
    if ((cl = class_create(THIS_MODULE, "chardrv")) == NULL)
    {
        unregister_chrdev_region(first, 1);
        return -1;
    }

    if (device_create(cl, NULL, first, NULL, "var8") == NULL)
    {
        class_destroy(cl);
        unregister_chrdev_region(first, 1);
        return -1;
    }


    cdev_init(&c_dev, &mychdev_fops);
    if (cdev_add(&c_dev, first, 1) == -1)
    {
        device_destroy(cl, first);
        class_destroy(cl);
        unregister_chrdev_region(first, 1);
        return -1;
    }
    return 0;
}

static void __exit ch_drv_exit(void)
{
    cdev_del(&c_dev);
    device_destroy(cl, first);
    class_destroy(cl);
    unregister_chrdev_region(first, 1);
    printk(KERN_INFO "Exit from module. See you soon!!!\n");
}

module_init(ch_drv_init);
module_exit(ch_drv_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Masha, Dora and Lara");
MODULE_DESCRIPTION("Our char driver");