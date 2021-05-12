#define GAME_H

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

int are_valid_game_digits(int game_size, char *game_digits);
int is_valid_game(Game game);

void print_board_line(int game_size, Node node_board[], int node_index);
void print_board(Game game, Node node_board[], int board_size);

void solve(char **constraints);