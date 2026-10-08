#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

char** get_dictionary_array();

char** generate(int WORD_LENGTH)
{
	char** dictionary = get_dictionary_array();
	char** wordlist = malloc(WORD_LENGTH * sizeof(char*));

	int count = 3173;

	srand(time(NULL)); //sets seed as the current time for the random number generator

	for (int i = 0; i < WORD_LENGTH; i++)
	{
		int random = rand() % count;
    		wordlist[i] = malloc(strlen(dictionary[random]) + 1);
		strcpy(wordlist[i], dictionary[random]); //safe deepcopy?
	}

	printf("-----------------------------------------------------------------------------\n");
	for(int i = 0; i < WORD_LENGTH; i++)
	{
		printf("%s ", wordlist[i]);
		if (i%16 == 15)
		{
			printf("\n");
		}
	}
	printf("\n----------------------------------------------------------------------------\n");
	printf("----------------------------------------------------------------------------\n");


	return wordlist;
}
