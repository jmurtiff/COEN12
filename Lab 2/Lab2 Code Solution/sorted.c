/******************************************************************/
/* Jordan Murtiff												  */
/* Lab #2						 	 							  */
/* Lab Section: Wednesday (60312)	                              */
/* Date: January 28, 2018				         				  */
/*							  	  								  */
/* This program is the implementation of the abstract data type   */
/* SET. A SET is a unsorted collection of distinct elements. In   */
/* this case, the user only cares about the main functions        */
/* (unique.c and parity.c) and the outside implementation of these*/
/* main programs is left up to the programmer to be created.      */
/* For this lab, an array of character array pointers has been    */
/* used to store the different words found in different text files*/
/* that are being read. The total number of possible words        */
/* (length) and the current number of words (count) are two       */
/* integer variables that are used to keep track of how many words*/
/* exist in the SET at one time (count) and how many are possible */
/* (length). The data pointer points to the first character array */
/* pointer as part of our SET and is used to traverse the         */
/* different words that are added or removed from the SET. This   */ 
/* program allows the SET to be modified by the addition and      */ 
/* removal of elements found with the SET by use of a binary      */
/* search (which is acessible only within sorted.c) to maintain   */
/* a set of distinct elements. Additionally, the program is able  */
/* to deallocate any memory used by the createSET function and    */
/* find the current number of words within the set (using         */
/* numElements). Finally, the program is able to return an array  */
/* of elements (in this case words) that are in the current SET.  */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include "set.h"


//Include the decleration for the search function (since it is not included in set.h).
static int search(SET*, char*, bool*);

/* Define a structure that will be the basis for our SET abstract data type. It includes two integer
 * variables that contain the length of our array of character array pointers, 
 * the current number of character array pointers, and a pointer to the first character array pointer.
 */    
struct set
{
	//An integer variable that tells us how many character array pointers we currently have in our SET. 
	int count;
	
	//An integer variable that tells us the maximum number of character array pointers that have been allocated to be stored in our SET.
	int length;

	//A pointer that points to the first array element in our array of character array pointers. This is the reason it's a pointer to a pointer.
	char **data;
}; 


/* Function: To allocate memory for a SET data type (both the structure and the array of 
 * character pointers) with the array of character array pointers 
 * having a maximum capacity of maxElts.
 * 
 * Efficency: O(1)
 * 
 * Return Value: A pointer to the newly created SET abstract data structure.
 */
SET *createSet(int maxElts)
{
	SET*sp = malloc(sizeof(SET));
	assert(sp!=NULL);
	sp->count = 0;
	sp->length = maxElts;
	sp->data = malloc(sizeof(char*)*maxElts);
	assert(sp->data != NULL);
	return sp;
}

/* Function: To search through the SET and find if the element char *elt exists within the SET already. 
 * If char *elt does exist, then the function returns the index where it is located within the SET
 * and sets the boolean pointer "found" to true, otherwise, the function returns the index where
 * char *elt should be located and sets the boolean pointer "found" to be false
 * 
 * Efficency: O(logn)
 * 
 * Return Type: The index of the element char* elt (if found in the array of character pointers) or the index
 * where the element char *elt should be placed (to keep the SET ordered).
 */
static int search(SET *sp, char *elt, bool*found)
{ 
	assert(sp != NULL);
	int lo, hi, mid; lo = 0; hi = sp->count-1;
	while(lo <= hi)
	{
		mid = (lo + hi)/2;
		if(strcmp(sp->data[mid],elt) > 0) 
		{
			hi = mid - 1;
		}
		else if (strcmp(sp->data[mid],elt) < 0)
		{
			lo = mid + 1;
		}
		else
		{
			*found = true;
			return mid;
		}	
	}
	*found = false;
	return lo;
}

/* Function: Frees all the memory previously allocated from createSet. The function
 * begins by freeing all the character array pointers within the array of character pointers,
 * then the array itself, and finally the structure containing the count, length, and pointer
 * to the array of character pointers.
 * 
 * Efficency: O(1)
 * 
 * Return Value: No return value.
 */
void destroySet(SET *sp)
{
	assert(sp!=NULL);
	for(int i=0; i < sp->count; i++)
	{
		free(sp->data[i]);
	}
	free(sp->data);
	free(sp);	
}

/* Function: Returns the integer variable count as part of the SET structure, used to find
 * out how many elements are currently in the array of character pointers (the current 
 * number of elements, not the total possible number of possible elements).
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

/* Function: Allocates memory and inserts the element char *elt within the SET after
 * not finding it already within the SET. The function starts from the bottom
 * of the SET data type and shifts each elemtns down one slot to make room for the 
 * newly added element. If the element char* elt is found within the SET, the 
 * function returns with nothing added to the SET.
 *
 * Efficency: O(n)
 *
 * Return Value: No return value.
 */
void addElement(SET *sp, char *elt)
{	
	assert(sp != NULL);
	bool found = false;
	int position = search(sp, elt,&found);
	if(found == false)
	{
		for(int i=sp->count; i > position; i--)
		{
			sp->data[i] = sp->data[i-1];
		}
		sp->data[position] = strdup(elt);
		sp->count++;
	}
	else 
	{
		return;
	}
		
}

/* Function: Removes the element char* elt if found within the SET data type. 
 * If found, the elements withn the SET are shifted in order to overide the element
 * to be removed. In this case, we start from the element following the one to be removed
 * and shift each subsequent element up one index (thereby overriding the element we wish to
 * remove) and decrease count's value to compensate for the removed element.
 * 
 * Efficency: O(n)
 * 
 * Return Value: No return value.
 */
void removeElement(SET *sp, char *elt)
{
	assert(sp != NULL);
	bool found = false;
	int position = search(sp, elt,&found);
	if(found == false)
	{
		return;
	}
	else
	{
		for(int i = position+1; i < sp->count; i++)
		{
			sp->data[i-1] = sp->data[i];	
		}
		sp->count--;
	}	
	
}

/* Function: Returns the element char* elt if found by the search function. This function acts
 * similar to a wrapper that calls the search function and then instead of returing the
 * index where the element is found, it returns the element itself.
 * 
 * Efficency: O(logn) 
 * 
 * Return Value: The element elt if found in the array of character pointers,
 * otherwise, NULL.
 */
char *findElement(SET *sp, char *elt)
{
	assert(sp != NULL);
	bool found = false;
	int position = search(sp, elt,&found);
	if(found == false)
	{
		return NULL;
	}
	else
	{
		return sp->data[position];
	} 
}


/* Function: Allocates memory for an array of elements in SET pointed to by 
 * sp. This function allows for the program to return the individual words
 * within the SET data type. 
 * 
 * Efficency: O(1)
 * 
 * Return Value: The array of all elements (in this case words) that are within a SET data type.
 */
char **getElements(SET *sp)
{
	assert(sp != NULL);
	char**dataSet = malloc(sizeof(char**)*sp->count); 
	memcpy(dataSet,sp->data,sizeof(char**)*sp->count);
	return dataSet; 	
}
