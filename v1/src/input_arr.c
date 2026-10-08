#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <sys/select.h>
#include <time.h>

char* get_input(int seconds)
{
	char* buffer = malloc(10000);
	int i = 0;
	char c;

	struct termios old, new; //save current terminal settings
	tcgetattr(STDIN_FILENO, &old); //gets terminals current settings and stores them in old
	new = old; //creates an entire struct in this case, not a soft copy
	new.c_lflag &= ~ICANON; //~ICANON- makes input immediate
	new.c_lflag &= ~ECHO; //we will print it ourselves
	tcsetattr(STDIN_FILENO, TCSANOW, &new); //set new attributes

	read(STDIN_FILENO, &c, 1); //wait exactly for one key
	buffer[i++] = c;
	putchar(c);
	fflush(stdout);

	struct timespec start, now;

	clock_gettime(CLOCK_MONOTONIC, &start);

	while(1)
	{
		clock_gettime(CLOCK_MONOTONIC, &now);

		double elapsed = (now.tv_sec - start.tv_sec) + (now.tv_nsec - start.tv_nsec)/1e9;

		if (elapsed >= seconds)
		{
			break;
		}
		fd_set input = {0};
		FD_SET(STDIN_FILENO, &input); //basically enables it to take keyboard input

		struct timeval timeout = {0, 100000}; //dont wait for more than 0.1 seconds for an input, the while needs to run
		if (select(STDIN_FILENO + 1, &input, NULL, NULL, &timeout) > 0)
		{
			read(STDIN_FILENO, &c, 1);
			if (c == '\b' || c == 127) //delete or backspace
			{
				if(i > 0)
				{
					i--;
					printf("\b \b"); //move cursor to the left, put space and move cursor to the left again
				}
			}
			else
			{
				buffer[i++] = c;
				putchar(c);
			}
			fflush(stdout); //forces output immediatly
		}
	}
	buffer[i] = '\0';
       	tcsetattr(STDIN_FILENO, TCSANOW, &old); //reset the terminal settings back to normal

	return buffer;	
}

