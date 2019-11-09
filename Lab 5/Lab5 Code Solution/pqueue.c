/******************************************************************/
/* Jordan Murtiff												  */
/* Lab #5						 	 							  */
/* Lab Section: Wednesday (63650)	                              */
/* Date: March 11, 2018					         				  */
/*																  */
/* This program is the implementation of the abstract data type   */
/* Heap (specifically, a min heap). This implementation A Heap    */
/* is an abstract data type in which the minimum value of all     */
/* elements is always the root node our our minimum heap (which in*/
/* this case is taking the form of a tree). In this case, the     */ 
/* user only cares about the main functions (sort.c) and the      */ 
/* outside implementation of these main programs is left up to the*/
/* programmer to be created. With the minimum heap being based off*/
/* a binary tree, each node withn the heap has two children (a    */
/* right and left child). This Heap abstract data type is being   */
/* used to form a Priority Queue (in which the element with the   */
/* lowest priority value is the first element to come out of the  */
/* queue) and continually reshapes itself to have the root of the */
/* heap always be the element with the smallest priority value.   */
/* This implementation allows for the creation and destruction of */
/* Priority Queues, the add and removal of elements within the    */
/* Priority Queue, and a function to tell you the current number  */
/* of elements within the Priority Queue. Since our Queue does not*/
/* have a set size, the size of the Priority Queue may change as  */
/* an element is inserted into it. If the addEntry function finds */
/* that the Priority Queue is full, it will reallocate more memory*/
/* to be double the current size of the Queue. As the Queue grows */
/* its size will be dynamically changed to accomodate for more    */
/* elements. Also, since we are using void pointers to hold the   */
/* data within our Priority Queue, we can use any data type to    */
/* hold the data within our Queue. In doing so, we must also use  */
/* the user defined compare function to compare two elements      */
/* within our Priority Queue (since they may not always be        */ 
/* integers) and must use these functions in our addEntry and     */
/* removeEntry functions to ensure that we are reheaping up and   */
/* down correctly each time we either add or remove an element    */
/* from our Priority Queue. 							          */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include "pqueue.h"


/* Since the stucture of a Priority Queue is a Min Heap, it its important that we can easily
 * locat the children or parents of a particular node with a Heap. If you know the index 
 * of a specifc node in a Min Heap, you can easily find it's parent, left child, and right
 * child through the use of of these three formulas shown below. Given a certain index "x",
 * P(x) tells you the parent, L(x) tells you the left child, and R(x) tells you the right 
 * child of any node in the tree (if that node exists). Since we will be using these formulas
 * frequently in our addEntry and removeEntry functions, it is eaiser to just define them 
 * for later use in our implementation.
 */
#define P(x)  (x-1)/2
#define L(x)  (x*2+1)
#define R(x)  (x*2+2)


/* Define a structure that will be the basis for our Priority Queue ADT. It includes two 
 * integer variables that contain the total number of possible elements in our Queue (length) and 
 * the current number of elements (count). Additionally, since our Priority Queue is a generic ADT, 
 * we will be using void pointers to hold the data in each element of our Queue and so we need a 
 * void double pointer to point to the data within each element in our Queue. Since we are using void
 * pointers, we also have to pass the user defined "compare" function pointers that can be used within 
 * our Queue functions so we can compare two elements with one another to see which element has the lower or higher
 * priority value.
 */ 
struct pqueue
{
	//An integer variable that tells us how many elements there are currently within our Priority Queue.
	int count;

	//An integer variable that tells us the maximum number of elements that can be within our Priority
	//Queue at one time.
	int length;

	//A pointer that points to the first void pointer  in our Priority Queue (which can be made up of any data 
	//type). This is the reason it is a pointer to a pointer.
	void **data;

	//A pointer to the user defined "compare" function that compares two elements within our Priority Queue 
	//and returns whether the elements are the same or different from one another.
	int (*compare)();
};

/* Function: To allocate memory for a Priority Queue data type (both the structure and the array of void 
 * pointers that make up the elements in our Queue). Since we are able to reallocate memory to increase
 * or decrease the size of our Priority Queue, we do not have to define a maximum number of elements for
 * our Queue. Additionally, since we are using void pointers, we have to set the user defined "compare" function 
 * to equal the values given by these functions within our Priority Queue so their values may be used in our
 * addEntry and removeEntry functions.
 *
 * Efficiency: O(1)
 *
 * Return Value: A pointer to the newly created Priority Queue abstract data structur.e
 */
PQ *createQueue(int (*compare)())
{
	struct pqueue *pq;
	pq = malloc(sizeof(struct pqueue));
	assert(pq != NULL);
	pq->count = 0;
	pq->length = 10;
	pq->compare = compare;
	pq->data = malloc(sizeof(void*)*pq->length);
	assert(pq->data != NULL);
	return pq;
}

/* Function: Frees all the memory previously allocated from createQueue. The function
 * first frees each element in the Priority Queue (each void pointer) and then moves
 * to free the data pointer (the doble void pointer pointing to the elements in the 
 * Priority Queue) before finally freeing the Priority Queue itself.
 *
 * Efficiency: O(n)
 *
 * Return Value: No return value
 */
void destroyQueue(PQ *pq)
{
	assert(pq != NULL);
	for (int i = 0; i < pq->count; i++)
		{
			free(pq->data[i]);
		}
	free(pq->data);
	free(pq);
}

/* Function: Returns the integer variable count as part of the Priority Queue structure, used to find
 * out how many elements are currently in the Priority Queue (the current number of elements, not the 
 * total possible number of elements).
 *
 * Efficiency: O(1)
 *
 * Return Value: An integer variable, being the number of elements currently in the 
 * Priority Queue abstract data type.
 */
int numEntries(PQ *pq)
{
	assert(pq != NULL);
	return pq->count;
}

/* Funtion: The function tries to add an element to a Priority Queue. After checking that there is enough space
 * (or if there isn't, doubling the current size of the Prioity Queue), the function adds the new element as 
 * the right or left child of the bottomost parent node of the minimum heap. After this new element is inserted
 * into the priority queue, the heap has to be reheaped in order to maintain its shape as a minimum heap. This
 * is done by continually comparing the newly inserted node with its parents. If the parent node is larger than
 * the new node, it switches with its parent. This is done until either the new node has becomes the root of the
 * heap (no parents left to swap with) or if the parent is smaller than the new node. Count is also increased to
 * show that a new element has been added to our Priority Queue.
 *
 * Efficiency: O(logn)
 *
 * Return Value: No return value.
 */
void addEntry(PQ *pq, void *entry)
{
	assert(pq != NULL);
	if(pq->count == pq->length)
	{
		pq->data = (void *) realloc(pq->data,sizeof(void*)* pq->length*2);
		assert (pq->data != NULL);
		pq->length = pq->length * 2;		
	}
	pq->data[pq->count] = entry; 
	int x = pq->count;
	pq->count++;
	while(x > 0 && (*pq->compare)(pq->data[x],pq->data[P(x)]) < 0)
	{
		void* temp = pq->data[x];
		pq->data[x] = pq->data[P(x)];
		pq->data[P(x)] = temp;
		x = P(x);
	}
}

/* Function: Removes the root node of our minimum heap that makes up our Priority Queue and replaces it with
 * the bottommost child node of our min heap. Since this often causes our Priority Queue to not maintain 
 * its min heap shape, we also have to reheap down to set the root node to its rightful place in our heap.
 * After setting a temporary variable to equal the root node's value (before it's deleted), the function
 * checks to see if a left child exists to the new root node (which has replaced the old root node that has
 * been deleted). If a right child does exist, the function then checks if a right child exists. Remember,
 * a node may have either 0 children, 1 child, or 2 children. If a node has 0 children, that means that the
 * node does not have either a right or left child, if a node has 1 child, that means that the node has only
 * a left child, and if a node has 2 children, that means that the node has both a right and left child.
 * The first node you must always check for is a left child (since if that does not exist, there cannot be
 * a right child and if does exist, then there can be a right child). So the ending condition of our remove
 * function is when we end up either not having a left child or that our left child is a value less than the
 * new root's value (whom we are reheaping down through our Priority Queue). So after checking if there is a 
 * left child, we must then check if there is a right child to our new root node. If there is only a left 
 * child, we then compare the left child with our new root node and see if our child is either smaller or 
 * larger than our root node. If our left child is smaller, we swap with it and then repeat the process of 
 * finding if our node has children, and then swapping with the smallest child. The same process happens
 * if we have both a left and right child, except that we have to then compare our left and right child
 * to see which child is smaller, and then swap with that one child. Once we either have no children or
 * one or both of our children are larger than the newly made root node (who may have moved down our min
 * heap and is no longer the root) we then return the value of our original node that was removed (the old
 * root node).
 *
 * Efficiencey: O(logn)
 *
 * Return Value: The Priority value of the root node of the min heap that makes
 * up our Priority Queue. 
 */
void *removeEntry(PQ *pq)
{
	assert(pq != NULL);
	int x = 0;
	void *temp = pq->data[0];
	pq->data[0]=pq->data[pq->count-1];
	pq->count--;
	while(pq->count > L(x))
	{
		if(pq->count > R(x))
		{
			if((*pq->compare)(pq->data[R(x)],pq->data[L(x)]) < 0)
			{
				if((*pq->compare)(pq->data[R(x)],pq->data[x]) < 0)
				{
					void *tempData = pq->data[x];
					pq->data[x] = pq->data[R(x)];
					pq->data[R(x)] = tempData;
					x = R(x);
				}
				else
				{
					break;
				}
			}
			else
			{	
				if((*pq->compare)(pq->data[L(x)],pq->data[x]) < 0)
				{
					void *tempData = pq->data[x];
					pq->data[x] = pq->data[L(x)];
					pq->data[L(x)] = tempData;
					x = L(x);
				}
				else
				{
					break;
				}		
			}
		}
		else
		{
			if((*pq->compare)(pq->data[L(x)],pq->data[x]) < 0)
			{
				void* tempData = pq->data[x];
				pq->data[x] = pq->data[L(x)];
				pq->data[L(x)] = tempData;
				x = L(x);
			}
			else
			{
				break;
			}
		}
	}
	return temp;
}
