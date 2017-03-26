#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>		//character driver support
#include <asm/uaccess.h>	//for special copying functions

#define BUFFER_SIZE 1024
#define DEVICENAME "charDevice"

//Prototypes. taken from fs.h
int devOpen(struct inode *inodePtr, struct file *filePtr);
ssize_t devRead(struct file *filePtr, char *buffer, size_t length, loff_t *offset);
ssize_t devWrite(struct file *filePtr, const char *buffer, size_t length, loff_t *offset);
int devRelease(struct inode *inodePtr, struct file *filePtr);

//Globals
int majorNumber;
char message[BUFFER_SIZE];

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
	int bytesRead;	
	int bytesToBeRead;
	int bytesUnableToRead;
	int maxBytesLeft;

	maxBytesLeft = BUFFER_SIZE - *offset;

	// If the requested length is more than we can return from the buffer,
	// return msg of requested length. If not, return the rest of the buffer.
	bytesToBeRead = maxBytesLeft > length ? length : maxBytesLeft;	

	printk(KERN_INFO "Device is reading \n");

	// copy_to_user returns the number of bytes unable to be read;
	// 0 indicates complete success
	bytesUnableToRead = copy_to_user(buffer, message + *offset, bytesToBeRead);
	
	bytesRead = bytesToBeRead - bytesUnableToRead;
	*offset += bytesRead;

	printk(KERN_INFO "Device has finished reading \n");
	return 0;
}

ssize_t devWrite(struct file *filePtr, const char *buffer, size_t length, loff_t *offset)
{
	int bytesToWrite;
	int bytesUnableToWrite;
	int bytesWritten;
	int maxBytesLeft;

	maxBytesLeft = BUFFER_SIZE - *offset;

	// If the requested length is more than we can write to the buffer,
	// write msg of requested length. If not, write until the end.
	bytesToWrite = maxBytesLeft > length ? length : maxBytesLeft;	
	
	printk(KERN_INFO "Device is writing \n");

	// Same idea as copy_to_user, only we're now taking the message from
	// the user and placing it inside the device buffer
	bytesUnableToWrite = copy_from_user(message + *offset, buffer, bytesToWrite);

	bytesWritten = bytesToWrite - bytesUnableToWrite;
	*offset += bytesWritten;
	
	printk(KERN_INFO "Device has finished writing \n");
	return 0;
}

int devRelease(struct inode *inodePtr, struct file *filePtr)
{
	printk(KERN_INFO "Device has been closed \n");

	return 0;
}






