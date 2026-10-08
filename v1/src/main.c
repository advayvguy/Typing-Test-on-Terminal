#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

#define CYAN  "\033[36m"
#define RESET "\033[0m"
#define YELLOW "\033[1;33m"

char** generate(int wordcount);
char* get_input(int seconds);

char** split_words(char* input, int* count)
{
	int capacity = 100;
	int n = 0;
	int i = 0;

	char** words = malloc(capacity*sizeof(char*));

	while(input[i] != 0)
	{
		while(input[i] == ' ' || input[i] == '\n' || input[i] == '\t')
		{
			i++;
		}
		if (input[i] == '\0')
		{
			break;
		}

		int start = i;
		while (
				input[i] != '\0' &&
				input[i] != '\t' &&
				input[i] != ' ' &&
				input[i] != '\n'
		      ) 
		{
			i++;
		}
		int length = i - start;
		words[n] = malloc(length + 1);

		for(int j = 0; j < length; j++)
		{
			words[n][j] = input[start + j];
		}
		words[n][length] = '\0';

		n++;
	}
	*count = n;
	return words;
}

void calculate_result(int time,
        char** inputlist,
        int input_count,
        char** wordlist,
        int word_count)
{
    int correct_chars = 0;
    int total_chars = 0;

    if (input_count > word_count)
        input_count = word_count;

    for (int i = 0; i < input_count; i++)
    {
        int input_len = strlen(inputlist[i]);
        int word_len = strlen(wordlist[i]);

        total_chars += input_len;

        int compare_len = input_len;

        if (compare_len > word_len)
            compare_len = word_len;

        for (int j = 0; j < compare_len; j++)
        {
            if (inputlist[i][j] == wordlist[i][j])
                correct_chars++;
        }
    }

    double mins = time / 60.0;

    double accuracy = 0;

    correct_chars += input_count - 1;
    if (total_chars > 0)
        accuracy = (double)correct_chars / total_chars * 100;

    double wpm = (correct_chars / 5.0) / mins;

    printf("\n--------------------------------------------------\n");
	printf(CYAN "time:               " RESET "%d seconds\n", time);
	printf(CYAN "correct characters: " RESET "%d\n", correct_chars);
	printf(CYAN "accuracy:           " RESET "%.2f%%\n", accuracy);
	printf(CYAN "WPM:                " RESET "%.2f\n", wpm);
}

int main()
{
	int time;

	printf("\033[93m\033[3m");
    	printf("___________             .__                 ___________              __   \n");
    	printf("\\__    ___/__.__.______ |__| ____    ____   \\__    ___/___   _______/  |_ \n");
    	printf("  |    | <   |  |\\____ \\|  |/    \\ / _ \__\\    |    |_/ __ \\ /  ___/\\   __\\\n");
    	printf("  |    |  \\___  ||  |_> >  |   |  \\/ /_/  >   |    |\\  ___/ \\___ \\  |  |  \n");
    	printf("  |____|  / ____||   __/|__|___|  /\\___  /    |____| \\___  >____  > |__|  \n");
    	printf("          \\/     |__|           \\//_____/                \\/     \\/        \n");
    	printf("\033[0m\n");

	printf("Enter time limit (seconds): ");
	scanf("%d", &time);

	char** wordlist = generate(6*time);
	char *input = get_input(time);

	int words = 0;

	char** inputlist = split_words(input, &words);

	calculate_result(time, inputlist, words, wordlist, 6*time);
	free(input);
}
