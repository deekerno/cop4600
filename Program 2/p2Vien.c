#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/fs.h>    //character driver support
#include <asm/uaccess.h>

#define DEVICENAME "charDevice"
#define CLASSNAME "char"
#define MAXSIZE 10

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
	printk(KERN_INFO "Initializing the char device driver \n");	

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

	if(position == 0)
		printk(KERN_INFO "Nothing to read \n");

	else
	{
		//print message and empty whole array afterwards
        printk(KERN_INFO "MessageContents: %s", message);
        for(i=0; i<sizeof(message); i++)
        {
            message[i] = ' ';
        }
        position = 0;
	}

	return 0;
}

ssize_t devWrite(struct file *filePtr, const char *buffer, size_t length, loff_t *offset)
{
	printk(KERN_INFO "Device is writing \n");

	//copy_from_user(message, buffer, length-1);
	printk(KERN_INFO "buffer: %s", buffer);

	if(position >= MAXSIZE)
	{
		printk(KERN_INFO "Buffer is full \n");
		//return 0; //causes like an infinite loop
	}

	else
	{
		for(i=0; i<length-1; i++)
		{			
			message[position] = buffer[i];
			position++;
			
			if(position >= MAXSIZE)
				break;
		}
	}

	printk(KERN_INFO "message: %s", message);

/*
	if(position == 10)
	{
		for(i=0; i<10; i++)
			printk(KERN_INFO "%c\n", message[i]);
	}
*/

	return length;
}

int devRelease(struct inode *inodePtr, struct file *filePtr)
{
	printk(KERN_INFO "Device has been closed \n");

	return 0;
}






