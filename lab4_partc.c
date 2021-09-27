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

MODULE_LICENSE ("Dual BSD/GPL");

int memory_open (struct inode *inode, struct file *filp);
int memory_release (struct inode *inode, struct file *filp);
ssize_t memory_read (struct file *filp, char *buf, size_t count,
		     loff_t * f_pos);
ssize_t memory_write (struct file *filp, const char *buf, size_t count,
		      loff_t * f_pos);
void memory_exit (void);
int memory_init (void);

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
int pos = 0;
int wrap = 0;

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
  memory_buffer = kmalloc (5, GFP_KERNEL); // need to "allocate" 5 bytes
  if (!memory_buffer)
    {
      result = -ENOMEM;
      goto fail;
    }

  memset (memory_buffer, 0, 5);
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
memory_read (struct file * filp, char *buf, size_t count, loff_t * f_pos) //when calling read (cat /dev/memory), how is count determined? how is the size of the requested data transfer known?
{


  // the count is just the avaible space to write to, its not actaully the number of bytes that we are writting

  int rv;
  /* Transfering data to user space */
  /* Changing reading position as best suits */

  if (*f_pos == 0) {

    int start = 0;

    // since pos is indexing to the next place to write, we need to subtract 1 (or wrap around back to 4)
    if (wrap == 1) {
      if (pos == 0) {
        start = 4;
      } else {
        start = pos - 1;
      }
    } else {
      start = pos - 1;
    }

    // read cirular buf backwards
    int mem_buff_i = start;
    int buff_i = 0;

    for (int i = 0; i < 5; i++) {
      rv=copy_to_user (buf + i, memory_buffer + mem_buff_i, 1);

      if(rv) {
        printk("copy to user failed");
        return(0);
      }

      mem_buff_i = mem_buff_i == 0 ? mem_buff_i = 4 : mem_buff_i - 1;

      if (((i == pos) && wrap == 0)) {
        break;
      }
    }
    ret = wrap == 1 : 5 ? pos;
    return ret;
} else {
  return 0;
}

ssize_t
memory_write (struct file * filp, const char *buf, size_t count, loff_t * f_pos)
{
  int rv;
  const char *tmp;
  temp = buf;

  // circular buf
  for (size_t i = 0; i < count; i++) {
    tmp = tmp + 1;
    rv = copy_from_user (memory_buffer + pos, tmp, 1);
    pos++;
    if(pos == 5) {
      pos = 0;
      wrap = 1;
    }

    if(rv) {
       printk("copy from user failed");
       return(0);
    }
  }
  return count;
}
