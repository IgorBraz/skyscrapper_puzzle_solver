#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "game.h"

Game GAME;

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
            {
                free(node_board[i].options);
            }

            return 0;
        }

        node_board[i].options[0] = 0;
    }

    return 1;
}

int get_adjacent_node_index(char cardinal_point[], int game_index, int game_size)
{
    if (cardinal_point == "north")
    {
        return game_index;
    }
    else if (cardinal_point == "east")
    {
        return game_index * game_size + (game_size - 1);
    }
    else if (cardinal_point == "south")
    {
        return game_size * game_size - (game_index + 1);
    }
    else if (cardinal_point == "west")
    {
        return game_size * (game_size - (game_index + 1));
    }
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
        {
            node_board[node_index].options[i + 1] = 0;
        }
    }
}

void set_node_values_from_direction(Node node_board[], int node_index, char *direction_description)
{
    for (int i = 0; i < GAME.size; i++)
    {
        set_node_value(node_board, node_index, (i + 1) + '0');

        if (direction_description == "north")
        {
            node_index += GAME.size;
        }
        else if (direction_description == "east")
        {
            node_index -= 1;
        }
        else if (direction_description == "south")
        {
            node_index -= GAME.size;
        }
        else if (direction_description == "west")
        {
            node_index += 1;
        }
    }
}

void set_interior_node_options(Node node_board[], int board_size)
{
    int line_moves = GAME.size - 2;
    int node_index = 0;

    for (int i = GAME.size; i < board_size - GAME.size; i += GAME.size)
    {
        for (int j = 0; j < line_moves; j++)
        {
            for (int k = 0; k < GAME.size; k++)
            {
                node_board[i + j + 1].options[k] = (k + 1) + '0';
            }
        }
    }
}

void set_values_from_clues(Node board[], char *direction_description, char *directions, int board_index)
{
    char game_size_char = GAME.size + '0';
    char game_last_index_char = (game_size_char - '1') + '0';
    int adjacent_node_index = get_adjacent_node_index(direction_description, board_index, GAME.size);

    if (directions[board_index] == '1')
    {
        set_node_value(board, adjacent_node_index, game_size_char);
    }
    else if (directions[board_index] == game_size_char)
    {
        set_node_values_from_direction(board, adjacent_node_index, direction_description);
    }
    else if (directions[board_index] == game_last_index_char)
    {
        for (int i = 0; i < GAME.size / 2; i++)
        {
            board[adjacent_node_index].options[i] = (i + 1) + '0';
        }
    }
    else if (!board[adjacent_node_index].value)
    {
        set_all_node_options(board, adjacent_node_index);
    }
}

void guess_from_clues(Node board[], int board_size)
{
    char game_size_char = GAME.size + '0';
    char game_last_index_char = (game_size_char - '1') + '0';
    int game_index = 0;
    int adjacent_node_index = 0;

    for (int i = 0; i < board_size; i++)
    {
        if (i < GAME.size)
        {
            game_index = i;
            set_values_from_clues(board, "north", GAME.north, game_index);
        }
        else if (i < GAME.size * 2)
        {
            game_index = i - GAME.size;
            set_values_from_clues(board, "south", GAME.south, game_index);
        }
        else if (i < GAME.size * 3)
        {
            game_index = i - (GAME.size * 2);
            set_values_from_clues(board, "east", GAME.east, game_index);
        }
        else if (i < GAME.size * 4)
        {
            game_index = i - (GAME.size * 3);
            set_values_from_clues(board, "west", GAME.west, game_index);
        }
    }

    set_interior_node_options(board, board_size);
}

int solved(Node node_board[], int board_size)
{
    for (int i = 0; i < board_size; i++)
    {
        if (!node_board[i].value)
        {
            return 0;
        }
    }

    return 1;
}

void get_line_indexes(int node_index, int board_size, int *line_indexes)
{
    int line_start_index;

    if (node_index > GAME.size)
    {
        line_start_index = node_index - (node_index % GAME.size);
    }
    else
    {
        line_start_index = 0;
    }

    for (int i = 0; i < GAME.size; i++)
    {
        line_indexes[i] = line_start_index;
        line_start_index++;
    }
}

void get_column_indexes(int node_index, int board_size, int *column_indexes)
{
    int column_start_index;

    if (node_index > GAME.size)
    {
        column_start_index = node_index % GAME.size;
    }
    else
    {
        column_start_index = node_index;
    }

    for (int i = 0; i < GAME.size; i++)
    {
        column_indexes[i] = column_start_index;
        column_start_index += GAME.size;
    }
}

void remove_option(Node node, int option_index)
{
    int this_index = option_index;

    for (int next_index = option_index + 1; next_index <= GAME.size; next_index++)
    {
        node.options[this_index++] = node.options[next_index];
    }
}

void remove_all_claimed_options(Node board[], Node node, char compare_node_value, int node_index)
{
    for (int j = 0; j < GAME.size; j++)
    {
        if (compare_node_value == node.options[j])
        {
            remove_option(node, j);

            if (strlen(node.options) == 1)
            {
                set_node_value(board, node_index, node.options[0]);
            }
        }
    }
}

void eliminate_options(Node board[], Node node, int node_index, int *indexes)
{
    int compare_node_index;
    char compare_node_value;
    char *previous_node_options = (char *)calloc(GAME.size + 1, sizeof(char));
    int repeating_options_counter = 0;

    for (int i = 0; i < GAME.size; i++)
    {
        compare_node_index = indexes[i];

        if (node_index == compare_node_index)
        {
            continue;
        }

        compare_node_value = board[compare_node_index].value;

        if (board[compare_node_index].options[0] != '\0')
        {
            if (strlen(board[compare_node_index].options) == 2 && strcmp(previous_node_options, board[compare_node_index].options) == 0)
            {
                repeating_options_counter++;
            }

            strcpy(previous_node_options, board[compare_node_index].options);
        }

        if (repeating_options_counter == GAME.size - 3)
        {
            remove_all_claimed_options(board, node, board[compare_node_index].options[0], node_index);
        }

        if (!compare_node_value)
        {
            continue;
        }

        remove_all_claimed_options(board, node, compare_node_value, node_index);
    }

    free(previous_node_options);
}

void propagate_constraints(Node board[], int board_size)
{
    int line_indexes[GAME.size];
    int column_indexes[GAME.size];

    for (int node_index = 0; node_index < board_size; node_index++)
    {
        if (board[node_index].value)
        {
            continue;
        }

        get_line_indexes(node_index, board_size, line_indexes);
        get_column_indexes(node_index, board_size, column_indexes);

        eliminate_options(board, board[node_index], node_index, line_indexes);
        eliminate_options(board, board[node_index], node_index, column_indexes);
    }
}

void solve(char **constraints)
{
    GAME.size = strlen(constraints[0]);
    GAME.north = constraints[0];
    GAME.east = constraints[1];
    GAME.south = constraints[2];
    GAME.west = constraints[3];

    if (!is_valid_game(GAME))
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

    int tries = 0;

    while (!solved(node_board, board_size))
    {
        propagate_constraints(node_board, board_size);

        tries++;

        if (tries > 2)
        {
            break;
        }
    }

    print_board(GAME, node_board, board_size);

    destroy_board(node_board, board_size);
}