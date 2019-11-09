/*
 * File:        huffman.c
 *
 * Description: This file contains the main function for testing huffman
 * 				encoding on a given file.
 *
 * 				The program creates a huffman tree that is used to encode data
 * 				more efficently based on the frequency of characters/symbols used
 * 				within a given file. The huffman tree is created based on continually
 * 				summing the two smallest frequencies (which are easily found within 
 * 				a Priority Queue [otherwise known as a Min Heap]) until there is only
 * 				one element left in the Priority Queue. This single element is our 
 * 				huffman tree, and it's used to set the unqiue data size of each unique
 * 				character/symbol found within the file. The more common characters 
 * 				are assigned a smaller data size while the least common common characters
 * 				are assigned a larger data size. This methodology of assigning different
 * 				characters/symbols different data sizes (bit values) helps in decreasing
 * 				the overall size of a file and can be very useful in sending information to
 * 				other places without risk of lost data. 
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <ctype.h>
#include "pqueue.h"
#include "pack.h"


/* Function: Allocates memory and links a node within a huffman tree given its right
 * and left children (assuming two children pointers are given). This function is 
 * the basis of how nodes are linked for our huffman tree and how the shape of the
 * tree mimicks a binary tree. The "count" variable that is passed to the function
 * is the frequency of a certain character or letter in a given text. 
 *
 * Efficiency: O(1)
 *
 * Return Value: A node pointer to the newly created node that becomes part
 * of a huffman tree.
 */
static struct node* mkNode(int data, struct node* left, struct node* right)
{
	struct node* np = malloc(sizeof(struct node));
	assert(np != NULL);
	np->count = data;
	np->parent = NULL;
	if(left != NULL)
	{
		left->parent = np;
	}
	if(right != NULL)
	{
		right->parent = np;
	}
	return np;
}

/* Function: Recursively finds the depth of a certain node within a huffman tree.
 * The depth of a node tells you the number of bits that make up a certain character
 * as defined within a huffman tree. The smaller the depth, the more often the character
 * or symbol is used within a certain text (and the smaller the data size of the character). 
 * The opposite is also true, the greater the depth, the less often the character or symbol 
 * is used within a text and the larger its data size is.
 *
 * Efficiency: O(h)
 *
 * Return Value: The depth of a certain node within a huffman tree.
 */
int depth(struct node* node)
{
	if(node->parent == NULL)
	{
		return 0;
	}
	return 1 + depth(node->parent);
}


/* Function: Compares two nodes together to see which of the two given nodes is 
 * smaller or larger.
 *
 * Efficiency: O(1)
 *
 * Return Value: Either 1 or -1 depending on if the first given node is either smaller 
 * or larger than the second given node.
 */
int nodecmp(struct node* first, struct node* second)
{
	assert(first != NULL && second != NULL);
	return (first->count < second->count) ? -1 : (first->count > second->count);
}



/* Function: The driver function of our program. The main function takes in two file parameters
 * (an input file and an output file) and creates a huffman tree based on the frequency of characters
 * and symbol used within the input file. Once this huffman tree is created, the "pack" function 
 * is called to take in the two files and an array of node pointers (the nodes of our huffman tree).
 * The "pack" function is what changes the encoding of characters within the first file and then outputs
 * the same text file (but with different size values for each of the characters/symbols) into the
 * second file parameter. This second file is a compressed version of the first file parameter based
 * on the encoding values of the huffman tree. 
 *
 * The process of creating a huffman tree starts with counting the frequency of the characters/symbols
 * used within a file. An array of 257 characters (256 possible characters with the last character being
 * the end of file "EOF" character). The first file is read character by character with each index of
 * our integer array "counts" keeping track of the number of occurance of each character. Once the file
 * has been completly read, an array of node pointers (the nodes making up a huffman tree) are created
 * and created for each character that has a frequency greater than 0. Once all the nodes are made within
 * the array of node pointers, all the node pointers are then inserted into a Priority Queue. Once all the
 * node pointers are put into the Priority Queue (whom is the shape of a minimum heap), a loop continually 
 * takes out the first two nodes of the Priority Queue (the two smallest nodes within our tree) and combines
 * these two nodes to form one single parent node. Once this parent node is created, it is then inserted 
 * back into the Priority Queue. Eventually, as two elements are removed and cobined, there only will remain
 * one single element within our Priority Queue. This one element left is our huffman tree. With our huffman 
 * tree created (and the encoding of each character/symbol being unique) we are finally able to call the 
 * "pack" function and compress our first given file parameter to create our second output file parameter. 
 * A print statement is done to show the frequency (from our first file parameter) and the new size (for our 
 * second file parameter). Additionally, the total number of bits used for the second file parameter is also
 * printed out.
 *
 * Return Value: The integer 0 once the main function has ended.
 */
int main(int argc, char*argv[])
{
	int counts[257] = {0};
	FILE* fp;
	if (argc == 2)
	{
		printf("A file paramater is missing");
		return -1;
	}
	else
		{
			fp = fopen(argv[1], "rb");
		}
	if(fp == NULL)
	{
		printf("Error opening file");
		return -1;
	}
	int c;
	while((c = getc(fp)) != EOF)
		{
			counts[c]++;
		}
	PQ* pq = createQueue(nodecmp);
	struct node* node [257];
	for(int i = 0; i<256; i++)
	{
		if(counts[i] == 0)
		{
			node[i] = NULL;
		}
		else
		{
			node[i] = mkNode(counts[i], NULL, NULL);
		}	
	}	
	node[256] = mkNode(0, NULL, NULL);
	addEntry(pq, node[256]);
	for(int i = 0; i <256; i++)
	{
		if(node[i] != NULL)
		{	
			addEntry(pq, node[i]);
		}
	}
	while(numEntries(pq) != 1)
	{
		struct node* right = removeEntry(pq);
		struct node* left = removeEntry(pq);
		struct node* newNode = mkNode((left->count+right->count),left,right);
		addEntry(pq,newNode);
	}
	for(int i = 0; i < 257; i++)
	{
		if(node[i] != NULL)
		{
			printf(isprint(i)?"'%c'":"%03o",i);
			printf(": %d x %d bits = %d bits\n",node[i]->count,depth(node[i]),node[i]->count*depth(node[i]));
		}
	}
	pack(argv[1],argv[2],node);
	return 0;
}
