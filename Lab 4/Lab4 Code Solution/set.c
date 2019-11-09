/******************************************************************/
/* Jordan Murtiff												  */
/* Lab #4						 	 							  */
/* Lab Section: Wednesday (63650)	                              */
/* Date: Feburary 25, 2018				         				  */
/*													              */
/* This program is the implementation of the abstract data type   */
/* SET. A SET is a unsorted collection of distinct elements. In   */
/* this case, the user only cares about the main functions        */
/* (unique.c and parity.c) and the outside implementation of these*/
/* main programs is left up to the programmer to be created. Hash */
/* tables are the method of searching the SET data type and are   */
/* used to find the home location for the insertions and          */ 
/* deletions of specific elements within the hash table. Void     */ 
/* pointers are used to within our SET to hold any data type the  */ 
/* client chooses to use. The power in using void pointers in our */ 
/* SET is that any data type may be attributed for use to hold the*/ 
/* words within our SET. Instead of past implementations having   */ 
/* only one spot in the hash table for each element, there instead*/
/* is a LIST abstract data type within each space that holds all  */ 
/* the elements related to the home location given by the user    */
/* implemented hash function. The structure that makes up a SET   */
/* data type contains two integer variables (count and length)    */
/* that hold the total number of LIST data types (length) and the */ 
/* current number of LIST nodes (count) within the SET at one     */
/* time. The lists pointer points to the first LIST data type and */ 
/* is used as part of our hash function to find which LIST to add */ 
/* or remove an element from. Additionally, since we are using    */ 
/* void pointers as our primary means of storing words in our SET,*/
/* we have to use user defined comparison and hash functions to   */ 
/* compensate for different data types. We do this by adding two  */
/* function pointers (compare and hash) to our SET data type and  */
/* pass the compare function pointer to the createLIST of our LIST*/
/* (so our LIST knows how to compare two nodes aganst one another)*/
/* and the hash function is used to find our starting location    */
/* within our addElement, removeElement, and findElement          */
/* functions. Becuase we have already implemented the LIST        */
/* abstract data type, we can just include the list.h file and    */
/* call the required list functions to create a LIST, destroy a   */
/* LIST, add a node to a LIST, remove a node from a LIST, and     */
/* find a specific node within a LIST. Along with this, we can    */
/* use the getItems functions from list.c to allow us to return   */
/* an array of void pointers that contains each node's data in    */
/* all of our LISTS across our entire hash table. As stated before*/
/* this hash table implementation allows the insertion, deletion, */
/* and searching of nodes within our hash table along with the    */
/* ability to allocate and deallocate memory for the LISTS and SET*/
/* used to hold all the data. Finally, this implementation is     */
/* able to return an array of void pointers that contains every   */
/* node's data contained within every LIST of the entire SET.     */ 

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include "set.h"
#include "list.h"

/* Define a structure that will be the basis for our SET abstract data type. It includes two integer
 * variables that contain the total number of LIST abstract data types, the current number of nodes 
 * between all LIST data types, and a pointer to the first LIST data types. Additionally, since we
 * are using void pointers to hold the data in each node within our LISTS, we have to pass the user
 * defined "compare" and "hash" function pointers that can be used within our LISTS so we can compare two
 * nodes with one another in our LISTS and easily find the home location (or specifc LIST) that contains
 * a specific element.
 */    
struct set
{
	//An integer variable that tells us how many nodes there are currently between all the LISTS within our SET. 
	int count;
	
	//An integer variable that tells us the maximum number of LISTS that can be created within our hash table.
	int length;

	//A pointer that points to the first LIST pointer within our SET. It is used in conjunction with our hash function to see
	//which specific LIST a node may be added, deleted, or searched from. 
	LIST **lists;

	//A pointer to the user defined "compare" function that compares two elements within our LIST and returns whether the elements are
	//the same or different from one another.
	int (*compare)();

	//A pointer to the user defined "hash" function that gives us the home location within our SET to be used for the insertion, deletion, and 
	//searching of nodes within a specific LIST.
	unsigned (*hash)();
}; 


/* Function: To allocate memory for a SET data type along with the array of
 * LIST pointers having a maximum capacity of maxElts. The array of LIST pointers
 * makes up our hash table and each individual list can contain multiple elements.
 * Also, the two user defined "compare" and "hash" functions are also set to equal the 
 * values given by these functions within our SET so their values may be in our findElement
 * function and our hash function in addElement, removeElement, and findElement.
 * 
 * Efficency: O(m) 
 * 
 * Return Value: A pointer to the newly created SET abstract data structure.
 */
SET *createSet(int maxElts, int (*compare)(), unsigned(*hash)())
{	
	assert(compare != NULL && hash != NULL);
	SET*sp = malloc(sizeof(SET));
	assert(sp!=NULL);
	sp->count = 0;
	sp->length = maxElts/20;
	sp->compare = compare;
	sp->hash = hash;
	sp->lists = malloc(sizeof(struct list*)*sp->length);
	assert(sp->lists != NULL);
	for(int i = 0; i<sp->length; i++)
	{
		LIST* newList = createList(sp->compare);
		sp->lists[i] = newList;
	}	
	return sp;
}

/* Function: Frees all the memory previously allocated from createSet. All of the
 * LISTS previously allocated are freed (through the use of destroyList) and then the 
 * SET structure itself is freed. 
 * 
 * Efficency: O(m)
 * 
 * Return Value: No return value.
 */
void destroySet(SET *sp)
{
	assert(sp!=NULL);
	for(int i = 0; i < sp->length; i++)
	{
		destroyList(sp->lists[i]);
	}
	free(sp);	
}

/* Function: Returns the integer variable count as part of the SET structure, used to find
 * out how many total nodes are currently among all of the LISTS within our SET. 
 * 
 * Efficency: O(1)
 * 
 * Return Type: An integer variable, being the number of elements (or words) currently in 
 * the SET data type.
 */
int numElements(SET *sp)
{
	assert(sp!=NULL);
	return sp->count;
}

/* Function: After hashing the home location, the addElement function first calls the findItem
 * function to see whether the specific data we are trying to insert into a LIST already exists.
 * If the element cannot be found within the list located by the hash function, then addElement
 * calls the addFirst function which then adds a node to the LIST with the data of void *elt. Once
 * this is completed, count is increased to show that a new node has been inserted into one of the
 * LISTS within the hash table. If the element void *elt is found within one of the nodes in the 
 * LIST found by our home location, the function returns with nothing added to the SET.
 *
 * Efficency: O(n)
 *
 * Return Value: No return value.
 */
void addElement(SET *sp, void *elt)
{		
	assert(sp != NULL);
	assert(elt != NULL);
	int startLoc = (*sp->hash)(elt)%sp->length;
	if(findItem(sp->lists[startLoc],elt) == NULL)
	{
		addFirst(sp->lists[startLoc],elt);
		sp->count++;
	}
	return;	
}

/* Function: The function first finds the home location for the specific LIST that the 
 * element void *elt should be located in. Once that location is found, the numItems function
 * is called to ensure that there is a node within the specifc List found by the home location.
 * If there is at least one node in the LIST, then the function calls the removeItem function to 
 * search for (and if found, remove) the node matching the void pointer elt that is passed to the 
 * removeElement function. Since an element is removed from the LIST, sp's count variable must be
 * decreased by one to make up for an element being removed from a LIST. 
 * 
 * Efficency: O(n)
 * 
 * Return Value: No return value.
 */
void removeElement(SET *sp, void *elt)
{
	assert(sp != NULL);
	assert(elt != NULL);
	int startLoc = (*sp->hash)(elt)%sp->length;
	if(numItems(sp->lists[startLoc]) > 0)
	{	
		removeItem(sp->lists[startLoc], elt);
		sp->count--;
	}
}

/* Function: Returns the element void* elt if found by the findItem function. This function acts
 * similar to a wrapper that calls the findItem function and then instead of returning the
 * node where the element is found, it returns the void pointer itself instead.
 * 
 * Efficency: O(n)
 * 
 * Return Value: The element void *elt if found in the LIST located by the hash function.
 */
void *findElement(SET *sp, void *elt)
{
	assert(sp != NULL);
	assert(elt != NULL);
	int startLoc = (*sp->hash)(elt)%sp->length;
	return findItem(sp->lists[startLoc], elt);
}


/* Function: Allocates memory for an array of void pointers in the SET pointed to by 
 * sp. This function copies over all the void pointers from each node across each LIST
 * contained in the SET. Since we don't know how many nodes are in each LIST, we have to
 * create a variable that offsets each memcpy that is called for each list. The for loop 
 * within the function first checks if there are elements in a specific LIST, if that's true, 
 * then the function copies the void pointers (which are recieved by the getItems function) from
 * a specific LIST and then puts those void pointers into the "dataSet" array with each element
 * added create an offshoot for the next LIST. For example, if the first LIST has 10 nodes (and
 * therefore 10 void pointers) then the offshoot would be increased by 10 for the next LIST. As each
 * LIST has its data copied over to the array of void pointers, the offshoot increases to make up 
 * for the different indicies that have been previously filled already (otherwise these indicies would
 * be overwritten as each LIST's data would cover the same indices as before and you would continually overwrite
 * the same data over and over again). 
 * 
 * Efficency: O(m)
 *  
 * Return Value: An array of all void pointers that are within our SET data type.
 */
void *getElements(SET *sp)
{
	assert(sp != NULL);
	void**dataSet = malloc(sizeof(void*)*sp->count);
	assert(dataSet != NULL);
	int elementsInserted = 0;
	for (int i = 0; i < sp->length; i++)
		{	
			if(numItems(sp->lists[i]) > 0)
				{
					memcpy(dataSet+elementsInserted,getItems(sp->lists[i]),sizeof(void*)*numItems(sp->lists[i]));
					elementsInserted += numItems(sp->lists[i]);
				}
		}
	return dataSet; 
}
