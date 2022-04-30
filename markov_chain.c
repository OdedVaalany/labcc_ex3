//
// Created by עודד ועלני on 26/04/2022.
//
#include "markov_chain.h"
#include <string.h>

MarkovNode *get_first_random_node (MarkovChain *markov_chain)
{
  Node *ptr;
  do
    {
      int position = randomize_number (markov_chain->database->size);
      ptr = markov_chain->database->first;
      for (int i = 0; i < position; ++i)
        {
          ptr = ptr->next;
        }
    }
  while (((MarkovNode *)ptr->data)->final);
  return ptr->data;
}

void Print(int num, MarkovNode *a)
{
  for(int i =0;i<a->counter_list_len ; i++)
    {
      printf("%d %d",num,(a->counter_list+i)->frequency);
    }
    printf("\n");
}

MarkovNode *get_next_random_node (MarkovNode *state_struct_ptr)
{
  int random_number = randomize_number (state_struct_ptr->total_next_word);
  int sum = 0;
  Print (random_number,state_struct_ptr);
  NextNodeCounter *ptr = state_struct_ptr->counter_list;
  while(ptr!= NULL)
    {
      if (random_number >= sum && random_number < ptr->frequency + sum)
        {
          return ptr->markov_node;
        }
      sum += ptr->frequency;
      ptr++;
    }
  return NULL;
}

void generate_random_sequence (MarkovChain *markov_chain, MarkovNode *
first_node, int max_length)
{
  if (markov_chain->database->size != 0)
    {
      MarkovNode *ptr = first_node;
      for (int i = 0; i < max_length; ++i)
        {
          if (!ptr->final && i + 1 < max_length && ptr->counter_list_len > 0)
            {
              markov_chain->print_func (ptr->data);
            }
          else
            {
              markov_chain->print_func (ptr->data);
              break;
            }
          ptr = get_next_random_node (ptr);
        }
    }
  printf ("\n");
}

void free_markov_chain (MarkovChain **markov_chain)
{
  Node *ptr = (*markov_chain)->database->first;
  while (ptr != NULL)
    {
      Node *temp = ptr;
      ptr = ptr->next;
      MarkovNode *markov = (MarkovNode *) temp->data;
      (*markov_chain)->free_data (markov->data);
      free (markov->counter_list);
      free (markov);
      free (temp);
    }
  free ((*markov_chain)->database);
  free ((*markov_chain));
  (*markov_chain) = NULL;
}

bool add_node_to_counter_list (MarkovNode *first_node, MarkovNode
*second_node, MarkovChain *markov_chain)
{
  NextNodeCounter *runner = first_node->counter_list;
  for (int i = 0; i < first_node->counter_list_len; i++)
    {
      if (markov_chain->comp_func (first_node->data, (runner
                                                      + i)->markov_node->data)
          == 0)
        {
          (runner + i)->frequency++;
          first_node->total_next_word++;
          return true;
        }
    }

  first_node->counter_list =
      realloc (first_node->counter_list,
               sizeof (NextNodeCounter) * (first_node->counter_list_len + 1));
  if (first_node->counter_list == NULL)
    {
      return false;
    }
  (first_node->counter_list + first_node->counter_list_len)->frequency = 1;
  (first_node->counter_list
   + first_node->counter_list_len)->markov_node = second_node;
  first_node->counter_list_len++;
  first_node->total_next_word++;
  return true;

}

Node *get_node_from_database (MarkovChain *markov_chain, void *data_ptr)
{
  Node *runner = markov_chain->database->first;
  while (runner != NULL)
    {
      if (markov_chain->comp_func
          (((MarkovNode *) runner->data)->data, data_ptr)
          == 0)
        {
          return runner;
        }
      runner = runner->next;
    }
  return NULL;
}

Node *add_to_database (MarkovChain *markov_chain, void *data_ptr)
{
  Node *temp = get_node_from_database (markov_chain, data_ptr);
  if (temp != NULL)
    {
      return temp;
    }
  else
    {
      MarkovNode *temp_markov_node =
          init_markov_node (data_ptr, markov_chain);
      if (temp_markov_node == NULL)
        {
          return NULL;
        }
      if (add (markov_chain->database, (void *) temp_markov_node))
        {
          markov_chain->free_data (temp_markov_node->data);
          free (temp_markov_node);
          return NULL;
        }
      return markov_chain->database->last;
    }

}

MarkovNode *init_markov_node (void *data_ptr, MarkovChain *markov_chain)
{
  MarkovNode *temp = malloc (sizeof (MarkovNode));
  if (temp == NULL)
    {
      return NULL;
    }
  temp->counter_list = malloc (sizeof (NextNodeCounter));
  if (temp->counter_list == NULL)
    {
      free (temp);
      return NULL;
    }
  temp->data = (markov_chain->copy_func (data_ptr));
  temp->counter_list_len = 0;
  if (temp->data == NULL)
    {
      free (temp->counter_list);
      free (temp);
      return NULL;
    }
  temp->total_next_word = 0;
  temp->final = markov_chain->is_last (data_ptr);
  return temp;
}

MarkovChain *init_markov_chain (void_to_void
                                print_func,
                                void_to_void free_data, two_void_to_int
                                comp_func,
                                gcopy copy_func, void_to_bool
                                is_last)
{
  MarkovChain *temp = malloc (sizeof (MarkovChain));
  if (temp == NULL)
    {
      return NULL;
    }
  temp->database = malloc (sizeof (LinkedList));
  if (temp->database == NULL)
    {
      free (temp);
      return NULL;
    }
  temp->free_data = free_data;
  temp->comp_func = comp_func;
  temp->print_func = print_func;
  temp->is_last = is_last;
  temp->copy_func = copy_func;
  return temp;
}

int randomize_number (int max_value)
{
  return (rand ()) % max_value;
}
