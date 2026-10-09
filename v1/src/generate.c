#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

char** get_dictionary_array();

char* generate(int WORD_LENGTH)
{
    char** dictionary = get_dictionary_array();
    int count = 3173;

    if (dictionary == NULL || WORD_LENGTH <= 0)
        return NULL;

    // Calculate the required buffer size
    size_t buffer_size = 1;

    for (int i = 0; i < WORD_LENGTH; i++)
    {
        int random = rand() % count;
        buffer_size += strlen(dictionary[random]) + 1;
    }

    // Allocate one buffer for all words
    char* buffer = malloc(buffer_size);

    if (buffer == NULL)
        return NULL;

    buffer[0] = '\0';

    for (int i = 0; i < WORD_LENGTH; i++)
    {
        int random = rand() % count;

        strcat(buffer, dictionary[random]);

        if ((i + 1) % 10 == 0 && i < WORD_LENGTH - 1)
        {
            strcat(buffer, "\n");
        }
        else if (i < WORD_LENGTH - 1)
        {
            strcat(buffer, " ");
        }
    }

    printf("-----------------------------------------------------------------------------\n");
    printf("%s\n", buffer);
    printf("-----------------------------------------------------------------------------\n");

    free(dictionary);

    return buffer;
}

