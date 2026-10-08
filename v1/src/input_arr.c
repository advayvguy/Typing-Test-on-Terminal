#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <termios.h>
#include <sys/select.h>
#include <time.h>
#include <sys/ioctl.h>   // add to the includes

static int input_len = 0;   // current number of typed characters

void show_timer(double remaining)
{
    struct winsize ws;
    int cols = 80;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0 && ws.ws_col > 0)
        cols = ws.ws_col;

    int extra_rows = (input_len > 0) ? (input_len - 1) / cols : 0;

    printf("\0337");                          // save cursor
    printf("\033[%dA\r", 1 + extra_rows);     // up to the timer line, column 0
    printf("\033[33m");                       // yellow
    printf("Time remaining:%2.0f seconds", ceil(remaining));
    printf("\033[0m");                        // reset color
    printf("\033[K");                         // clear rest of line
    printf("\0338");                          // restore cursor
    fflush(stdout);
}

char* get_input(int seconds)
{
	char* buffer = malloc(10000);
	int i = 0;
	char c;

	input_len = 0;

	struct termios old, new; //save current terminal settings
	tcgetattr(STDIN_FILENO, &old); //gets terminals current settings and stores them in old
	new = old; //creates an entire struct in this case, not a soft copy
	new.c_lflag &= ~ICANON; //~ICANON- makes input immediate
	new.c_lflag &= ~ECHO; //we will print it ourselves
	tcsetattr(STDIN_FILENO, TCSANOW, &new); //set new attributes
						
	printf("\033[33mTime remaining: %d seconds\033[0m\n", seconds);
	fflush(stdout);
	do { read(STDIN_FILENO, &c, 1); } while (c == '\n' || c == '\r');
	buffer[i++] = c;
	putchar(c);
	fflush(stdout);

	struct timespec start, now;

	clock_gettime(CLOCK_MONOTONIC, &start);

	while(1)
	{
		clock_gettime(CLOCK_MONOTONIC, &now);

		double elapsed = (now.tv_sec - start.tv_sec) + (now.tv_nsec - start.tv_nsec)/1e9;

		double remaining = seconds - elapsed;
		if (remaining < 0) remaining = 0;
		input_len = i;
		show_timer(remaining);
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
			if (c == '\n' || c == '\r') continue;
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

