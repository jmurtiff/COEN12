/******************************************************************/
/* Jordan Murtiff  												  */
/* Lab #2						 	  						      */
/* Lab Section: Wednesday (60312)	                              */
/* Date: January 28, 2018				                          */
/*							  	                                  */
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
/* removal of elements found with the SET by use of a sequential  */
/* search (which is acessible only within unsorted.c) to maintain */
/* a set of distinct elements. Additionally, the program is able  */
/* to deallocate any memory used by createSET function  and       */
/* find the current number of words within the set (using         */
/* numElements). Finally, the program is able to return an array  */
/* of elements (in this case words) that are in the current SET.  */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "set.h"

//Include the decleration for the search function (since it is not included in set.h).
static int search(SET*, char*);

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
 * having a maximum capacity of maxElts (which is defined by the user).
 * 
 * Efficency: O(1)
 * 
 * Return Type: A pointer to the newly created SET abstract data structure.
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
 * If the element char *elt does exist, then the function returns the index where it 
 * is located within the SET, otherwise, the function return -1 
 * (indicating that the character array could not be found).
 * 
 * Efficency: O(n)
 * 
 * Return Type: The index of the element (if found in the array of character pointers) or -1, if
 * the element char *elt was not found in the SET.
 */
static int search(SET *sp, char *elt)
{ 
	assert(sp != NULL);
	for(int i = 0; i < sp->count; i++)
	{
		if(strcmp(sp->data[i], elt) == 0)
		{
			return i;
		}	
	}
	return -1;
}

/* Function: Frees all the memory previously allocated from the createSet function. The function
 * begins by freeing all the character array pointers within the array of character pointers,
 * then the array itself, and finally the structure containing the count, length, and pointer
 * to the array of character pointers.
 * 
 * Efficency: O(1)
 * 
 * Return Type: No return type.
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
 * the array of character array pointers.
 */
int numElements(SET *sp)
{
	assert(sp!=NULL);
	return sp->count;
}

/* Function: Allocates memory and adds the element char *elt into the array of character pointers
 * if it is not already found in the array of character pointers. If the element char* elt
 * is found in the array of character pointers, the function returns with nothing added
 * to the SET.
 * 
 * Efficency: O(n)
 *
 * Return Type: No return type.
 */
void addElement(SET *sp, char *elt)
{
	assert(sp != NULL);
	if(search(sp, elt) == -1)
	{
		sp->data[sp->count] = strdup(elt);
		sp->count++;
	}
	else 
	{
		return;
	}
		
}

/* Function: Removes the element char *elt if found within the array of character array pointers. 
 * If found, the element elt is replaced with the last element of the SET. Since we do not
 * care if the SET is ordered or not, we can simply override the space we wish to remove
 * with a different element that exists within our SET (in this case, we can use the last 
 * element of the SET and decrease count's value). 
 * 
 * Efficency: O(n)
 * 
 * Return Value: No return value.
 */
void removeElement(SET *sp, char *elt)
{
	assert(sp != NULL);
	int position = search(sp,elt);
	if(position  == -1)
	{
		return;
	}
	else
	{
		sp->data[position] = sp->data[sp->count-1];
		sp->count--;
	}	
	
}

/* Function: Returns the element char *elt if found by the search function. This function acts
 * similar to a wrapper that calls the search function and then instead of returning the
 * index where the element is found, it instead returns the element itself.
 * 
 * Efficency: O(n) 
 * 
 * Return Value: The element char* elt if found in the array of character pointers,
 * otherwise, NULL.
 */
char *findElement(SET *sp, char *elt)
{
	assert(sp != NULL);
	int position = search(sp, elt);
	if(position == -1)
	{
		return NULL;
	}
	else
	{
		return sp->data[position];
	} 
}


/* Function: Allocates memory for an array of elements in the SET
 * data type pointed to by sp. This function  allows for the program 
 * to return an array of elements within the SET (which in this case,
 * is words). 
 * 
 * Efficency: O(1)
 * 
 * Return Value: The array of all elements within a SET data type
 * (which in this case, is words).
 */
char **getElements(SET *sp)
{
	assert(sp != NULL);
	char**dataSet = malloc(sizeof(char**)*sp->count); 
	memcpy(dataSet,sp->data,sizeof(char**)*sp->count);
	return dataSet; 	
}
