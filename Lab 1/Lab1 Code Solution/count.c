/******************************************************************/
/* Name: Jordan Murtiff						                      */
/* Lab #1						 	                              */
/* Lab Section: Wednesday (63650)                                 */
/* 2:15PM - 5:00PM		 	                                      */
/* Date: January 10, 2017				                          */
/*							  	                                  */
/* This program reads and counts words from a text file that is   */
/* supplied from the command line. Once the file is completly read*/
/* the total number of words is printed to the screen. Only words */
/* that are 30 characters and under qualify to be counted.        */

//All libraries used.
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//Defines the maximum word length to be 30 characters.
#define MAX_WORD_LENGTH 30

int main(int argc, char*argv[])
{
	//Creates a string variable that is the maxiMum word length to be recorded. The extra character include the '/0' character as part of the string (since a 30 byte string can only have 29 characters, not the 30 maximum characters).
	char word[MAX_WORD_LENGTH+1];
	
	//Creates a file pointer that points to the file we are reading from.
	FILE *fp;

	//Creates an integer variable that tells you the number of total words. Start with zero since you haven't counted any words yet.
	int wordTotal = 0;	

	//If only one parameter is entered in, tell the user so and end the program.
	if (argc == 1)
	{
		printf("The name of the file is missing.\n");
		return -1;
	}
	
	//Otherwise, if the user has entered in the name of a text file in the command line, execute the program.
	else
	{
		//Opens the text file to be read.
		fp = fopen(argv[1], "r");
		
		//If the file cannot be opened, print an error message and end the program.
		if (fp == NULL)
		{
			printf("Error opening up text file");
			return -1;
		}
		
		//Reads the text file and puts each string into the "word" variable. Assuming that the word is 30 characters or less, then there is enough space in the "word" variable to hold the word from the text file. If this is true, then increase the word total.
		while(fscanf(fp,"%s",word)>0)
		{	
			wordTotal++;
		}
		
		//Closes the file after the reaching the end of the file.
		fclose(fp);
	
		//Prints out the total number of words in the text file.
		printf("%i total words\n",wordTotal);

	}

//End the main function.
return 0;
}	
