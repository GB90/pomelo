/*********************************************************************************
 *      Copyright:  (C) 2012 KEWELL
 *
 *       Filename:  dev_led.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(09/26/2012~)
 *         Author:  WENJING <WENJIGN0101@GMAIL.COM>
 *      ChangeLog:  1, Release initial version on "09/26/2012 09:42:03 AM"
 *                  2,
 *                 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 ********************************************************************************/

#include "include/plat_driver.h"

/*Driver version*/
#define DRV_MAJOR_VER             2
#define DRV_MINOR_VER             0
#define DRV_REVER_VER             0

#define DEV_NAME                  DEV_LED_NAME

#ifndef DEV_MAJOR
#define DEV_MAJOR                 0 /*dynamic major by default */
#endif

static struct cdev dev_cdev;
static struct class *dev_class = NULL;

static int debug = DISABLE;
static int dev_major = DEV_MAJOR;
static int dev_minor = 0;

module_param(debug, int, S_IRUGO);
module_param(dev_major, int, S_IRUGO);
module_param(dev_minor, int, S_IRUGO);

#define dbg_print(format,args...) if(DISABLE!=debug) \
        {printk("[kernel] ");printk(format, ##args);}    

#define ALL_LED       0xff  /*control all led on/off/blink*/

#define ON            1
#define OFF           0
#define BIT_BLINK     0  /* led_status bit[0]: 1->Blink mode, 0->common mode */

static unsigned char led_status[LED_COUNT];

const int LED[LED_COUNT] = {LED_D1_RUN, LED_D2_0, LED_D3_1, LED_D4_2, LED_D5_3, LED_D6_4, LED_D7_5, LED_D8_6};

static struct timer_list blink_timer;

static void turn_led_onoff(unsigned int cmd, unsigned int arg)
{
    int i;
    int level;

    level = (ON == cmd) ? LOWLEVEL : HIGHLEVEL;
    
    if(ALL_LED == arg)
    {
        for(i=0; i<LED_COUNT; i++)
        {
            CLR_BIT(led_status[i], BIT_BLINK);  
            at91_set_gpio_value(LED[i], level);
        }
    }
    else
    {
        CLR_BIT(led_status[arg], BIT_BLINK); 
        at91_set_gpio_value(LED[arg], level);
    }
}

static void turn_led_blink(unsigned int arg)
{
    int i;

    if(ALL_LED == arg)
    {
        for(i=0; i<LED_COUNT; i++)
            SET_BIT(led_status[i], BIT_BLINK);  
    }
    else
    {
        SET_BIT(led_status[arg], BIT_BLINK);  
    }
}

static int led_open(struct inode *inode, struct file *file)
{
    return 0;
}

static int led_release(struct inode *inode, struct file *file)
{
    return 0;
}

//compatible with kernel version 2.6.38 
static long led_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	switch (cmd)
    {
		case SET_DRV_DEBUG:
			dbg_print("%s driver debug now.\n", DISABLE == arg ? "Disable" : "Enable");
			if (0 == arg)
				debug = DISABLE;
			else
				debug = ENABLE;
			break;
        
		case GET_DRV_VER:
			print_version(DRV_VERSION);
			return DRV_VERSION;
		
		case LED_ON:
			if(LED_COUNT <= arg)
                return -1;
            turn_led_onoff(ON, arg);
            break;

		case LED_OFF:
            if(LED_COUNT <= arg)
                return -1;
            turn_led_onoff(OFF, arg); 
            break;

        case 2:       /*use the ioctl command in kernel 2.6.22, compatible the old application*/
        case LED_BLINK:
            if(LED_COUNT <= arg)
                return -1;
            turn_led_blink(arg);
            break;

        case ALL_LED_ON:
            turn_led_onoff(ON, ALL_LED);
            break;

        case ALL_LED_OFF:
            turn_led_onoff(OFF, ALL_LED);
            break;

		case ALL_LED_BLINK:
            turn_led_blink(ALL_LED);    
            break;
			
		default:
			dbg_print("%s driver don't support ioctl command=%d\n", DEV_NAME, cmd);
			return -1;
    }

    return 0;
}

static struct file_operations led_fops = {
    .owner = THIS_MODULE,
    .open = led_open,
    .release = led_release,
    .unlocked_ioctl = led_ioctl, //compatible with kernel version 2.6.38 
};

static void blink_timer_handle(unsigned long arg)
{
    int  i;

    for(i=0; i<LED_COUNT; i++)
    {
        if(ON == GET_BIT(led_status[i], BIT_BLINK))  /* LED is in blink mode */
        {
            if(LOWLEVEL == at91_get_gpio_value(LED[i]))
            {
                at91_set_gpio_value(LED[i], HIGHLEVEL);
            }    
            else
            {
                at91_set_gpio_value(LED[i],LOWLEVEL);
            }
        }
    }
    mod_timer(&blink_timer, jiffies + HZ/2);
}   


static void led_cleanup(void)
{
    int i;
    dev_t devno = MKDEV(dev_major, dev_minor);

    del_timer(&blink_timer);

    for(i=0; i<LED_COUNT; i++)
        at91_set_gpio_value(LED[i], HIGHLEVEL);  /* Turn all LED off */
    
    device_destroy(dev_class, devno);   
    class_destroy(dev_class);

    cdev_del(&dev_cdev);

    unregister_chrdev_region(devno, 1);

    printk("%s driver removed\n", DEV_NAME);
}

static int __init led_init(void)
{
    int result;
    int i;
    dev_t devno;

    /*init led hardware, default: off*/
    for(i=0; i<LED_COUNT; i++)
        at91_set_gpio_output(LED[i], HIGHLEVEL);     

    /* Alloc for the device for driver */
    if (0 != dev_major)
    {
        devno = MKDEV(dev_major, dev_minor);
        result = register_chrdev_region(devno, 1, DEV_NAME);
    }
    else
    {
        result = alloc_chrdev_region(&devno, dev_minor, 1, DEV_NAME);
        dev_major = MAJOR(devno);
    }

    /*Alloc for device major failure */
    if (result < 0)
    {
        printk("%s driver can't get major %d\n", DEV_NAME, dev_major);
        return result;
    }
	
    /*Initialize cdev structure and register it*/
	cdev_init (&dev_cdev, &led_fops);
	dev_cdev.owner 	= THIS_MODULE;
	
	result = cdev_add (&dev_cdev, devno , 1);
	if (result)
	{
	    printk (KERN_NOTICE "error %d add led device", result);
		goto ERROR;
	}

    dev_class = class_create(THIS_MODULE, DEV_NAME);
    if(IS_ERR(dev_class)) 
    {           
        printk("%s driver create class failture\n",DEV_NAME);           
        result =  -ENOMEM;  
        goto ERROR;   
    }       

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,24)     
    device_create(dev_class, NULL, devno, NULL, DEV_NAME);
#else
    device_create (dev_class, NULL, devno, DEV_NAME);
#endif  
    
    /* Initial the LED blink timer */
    init_timer(&blink_timer);
    blink_timer.function = &blink_timer_handle;
    //blink_timer.data = 0; //only RUN_LED need to blink
    blink_timer.expires = jiffies + HZ;
    add_timer(&blink_timer);

    printk("%s driver version %d.%d.%d initiliazed\n", DEV_NAME, DRV_MAJOR_VER, DRV_MINOR_VER,
               DRV_REVER_VER);
	return 0;

ERROR:
    cdev_del(&dev_cdev);
    unregister_chrdev_region(devno, 1);
    return result;
}

module_init(led_init);
module_exit(led_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRV_AUTHOR);
MODULE_DESCRIPTION(DRV_DESC);

