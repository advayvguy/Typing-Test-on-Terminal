#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

char** get_dictionary_array()
{
	FILE* fp = fopen("v1/include/dictionary.txt", "r");
	if(fp == NULL)
	{
		printf("ERROR: couldnt open the dictionary file");
		fclose(fp);
		return 0;
	}

	const int count = 3173; 

	char** dictionary = malloc(count * sizeof(char*));

	char buff[1024];
	int j = 0;

	for(int i = 0; i < count; i++)
	{
		int c;
		while((c = fgetc(fp)) != '\n' && c != EOF)
		{
			buff[j++] = c;
		}
		buff[j++] = '\0';
		dictionary[i] = malloc(strlen(buff) + 1);
		strcpy(dictionary[i], buff);

		j = 0;
	}
	fclose(fp);
	return dictionary;
}

