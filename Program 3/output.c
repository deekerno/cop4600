#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/fs.h>    //character driver support
#include <asm/uaccess.h>
#include "p3.h"

#define DEVICENAME "outDevice"
#define CLASSNAME "outDev"

MODULE_LICENSE("GPL");

//Prototypes. taken from fs.h
int devOpen(struct inode *inodePtr, struct file *filePtr);
ssize_t devRead(struct file *filePtr, char *buffer, size_t length, loff_t *offset);
ssize_t devWrite(struct file *filePtr, const char *buffer, size_t length, loff_t *offset);
int devRelease(struct inode *inodePtr, struct file *filePtr);

//Globals
int majorNumber, i;
extern char message[MAXSIZE];
extern int position;

static DEFINE_MUTEX(mutexLock);  //declare mutex

struct class* outDeviceClass = NULL;
struct device* outDeviceDevice = NULL;

//use this struct from fs.h and give names to the functions
struct file_operations fOps =
{
	.owner = THIS_MODULE,
	.open = devOpen,        //open device
	.read = devRead,		//read from device
	.write = devWrite,      //output device can't write
	.release = devRelease,  //close device
};


int init_module(void)
{
	printk(KERN_INFO "Initializing the output device driver \n\n");	

	//register char device driver
	majorNumber = register_chrdev(0, DEVICENAME, &fOps);

	if(majorNumber < 0)
	{
		printk(KERN_INFO "Device registering failed with %d \n", majorNumber);		
		return majorNumber;
	}	

	outDeviceClass = class_create(THIS_MODULE, CLASSNAME);
	if(IS_ERR(outDeviceClass))
	{
		unregister_chrdev(majorNumber, DEVICENAME);
		printk(KERN_INFO "Failed to register device class \n");
		return PTR_ERR(outDeviceClass);
	}

	outDeviceDevice = device_create(outDeviceClass, NULL, MKDEV(majorNumber,0), NULL, DEVICENAME);
	if(IS_ERR(outDeviceDevice))
	{
		class_destroy(outDeviceClass);
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

	device_destroy(outDeviceClass, MKDEV(majorNumber,0));
	class_unregister(outDeviceClass);	
	class_destroy(outDeviceClass);

	//unregister char device driver
	unregister_chrdev(majorNumber, DEVICENAME);
	
	printk(KERN_INFO "Goodbye from Output \n");	
}


/* Methods */

int devOpen(struct inode *inodePtr, struct file *filePtr)
{
	printk(KERN_INFO "Output device has been opened \n");
	
	return 0;
}

ssize_t devRead(struct file *filePtr, char *buffer, size_t length, loff_t *offset)
{
	//turn on lock	
	if(!mutex_trylock(&mutexLock)) {
		printk(KERN_INFO "Sorry, input device in use! \n");
		return -EBUSY;
	}		

	printk(KERN_INFO "Device is reading \n");

	if(position==0 && strlen(message)==0) {
		printk(KERN_INFO "Nothing to read \n");
	}

	else { 
		//print message and empty whole array afterwards
        printk(KERN_INFO "StoredContents: %s \n", message);
		memset(message, 0, MAXSIZE);
        position = 0;

		printk(KERN_INFO "Reading finished \n");
	}

	printk(KERN_INFO "CurrentBuffSize: %d \n", position);

	//release lock
	mutex_unlock(&mutexLock);

	return 0;
}


ssize_t devWrite(struct file *filePtr, const char *buffer, size_t length, loff_t *offset)
{
	printk(KERN_INFO "Sorry, output device can't write! \n");	

	return length;
}


int devRelease(struct inode *inodePtr, struct file *filePtr)
{
	printk(KERN_INFO "Output device has been closed \n\n");

	return 0;
}

