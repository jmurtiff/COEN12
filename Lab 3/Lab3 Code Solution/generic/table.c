/******************************************************************/
/* Jordan Murtiff												  */
/* Lab #3						 	 							  */
/* Lab Section: Wednesday (63650)	                              */
/* Date: Feburary 11, 2018				         				  */
/*													              */
/* This program is the implementation of the abstract data type   */
/* SET. A SET is a unsorted collection of distinct elements. In   */
/* this case, the user only cares about the main functions        */
/* (unique.c and parity.c) and the outside implementation of these*/
/* main programs is left up to the programmer to be created. This */
/* SET abstract data type has been used with unsorted and sorted  */
/* arrays while table.c (this implementation) is using hash       */
/* tables. Hash tables are the method of searching the SET data   */
/* type and are also used in the means of inserting and deleting  */
/* elements from the SET. An array of void pointers is used to    */
/* hold any data type the client chooses to use. The power in     */
/* using void pointers in our SET is that any data type may be    */
/* attributed for use to hold the words within our SET. We are    */
/* still using a hash function to more quickly find the elements  */
/* we are looking for within the SET itself. The structure that   */
/* makes up a SET data type contains two integer variables (count */
/* and length) that hold the total number of possible words       */
/* (length) and the current number of words (count) within the SET*/
/* at one time. The data pointer points to the first void pointer */
/* as part of our SET and is used within our search function to   */
/* compare strings with one another an find matches within our    */
/* SET. Additionally, since we are using void pointers as our     */
/* primary means of storing words in our SET, we have to use new  */
/* comparison and hash functions to compensate for different data */
/* types. Since we do not know exactly which data types are going */
/* to be used, we have to depend on the user's functions that are */
/* given to us for this implementation. We do this by adding two  */
/* function pointers (compare and hash) that are used in our      */
/* search function to compare two elements (compare) and give us  */
/* our home location in our hash function (hash). As before, we   */
/* are using an array of flags to to check with spaces in our SET */
/* are empty (not previously filled), removed (previously filled  */
/* and then deleted) and filled (currently containing an element).*/
/* In addition to our search function using hash tables, this     */
/* program can also deallocate the memory used for a SET, return  */
/* the current number of elements within the SET, and also return */
/* individual words within a SET along with an array of all       */
/* elements within the current SET.                               */

/* For hash tables an array of "flags" are used to mark which locations
 * within the SET data type are filled (with a character array pointer), removed
 * (previously deleted by the removeElement function), or empty (waiting to be filled
 * by the addElement function). These flags make it easier to tell which spots 
 * in our SET can have new elements inserted (removed or empty spots) and which spots
 * have character pointers that we need to look at (filled spots). These flags have
 * been defined in our program to represent "empty", "filled", and "removed" instead
 * of their character counterparts of "0", "1", and "2" to make the program easier to
 * read and understand the implemntation of our hash table.
 */  
#define Empty 0
#define Filled 1
#define Removed 2


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include "set.h"

//Include the decleration for the search function (since it is not included in set.h).
static int search(SET*, void*, bool*);

/* Define a structure that will be the basis for our SET abstract data type. It includes two integer
 * variables that contain the length of our array of character array pointers, 
 * the current number of character array pointers, and a pointer to the first character array pointer.
 * Additionally, a SET contains a character pointer to the first flag in our array of flags used
 * to access which elements are filled, empty, or previously removed from our SET.
 */    
struct set
{
	//An integer variable that tells us how many character array pointers we currently have in our SET. 
	int count;
	
	//An integer variable that tells us the maximum number of character array pointers that have been allocated to be stored in our SET.
	int length;

	//A pointer that points to the first array element in our array of character array pointers. This is the reason it's a pointer to a pointer.
	char **data;
	
	//A character pointer that points to the first flag (being a single character) in our array of characters that represent the flags
	//of our hash table.  
	char *flag;

	//A pointer to the user defined "compare" function that compares two elements within our SET and returns whether the elements are
	//the same or different from one another.
	int (*compare)();

	//A pointer to the user defined "hash" function that gives us the home location within our SET to begin searching for an elements
	//before resorting to linear probing.
	unsigned (*hash)();
}; 

/* Function: To allocate memory for a SET data type (both the structure and the array of 
 * character pointers) with the array of character array pointers 
 * having a maximum capacity of maxElts. Additionally, as a SET data type is created with all
 * flags used as part of our hash table are set to Empty (in this case 0) since our
 * SET is newly created and there are no elements within the SET at this point in time.
 * The two user defined "compare" and "hash" functions are also set to equal the values
 * given by these functions within our SET so their values may be used within our
 * search function.
 * 
 * Efficency: O(m) 
 * 
 * Return Value: A pointer to the newly created SET abstract data structure.
 */
SET *createSet(int maxElts, int (*compare)(), unsigned(*hash)())
{	
	SET*sp = malloc(sizeof(SET));
	assert(sp!=NULL);
	sp->count = 0;
	sp->length = maxElts;
	sp->data = malloc(sizeof(void*)*maxElts);
	assert(sp->data != NULL);
	sp->flag = malloc(sizeof(char)*maxElts);
	assert(sp->flag != NULL);
	for(int i = 0; i < sp->length; i++)
	{
		sp->flag[i] = Empty;
	}
	sp->compare = compare;
	sp->hash = hash;
	return sp;
}

/* Function: To search through the SET and find if the element void *elt exists within the SET already using
 * a hash function along with linear probing to resolve collisions. If void *elt does exist, then the
 * function returns the index where it is located within the SET and sets the boolean pointer "found" to true,
 * otherwise, after searching through all fillled and removed spaces, the function returns the index where 
 * void *elt should be located and sets the boolean pointer "found" to be false. Since in this case ordering 
 * does not matter, the function looks first for an empty slot within the SET (and since we are using linear 
 * probing, the first empty slot we find means that the element void *elt cannot exist in our SET) and promptly
 * returns that location. Otherwise, if removed slots are found within our SET before empty slots are found, 
 * the first deleted slot is returned instead of the first empty slot found.
 *
 * Efficency: O(n) with O(1) being the ideal case
 * 
 * Return Type: The index of the element void* elt (if found in the array of void pointers) and 
 * thereby the index used for the removal of an element in the SET or the first empty or removed slot 
 * in the SET (whichever comes first) where the element void *elt should be placed for insertion of
 * a new element in the SET.
 */
static int search(SET *sp, void *elt, bool *found)
{
	assert(found != NULL);
	*found = false;
	assert(sp != NULL);
	assert(elt != NULL);
	int position = (*sp->hash)(elt) % sp->length;
	int flag = 0;
	int firstRemoved = 0;
	for (int i = 0; i < sp->length; i++)
	{	
		int newPosition = (position + i) % sp->length;
		if (sp->flag[newPosition] == Empty)
		{
			if (flag == 1)
			{
				return firstRemoved;
			}
			else if (flag  == 0)
			{
				return newPosition;
			}
		}
		if(flag == 0)
		{
			if(sp->flag[newPosition] == Removed)
			{
				firstRemoved = newPosition;
				flag = 1;
			}
		}	
		if(sp->flag[newPosition] == Filled)
		{
			if((*sp->compare)(elt, sp->data[newPosition]) == 0)
			{
				*found = true;
				return newPosition;
			}
		}
	}
	return firstRemoved;
}

/* Function: Frees all the memory previously allocated from createSet. The user
 * handles the freeing of the elements within our array of void pointers. All
 * that must be handled in this function is the freeing of all the pointers 
 * used within our SET (for flags, for our array of void pointers, and finally for 
 * the SET itself). 
 * 
 * Efficency: O(1)
 * 
 * Return Value: No return value.
 */
void destroySet(SET *sp)
{
	assert(sp!=NULL);
	free(sp->flag);
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

/* Function: The user handles memory allocation for new elements added to the SET. This funtion
 * Inserts the element void *elt within the SET after not finding it already within the SET. 
 * The function uses the search function to find the correct index for insertion and then checks
 * if there is room in the SET for a new element. If there is room in the SET, then the new 
 * element void *elt is added to the SET and the flag character is changed to filled (1) and 
 * the count is increased to reflect a new element being added to the SET. If the element void
 * *elt is found within the SET, the function returns with nothing added to the SET.
 *
 * Efficency: O(n) with O(1) being expected
 *
 * Return Value: No return value.
 */
void addElement(SET *sp, void *elt)
{		
	assert(sp != NULL);
	assert(elt != NULL);
	assert(sp->count < sp->length);
	bool found = false;
	int position = search(sp, elt, &found);
	if(found == false)
	{
		sp->data[position] = elt;
		sp->flag[position] = Filled;
		sp->count++;
	}
	else 
	{
		return;
	}
		
}

/* Function: The user handles memory deallocation for elements being removed from the SET. 
 * The function uses the search function to find the element void *elt within the SET and 
 * then frees the void pointer at that location. Once freed, the position is set to NULL and
 * the flag character is set to Removed (2) and the count is decreased to reflect an element
 * being removed from the SET. If the search function does not find the specific element void*
 * elt to be removed, then the function returns with nothing being deleted from the SET.
 * 
 * Efficency: O(n) with O(1) being expected
 * 
 * Return Value: No return value.
 */
void removeElement(SET *sp, void *elt)
{
	assert(sp != NULL);
	assert(elt != NULL);
	bool found = false;
	int position = search(sp, elt,&found);
	if(found == false)
	{
		return;
	}
	else
	{
		sp->flag[position] = Removed;
		sp->count--;
	}	
	
}

/* Function: Returns the element void* elt if found by the search function. This function acts
 * similar to a wrapper that calls the search function and then instead of returning the
 * index where the element is found, it returns the element itself.
 * 
 * Efficency: O(n) with O(1) being expected 
 * 
 * Return Value: The element void *elt if found in the array of character pointers,
 * otherwise, NULL.
 */
void *findElement(SET *sp, void *elt)
{
	assert(sp != NULL);
	assert(elt != NULL);
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
 * within the SET data type. Since we are using a hash table as our searching
 * algorithm we should only return an array of the "filled" elements within 
 * our SET. By this logic, the function goes through the entire SET and checks
 * to see which elements within the SET have void pointers and then copies them 
 * over to an allocated array of character array pointers. This newly allocated 
 * array contains no empty spaces after going through the entirety of our SET and 
 * only contains only the character array pointers of the filled elements of our SET. 
 * 
 * Efficency: O(m)
 *  
 * Return Value: The array of all elements that are within our SET data type.
 */
void *getElements(SET *sp)
{
	assert(sp != NULL);
	void**dataSet = malloc(sizeof(void*)*sp->count);
	assert(dataSet != NULL);
	int count = 0;
	for (int i = 0; i < sp->length; i++)
	{
		if(sp->flag[i] == Filled)
		{
			dataSet[count] = sp->data[i];
			count++;
		}
	}
	return dataSet; 	
}
