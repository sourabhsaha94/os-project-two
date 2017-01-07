//////////////////////////////////////////////////////////////////////
//                             North Carolina State University
//
//
//
//                             Copyright 2016
//
////////////////////////////////////////////////////////////////////////
//
// This program is free software; you can redistribute it and/or modify it
// under the terms and conditions of the GNU General Public License,
// version 2, as published by the Free Software Foundation.
//
// This program is distributed in the hope it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
// more details.
//
// You should have received a copy of the GNU General Public License along with
// this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
//
////////////////////////////////////////////////////////////////////////
//
//   Author:  Hung-Wei Tseng
//
//   Description:
//     Skeleton of KeyValue Pseudo Device
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

unsigned transaction_id;
struct semaphore sem;
/*NODE STRUCT DEFINITION*/
typedef struct node {
  __u64 key;
  __u64 size;
  void* data;
  struct node * next;
} node_kv;

/*SIZE OF HASHMAP*/
#define MAP_SIZE 10

/*DUMMY HEAD FOR LINKED LIST*/
node_kv * hash_map[MAP_SIZE];

/*DUMMY LINKED LIST HEAD*/
static node_kv * dummy_head;

static int linked_list_insert(node_kv *);
static int linked_list_delete(__u64);
static node_kv * linked_list_get(__u64, __u64, void *);

static node_kv * new_node(__u64, __u64);
static node * kv get_previous_node(__u64);
static int delete_node(node_kv *);

static void __initialize_dummy_heads_for_hash_map(void);
static node_kv * newNode(__u64, __u64, void *);

void __initialize_dummy_head(void);
void print_hash_map(void);

static int linked_list_insert(node_kv * node_to_insert){
    if(dummy_head == NULL){
      printk(KERN_INFO"[INSERT]: NO LINKED LIST HEAD FOUND. COULD NOT ADD NODE.");
      return -1;
    }

    node_kv * prev = get_previous_node(node_to_insert->key);
    if(prev == NULL){
      printk(KERN_INFO"[INSERT]: NO PARENT NODE FOUND. COULD NOT ADD NODE.");
      return -1;
    }

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

    printk(KERN_INFO"[INSERT]: SUCCESSFULLY ADDED NODE TO LINKED LIST.");
    return 0;
}

static int linked_list_delete(__u64 key){
  node_kv * prev_node = get_previous_node(key);
  if(prev_node == NULL) {return -1;}
  if(prev_node->next == NULL) {return -1;}
  if(prev_node->next->key != key){return -1;}
  delete_node(prev);
}

static node * kv linked_list_get(__u64 key, __u64 * size, void * data){
  node_kv * prev_node = get_previous_node(key);
  if(prev_node == NULL) {return NULL;}
  if(prev_node->next == NULL) {return NULL;}
  if(prev_node->next->key == key){return prev_node->next;}
  return NULL;
}

static node * kv get_previous_node(__u64 key){
   printk(KERN_INFO"[GET PREVIOUS]: LOOKING FOR NODE BEFORE KEY %d", key);
    if(dummy_head == NULL){
      printk(KERN_INFO"[GET PREVIOUS]: COULD NOT GET NODE SINCE DUMMY HEAD WAS NULL");
      return NULL;
    }

    node_kv * ptr = dummy_head;
    while((ptr->next != NULL) && (ptr->next->key < key))
      ptr = ptr->next;

    printk(KERN_INFO"[GET PREVIOUS]: GOT PREVIOUS NODE [KEY:%d]", ptr->key);
    return ptr;
}

int delete_node(node_kv * prev_node){
    node_kv * node_to_remove = prev_node->next;
    if(node_to_remove->next == NULL){
      prev_node->next = NULL;
    }else{
      prev_node->next = node_to_remove->next; 
    }
    printk(KERN_INFO"[DELETE NODE]: REMOVED NODE TO BE REMOVED FROM LIST");
    kfree(node_to_remove->data);
    kfree(node_to_remove);
    printk(KERN_INFO"[DELETE NODE]: FREED NODE MEMORY");
  }

static node_kv * new_node(__u64 key, __u64 size, void* data){
  node_kv* newNode = kmalloc(sizeof(node_kv),GFP_KERNEL);
  if(!newNode){
    printk(KERN_INFO"[NEW NODE]: COULD NOT ALLOCATE MEMORY IN KERNEL FOR NEW NODE.");
    return NULL;
  }
  void * new_data = kmalloc(size, GFP_KERNEL);
  if(!new_data){
    printk(KERN_INFO"[NEW NODE]: COULD NOT ALLOCATE MEMORY IN KERNEL FOR NEW NODE's DATA.");
    kfree(newNode);
    return NULL;
  }
  newNode->key = key;
  memset(new_data, 0, size);
  copy_from_user(new_data, data, size);
  newNode->size = size;
  newNode->data = new_data;
  newNode->next = NULL;
  printk(KERN_INFO"[NEW NODE]: CREATED NEW NODE [KEY:%d, SIZE:%d, DATA:%s]", newNode->key, newNode->size, newNode->data);
  return newNode;
}

/*HELPER FUNCTION - Creates a new node_kv struct with the given key,
  value and returns a pointer to it */
void __initialize_dummy_head(){
  dummy_head = newNode(0, 0, 0);
}


//----------------------------------------------------------------------------------------------------------------------------

  static void free_callback(void *data)
  {
  }

  static long keyvalue_get(struct keyvalue_get __user *ukv)
  {

    return transaction_id++;
  }

  static long keyvalue_set(struct keyvalue_set __user *ukv)
  {
  down_interruptible(&sem);	//hold lock
  node_kv* node = new_node(ukv->key, ukv->size, ukv->data);
  if(!node){
    printk(KERN_INFO"[KEYVALUE SET]: NO NEW NODE COULD BE CREATED.");
    return -1;
  }
  printk(KERN_INFO"[KEYVALUE SET]: NEW NODE CREATED SUCCESSFULLY.");
  linked_list_insert(node);
  printk(KERN_INFO"[KEYVALUE SET]: NEW NODE INSERTED SUCCESSFULLY.");
  print_hash_map();
  up(&sem); //release lock
  return transaction_id++;
}

static long keyvalue_delete(struct keyvalue_delete __user *ukv)
{
  //struct keyvalue_delete kv;
  int i = hash_map_delete(ukv->key);
  if(i==-1)
    printk(KERN_INFO "Nothing to delete");
  print_hash_map();
  return transaction_id++;
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
  //    .poll		  = keyvalue_poll,
};

static struct miscdevice keyvalue_dev = {
  .minor = MISC_DYNAMIC_MINOR,
  .name = "keyvalue",
  .fops = &keyvalue_fops,
};

static int __init keyvalue_init(void)
{
  int ret;
  __initialize_dummy_heads_for_hash_map();
  sema_init(&sem,1);
  printk(KERN_INFO "KeyValue module inserted");
  if ((ret = misc_register(&keyvalue_dev)))
    printk(KERN_ERR "Unable to register \"keyvalue\" misc device\n");
  return ret;
}

static void __exit keyvalue_exit(void)
{
  int index;
  for(index = 0; index<MAP_SIZE; index++){
    freeList(hash_map[index]);
  }

  printk(KERN_INFO "KeyValue module removed");
  // for(index =0;index<MAP_SIZE;index++){
  //  kfree(hash_map[index]);
  // }
  misc_deregister(&keyvalue_dev);
}

MODULE_AUTHOR("Hung-Wei Tseng <htseng3@ncsu.edu>");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
module_init(keyvalue_init);
module_exit(keyvalue_exit);
