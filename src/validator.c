#include <ctype.h>
#include "game.h"

int are_valid_game_digits(int game_size, char *game_digits)
{
    const char zero = '0';
    char game_digit;
    char game_size_char = game_size + zero;

    for (int i = 0; i < game_size; i++)
    {
        game_digit = game_digits[i];

        if (!game_digit || !isdigit(game_digit) || game_digit == zero || game_digit > game_size_char)
            return 0;
    }

    return 1;
}

int is_valid_game(Game game)
{
    if (game.size == 0)
    {
        return 0;
    }

    if (!are_valid_game_digits(game.size, game.north))
    {
        return 0;
    }
        
    if (!are_valid_game_digits(game.size, game.south))
    {
        return 0;
    }

    if (!are_valid_game_digits(game.size, game.east))
    {
        return 0;
    }

    if (!are_valid_game_digits(game.size, game.west))
    {
        return 0;
    }

    return 1;
}