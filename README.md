# CFR
  ### This is an AI which uses counterfactual regret to calculate a Nash equilibrium for a simple card game.
  ##### Game Rules:
  ##### 1. Start with a deck of n cards of unique rank (no two cards are equal)
  ##### 2. Two cards are dealt to each player
  ##### 3. Both players play a card at the same time. The player who played the lower card wins half a point.
  ##### 4. Repeat step 3 with the other card.

  ##### Each game has one random step (dealing the cards) and 2 strategy steps (each player's action)
  ##### Each strategy step has two options (both players have a binary option: pick their high card or pick their low card)
  ##### Possible outcomes are -1, 0, and 1.
  
# Installation:
  ### `g++ -O3 cfr.cpp -o cfr`
  ### `./cfr [DECK_SIZE] [ITERATIONS] RANDOM`
  
  ### If you want to use default values, else include "RANDOM" to randomize the initial strategy
