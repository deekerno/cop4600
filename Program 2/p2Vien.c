#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/fs.h>    //character driver support
#include <asm/uaccess.h>

#define DEVICENAME "charDevice"
#define CLASSNAME "char"
#define MAXSIZE 1024

MODULE_LICENSE("GPL");

//Prototypes. taken from fs.h
int devOpen(struct inode *inodePtr, struct file *filePtr);
ssize_t devRead(struct file *filePtr, char *buffer, size_t length, loff_t *offset);
ssize_t devWrite(struct file *filePtr, const char *buffer, size_t length, loff_t *offset);
int devRelease(struct inode *inodePtr, struct file *filePtr);

//Globals
int majorNumber, i;
char message[MAXSIZE] = {0};
int position = 0;
struct class* charDeviceClass = NULL;
struct device* charDeviceDevice = NULL;

//use this struct from fs.h and give names to the functions
struct file_operations fOps =
{
	.owner = THIS_MODULE,
	.open = devOpen,        //open device
	.read = devRead,		//read from device
	.write = devWrite,      //write to device
	.release = devRelease,  //close device
};


int init_module(void)
{
	printk(KERN_INFO "Initializing the char device driver \n\n");	

	//register char device driver
	majorNumber = register_chrdev(0, DEVICENAME, &fOps);

	if(majorNumber < 0)
	{
		printk(KERN_INFO "Device registering failed with %d \n", majorNumber);		
		return majorNumber;
	}	

	charDeviceClass = class_create(THIS_MODULE, CLASSNAME);
	if(IS_ERR(charDeviceClass))
	{
		unregister_chrdev(majorNumber, DEVICENAME);
		printk(KERN_INFO "Failed to register device class \n");
		return PTR_ERR(charDeviceClass);
	}

	charDeviceDevice = device_create(charDeviceClass, NULL, MKDEV(majorNumber,0), NULL, DEVICENAME);
	if(IS_ERR(charDeviceDevice))
	{
		class_destroy(charDeviceClass);
		unregister_chrdev(majorNumber, DEVICENAME);
		printk(KERN_INFO "Failed to create device \n");
	}

	return 0;
}

void cleanup_module(void)
{
	device_destroy(charDeviceClass, MKDEV(majorNumber,0));
	class_unregister(charDeviceClass);	
	class_destroy(charDeviceClass);

	//unregister char device driver
	unregister_chrdev(majorNumber, DEVICENAME);
	
	printk(KERN_INFO "Goodbye \n");	
}


/* Methods */

int devOpen(struct inode *inodePtr, struct file *filePtr)
{
	printk(KERN_INFO "Device has been opened \n");
	
	return 0;
}

ssize_t devRead(struct file *filePtr, char *buffer, size_t length, loff_t *offset)
{
	printk(KERN_INFO "Device is reading \n");

	if(position==0 && strlen(message)==0) {
		printk(KERN_INFO "Nothing to read \n");
	}

	else { 
		//printk(KERN_INFO "pos: %d\n", position);
		
		//print message and empty whole array afterwards
        printk(KERN_INFO "Contents: %s \n", message);
		memset(message, 0, MAXSIZE);
        position = 0;

		printk(KERN_INFO "Reading finished \n");
	}

	return 0;
}

ssize_t devWrite(struct file *filePtr, const char *buffer, size_t length, loff_t *offset)
{
	printk(KERN_INFO "Device is writing \n");
	//printk(KERN_INFO "buffer: %s", buffer);

	if(position >= MAXSIZE) {
		printk(KERN_INFO "Buffer is full \n");
	}

	else 
	{
		//copy buffer into message
		for(i=0; i<length-1; i++)
		{			
			message[position] = buffer[i];
			position++;
			
			//when a string is too long
			if(position >= MAXSIZE)
				break;
		}

		//printk(KERN_INFO "message: %s", message);
		printk(KERN_INFO "Writing finished \n");

		if(position >= MAXSIZE)
			printk(KERN_INFO "Buffer is full \n");
	}

	return length;
}

int devRelease(struct inode *inodePtr, struct file *filePtr)
{
	printk(KERN_INFO "Device has been closed \n\n");

	return 0;
}


