#ifndef JOEMEM_H
#define JOEMEM_H

#ifndef JOEMEM_MAJOR
#define JOEMEM_MAJOR            0
#endif

#ifndef JOEMEM_NUM_DEVS
#define JOEMEM_NUM_DEVS         4
#endif

#define JOEMEM_DATA_SIZE        1024

struct joemem_data {
    void *data;
    struct joemem_data *next;
};

struct joemem_dev {
    // Data Point 
    struct joemem_data *data;

    unsigned int data_size;
    unsigned int total_size;

    //struct cdev (charator device)
    struct cdev cdev;
};
    



#endif
