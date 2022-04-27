#include <string.h> // For strlen(), strcmp(), strcpy()
#include "markov_chain.h"

#define MAX(X, Y) (((X) < (Y)) ? (Y) : (X))

#define EMPTY -1
#define BOARD_SIZE 100
#define MAX_GENERATION_LENGTH 60
#define SAT_ARGS 3
#define DICE_MAX 6
#define NUM_OF_TRANSITIONS 20
#define INPUT_ERROR_FOR_THE_USER "Usage: insert <seed> <paths to find>\n"

/**
 * represents the transitions by ladders and snakes in the game
 * each tuple (x,y) represents a ladder from x to if x<y or a snake otherwise
 */
const int transitions[][2] = {{13, 4},
                              {85, 17},
                              {95, 67},
                              {97, 58},
                              {66, 89},
                              {87, 31},
                              {57, 83},
                              {91, 25},
                              {28, 50},
                              {35, 11},
                              {8,  30},
                              {41, 62},
                              {81, 43},
                              {69, 32},
                              {20, 39},
                              {33, 70},
                              {79, 99},
                              {23, 76},
                              {15, 47},
                              {61, 14}};

/**
 * struct represents a Cell in the game board
 */
typedef struct Cell {
    int number; // Cell number 1-100
    int ladder_to;  // ladder_to represents the jump of the ladder
    // in case there is one from this square
    int snake_to;  // snake_to represents the jump of the snake in
    // case there is one from this square
    //both ladder_to and snake_to should be -1 if the Cell doesn't have them
} Cell;

/** Error handler **/
static int handle_error (char *error_msg, MarkovChain **database)
{
  printf ("%s", error_msg);
  if (database != NULL)
    {
      free_markov_chain (database);
    }
  return EXIT_FAILURE;
}

static int create_board (Cell *cells[BOARD_SIZE])
{
  for (int i = 0; i < BOARD_SIZE; i++)
    {
      cells[i] = malloc (sizeof (Cell));
      if (cells[i] == NULL)
        {
          for (int j = 0; j < i; j++)
            {
              free (cells[j]);
            }
          handle_error (ALLOCATION_ERROR_MASSAGE, NULL);
          return EXIT_FAILURE;
        }
      *(cells[i]) = (Cell) {i + 1, EMPTY, EMPTY};
    }

  for (int i = 0; i < NUM_OF_TRANSITIONS; i++)
    {
      int from = transitions[i][0];
      int to = transitions[i][1];
      if (from < to)
        {
          cells[from - 1]->ladder_to = to;
        }
      else
        {
          cells[from - 1]->snake_to = to;
        }
    }
  return EXIT_SUCCESS;
}

/**
 * fills database
 * @param markov_chain
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
static int fill_database (MarkovChain *markov_chain)
{
  Cell *cells[BOARD_SIZE];
  if (create_board (cells) == EXIT_FAILURE)
    {
      return EXIT_FAILURE;
    }
  MarkovNode *from_node = NULL, *to_node = NULL;
  size_t index_to;
  for (size_t i = 0; i < BOARD_SIZE; i++)
    {
      add_to_database (markov_chain, cells[i]);
    }

  for (size_t i = 0; i < BOARD_SIZE; i++)
    {
      from_node = get_node_from_database (markov_chain,
                                          cells[i])->data;

      if (cells[i]->snake_to != EMPTY || cells[i]->ladder_to != EMPTY)
        {
          index_to = MAX(cells[i]->snake_to, cells[i]->ladder_to) - 1;
          to_node = get_node_from_database (markov_chain,
                                            cells[index_to])
              ->data;
          add_node_to_counter_list (from_node,
                                    to_node, markov_chain);
        }
      else
        {
          for (int j = 1; j <= DICE_MAX; j++)
            {
              index_to = ((Cell *) (from_node->data))->number + j - 1;
              if (index_to >= BOARD_SIZE)
                {
                  break;
                }
              to_node = get_node_from_database (markov_chain,
                                                cells[index_to])
                  ->data;
              add_node_to_counter_list (from_node,
                                        to_node, markov_chain);
            }
        }
    }
  // free temp arr
  for (size_t i = 0; i < BOARD_SIZE; i++)
    {
      free (cells[i]);
    }
  return EXIT_SUCCESS;
}
/**
 * the function get string to convrt to int, and put the value in the value arg
 * @param s the string to read from
 * @param value where to put the value
 * @return true if success false either
 */
bool str_to_int (char *s, int *value)
{
  if (sscanf (s, "%d", value) == 1)
    {
      return true;
    }
  return false;
}

bool is_last (void *cell)
{
  return BOARD_SIZE == ((Cell *) cell)->number;
}

void print_cell (void *cell)
{
  Cell *temp = (Cell *) cell;
  if (temp->ladder_to != -1)
    {
      printf ("[%d]-ladder to %d ->", temp->number, temp->ladder_to);
    }
  else if (temp->snake_to != -1)
    {
      printf ("[%d]-snake to %d ->", temp->number, temp->snake_to);
    }
  else
    {
      if (temp->number == BOARD_SIZE)
        {
          printf ("[%d]", temp->number);
        }
      else
        {
          printf ("[%d] -> ", temp->number);
        }
    }
}

int comp_cells (void *first, void *second)
{
  return ((Cell *) first)->number - ((Cell *) second)->number;
}

void *copy_cell (void *cell)
{
  void *temp = malloc (sizeof (Cell));
  if (temp == NULL)
    {
      return NULL;
    }
  memcpy (temp, cell, sizeof (Cell));
  return temp;
}

/**
 * thie function print paths within the game
 * @param how_much_paths number of paths
 * @param chain the database
 */
void print_paths (int how_much_paths, MarkovChain *chain)
{
  MarkovNode *first_node;
  for (int i = 0; i < how_much_paths; ++i)
    {
      first_node = (MarkovNode *) chain->database->first->data;
      printf ("Random Walk %d: ", i + 1);
      generate_random_sequence (chain,
                                first_node, MAX_GENERATION_LENGTH);
    }
}

/**
 * @param argc num of arguments
 * @param argv 1) Seed
 *             2) Number of sentences to generate
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
int main (int argc, char *argv[])
{
  int seed, number_of_paths;
  if (argc == SAT_ARGS && str_to_int (argv[1], &seed)
      && str_to_int (argv[2], &number_of_paths))
    {
      srand (seed);
      MarkovChain *chain = init_markov_chain
          (&print_cell, &free,
           &comp_cells, &copy_cell, &is_last);
      if (chain == NULL)
        {
          return handle_error (ALLOCATION_ERROR_MASSAGE,
                               NULL);
        }
      if (fill_database (chain) == EXIT_FAILURE)
        {
          return handle_error (ALLOCATION_ERROR_MASSAGE,
                               &chain);
        }
      print_paths (number_of_paths, chain);
      free_markov_chain (&chain);
      return EXIT_SUCCESS;

    }
  else
    {
      printf (INPUT_ERROR_FOR_THE_USER);
    }
  return EXIT_SUCCESS;
}

