#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/*NODE STRUCT DEFINITION*/
typedef struct node {
	int64_t key;
	int64_t data;
	struct node * next;
} node_kv;

/*SIZE OF HASHMAP*/
#define MAP_SIZE 10

/*DUMMY HEAD FOR LINKED LIST*/
node_kv * hash_map[MAP_SIZE];

/*HELPER FUNCTION - Creates a new node_kv struct with the given key, 
	value and returns a pointer to it */
node_kv * newNode(int64_t key, int64_t data){
	node_kv* newNode = malloc(sizeof(node_kv));
	newNode->key = key;
	newNode->data = data;
	newNode->next = NULL;
	return newNode;
}

/*HELPER FUNCTION - Checks if the next node to a given node exists
	if it does, then checks if the next node's key 
	value is the same as given */
		int isNextNodeEqualToKey(node_kv * prev, int64_t key){
			return ((prev->next) && (prev->next->key == key));
		}

/*HELPER FUNCTION - 
	Function to populate the dummy_heads hashmap */
		void __initialize_dummy_heads_for_hash_map(){
			for(int index = 0; index<MAP_SIZE; index++)
				hash_map[index] = newNode(0,0);
		}

/*HELPER FUNCTION - This function will traverse over the
	linked list until it reaches the end or until the node
	it is currently on is less than the given key. Then returns
	the node it is currently on. Basically, it will return
	the previous node of an node that could have the given key.
	This was made due to reusability purposes */
	node_kv * getPrev(node_kv * dummy_head, int64_t key){
		node_kv * ptr = dummy_head;
		while((ptr->next) && (ptr->next->key < key))
			ptr = ptr->next;
		return ptr;
	}


/*HELPER FUNCTION - Prints the full linked list in order.
	Used for logging */
	void printLinkedList(node_kv * dummy_head){
		node_kv * ptr = dummy_head;
		printf("[DUMMY-HEAD]");
		while(ptr->next){
			ptr = ptr->next;
			printf("=>[KEY:%lu, VAL:%lu]", ptr->key, ptr->data);
		}
		printf("=>[END]\n");
	}

/*HELPER FUNCTION - Prints the hash_map*/
	void print_hash_map(){
		for(int index = 0; index<MAP_SIZE; index++){
			printf("MAP[%d] : \n", index);
			printLinkedList(hash_map[index]);
		}
	}

/*INSERT FUNCTION - Inserts the given node to the linked list. This is an ascending,
	sorted linked list, so the function can insert the new node in 
	the middle of the list depending on the node's key value */
	int insert(node_kv * dummy_head, node_kv * node){
		node_kv * prev = getPrev(dummy_head, node->key);
		if(isNextNodeEqualToKey(prev, node->key)){
			prev->next->data = node->data;
		}else{
			node_kv * temp = prev->next;
			prev->next = node;
			node->next = temp;
		}
		return 0;
	}

/*DELETE FUNCTION - Removes the node with the given key (or does 
	nothing if the node was not found), and keeps the structure sorted */
	int delete(node_kv * dummy_head, int64_t key){
		node_kv * prev = getPrev(dummy_head, key);
		if(!isNextNodeEqualToKey(prev, key)) return -1;
		node_kv * node_to_remove = prev->next;
		node_kv * next = node_to_remove->next;
		prev->next = next;
		free(node_to_remove);
		return 0;
	}

/*GET FUNCTION - Retrives the node with the given key, or returns NULL
	if not found */
	node_kv * get(node_kv * dummy_head, int64_t key){
		node_kv * ptr = getPrev(dummy_head, key);
		if(isNextNodeEqualToKey(ptr, key))
			return ptr->next;
		return NULL;
	}


/*HELPER FUNCTION - Gets the appropriate dummy head 
	from the hash_map array */
	node_kv * get_dummy_head(int64_t key){
		return hash_map[key % MAP_SIZE];
	}

/*INSERT FUNCTION FOR HASHMAP */
	int hash_map_insert(node_kv * node){
		node_kv * dummy_head = get_dummy_head(node->key);
		return insert(dummy_head, node);
	}

/*DELETE FUNCTION FOR HASHMAP */
	int hash_map_delete(int64_t key){
		node_kv * dummy_head = get_dummy_head(key);
		return delete(dummy_head, key);
	}

/*GET FUNCTION FOR HASHMAP */
	node_kv * hash_map_get(int64_t key){
		node_kv * dummy_head = get_dummy_head(key);
		return get(dummy_head, key);
	}

	int main(){
		__initialize_dummy_heads_for_hash_map();
		node_kv * node = newNode(0,1);
		hash_map_insert(node);
		node = newNode(1,11);
		hash_map_insert(node);
		node = newNode(111,154);
		hash_map_insert(node);
		node = newNode(11,13);
		hash_map_insert(node);
		node = newNode(0,4);
		hash_map_insert(node);
		node = newNode(7,4);
		hash_map_insert(node);
		node = newNode(8,4);
		hash_map_insert(node);
		node = newNode(9,4);
		hash_map_insert(node);
		node = newNode(10,4);
		hash_map_insert(node);
		print_hash_map();
		hash_map_delete(11);
		hash_map_delete(16);
		hash_map_delete(0);
		print_hash_map();
		node = hash_map_get(16);
		if(node){
			printf("Got Node [KEY:%lu, VALUE:%lu]\n", node->key, node->data);
		}else{
			printf("Got No Node\n");
		}
		node = hash_map_get(26);
		if(node){
			printf("Got Node [KEY:%lu, VALUE:%lu]\n", node->key, node->data);
		}else{
			printf("Got No Node\n");
		}
		node = hash_map_get(7);
		if(node){
			printf("Got Node [KEY:%lu, VALUE:%lu]\n", node->key, node->data);
		}else{
			printf("Got No Node\n");
		}

		return 0;
	}
