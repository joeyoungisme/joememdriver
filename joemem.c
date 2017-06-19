#include<linux/types.h>
// MKDEV MAJOR MINOR ...

#include<linux/fs.h>
// register_chrdev_region alloc_chrdev_region unregister....

#include<linux/module.h>
// module_init & module_exit

#include<linux/cdev.h>
// cdev ... function

#include<linux/slab.h>
// kmalloc  kfree 

#include "joemem.h"

MODULE_AUTHOR("Joeyoung Chen");
MODULE_LICENSE("Daul BSD/GPL");

unsigned int joemem_major = JOEMEM_MAJOR;
unsigned int joemem_minor = 0;
unsigned int joemem_num_devs = JOEMEM_NUM_DEVS;

//Globle struct joemem_dev pointer, allocate in init_module
struct joemem_dev *joemem_dev;

struct file_operations joemem_fops = {
    .owner = THIS_MODULE
};

int joemem_dataclear(struct joemem_dev *dev)
{
    struct joemem_data *pdata, *next;

    for(pdata = dev->data; pdata; pdata = next) {
        next = pdata->next;
        kfree(pdata->data);
    }

    dev->total_size = 0;
    dev->data = NULL;

    return 0;
}

void joemem_cleanup_module(void)
{
    dev_t devnum = MKDEV(joemem_major, joemem_minor);

    printk(KERN_INFO "joemem driver exit \n");

    //kfree data space and cdev del every joemem_dev (device number)
    if(joemem_dev) {
        int index;
        for(index = 0; index < joemem_num_devs; ++index) {
            joemem_dataclear(joemem_dev + index);
            cdev_del(&joemem_dev[index].cdev);
        }
    }

    unregister_chrdev_region(devnum, joemem_num_devs);

    printk(KERN_INFO "joemem : unregister success\n");

}

void joemem_cdev_init(struct joemem_dev *dev, int index)
{
    int res, devnum = MKDEV(joemem_major, joemem_minor + index);
    
    //Initial struct joemem_dev 's cdev and fops
    cdev_init(&dev->cdev, &joemem_fops);
    dev->cdev.owner = THIS_MODULE;
    dev->cdev.ops = &joemem_fops;

    //cdev add relate device number and cdev 
    res = cdev_add(&dev->cdev, devnum, 1);
    if(res)
        printk(KERN_NOTICE "Error %d Adding joemem %d\n", res, index);
}

int joemem_init_module(void)
{
    int res, index;
    dev_t dev = 0;

    printk(KERN_INFO "Hello This is joemem init module \n");

    //Register Device Number,
    if(joemem_major) {
        dev = MKDEV(joemem_major, joemem_minor);
        res = register_chrdev_region(dev, joemem_num_devs, "joemem");
    }
    else {
        res = alloc_chrdev_region(&dev, joemem_minor, joemem_num_devs, "joemem");
        joemem_major = MAJOR(dev);
    }
    if(res < 0) {
        printk(KERN_WARNING "joemem : Can't get major %d\n", joemem_major);
        return res;
    }
        
    //kernel memory allocate, joemem_num_devs * sizeof(struct joemem_dev)
    joemem_dev = kmalloc(joemem_num_devs * sizeof(struct joemem_dev), GFP_KERNEL);
    if(!joemem_dev) {
        res = -ENOMEM;
        goto fail;
    }
    memset(joemem_dev, 0, joemem_num_devs * sizeof(struct joemem_dev));

    //cdev initial and add to device numver
    for(index = 0; index < joemem_num_devs; ++index) {
        joemem_dev[index].data_size = JOEMEM_DATA_SIZE;
        joemem_cdev_init(joemem_dev + index, index);
    }

    return 0;

fail:
    joemem_cleanup_module();
    return res;

}

module_init(joemem_init_module);
module_exit(joemem_cleanup_module);
