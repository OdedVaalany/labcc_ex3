//
// Created by עודד ועלני on 08/04/2022.
//

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "markov_chain.h"

#define INPUT_ERROR_FOR_THE_USER \
"Usage: insert <seed> <tweets to generate> \
<path to file> <num of words to read>\n"
#define MAX_WORD_SIZE 100
#define MIN_ARGS 4
#define MAX_ARGS 5
#define MAX_WORDS_AT_TWEET 20

static int fill_database (FILE *fp, int words_to_read, MarkovChain
*markov_chain);

static bool str_to_int (char *s, int *value);

static bool get_file (char *s, FILE **pf);

static void print_tweets (int how_much_tweets, MarkovChain *chain);

bool is_last (void *s)
{
  return ((char *) s)[strlen (((char *) s)) - 1] == '.';
}

void print (void *s)
{
  if (is_last (s))
    {
      printf ("%s", (char *) s);
    }
  else
    {
      printf ("%s ", (char *) s);
    }
}

void *copy (void *s)
{
  char *temp = malloc (strlen (s) + 1);
  memcpy (temp, s, strlen ((char *) s) + 1);
  return (void *) temp;
}

int compare (void *a, void *b)
{
  return strcmp ((char *) a, (char *) b);
}
int main (int argc, char *argv[])
{
  int tweets_to_generate, seed, words_to_read;
  FILE *fp;
  if (!(argc == MAX_ARGS && str_to_int (argv[4], &words_to_read)))
    {
      words_to_read = -1;
    }
  if (((argc == MIN_ARGS) || (argc == MAX_ARGS))
      && str_to_int (argv[1], &seed) &&
      str_to_int (argv[2], &tweets_to_generate) &&
      get_file (argv[3], &fp))
    {
      srand (seed);
      MarkovChain *chain = init_markov_chain
          (&print, &free,
           &compare, &copy, &is_last);
      if (chain == NULL)
        {
          printf (ALLOCATION_ERROR_MASSAGE);
          EXIT_FAILURE;
        }
      if (fill_database (fp, words_to_read, chain))
        {
          printf (ALLOCATION_ERROR_MASSAGE);
          EXIT_FAILURE;
        }
      print_tweets (tweets_to_generate, chain);
      free_markov_chain (&chain);
      fclose (fp);
    }
  else
    {
      printf (INPUT_ERROR_FOR_THE_USER);
      return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}

static int fill_database (FILE *fp, int words_to_read, MarkovChain
*markov_chain)
{
  int word_counter = 0, position = 0;
  char word[MAX_WORD_SIZE];
  char x = fgetc (fp);
  MarkovNode *previous_word, *new_node;
  while (word_counter < words_to_read || words_to_read == -1)
    {
      if ((x == ' ' || x == EOF || x == '\n') && position != 0)
        {
          word[position] = '\0';
          new_node = add_to_database (markov_chain, word)->data;
          if (new_node == NULL)
            {
              return 1;
            }
          if (word_counter > 0 && !previous_word->final)
            {
              if (add_node_to_counter_list (previous_word,
                                            new_node,
                                            markov_chain) == false)
                {
                  return 1;
                }
            }
          previous_word = new_node;
          position = 0;
          word_counter++;
          if (x == EOF)
            {
              break;
            }
        }
      else if (x != ' ')
        {
          word[position] = x;
          position++;
        }
      x = fgetc (fp);
    }
  return 0;
}

static void print_tweets (int how_much_tweets, MarkovChain *chain)
{
  MarkovNode *first_node;
  for (int i = 0; i < how_much_tweets; ++i)
    {
      first_node = get_first_random_node (chain);
      printf ("Tweet %d: ", i + 1);
      generate_random_sequence (chain,
                                first_node, MAX_WORDS_AT_TWEET);
    }
}

bool str_to_int (char *s, int *value)
{
  if (sscanf (s, "%d", value) == 1)
    {
      return true;
    }
  return false;
}

static bool get_file (char *s, FILE **pf)
{
  FILE *temp = fopen (s, "r");
  if (temp == NULL)
    {
      return false;
    }
  *pf = temp;
  return true;
}