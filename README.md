# Reinfrocement-Learning-with-Two-Player-Imperfect-Information-Games
Inspired by ideas taken from the Alpha (Go) Zero paper (https://deepmind.com/blog/alphago-zero-learning-scratch/), I implemented a general reinforcement learning algorithm that can learn to play two player imperfect information games. This required significant insights and new methods in addition to the core ideas (deep q learning, Monte Carlo tree search) taken from the original paper. The neural network was implemented with Keras.

The game that I was testing this algorithm on is as follows: two players make a series of simultaneous bids to decide how a bottle that is in between them moves. The game is won when one player gets the bottle, and drawn if neither gets it (more details here: https://www.hackerrank.com/contests/twitch/challenges/the-bidding-game).

Although the reinforcement learning algorithm came up with a strategy that performed well, I subsequently made a much more powerful algorithm based on game tree searching. However, because the players bet simultaneously, I could not just use a regular tree search algorithm like minimax. Instead, I made an algorithm analogous to minimax that searches the game tree until it reaches a root node (just like minimax, this could be replaced with evaluator and pruning functions for faster results), and propagates the value of the encountered state back up the game tree. However, unlike minimax states further up the game tree take the form of a zero-sum non-cooperative game, instead of a simple turn-based sequence. Thus what was needed was to find Nash equilibria for each of these states and based on that calculate the expected utility of that stable strategy, and let that be the value of that state, which could be further propagated up the tree. An iterative algorithm for finding mixed Nash equilibria was used for this. When we are trying to find what action to take, we calculate the values of all resulting states of the combined possibilities of ours and the opponent's actions based on the above algorithm, and run the iterative algorithm one final time to find a Nash equilibrium, which constitutes the strategy that we will take. However, in the online programming challenge we only have 15 seconds to make each move, and we have to start from scratch each time. The above algorithm takes about 4 minutes (when implemented in C++) to solve the game fully. So in the actual implementation a combination of different tricks (memoization, preprocessing the values of certain states, and approximating states by reducing them to smaller states given their proportions) were used to make the algorithm fast enough. The final algorithm placed 21st out of 497 submissions in the online challenge.

Here is a summary of all the files:

* main.py: The reinforcement learning approach to solving the game, implemented in Python.

* Game theoric approach.py: The game theoric approach to solving the game, implemented in Python.

* C++ Implementation.cpp: The game theoric approach implemented in C++ for faster results.

* state proportions.py: A fast approach to pre-compute a large number of the winning states by estimating the smallest proportion of the total sum of money required to win when the bottle is one spot away from the player, and reasoning recursively to find the proportions for other distances.

* find indexes.cpp: Finds the values of all states without state approximations. Not fast enough for the actual algorithm, but can be used to make a very optimal (perhaps perfect) strategy.

* Precomputed Implementation.cpp: A version of the algorithm with the values found in find indexes.cpp hardcoded into it.

* Flow Algorithm.py: An idea to compute all winning states by starting from the root nodes and working up instead of the other way around which is what was done in the original algorithm. Could be promising as computing from bottom to up is much faster, but I could not get it to work (had both accuracy and speed problems) and settled with my original implementation.




