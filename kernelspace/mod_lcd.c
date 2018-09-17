#define VERSION "mod_lcd 1.0"

/* 	
Author : 
	Pierluigi iz4ako Guerzoni (user space driver)
	Munehiro (kernel space port)
	
Product: 
	mod_lcd

Info:
	kernel module for simple driving an lcd display based on HD44780 chip from Hitachi
	2x16 chars dot matrix.

Version:
	1.0 stable (seems to be)

License:

	GPL 
*/
   
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/ioport.h>
#include <asm/io.h> 
#include <asm/uaccess.h>
#if CONFIG_MODVERSIONS == 1
#define MODVERSIONS
#include <linux/modversions.h>
#endif

#include <linux/fs.h>
#include <linux/wrapper.h>
#include "mod_lcd.h"

#define SUCCESS 0

/* static variables */
/* let it be 1 when the device is busy */
static int device_open = 0;

static char lcd_kbuffer[LCD_KBUF_LEN+1];

struct file_operations fileops = {
	NULL,   /* seek */
	NULL,   /* TODO: add lcd_read, it returns the current
		     displayed message */
	lcd_write,
	NULL,   /* readdir */
	NULL,   /* select  */
	NULL,   /* TODO: add ioctl   */
	NULL,   /* mmap    */
	lcd_open,
	NULL,
	lcd_close,
};


int init_module()
{
	/* registering device */
	int i,ret;

	if ( (ret = register_lcd(MAJOR_NUM,DEVICE_NAME,&fileops)) < 0)
		return ret;
	
	/* we have allocated 32+1 chars, so we have to clean up
	   from 0 to 32 (33 chars: 32 for display plus 1 for last
	   termination). Normally, we use i < LCD_KBUF_LEN, so
	   we write from char 0 to char 31. After this write, the
	   'i' counter is set to 32, so we can directly set it to \0.
	   IN this case, we check i <= LCD_KBUF_LEN, so 'i' can ride
	   from 0 to 32 (33rd char) inclusive, and put a \0 over there.
	   Too much explained? maybe, but better explain deeply. This
	   module could fall in newbie's hands ;) */
	
	for (i = 0; i <= LCD_KBUF_LEN; i++) lcd_kbuffer[i] = 0;
														
	if (!check_region(PORT_ADDRESS,3)) {
		request_region(PORT_ADDRESS,3,"mod_lcd");
		printk("mod_lcd: successful check_region on %#x\n",PORT_ADDRESS);
		init_lcd();
	} else {
		printk("mod_lcd: unable to check region %#x\n",PORT_ADDRESS);
		unregister_lcd(MAJOR_NUM,DEVICE_NAME);
	}

	return SUCCESS;

}

void cleanup_module()
{
	release_region(PORT_ADDRESS,3);
	unregister_lcd(MAJOR_NUM,DEVICE_NAME);
}


/* aux functions */

static void unregister_lcd(int major, char *device_name) {

	int ret = module_unregister_chrdev(major,device_name);
	if (ret < 0)
		printk("mod_lcd: Error in module_unregister_chrdev: %d\n", ret);
	else
		printk("mod_lcd: Major number %d successfully unregistered\n", major);
		
}

static int register_lcd(int major, char *device_name, struct file_operations *fileops) {
	
	int ret = module_register_chrdev(major, device_name, fileops);
	if (ret < 0) {
		printk("mod_lcd: Unable to register major number %d for lcd driver: error %d\n",major,ret);
		return ret;
	}

	printk("mod_lcd: Module successfully loaded. %s assigned major number %d\n",device_name,major);

	return SUCCESS;
}

static void init_lcd(void) {

outb(inb(CONTROL) & 0xDF, CONTROL); /* reset control port */
outb(inb(CONTROL) | 0x01, CONTROL); /* reset strobe */
outb(inb(CONTROL) | 0x08, CONTROL); /* reset pin 17 */
outb(inb(CONTROL) | 0x04, CONTROL); /* set pin 11 */

send_lcd_raw_data(0x0f,40); /* off display off cursor off blink */
send_lcd_raw_data(0x38,40); /* on interface data length 
							   on number of display line
							   off character font */
send_lcd_raw_data(0x01,40); /* clear display and home */
send_lcd_raw_data(0x0c,40); /* turn on display */

}

static void send_lcd_raw_data(char token,int msdelay)
{
	int usdelay=msdelay*1000;
	outb(token,DATA);
	outb(inb(CONTROL) & 0xFE,CONTROL); /* set strobe pin 1 */
	udelay(usdelay);
	outb(inb(CONTROL) | 0x01,CONTROL); /* reset strobe */
	udelay(usdelay);
}
					
inline static void clear_display(void) { send_lcd_raw_data(0x01,40); }
inline static void set_cursor_home(void) { send_lcd_raw_data(0x02,40); }

void write_string_to_lcd(char *buffer,int len) {

	int i, count;

	outb(inb(CONTROL) & 0xF7, CONTROL); /* set pin 17 (SLCT_IN) for sending data */
	
	for (i=0; i < len ; i++) {
		/* for now, an \n char will be replaced by a space */
		if (buffer[i]=='\n') send_lcd_raw_data(' ',2);
		else send_lcd_raw_data(buffer[i],2);
		if (i == 15) /* append padding data for lcd */
		for (count=0;count<24;count++) send_lcd_raw_data(' ',2);
	}
	outb(inb(CONTROL) | 0x08, CONTROL); /* reset pin 17 (SLCL_IN) */
}


/* device handling functions */
/* opening device */

static int lcd_open(struct inode *inode, struct file *file)
{

#ifdef DEBUG
	printk("mod_lcd: opening device\n");
#endif
		
	if (device_open) {
#ifdef DEBUG
		printk("mod_lcd: device busy in lcd_open\n");
#endif
		return -EBUSY;
	}
	device_open++; /* lock device from simultaneous use */
	MOD_INC_USE_COUNT; /* so we prevent rmmod while we are using
						 our device */
	return SUCCESS;

}

/* closing device */

static int lcd_close(struct inode *inode, struct file *file)
{
#ifdef DEBUG
	printk("mod_lcd: closing device\n");
#endif
	device_open--;	/* release device */
	MOD_DEC_USE_COUNT; /* release module */
	return SUCCESS;
}


/* this function is called whenever we write to our device */

static ssize_t lcd_write( struct file *file,
						const char *buffer,
						size_t len,
						loff_t* offset)
{

	int i;
	
	for (i=0; i < len && i < LCD_KBUF_LEN; i++) {
		/* because the buffer is in user space and the message
		   buffer is in kernel space, a simple assignment
		   wouldnt' work... instead we use get_user. 
		   BTW, i tried a simple assignment, and
		   it works, but maybe only 'cause i'm moving
		   a small set of data. I'm not so experienced
		   with kernel stuff, but a friend told me is
		   a memory page trick.
		   be warned: we pass a char and a char* to
		   get_user function, NOT two char*'s.
		   I read into kernel includes (uaccess.h)
		   that this joke is useful to read more than one
		   byte at a time (ie for read an int from user
		   space.) I hope i'll understand something more
		   in the future :) */
		get_user(lcd_kbuffer[i],buffer+i); 
	} 

	lcd_kbuffer[i]=0; /* terminate our kernel buffer */


#ifdef DEBUG
	printk("mod_lcd: lcd_write(%s,%d)\n",lcd_kbuffer,i);
#endif
	
	clear_display();
	write_string_to_lcd(lcd_kbuffer,i);

	return i; /* number of chars read.
				 If an user process send us more than BUF_LEN
				 chars, kernel will call this function unless
				 we suck up all data */
	
}




