#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>	/* printk() */
#include <linux/slab.h>		/* kmalloc() */
#include <linux/fs.h>		/* everything... */
#include <linux/errno.h>	/* error codes */
#include <linux/types.h>	/* size_t */
#include <linux/proc_fs.h>
#include <linux/fcntl.h>	/* O_ACCMODE */
#include <linux/uaccess.h>	/* copy_from/to_user */
#include <linux/gpio.h>

MODULE_LICENSE ("Dual BSD/GPL");


//These pins are for the RPI3 B, adjust if using a different board
#define EN1 20  //pwm pin, left motor
#define EN2 21  //pwm pin, right motor
#define A_1 6   //Y1, left motor positive
#define A_2 13  //Y2, left motor negative
#define A_3 19  //Y3, right motor positive
#define A_4 26  //Y4, right motor negative

int memory_open (struct inode *inode, struct file *filp);
int memory_release (struct inode *inode, struct file *filp);
ssize_t memory_read (struct file *filp, char *buf, size_t count,
		     loff_t * f_pos);
ssize_t memory_write (struct file *filp, const char *buf, size_t count,
		      loff_t * f_pos);
void memory_exit (void);
int memory_init (void);
void setPin(int PIN);

void setPin(int PIN) {
    
    // check if pin is valid
    if (!gpio_is_valid(PIN)) {
        printk("Invalid GPIO pin\n");
        return;
    }

    // requect the pin
    if (gpio_request(PIN, "PIN") < 0 ) {
        printk("failed request for GPIO pin set\n");
        return;
    }

    // set direction
    gpio_direction_output(PIN, 0);

    // export the pin
    if (gpio_export(PIN, false) < 0) {
        printk("GPIO pin could not be exported\n");
        return;
    }
    printk("GPIO pin %d exported... Pin state is currently: %d\n", PIN, gpio_get_value(PIN));
}


struct file_operations memory_fops = 
{
  .read = memory_read,
  .write = memory_write,
  .open = memory_open,
  .release = memory_release
};

module_init (memory_init);
module_exit (memory_exit);

int memory_major = 60;
char *memory_buffer;

int
memory_init (void)
{
  int result;
  result = register_chrdev (memory_major, "memory", &memory_fops);
  if (result < 0)
    {
      printk ("<1>memory: cannot obtain major number %d\n", memory_major);
      return result;
    }

  /* Allocating memory for the buffer */
  memory_buffer = kmalloc (1, GFP_KERNEL);
  if (!memory_buffer)
    {
      result = -ENOMEM;
      goto fail;
    }


  // init gpio pins
  setPin(EN1);
  setPin(EN2);
  setPin(A_1);
  setPin(A_2);
  setPin(A_3);
  setPin(A_4); 

  memset (memory_buffer, 0, 1);
  printk ("<1> Inserting memory module\n");
  return 0;

fail:
  memory_exit ();
  return result;
}

void
memory_exit (void)
{
  unregister_chrdev (memory_major, "memory");
  if (memory_buffer)
    {
      kfree (memory_buffer);
    }
  printk ("<1>Removing memory module\n");
}

int
memory_open (struct inode *inode, struct file *filp)
{
  return 0;
}

int
memory_release (struct inode *inode, struct file *filp)
{
  return 0;
}

ssize_t
memory_read (struct file * filp, char *buf, size_t count, loff_t * f_pos)
{
  int rv;
  /* Transfering data to user space */
  /* Changing reading position as best suits */
  if (*f_pos == 0)
    {
      rv=copy_to_user (buf, memory_buffer, 1);
      if(rv)
      {
        printk("copy to user failed");
        return(0);
      }
      *f_pos += 1;
      return 1;
    }
  else
    {
      return 0;
    }
}

ssize_t
memory_write (struct file * filp, const char *buf, size_t count, loff_t * f_pos)
{
  int rv;
  const char *tmp;
  tmp = buf + count - 1;
  rv=copy_from_user (memory_buffer, tmp, 1);
    if(rv)
    {
       printk("copy from user failed");
       return(0);
    }
  *f_pos += 1;


  // SET THE GPIO PINS
  if (memory_buffer[0] == 'F') {
    
    // left motor
    gpio_set_value(EN1, 1);
    gpio_set_value(A_1, 1);
    gpio_set_value(A_2, 0);

    // right motor
    gpio_set_value(EN2, 1);
    gpio_set_value(A_3, 1);
    gpio_set_value(A_4, 0);


  }
  else if (memory_buffer[0] == 'S') {

    // left motor
    gpio_set_value(A_1, 0);
    gpio_set_value(A_2, 0);

    // right motor
    gpio_set_value(A_3, 0);
    gpio_set_value(A_4, 0);

  }
  else if (memory_buffer[0] == 'L') {

    // left motor
    gpio_set_value(EN1, 1);
    gpio_set_value(A_1, 1);
    gpio_set_value(A_2, 0);

    // right motor
    gpio_set_value(A_3, 0);
    gpio_set_value(A_4, 0);

  } 
  else if (memory_buffer[0] == 'R') {

    // left motor
    gpio_set_value(A_1, 0);
    gpio_set_value(A_2, 0);

    // right motor
    gpio_set_value(EN2, 1);
    gpio_set_value(A_3, 1);
    gpio_set_value(A_4, 0);

  }
  else if (memory_buffer[0] == 'B') {
    
    // left motor
    gpio_set_value(EN1, 1);
    gpio_set_value(A_1, 0);
    gpio_set_value(A_2, 1);

    // right motor
    gpio_set_value(EN2, 1);
    gpio_set_value(A_3, 0);
    gpio_set_value(A_4, 1);
  }
  else {
    printk("error: unknown command\n");
  }


  return count;
}
