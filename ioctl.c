#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

#define MY_MACIG 'G'
#define READ_IOCTL _IOR(MY_MACIG, 0, int)
#define WRITE_IOCTL _IOW(MY_MACIG, 1, int)
#define LSEEK_IOCTL _IOW(MY_MACIG, 3, int)
#define CHTAPE_IOCTL _IOR(MY_MACIG, 4, int)
#define FMTTAPE_IOCTL _IOW(MY_MACIG, 5, int)
#define GETSTATUS_IOCTL _IOR(MY_MACIG, 6, int)
#define SETSIZE_IOCTL _IOR(MY_MACIG,7,int)

typedef struct {
	char data[2000];
	off_t fSize;
	bool isUse;
} camera ;

static int major;
static camera cameras[10];
static camera* p=cameras;
static int j;
static off_t offs;

int device_ioctl(struct file *filep, unsigned int cmd, unsigned long arg) {
	int len = 2000;
	switch(cmd) {
		case READ_IOCTL: {
			copy_to_user((char *)arg, (p+j)->data+offs, p->fSize);
			printk("reading... offs: %ld j:%d\n",offs,j);
			break;
		}
		case WRITE_IOCTL: {
			if((p+j)->fSize>len) return -1;
			copy_from_user((p+j)->data, (char *)arg, (p+j)->fSize);
			offs=(p+j)->fSize;
			printk("writing...offs: %ld j:%d\n",offs,j);
			break;
		}	
		case FMTTAPE_IOCTL: {
			int i = 0;
			for( ; i < 1000 ; i++ ) (p+j)->data[i]='\0';
			( p + j )->isUse = false ;
			( p + j )->fSize = 0;
			offs =0;
			printk("formating...offs: %ld j:%d\n",offs,j);
			break;
		}
		case LSEEK_IOCTL: {
			if(arg>=0 && arg<(p+j)->fSize) offs=arg;
			printk("lseek...offs: %ld j:%d\n",offs,j);
			break;
		}
		case CHTAPE_IOCTL: {
			offs = 0;
			if(arg==1){
				j = 0;
				break;
			}
			if(j >= 0 && j < 10){
				if(j==9) j=0;
				else j++;
				printk("chcamera...offs: %ld j:%d\n",offs,j);
			}
			break;
		}	
		case GETSTATUS_IOCTL: {
			camera* p2= cameras;
			int x = 0 ;
			while( !p2->isUse && x < 10 ) {
					++p2; 
					++x;
			}
			if (!p2->isUse) return -1;
			return x;
		}
		case SETSIZE_IOCTL: {
			while( ( p + j )->isUse == true) device_ioctl(filep,CHTAPE_IOCTL,0);
			( p + j )->isUse = true;
			( p + j )->fSize = sizeof(char)*arg;
		}
		default : return -ENOTTY;
	}
	return len;
}

static struct file_operations fops = { .unlocked_ioctl = device_ioctl, };

static int __init cdevexample_module_init(void) {
	int i = 0 ;
	j = 0 ;
	offs = 0 ;
	for( ; i < 10 ; i++ ) {
		cameras[i].isUse = false;
		cameras[i].fSize = 0;
	}
	major = register_chrdev(245, "my_device", &fops);
	if (major < 0) {
     		printk ("Registering the character device failed with %d\n", major);
	     	return major;
	}
	printk("cdev example: assigned major: %d\n", major);
	printk("create node with mknod /dev/cdev_example c %d 0\n", major);
 	return 0;
}

static void __exit cdevexample_module_exit(void) {
	unregister_chrdev(245, "my_device");
}  

module_init(cdevexample_module_init);
module_exit(cdevexample_module_exit);
MODULE_LICENSE("GPL");
