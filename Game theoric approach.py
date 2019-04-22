from operator import add, neg
import random
import time
import signal

def signal_handler(signum, frame):
    raise Exception("Timed out!")

class bidding_game:
    def __init__(self):
        self.starting_money = 100
        self.length = 11
        
    def ended(self, s):
        my_money, opponent_money, distance, draw_advantage = s
        if my_money == 0 or opponent_money == 0 or distance == 0 or distance == self.length - 1:
            return True
        return False
    
    def startState(self):
        draw_advantage = 1
        if random.random() < 0.5:
            draw_advantage *= -1
        return (self.starting_money, self.starting_money, int((self.length - 1) / 2), draw_advantage)
    
    def nextState(self, s, my_action, opponent_action):
        my_money, opponent_money, distance, draw_advantage = s
        if my_action > opponent_action:
            my_money -= (my_action + 1)
            distance -= 1
        elif my_action < opponent_action:
            opponent_money -= (opponent_action + 1)
            distance += 1
        elif my_action == opponent_action:
            if draw_advantage == 1:
                my_money -= (my_action + 1)
                distance -= 1
                draw_advantage *= -1
            elif draw_advantage == -1:
                opponent_money -= (opponent_action + 1)
                distance += 1
                draw_advantage *= -1
        return (my_money, opponent_money, distance, draw_advantage)

    def getResult(self, s):
        my_money, opponent_money, distance, draw_advantage = s
        if distance == 0:
            return 1
        if distance == self.length - 1:
            return -1
        if my_money >= distance:
            return 1
        if opponent_money >= self.length - 1 - distance:
            return -1
        return 0
        
    def getActions(self, s):
        return [i for i in range(s[0])]

def solve(payoff_matrix, iterations=400):
    transpose = list(zip(*payoff_matrix))
    numrows = len(payoff_matrix)
    numcols = len(transpose)
    row_cum_payoff = [0] * numrows
    col_cum_payoff = [0] * numcols
    colpos = range(numcols)
    rowpos = list(map(neg, range(numrows)))
    colcnt = [0] * numcols
    rowcnt = [0] * numrows
    active = 0
    for i in range(iterations):
        rowcnt[active] += 1
        col_cum_payoff = list(map(add, payoff_matrix[active], col_cum_payoff))
        active = min(zip(col_cum_payoff, colpos))[1]
        colcnt[active] += 1
        row_cum_payoff = list(map(add, transpose[active], row_cum_payoff))
        active = -max(zip(row_cum_payoff, rowpos))[1]
    value_of_game = (max(row_cum_payoff) + min(col_cum_payoff)) / 2.0 / iterations
    return rowcnt, colcnt, value_of_game

def convert(my, op, di, dr):
    di *= 10000
    my *= 100
    op *= 1
    return di + my + op

def evaluate(my, op, di):
    Sum = 0
    if di > 5:
        return - evaluate(op, my, 10 - di)
    for d in range(di, 5):
        Sum += (0.5) ** (di - 1) * my
    my += Sum
    result = (my - op) / min(op, my)
    return max(-1, min(1, result))

cache = ["NC" for i in range(122412)]
def value(my, op, di, dr, new_game):
    s = (my, op, di, dr)
    
    if dr == -1:
        return -value(op, my, new_game.length - di - 1, 1, new_game)

    result = cache[convert(my, op, di, dr)]
    if not result == "NC":
        return result

    if new_game.ended(s):
        return new_game.getResult(s)

    values = [[0 for i in range(op)] for j in range(my)]
    for my_action in range(my):
        for op_action in range(op):
            next_s = new_game.nextState(s, my_action, op_action)
            my_money, opponent_money, distance, draw_advantage = next_s
            values[my_action][op_action] = value(my_money, opponent_money, distance, draw_advantage, new_game)

    result = solve(values)[2]
    cache[convert(my, op, di, dr)] = result
    return result

def predict(s):
    new_game = bidding_game()
    my, op, di, dr = s
    org_my = my
    org_op = op
    scl_factor = 1
    bound = 22 ** 2

    if my * op > bound:
        scl_factor = ((my * op) / bound) ** 0.5
        my = round(my / scl_factor)
        op = round(op / scl_factor)
        s = (my, op, di, dr)
    
    values = [[0 for i in range(op)] for j in range(my)]
    for my_action in range(my):
        for op_action in range(op):
            next_s = new_game.nextState(s, my_action, op_action)
            my_money, opponent_money, distance, draw_advantage = next_s
            values[my_action][op_action] = value(my_money, opponent_money, distance, draw_advantage, new_game)

    result = solve(values)[0]
    scl_factor = org_my / len(result)
    Max = float("-inf")
    pick = 0
    for index in range(len(result)):
        if result[index] > Max:
            Max = result[index]
            pick = index

    scaled = int(round((pick + 1) * scl_factor - 1))
    return min(org_my - 1, scaled)

def search_more(state, game, my_action, opponent_action, search_time):
    s1 = game.nextState(state, my_action, -1)
    s2 = game.nextState(state, opponent_action, -1)
    gap1 = abs(s1[0] - s1[1])
    gap2 = abs(s2[0] - s2[1])

    if gap1 > gap2:
        s = s1
        ns = s2
    else:
        s = s2
        ns = s1

    signal.signal(signal.SIGALRM, signal_handler)
    signal.alarm(search_time)
    try:
        predict(s)
        predict(ns)
    except Exception:
        return 0
                    
def play_game():
    new_game = bidding_game()
    s = new_game.startState()
    opponent_action = 0
    
    while True:
        computer_money, your_money, distance, draw_advantage = s
        print("Computer has: " + str(computer_money) + " coins")
        print("You have: " + str(your_money) + " coins")
        if draw_advantage == 1:
            print("Computer has the draw advantage")
        else:
            print("You have the draw advantage")
        print("P0" + (distance - 1) * " __ " + " _B_ " + (new_game.length - 2 - distance) * " __ " + "P1")
        
        actions = new_game.getActions(s)
        t = time.time()
        my_action = predict(s)
        time_taken = time.time() - t
        remaining_time = 4.95 - time_taken
        if remaining_time > 0:
            search_more(s, new_game, my_action, opponent_action, remaining_time)
        print(time.time() - t)
        opponent_action = int(input("Your bet: ")) - 1
        s = new_game.nextState(s, my_action, opponent_action)
        
        if new_game.ended(s):
            result = new_game.getResult(s)
            if result == 1:
                print("Computer has won")
            elif result == -1:
                print("You have won")
            else:
                print("The game was a draw")
            break

play_game()
#2 problems: buggy / unoptimal (although it could be because computer is counting on nash equilibrium response), too slow (probably many optimizations that could be made, if it is still too slow something like pruning and/or evaluation functions with depth may be tried, could load the calculated array so that it will be fast)
#test mixed policy against no random model
    #Submission with score 52.79 - bound of 30, 100 iterations | bound of 22, 400 iterations - pretty good, if I can make sure it doesnt loose against weaker oponents (it seeming to loose because of timeouts) it should score pretty high
#Computer is obviously not very strong if I can draw against it.
#When I bet 30 at the beginning it wins, but seems kinda wierd, like it always takes the narrowest win
#there seems to be a time limit of 15 seconds for initialization and 5 seconds for each move
#Computer is defeated by dragging it to the edge and then playing 12 6 8 16. Perhaps because of
#suboptimal corner play. Could be a strategy to play in itself. Still I think randomness should help.
#So close to fixing it, might not even be the time constraints.
#It is the 5 second time constraint for elevating it to that next level. C++ seems like instant fix. Is there a better way?
#Idea: Use all leftover time from the other predictions
