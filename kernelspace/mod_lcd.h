/* name for device */
#define DEVICE_NAME "lcd"

/* IO port address. This is for lpt2. I'll add */
/* insmod parameter support asap */
/* #define PORT_ADDRESS 0x378 uncomment this for lpt1 */
#define PORT_ADDRESS 0x278 /* for lpt2 */

/* other def's */
#define DATA PORT_ADDRESS
#define STATUS PORT_ADDRESS+1
#define CONTROL PORT_ADDRESS+2

/* this is the major number for our device. */
/* 60 is for experimental drivers. Check in */
/* kernel docs                              */

#define MAJOR_NUM 60

/* we drive an lcd display with a 16x2 array.      */
/* HD44780 chip supports up to 40 columns displays */
/* so we need to pad 24 (40 - 16) chars for the    */
/* first row                                       */

/* TODO: convert this static allocation in dynamic allocation
   'cause this module will drive a 4x10 and 2x40 lcd display
   too.. we'll pass display parameters at insmod time (what
   about PnP lcd displays? ;) ) */

#define LCD_KBUF_LEN 32

/* function prototypes */

static ssize_t lcd_write( struct file *, const char *, size_t, loff_t *);
static int lcd_open(struct inode *, struct file *);
static int lcd_close(struct inode *inode, struct file *file);
static void unregister_lcd(int major, char *device_name);
static int register_lcd(int major, char *device_name, struct file_operations *fileops);
static void send_lcd_raw_data(char token,int msdelay);

int init_module();
void cleanup_module();
static void write_to_lcd(char *,int );
static void send_lcd_data(char,int);
static void init_lcd(void);

/* missing inlined functions... new version soon */

