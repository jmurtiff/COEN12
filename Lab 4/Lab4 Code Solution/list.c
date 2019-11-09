/******************************************************************/
/* Jordan Murtiff												  */
/* Lab #4						 	 							  */
/* Lab Section: Wednesday (63650)	                              */
/* Date: Feburary 25, 2018				         				  */
/*													              */
/* This program is the implementation of the abstract data type   */
/* LIST. A LIST is an abstract data type in which operations (such*/
/* as retrievals, insertions, changes, and deletions can be done  */
/* anywhere in the list. In this case, the user only cares about  */
/* the main functions (radix.c and maze.c) and the outisde        */
/* implementation of these main programs is left up to the        */
/* programmer to be created. This LIST abstract data type is being*/
/* used with the basic data type of linked lists. Linked lists    */
/* are dynamic data structures whose length can be increased or   */
/* decreased at run time. In the case of linked lists, the        */
/* searching algorithim that must be used is sequential search    */
/* (since you cannot directly access an element in the middle of  */
/* the linked list without first traversing through the elements  */
/* before (for singly linked list) or after (for double linked    */
/* lists). For the case of list.c, we are using a circular doubly */
/* linked list that is allowed to either traverse from the end    */
/* (or tail end) to the beginning (or head) of the list or from   */
/* the beginning to the end. Since a linked list does contain a   */
/* space for data to be held, we are going to use void pointers to*/
/* hold the data in each node of our doubly linked circular list. */ 
/* The power in using void pointers in our LIST is that any data  */ 
/* type may be attributed for use to hold the information within  */
/* our LIST and ensure that any data type provided will work for  */ 
/* the user. The structure that makes up our LIST abstract data   */ 
/* type contains a integer variable (count) and a node pointer    */ 
/* "head" that will always point to the first node in our LIST    */ 
/* data type. Count will always tell us the current number of     */ 
/* elements within in our LIST data type and In the case of       */ 
/* list.c, we are using a dummy (or sentinel) node that will      */ 
/* always act as our first node in our LIST (even if the list     */ 
/* itself does not have any other nodes). This dummy node limits  */ 
/* the number of special cases for the deletion/insertion of nodes*/
/* within our LIST. In essence, the dummy node makes our          */ 
/* program easier to implement and easier to understand. In       */
/* addition to the head pointer, our LIST contains a function     */ 
/* pointer to the user defined compare function that will be used */ 
/* to compare two pieces of data within our LIST. Since we do not */ 
/* know exactly which data types are going to be used, we have to */ 
/* depend on the user's compare function that is given to us for  */
/* this implementation. In addition to the structure of our LIST  */ 
/* abstract data type, there is also a node structure that makes  */ 
/* up each node within our linked list (and therefore our LIST    */ 
/* data type). Each node contains a data slot (which can hold a   */
/* void pointer to whatever data type the user chooses to use) and*/
/* two node pointers that point to the previous node in the linked*/
/* list (prev) and the next node in the linked list (next). In    */ 
/* addition to the function alllowing the creation and deletion of*/
/* LIST data types, list.c can also return the current number of  */ 
/* elements within a LIST, and has the option of adding nodes at  */ 
/* the beginning and end of a LIST, removing items from the       */
/* beginning and end of a LIST, and also returning the data in the*/
/* current head or tail nodes within a LIST (with removal and     */ 
/* access asssuming that the LIST is currently not empty). With   */
/* our sequential search function, the LIST is able to remove and */ 
/* find specific items anywhere in the LIST and either return the */ 
/* data or remove the elements entirely. Finally, list.c is able  */ 
/* to return an array of void pointers that holds all the data    */ 
/* within the current LISTabstract data type.                     */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include "list.h"

//Include the decleration for the search function (since it is not included in list.h).
NODE* search(LIST*, void*, bool*);

/* Define a structure that will be the basis for our LIST abstract data type. It includes one integer
 * variable that contains the current number of nodes in our LIST abstract data type. Additionally, 
 * a LIST contains a head pointer that points to the first node in our linked list and a function
 * pointer that points to our user defined compare function that will be used to compare two elements
 * within our LIST. 
 */    
struct list
{	
	//An integer variable that tells us how many elements are currently in our LIST abstract data type. 
	int count;

	//The head pointer of our LIST that always points to the dummy node in our LIST abstract data type.
	struct node *head;
		
	//A pointer to the user defined "compare" function that compares two elements within our LIST and returns whether the elements are
	//the same or different from one another.
	int (*compare)();
};

/* Define a structure that will be the basis for the nodes in our LIST abstract data type. It includes one void
 * pointer that holds the data of each node and the two node pointers that allow our LIST to be a linked list. The
 * "next" pointer points to the next node succeeding a node in a LIST and the "prev" pointer points to the
 * previous node preceding a node in a LIST.
 */ 
struct node
{
	//The void pointer that points to the data of a specific node in our LIST.
	void *data;

	//The node pointer that points to the next node succeeding a node in a LIST.
	struct node *next;

	//The node pointer that points to the previous node preceding a node in a LIST.
	struct node *prev;
}; 

/* Function: To allocate memory for a LIST data type (both the structure and the dummy
 * node that is used at the beginning of the LIST). Additionally, since the LIST starts
 * off empty (only the dummy node is the only node allocated), the dummy node must have 
 * its next and prev pointers pointing to itself. Given that we are using void pointers
 * to store the data within our LIST, the user defined "compare" function is also set 
 * to equal the values given by this function within our LIST so their value may be used 
 * within our search function.
 * 
 * Efficency: O(1) 
 * 
 * Return Value: A pointer to the newly created LIST abstract data structure.
 */
LIST *createList(int (*compare)())
{	
	struct list *lp;
	lp = malloc(sizeof(struct list));
	assert(lp != NULL); 
	lp->count = 0;
	lp->compare = compare;	
	lp->head = malloc(sizeof(struct node));
	assert(lp->head != NULL);
	lp->head->next = lp->head;
	lp->head->prev = lp->head;
	return lp;
}

/* Function: To search through the LIST and find if the element void *elt exists within the LIST already using
 * sequential search. If void *elt does exist, then the function returns a node pointer that points to the location
 * where the element was found and sets the boolean "found" to true, otherwise, after searching through all elements
 * in the LIST, the function returns NULL and sets the boolean pointer "found" to be false. 
 *
 * Efficency: O(n)
 * 
 * Return Type: A NODE pointer to the NODE contaning element void* elt (if found in the LIST) and 
 * thereby the NODE that is to be removed from the LIST, otherwise, NULL if not found within the LIST.
 */
NODE* search(LIST *lp, void *item, bool *found)
{
	assert(found != NULL);
	*found = false;
	assert(lp != NULL);
	assert(item != NULL);
	if(lp->count == 0)
	{
		return NULL;
	}
	NODE *pLocate = lp->head->next;
	while(pLocate != lp->head)
	{
		if((*lp->compare)(item, pLocate->data) == 0)
			{
				*found = true;
				return pLocate;
			}
		else
			{
				pLocate = pLocate->next;
			}
	}
	*found = false;
	return NULL;
}

/* Function: Frees all the memory previously allocated from createLIST. The
 * function frees all nodes within the LIST starting from the last (or tail)
 * node and working up until the front (or head) node is reached. Once all 
 * nodes have been freed, the list itself is then freed.
 * 
 * Efficency: O(n)
 * 
 * Return Value: No return value.
 */
void destroyList(LIST *lp)
{
	assert(lp != NULL);
	NODE *pDel;
	NODE *pPrev = lp->head->prev;
	do
	{
		pDel = pPrev;
		pPrev = pDel -> prev;
		free(pDel);
	}
	while (pDel != lp->head);
		free(lp);	
}

/* Function: Returns the integer variable count as part of the LIST structure, used to find
 * out how many elements are currently in the LIST at one point in time.
 * 
 * Efficency: O(1)
 * 
 * Return Type: An integer variable, being the number of nodes currently in 
 * the LIST abstract data type.
 */
int numItems(LIST *lp)
{
	assert(lp!=NULL);
	return lp->count;
}


/*
 * Function: Allocates memory and adds a new node at the beginning of the LIST after the
 * dummy node. The newly created node's data is set to equal the passed in void pointer that 
 * is given. Since the newly allocated node is inserted after the dummy node, its "next" pointer
 * points to the node currently after the dummy node while its "prev" pointer points to the 
 * dummy node. Once those two pointers are set, the new node's subsequent node "prev" pointer
 * now points to the new node and the dummy node's "next" pointer points to the new node. Count
 * is increased by one to show that a new node has been inserted into the LIST.  
 *
 * Efficency: O(1)
 *
 * Return Type: No return value.
 */ 
void addFirst(LIST *lp, void *item)
{
	assert(lp != NULL);
	assert(item != NULL);
	NODE *pNew = malloc(sizeof(struct node));
	assert(pNew != NULL);
	pNew->data = item;	
	pNew->next = lp->head->next;
	pNew->prev = lp->head;
	lp->head->next->prev = pNew;
	lp->head->next = pNew;
	lp->count+=1;
}


/*  
 * Function: Allocates memory and adds a new node at the end of the LIST following the previously
 * last node. The newly created node's data is set to equal the passed in void pointer that 
 * is given. Since the newly allocated node is inserted at the end of the LIST, its "next" pointer
 * must point to the dummy node at the beginning of the LIST (since the linked list is circular) and
 * its "prev" pointer points to the previously last node in the LIST. Once those two pointers are
 * set, the new node's predecessor "next" pointer must point to the new node and the dummy node's
 * "prev" pointer must point to the new node (also to keep the linked list circular).
 *
 * Efficency: O(1)
 *
 * Return Type: No return value.  
 */ 
void addLast(LIST *lp, void *item)
{
	assert(lp != NULL);
	assert(item != NULL);
	NODE *pNew = malloc(sizeof(struct node));
	assert(pNew != NULL);
	pNew->data = item;
	pNew->next = lp->head;
	pNew->prev = lp->head->prev;
	lp->head->prev->next = pNew;
	lp->head->prev = pNew;
	lp->count+=1;
}

/*
 * Function: Frees the first node in the LIST following the dummy node and returns the data (void 
 * pointer) of the recently deleted node. After creating a void pointer to hold the data within
 * the node, the function changes the dummy node's "next" pointer to point the 2nd node in the 
 * LIST (the node subsequent to the one being deleted) and sets this new first node's "prev" pointer
 * to point to the dummy node. Once those two pointers are changed, the function frees the first 
 * node in the LIST, decreases count's value to show a node being removed, and returns the data (void
 * pointer) that was contained in the recently deleted node. 
 *
 * Efficency: O(1)
 *
 * Return Type: A void pointer that points to the data of the recently deleted node. 
 */ 
void *removeFirst(LIST *lp)
{
	assert (lp != NULL);
	assert (lp->count > 0);
	void *pDel = lp->head->next->data;
	NODE *pDeleted = lp->head->next;
	lp->head->next->next->prev = lp->head;
	lp->head->next = lp->head->next->next;
	free(pDeleted);
	lp->count-=1;
	return pDel;
}

/*
 * Function: Frees the last node in the LIST that is the predecessor to the dummy node and returns the
 * data (void pointer) of the recently deleted node. After creating a void pointer to hold the data within
 * the node, the function changes the "next" pointer of the 2nd to last node in the LIST to point to the 
 * dummy node and the dummy node's previous pointer to point to the 2nd to last node in the LIST. Once both
 * pointers are changed, the function frees the last node in the LIST, decreases count's value to show a node
 * being removed, and returns the data (void pointer) that was contained in the recently deleted node.
 *
 * Efficency: O(1)
 *
 * Return Type: A void pointer that points to the data of the recently deleted node. 
 */ 
void *removeLast (LIST *lp)
{
	assert (lp != NULL);
	assert (lp->count > 0);
	void *pDel = lp->head->prev->data;
	NODE *pDeleted = lp->head->prev;
	lp->head->prev->prev->next = lp->head->prev;
	lp->head->prev=lp->head->prev->prev;
	free(pDeleted);
	lp->count-=1;
	return pDel;
}

/*
 * Function: Returns the data (void pointer) contained in the node after the dummy node in the LIST.
 *
 * Efficency: O(1)
 *
 * Return Type: A void pointer that points to the data for the first node in the LIST.
 */ 
void *getFirst(LIST *lp)
{
	assert (lp != NULL);
	assert (lp->count > 0);
	return lp->head->next->data;
}

/*
 * Function: Returns the data (void pointer) contained in the last node (head's "prev" node) of the LIST.
 *
 * Efficency: O(1)
 *
 * Return Type: A void pointer that points to the data for the last node in the LIST.
 */ 
void *getLast(LIST *lp)
{
	assert (lp != NULL);
	assert (lp->count > 0);
	return lp->head->prev->data;
}

 
/* Function: The function searches the (non-empty) LIST looking for a specific node with the 
 * same data as void *item that is passed to the function. If a match is found, the soon to
 * be deleted node's previous node's "next" pointer will point to the successor of the soon to
 * be deleted node and the soon to be deleted node's successor "prev" pointer will point to the
 * soon to be deleted node's predecessor. Once this is all complete, decrease the count of the 
 * list by one to make up for the node being deleted. If the search function does not find the 
 * specific node to be removed, then the function returns with nothing being removed from the LIST.
 * 
 * Efficency: O(n)
 * 
 * Return Value: No return value.
 */
void removeItem(LIST *lp, void *item)
{
	bool found = false;
	assert (lp != NULL);
	assert (item != NULL);
	assert(lp->compare != NULL);
	assert(lp->count > 0);
	NODE *pRemove = search(lp, item,&found);
	if(found == false)
		{
			return;
		}
	else
		{
			pRemove->prev->next = pRemove->next;
			pRemove->next->prev = pRemove->prev;
			free(pRemove);
			lp->count--;
		}
}

/*
 * Function: Returns the element void *item if found by the search function. This function acts 
 * similar to a wrapper that calls the search function and the instead of returning the location
 * (or in this case, NODE pointer) of where an element is found, it instead returns the void pointer.
 *
 * Efficency: O(n)
 *
 * Return Value: The element void* item if found in the LIST, otherwise, NULL.
 */ 
void *findItem(LIST *lp, void *item)
{
	bool found = false;
	assert (lp != NULL);
	assert (item != NULL);
	NODE *pFind = search(lp, item,&found);
	if (found == false)
	{
		return NULL;
	}
	else
	{
		return pFind->data;
	}	
}


/* Function: Allocates memory for an array of elements in a LIST pointed to by 
 * lp. This function allows for the program to return the data within each node of
 * a LIST. Going through each node one by one starting at the node after the dummy
 * node, the function goes through and sets each data slot of each node equal to a 
 * subsequent index of an array of void pointers. Once the function has reached the 
 * dummy node (the node pointed to by the head pointer), the function then returns the 
 * array of void pointers.
 * 
 * Efficency: O(n)
 *  
 * Return Value: An array of all void pointers that are within our LIST data type.
 */
void *getItems(LIST *lp)
{
	assert(lp != NULL);
	void**dataSet = malloc(sizeof(void*)*lp->count);
	assert(dataSet != NULL);
	int count = 0;
	NODE *pElements = lp->head->next;
	while(pElements != lp->head)
	{
		dataSet[count] = pElements->data;
		pElements = pElements->next;
		count++;
	}
	return dataSet;
}
