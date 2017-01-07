////////////////////////////////////////////////////////////////////////
//
//   Collaborators:  
//		Syed A Zawad 	(200132685	sazawad)
//		Sourabh S Saha (200157857	sssaha2)			
//
//   Description:
//     KeyValue Pseudo Device
//
////////////////////////////////////////////////////////////////////////

#include "keyvalue.h"

#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/poll.h>
#include <asm/uaccess.h>
#include <linux/semaphore.h>
#include <linux/cdev.h>

unsigned transaction_id=0;
struct semaphore sem;

/*NODE STRUCT DEFINITION*/
typedef struct node {
  __u64 key;
  __u64 size;
  void* data;
  struct node * next;
} node_kv;

/*DUMMY LINKED LIST HEAD*/
static node_kv * dummy_head;

/*Insertion, deletion and getter functions*/
static int linked_list_insert(node_kv *);
static int linked_list_delete(__u64);
static node_kv * linked_list_get(__u64);

/*Helper functions for insertion, deletion and get*/
static node_kv * new_node(__u64, __u64, void *);
static node_kv * get_previous_node(__u64);
static int delete_node(node_kv *);

/*Helper functions for printing, initializing and freeing
  data*/
static void __initialize_dummy_head(void);
static void print_linked_list(void);
static void free_memory(void);

/*Takes a node_kv element and inserts it into the list.
  returns -1 if the predecessor node is NULL, or the dummy head
  is NULL. If a node with a key that already exists enters, then
  the pre-existing node with the key will be removed, its space freed
  and replaced by the new node.*/
static int linked_list_insert(node_kv * node_to_insert){
  if(dummy_head == NULL){return -1;}
  node_kv * prev = get_previous_node(node_to_insert->key);
  if(prev == NULL){return -1;}
  if(prev->next == NULL){
    prev->next = node_to_insert;
    return 0;
  }
  if(prev->next->key == node_to_insert->key){
    delete_node(prev);
  }
  if(prev->next != NULL){
    node_kv * temp = prev->next;
    node_to_insert->next = temp;
  }
  prev->next = node_to_insert;
  return 0;
}

/*Takes a key and gets the node, if present. If not found, then
  returns -1. Then frees node and its data using kfree. The linked
  list chain is not broken.*/ 
static int linked_list_delete(__u64 key){
  node_kv * prev_node = get_previous_node(key);
  if(prev_node == NULL) {return -1;}
  if(prev_node->next == NULL) {return -1;}
  if(prev_node->next->key != key){return -1;}
  return delete_node(prev_node);
}

/*Takes in a key and returns the corresponding node in the linked list.
  If a node is not found, then returns NULL*/
static node_kv * linked_list_get(__u64 key){
  node_kv * prev_node = get_previous_node(key);
  if(prev_node == NULL) {return NULL;}
  if(prev_node->next == NULL) {return NULL;}
  if(prev_node->next->key == key){return prev_node->next;}
  return NULL;
}

/*Helper function - takes a key and returns the node that should be the predecessor
  of the node with the key. Meaning that even if a node with the specified key does not
  exist, it will figure out where the node should have been if it existed and returns the 
  predecessor of that node.*/
static node_kv * get_previous_node(__u64 key){
  if(dummy_head == NULL){return NULL;}
  node_kv * ptr = dummy_head;
  while((ptr->next != NULL) && (ptr->next->key < key))
    ptr = ptr->next;
  return ptr;
}

/*Helper function - Takes the successor of the given node and deletes it, freeing up
  memory. Returns 1 always*/
static int delete_node(node_kv * prev_node){
  node_kv * node_to_remove = prev_node->next;
  if(node_to_remove->next == NULL){prev_node->next = NULL;}
  else{prev_node->next = node_to_remove->next;}
  kfree(node_to_remove->data);
  kfree(node_to_remove);
  return 1;
}

/*Creates a node with the given key, size and data pointer. Returns null if 
  space for either the node or the data could not be allocated. NOTE * Since it is using GFP_KERNEL
  it is not expected to return nulls to often*/
static node_kv * new_node(__u64 key, __u64 size, void* data){
  node_kv* newNode = kmalloc(sizeof(node_kv),GFP_KERNEL);
  if(!newNode){return NULL;}
  void * new_data = kmalloc(size, GFP_KERNEL);
  if(!new_data){
    kfree(newNode);
    return NULL;
  }
  newNode->key = key;
  memset(new_data, 0, size);
  copy_from_user(new_data, data, size);
  newNode->size = size;
  newNode->data = new_data;
  newNode->next = NULL;
  return newNode;
}

/*HELPER FUNCTION - Creates a new node_kv struct with the given key,
  value and returns a pointer to it */
static void __initialize_dummy_head(){
  dummy_head = new_node(0, 0, 0);
}

/*HELPER FUNCTION - Prints out the full list*/
static void print_linked_list(){
  node_kv * ptr = dummy_head;
  printk(KERN_INFO"[LINKED LIST]=>");
  while(ptr->next != NULL){
    ptr = ptr->next;
    printk(KERN_INFO"=>NODE-[KEY:%d, SIZE:%d, VALUE:%s]", ptr->key, ptr->size, ptr->data);
  }
  printk(KERN_INFO"<=[LINKED LIST]");
}

/*HELPER FUNCTION - Used when unloading the module. Frees up the nodes 
  from the linked list*/
static void free_memory(){
  node_kv * node = dummy_head;
  node_kv * prev;
  while(node != NULL){
    prev = node;
    node = node->next;
    kfree(prev->data);
    kfree(prev);
  }
}

//----------------------------------------------------------------------------------------------------------------------------

static void free_callback(void *data)
{
}

static long keyvalue_get(struct keyvalue_get __user *ukv)
{
	
  down(&sem);
  unsigned temp=0;
 
  node_kv * node = linked_list_get(ukv->key);
  if(node == NULL){
    temp=-1;
  }
  else{
    copy_to_user(ukv->data, node->data, node->size);
    copy_to_user(ukv->size, &(node->size), sizeof(node->size));
    temp = transaction_id;
    transaction_id++;
  }
  up(&sem); 

  return temp;
}

static long keyvalue_set(struct keyvalue_set __user *ukv){
  down(&sem);
  unsigned temp=0;
  node_kv* node = new_node(ukv->key, ukv->size, ukv->data);
  if(!node){
    temp = -1;
  }else{
  linked_list_insert(node);
  
  temp = transaction_id;
  transaction_id++;
  }
  up(&sem);
  return temp;
}

static long keyvalue_delete(struct keyvalue_delete __user *ukv){
  down(&sem);
  int result = linked_list_delete(ukv->key);
  unsigned temp=0;
  if(result==-1){
    temp = result;
  }
  else{
  temp=transaction_id;
  transaction_id++;
  }
  up(&sem);	
  return temp;
}

//Added by Hung-Wei

unsigned int keyvalue_poll(struct file *filp, struct poll_table_struct *wait)
{
  unsigned int mask = 0;
  printk("keyvalue_poll called. Process queued\n");
  return mask;
}

static long keyvalue_ioctl(struct file *filp, unsigned int cmd,
			   unsigned long arg)
{
  switch (cmd) {
  case KEYVALUE_IOCTL_GET:
    return keyvalue_get((void __user *) arg);
  case KEYVALUE_IOCTL_SET:
    return keyvalue_set((void __user *) arg);
  case KEYVALUE_IOCTL_DELETE:
    return keyvalue_delete((void __user *) arg);
  default:
    return -ENOTTY;
  }
}

static int keyvalue_mmap(struct file *filp, struct vm_area_struct *vma)
{
  return 0;
}

static const struct file_operations keyvalue_fops = {
  .owner                = THIS_MODULE,
  .unlocked_ioctl       = keyvalue_ioctl,
  .mmap                 = keyvalue_mmap,
  //    .poll     = keyvalue_poll,
};

static struct miscdevice keyvalue_dev = {
  .minor = MISC_DYNAMIC_MINOR,
  .name = "keyvalue",
  .fops = &keyvalue_fops,
};

static int __init keyvalue_init(void)
{
  int ret;
  __initialize_dummy_head();
  sema_init(&sem,1);
  printk(KERN_INFO "KeyValue module inserted");
  if ((ret = misc_register(&keyvalue_dev)))
    printk(KERN_ERR "Unable to register \"keyvalue\" misc device\n");
  return ret;
}

static void __exit keyvalue_exit(void)
{
  int index;
  printk(KERN_INFO "KeyValue module removed");

  misc_deregister(&keyvalue_dev);
}

MODULE_AUTHOR("Hung-Wei Tseng <htseng3@ncsu.edu>");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
module_init(keyvalue_init);
module_exit(keyvalue_exit);
