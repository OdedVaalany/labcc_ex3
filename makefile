tweets:
	gcc -Wall -Wextra -Wvla -std=c99 tweets_generator.c markov_chain.c linked_list.c -o test
	./test 1 1 justdoit_tweets.txt
snake:
	gcc -Wall -Wextra -Wvla -std=c99 snakes_and_ladders.c markov_chain.c linked_list.c -o test