#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>    //character driver support

#define DEVICENAME "charDevice"

//Prototypes. taken from fs.h
int devOpen(struct inode *inodePtr, struct file *filePtr);
ssize_t devRead(struct file *filePtr, char *buffer, size_t length, loff_t *offset);
ssize_t devWrite(struct file *filePtr, const char *buffer, size_t length, loff_t *offset);
int devRelease(struct inode *inodePtr, struct file *filePtr);

//Globals
int majorNumber;
char message[256];

//use this struct from fs.h and give names to the functions
struct file_operations fOps =
{
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


	return 0;
}

void cleanup_module(void)
{
	printk(KERN_INFO "Goodbye \n");	
	
	//unregister char device driver
	unregister_chrdev(majorNumber, DEVICENAME);
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
	
	return 0;
}

ssize_t devWrite(struct file *filePtr, const char *buffer, size_t length, loff_t *offset)
{
	printk(KERN_INFO "Device is writing \n");

	return length;
}

int devRelease(struct inode *inodePtr, struct file *filePtr)
{
	printk(KERN_INFO "Device has been closed \n");

	return 0;
}






