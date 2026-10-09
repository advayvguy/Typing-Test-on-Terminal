#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <time.h>

#define GREEN        "\033[32m"
#define RED          "\033[31m"
#define RESET        "\033[0m"
#define HIDE_CURSOR  "\033[?25l"
#define SHOW_CURSOR  "\033[?25h"
#define CLEAR_SCREEN "\033[2J\033[H"

typedef struct
{
    int row;
    int col;
} CharPos;


static void move_cursor(int row, int col)
{
    printf("\033[%d;%dH", row + 1, col + 1);
}


static CharPos *build_positions(
    const char *target,
    int len,
    int width,
    int *total_rows
)
{
    CharPos *positions = malloc(sizeof(*positions) * (len + 1));

    if (!positions)
        return NULL;

    int row = 0;
    int col = 0;

    for (int i = 0; i < len; i++)
    {
        positions[i].row = row;
        positions[i].col = col;

        if (target[i] == '\n')
        {
            row++;
            col = 0;
        }
        else
        {
            col++;

            if (col >= width)
            {
                row++;
                col = 0;
            }
        }
    }

    positions[len].row = row;
    positions[len].col = col;

    *total_rows = row + 1;

    return positions;
}


char *get_input(const char *target, int seconds)
{
    struct termios oldt;
    struct termios newt;
    struct winsize ws;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1)
    {
        perror("ioctl");
        return NULL;
    }

    int width = ws.ws_col - 1;

    if (width < 1)
        width = 80;

    int len = strlen(target);
    int total_rows = 0;

    CharPos *positions =
        build_positions(target, len, width, &total_rows);

    if (!positions)
        return NULL;

    if (total_rows + 3 >= ws.ws_row)
    {
        printf("Terminal is too small. Enlarge it and try again.\n");
        free(positions);
        return NULL;
    }

    char *buffer = calloc((size_t)len + 1, sizeof(*buffer));

    if (!buffer)
    {
        free(positions);
        return NULL;
    }

    if (tcgetattr(STDIN_FILENO, &oldt) == -1)
    {
        perror("tcgetattr");
        free(buffer);
        free(positions);
        return NULL;
    }

    newt = oldt;

    newt.c_lflag &= ~(ICANON | ECHO);
    newt.c_iflag &= ~(IXON | ICRNL);
    newt.c_cc[VMIN] = 1;
    newt.c_cc[VTIME] = 0;

    if (tcsetattr(STDIN_FILENO, TCSANOW, &newt) == -1)
    {
        perror("tcsetattr");
        free(buffer);
        free(positions);
        return NULL;
    }

    printf(CLEAR_SCREEN HIDE_CURSOR);

    /* Print target text */
    for (int i = 0; i < len; i++)
    {
        if (target[i] == '\n')
            continue;

        move_cursor(positions[i].row, positions[i].col);
        putchar(target[i]);
    }

    fflush(stdout);

    int i = 0;
    time_t start = time(NULL);

    while (i < len)
    {
        if (difftime(time(NULL), start) >= seconds)
            break;

        move_cursor(
            positions[i].row,
            positions[i].col
        );

        fflush(stdout);

        fd_set set;

        FD_ZERO(&set);
        FD_SET(STDIN_FILENO, &set);

        struct timeval timeout = {
            .tv_sec = 0,
            .tv_usec = 100000
        };

        int ready = select(
            STDIN_FILENO + 1,
            &set,
            NULL,
            NULL,
            &timeout
        );

        if (ready < 0)
            break;

        if (ready == 0)
            continue;

        char c;

        if (read(STDIN_FILENO, &c, 1) != 1)
            continue;

        /* Backspace */
        if (c == 127 || c == '\b')
        {
            if (i == 0)
                continue;

            i--;

            /*
             * Don't land on a newline.
             */
            if (target[i] == '\n')
            {
                if (i == 0)
                    continue;

                i--;
            }

            buffer[i] = '\0';

            move_cursor(
                positions[i].row,
                positions[i].col
            );

            printf(RESET "%c", target[i]);

            fflush(stdout);

            continue;
        }

        /* Newline */
        if (target[i] == '\n')
        {
            if (c != '\n' && c != '\r' && c != ' ')
                continue;

            buffer[i] = '\n';
            i++;

            continue;
        }

        /* Ignore non-printable characters */
        if (c < 32 || c > 126)
            continue;

        /* Spaces must match */
        if (target[i] == ' ' && c != ' ')
            continue;

        if (target[i] != ' ' && c == ' ')
            continue;

        buffer[i] = c;

        move_cursor(
            positions[i].row,
            positions[i].col
        );

        if (c == target[i])
            printf(GREEN "%c" RESET, target[i]);
        else
            printf(RED "%c" RESET, target[i]);

        i++;

        fflush(stdout);
    }

    /* Restore terminal */
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    printf(SHOW_CURSOR);
    move_cursor(total_rows + 1, 0);

    free(positions);

    return buffer;
}
