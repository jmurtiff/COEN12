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
/* elements from the SET. An array of character pointers still    */
/* holds all the pointers that point to each word in the SET, but */
/* in this case we use a hash function to more quickly find the   */
/* elements we are looking for within the SET itself. The         */
/* structure that makes up a SET data type contains two integer   */
/* variables (count and length) that hold the total number of     */
/* possible words (length) and the current number of words (count)*/
/* within the SET at one time. The data pointer points to the     */
/* first character array pointer as part of our SET and is used   */
/* within our search function to compare strings with one another */
/* and find matches within our SET. Since we are using hash tables*/
/* as our searching algorithim, we are not using the data pointe  */
/* as a means of traversing through our SET but as a way to access*/
/* individual elements within our SET itself. An array of flags   */
/* has also been added as part of our SET to check which spaces in*/
/* our SET are empty (not previously filled), removed (previously */
/* filled and then deleted) and filled (currently containing an   */
/* element). In addition to our search function using hash tables */
/* (acessible only within table.c) and the addition and removal of*/
/* elements within our SET by use of hash tables, this program can*/
/* also deallocate the memory used for a SET, return the current  */ 
/* number of elements within a SET, and also return individual    */
/* words within a SET along with an array of all words within the */
/* the current SET.                                               */

/* For hash tables an array of "flags" are used to mark which locations
 * within the SET data type are filled (with a character array pointer), removed
 * (previously deleted by the removeElement function), or empty (waiting to be filled
 * by the addElement function). These flags make it easier to tell which spots 
 * in our SET can have new elements inserted (removed or empty spots) and which spots
 * have character pointers that we can use to compare with other elements (filled spots).
 * These flags have been defined in our program to represent "empty", "filled", and "removed"
 * instead of their character counterparts of "0", "1", and "2" to make the program easier to 
 * read and understand the implementation of our hash table.
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
static int search(SET*, char*, bool*);

/* Define a structure that will be the basis for our SET abstract data type. It includes two integer
 * variables that contain the length of our array of character array pointers, 
 * the current number of character array pointers, and a pointer to the first character array pointer.
 * Additionally, a SET contains a character pointer to the first flag in our array of flags used
 * to tell which elements in our SET are filled, empty, or previously removed.
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
}; 

/*
 * Function:     strhash
 *
 * Description: Return a hash value for a string S.
 */ 
unsigned strhash(char *s)
{
	unsigned hash = 0;

	while (*s != '\0')
		hash = 31 * hash + *s ++;

	return hash;
}


/* Function: To allocate memory for a SET data type (both the structure and the array of 
 * character pointers) with the array of character array pointers 
 * having a maximum capacity of maxElts. Additionally, as a SET data type is created all
 * flags used as part of our hash table are set to Empty (in this case 0) since our
 * SET is newly created and there are no elements within the SET at this point in time. 
 * 
 * Efficency: O(m) 
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
	sp->flag = malloc(sizeof(char)*maxElts);
	assert(sp->flag != NULL);
	for(int i = 0; i < sp->length; i++)
	{
		sp->flag[i] = Empty;
	}
	return sp;
}

/* Function: To search through the SET and find if the element char *elt exists within the SET already using
 * a hash function along with linear probing to resolve collisions. If char *elt does exist, then the
 * function returns the index where it is located within the SET and sets the boolean pointer "found" to true,
 * otherwise, after searching through all filled and removed spaces, the function returns the index where char
 * *elt should be located and sets the boolean pointer "found" to be false. Since in this case ordering does 
 * not matter, the function looks first for an empty slot within the SET (and since we are using linear probing,
 * the first empty slot we find means that the element char *elt cannot exist in our SET) and promptly returns that 
 * location. Otherwise, if removed slots are found within our SET before empty slots are found, the first removed slot is returned 
 * instead of the first empty slot found.
 * 
 * Efficency: O(n) with O(1) being the ideal case
 * 
 * Return Type: The index of the element char* elt (if found in the array of character pointers) and 
 * thereby the index used for the removal of an element in the SET or the first empty or removed slot 
 * in the SET (whichever comes first) where the element char *elt should be placed for insertion of
 * a new element in the SET.
 */
static int search(SET *sp, char *elt, bool *found)
{
	assert(found != NULL);
	*found = false;
	assert(elt != NULL);
	assert(sp != NULL);
	int position = (strhash(elt) % sp->length);
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
			else if (flag == 0)
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
			if(strcmp(elt, sp->data[newPosition]) == 0)
			{
				*found = true;
				return newPosition;
			}
		}
	}
	return firstRemoved;
}

/* Function: Frees all the memory previously allocated from createSet. The function
 * begins by freeing all the character array pointers within the array of character pointers,
 * then the array itself, the array of flag characters, and finally the structure containing
 * the count, length, and pointer to the array of character pointers.
 * 
 * Efficency: O(n)
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

/* Function: Allocates memory and inserts the element char *elt within the SET after
 * not finding it already within the SET. The function uses the search function
 * to find the correct index for insertion and then checks if there is room in the SET
 * for a new element. If there is room in the SET, then the new element char *elt
 * is added to the SET and the flag character is changed to filled (1) and the count
 * is increased to reflect a new element being added to the SET. If the element char*
 * elt is found within the SET, the function returns with nothing added to the SET.
 *
 * Efficency: O(n) with O(1) being expected
 *
 * Return Value: No return value.
 */
void addElement(SET *sp, char *elt)
{		
	assert(sp != NULL);
	assert(elt != NULL);
	assert(sp->count < sp->length);
	bool found = false;
	int position = search(sp, elt, &found);
	if(found == false)
	{
		sp->data[position] = strdup(elt);
		sp->flag[position] = Filled;
		sp->count++;
	}
	else 
	{
		return;
	}
		
}

/* Function: Removes the element char* elt if found within the SET data type. 
 * The function uses the search function to find the element char *elt within the
 * SET and then frees the character array pointer at that location. Once freed, the 
 * position is set to NULL and the flag character is set to Removed (2) and the count
 * is decreased to reflect an element being removed from the SET. If the search function
 * does not find the specific element char* elt to be removed, then the function returns
 * with nothing being deleted from the SET.
 * 
 * Efficency: O(n) with O(1) being expected
 * 
 * Return Value: No return value.
 */
void removeElement(SET *sp, char *elt)
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
		free(sp->data[position]);
		sp->data[position] = NULL;
		sp->flag[position] = Removed;
		sp->count--;
	}	
	
}

/* Function: Returns the element char* elt if found by the search function. This function acts
 * similar to a wrapper that calls the search function and then instead of returning the
 * index where the element is found, it returns the element itself.
 * 
 * Efficency: O(n) with O(1) being expected 
 * 
 * Return Value: The element char *elt if found in the array of character pointers,
 * otherwise, NULL.
 */
char *findElement(SET *sp, char *elt)
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
 * algorithm, we should only return an array of the "filled" elements within 
 * our SET. By this logic, the function goes through the entire SET and checks
 * to see which elements within the SET have character array pointers and then 
 * copies them over to an allocated array of character array pointers. This 
 * newly allocated array contains no empty spaces after going through the entirety
 * of our SET and only contains only the character array pointers of the filled 
 * elements of our SET. 
 * 
 * Efficency: O(m)
 *  
 * Return Value: The array of all elements (in this case words) that are within our SET data type.
 */
char **getElements(SET *sp)
{
	assert(sp != NULL);
	char**dataSet = malloc(sizeof(char*)*sp->count);
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
