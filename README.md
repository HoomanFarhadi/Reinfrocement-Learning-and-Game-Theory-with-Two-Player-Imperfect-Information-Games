# Reinfrocement-Learning-with-Two-Player-Imperfect-Information-Games
Inspired by ideas taken from the Alpha(Go) Zero paper (https://deepmind.com/blog/alphago-zero-learning-scratch/), I implemented a general reinforcement learning algorithm that can learn to play two player imperfect information games. This required significant insights and new methods in addition to the core ideas (deep q learning, monte carlo tree search) taken from the original paper. The neural network was implemented with Keras.

The game that I was testing this algorithm on is as follows: two players make a series of simultaneous bids to decide how a bottle that is in between them moves. The game is won when one player gets the bottle, and drawn if neither gets it (more details here: https://www.hackerrank.com/contests/twitch/challenges/the-bidding-game).

Although the reinforcement learning algorithm came up with a strategy that performed well, I subsequently made a much more powerful algorithm based on game tree searching. However, because the players bet simultaneously, I could not just use a regular tree search algorithm like minimax. Instead, I made an algorithm similar to minimax bu
