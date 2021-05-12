#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <stdio.h>

typedef struct game
{
    int size;
    char *north;
    char *south;
    char *east;
    char *west;
} Game;

typedef struct node
{
    char value;
    char *options;
} Node;

Game GAME;

int are_valid_game_digits(char *game_digits)
{
    const char zero = '0';
    char game_digit;
    char game_size_char = GAME.size + zero;

    for (int i = 0; i < GAME.size; i++)
    {
        game_digit = game_digits[i];

        if (!game_digit || !isdigit(game_digit) || game_digit == zero || game_digit > game_size_char)
            return 0;
    }

    return 1;
}

int is_valid_game()
{
    if (GAME.size == 0)
        return 0;
    if (!are_valid_game_digits(GAME.north))
        return 0;
    if (!are_valid_game_digits(GAME.south))
        return 0;
    if (!are_valid_game_digits(GAME.east))
        return 0;
    if (!are_valid_game_digits(GAME.west))
        return 0;

    return 1;
}

void destroy_board(Node node_board[], int board_size)
{
    for (int i = 0; i < board_size; i++)
    {
        node_board[i].value = 0;
        free(node_board[i].options);
    }
}

int initialize_board_nodes(Node node_board[], int board_size)
{
    for (int i = 0; i < board_size; i++)
    {
        node_board[i].value = 0;
        node_board[i].options = (char *)calloc(GAME.size + 1, sizeof(char));
        if (!node_board[i].options)
        {
            printf("Can't allocate memory for board options at index %d.", i);
            for (i; i >= 0; --i)
                free(node_board[i].options);
            return 0;
        }
        node_board[i].options[0] = 0;
    }

    return 1;
}

int get_adjacent_node_index(char cardinal_point[], int game_index, int game_size)
{
    if (cardinal_point == "north")
        return game_index;
    else if (cardinal_point == "east")
        return game_index * game_size + (game_size - 1);
    else if (cardinal_point == "south")
        return game_size * game_size - (game_index + 1);
    else if (cardinal_point == "west")
        return game_size * (game_size - (game_index + 1));
}

void set_node_value(Node node_board[], int node_index, char node_value)
{
    node_board[node_index].value = node_value;
    node_board[node_index].options[0] = '\0';
}

void set_all_node_options(Node node_board[], int node_index)
{
    for (int i = 0; i < GAME.size; i++)
    {
        node_board[node_index].options[i] = (i + 1) + '0';

        if (i == GAME.size - 1)
            node_board[node_index].options[i + 1] = 0;
    }
}

void set_column_values_from_north(Node node_board[], int node_index)
{
    for (int i = 0; i < GAME.size; i++)
    {
        set_node_value(node_board, node_index, (i + 1) + '0');
        node_index += GAME.size;
    }
}

void set_line_values_from_east(Node node_board[], int node_index)
{
    for (int i = 0; i < GAME.size; i++)
    {
        set_node_value(node_board, node_index, (i + 1) + '0');
        node_index -= 1;
    }
}

void set_column_values_from_south(Node node_board[], int node_index)
{
    for (int i = 0; i < GAME.size; i++)
    {
        set_node_value(node_board, node_index, (i + 1) + '0');
        node_index -= GAME.size;
    }
}

void set_line_values_from_west(Node node_board[], int node_index)
{
    for (int i = 0; i < GAME.size; i++)
    {
        set_node_value(node_board, node_index, (i + 1) + '0');
        node_index += 1;
    }
}

void set_interior_node_options(Node node_board[], int board_size)
{
    int line_moves = GAME.size - 2;
    int node_index = 0;

    for (int i = GAME.size; i < board_size - GAME.size; i += GAME.size)
        for (int j = 0; j < line_moves; j++)
            for (int k = 0; k < GAME.size; k++)
                node_board[i + j + 1].options[k] = (k + 1) + '0';
}

void guess_from_clues(Node node_board[], int board_size)
{
    char game_size_char = GAME.size + '0';
    char game_last_index_char = (game_size_char - '1') + '0';
    int game_index = 0;
    int node_index = 0;

    for (int i = 0; i < board_size; i++)
    {
        if (i < 4)
        {
            game_index = i;
            node_index = get_adjacent_node_index("north", game_index, GAME.size);

            if (GAME.north[game_index] == '1')
                set_node_value(node_board, node_index, game_size_char);
            else if (GAME.north[game_index] == game_size_char)
                set_column_values_from_north(node_board, node_index);
            else if (GAME.north[game_index] == game_last_index_char)
                for (int i = 0; i < GAME.size / 2; i++)
                    node_board[node_index].options[i] = (i + 1) + '0';
            else if (!node_board[node_index].value)
                set_all_node_options(node_board, node_index);
        }
        else if (i < 8)
        {
            game_index = i - GAME.size;
            node_index = get_adjacent_node_index("east", game_index, GAME.size);

            if (GAME.east[game_index] == '1')
                set_node_value(node_board, node_index, game_size_char);
            else if (GAME.east[game_index] == game_size_char)
                set_line_values_from_east(node_board, node_index);
            else if (GAME.east[game_index] == game_last_index_char)
                for (int i = 0; i < GAME.size / 2; i++)
                    node_board[node_index].options[i] = (i + 1) + '0';
            else if (!node_board[node_index].value)
                set_all_node_options(node_board, node_index);
        }
        else if (i < 12)
        {
            game_index = i - (GAME.size * 2);
            node_index = get_adjacent_node_index("south", game_index, GAME.size);

            if (GAME.south[game_index] == '1')
                set_node_value(node_board, node_index, game_size_char);
            else if (GAME.south[game_index] == game_size_char)
                set_column_values_from_south(node_board, node_index);
            else if (GAME.south[game_index] == game_last_index_char)
                for (int i = 0; i < GAME.size / 2; i++)
                    node_board[node_index].options[i] = (i + 1) + '0';
            else if (!node_board[node_index].value)
                set_all_node_options(node_board, node_index);
        }
        else if (i < 16)
        {
            game_index = i - (GAME.size * 3);
            node_index = get_adjacent_node_index("west", game_index, GAME.size);

            if (GAME.west[game_index] == '1')
                set_node_value(node_board, node_index, game_size_char);
            else if (GAME.west[game_index] == game_size_char)
                set_line_values_from_west(node_board, node_index);
            else if (GAME.west[game_index] == game_last_index_char)
                for (int i = 0; i < GAME.size / 2; i++)
                    node_board[node_index].options[i] = (i + 1) + '0';
            else if (!node_board[node_index].value)
                set_all_node_options(node_board, node_index);
        }
    }

    set_interior_node_options(node_board, board_size);
}

int finished(Node node_board[], int board_size)
{
    for (int i = 0; i < board_size; i++)
    {
        if (!node_board[i].value)
            return 0;
    }

    return 1;
}

void print_board_line(Node node_board[], int node_index)
{
    char value;

    for (int j = 0; j < GAME.size; j++)
    {
        value = node_board[node_index].value;

        if (!value)
            write(1, "0", 1);
        else
            write(1, &value, 1);

        if (j == GAME.size - 1)
        {
            write(1, "\n", 1);
            return;
        }

        write(1, " ", 1);
        node_index++;
    }
}

void print(Node node_board[], int board_size)
{
    for (int i = 0; i < board_size; i += GAME.size)
        print_board_line(node_board, i);
}

void solve(char **constraints)
{
    GAME.size = strlen(constraints[0]);
    GAME.north = constraints[0];
    GAME.east = constraints[1];
    GAME.south = constraints[2];
    GAME.west = constraints[3];

    if (!is_valid_game())
    {
        return;
    }

    int board_size = GAME.size * GAME.size;
    Node node_board[board_size];

    if (!initialize_board_nodes(node_board, board_size))
    {
        return;
    }

    guess_from_clues(node_board, board_size);

    while (!finished(node_board, board_size))
    {
        break;
    }

    print(node_board, board_size);

    destroy_board(node_board, board_size);
}