#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/fs.h>    //character driver support
#include <linux/mutex.h>
#include <asm/uaccess.h>
#include "p3.h"

#define DEVICENAME "inDevice"
#define CLASSNAME "inDev"

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
EXPORT_SYMBOL(message);
EXPORT_SYMBOL(position);

static DEFINE_MUTEX(mutexLock);  //declare mutex

struct class* inDeviceClass = NULL;
struct device* inDeviceDevice = NULL;

//use this struct from fs.h and give names to the functions
struct file_operations fOps =
{
	.owner = THIS_MODULE,
	.open = devOpen,        //open device
	.read = devRead,		//input can't read
	.write = devWrite,      //write to device
	.release = devRelease,  //close device
};


int init_module(void)
{
	printk(KERN_INFO "Initializing the input device driver \n\n");	

	//register char device driver
	majorNumber = register_chrdev(0, DEVICENAME, &fOps);

	if(majorNumber < 0)
	{
		printk(KERN_INFO "Device registering failed with %d \n", majorNumber);		
		return majorNumber;
	}	

	inDeviceClass = class_create(THIS_MODULE, CLASSNAME);
	if(IS_ERR(inDeviceClass))
	{
		unregister_chrdev(majorNumber, DEVICENAME);
		printk(KERN_INFO "Failed to register device class \n");
		return PTR_ERR(inDeviceClass);
	}

	inDeviceDevice = device_create(inDeviceClass, NULL, MKDEV(majorNumber,0), NULL, DEVICENAME);
	if(IS_ERR(inDeviceDevice))
	{
		class_destroy(inDeviceClass);
		unregister_chrdev(majorNumber, DEVICENAME);
		printk(KERN_INFO "Failed to create device \n");
	}

	//initialize mutex
	mutex_init(&mutexLock);

	return 0;
}

void cleanup_module(void)
{
	mutex_destroy(&mutexLock);

	device_destroy(inDeviceClass, MKDEV(majorNumber,0));
	class_unregister(inDeviceClass);	
	class_destroy(inDeviceClass);

	//unregister char device driver
	unregister_chrdev(majorNumber, DEVICENAME);
	
	printk(KERN_INFO "Goodbye from Input \n");	
}


/* Methods */

int devOpen(struct inode *inodePtr, struct file *filePtr)
{
	printk(KERN_INFO "Input device has been opened \n");
	
	return 0;
}


ssize_t devRead(struct file *filePtr, char *buffer, size_t length, loff_t *offset)
{
	printk(KERN_INFO "Sorry, input device can't read! \n");

	return 0;
}


ssize_t devWrite(struct file *filePtr, const char *buffer, size_t length, loff_t *offset)
{
	//turn on lock	
	if(!mutex_trylock(&mutexLock)) {
		printk(KERN_INFO "Sorry, input device in use! \n");
		return -EBUSY;
	}	

	printk(KERN_INFO "Device is writing \n");
	printk(KERN_INFO "EchoString: %s", buffer);

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

	printk(KERN_INFO "CurrentBuffSize: %d \n", position);

	//release lock
	mutex_unlock(&mutexLock);

	return length;
}

int devRelease(struct inode *inodePtr, struct file *filePtr)
{
	printk(KERN_INFO "Input device has been closed \n\n");

	return 0;
}

