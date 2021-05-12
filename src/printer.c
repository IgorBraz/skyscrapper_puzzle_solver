#include <unistd.h>
#include <string.h>
#include "game.h"

void print_board_line(int game_size, Node node_board[], int node_index)
{
    char value;

    for (int j = 0; j < game_size; j++)
    {
        value = node_board[node_index].value;

        if (!value)
        {
            write(1, "0", 1);
        }
        else
        {
            write(1, &value, 1);
        }

        if (j == game_size - 1)
        {
            write(1, "\n", 1);
            return;
        }

        write(1, " ", 1);

        node_index++;
    }
}

void print_board(Game game, Node node_board[], int board_size)
{
    for (int i = 0; i < board_size; i += game.size)
    {
        print_board_line(game.size, node_board, i);
    }
}