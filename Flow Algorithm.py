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

def convert(my, op, di, dr):
    if dr == -1:
        dr = 0
    else:
        dr = 112211
    my *= 1111
    op *= 11
    return dr + di + my + op

#High possibility of being buggy
def old_states(state):
    won = []
    lost = []
    my, op, di, dr = state
    if di == 0 or di == 1:
        for old_my in range(my + 1, 101):
            if old_my - my > min(1, op):
                won.append([old_my, op, di + 1, dr, old_my - my])
            if op >= old_my - my and dr == -1:
                won.append([old_my, op, di + 1, 1, old_my - my])
    elif di == 10 or di == 9:
        for my_bet in range(1, my + 1):
            for op_bet in range(my_bet, 101 - op):
                if op_bet == my_bet and dr == 1:
                        lost.append([my, op + op_bet, di - 1, -1, my_bet])
                elif op_bet > my_bet:
                    lost.append([my, op + op_bet, di - 1, dr, my_bet])
    else:
        for old_my in range(my + 1, 101):
            if old_my - my > min(1, op):
                won.append([old_my, op, di + 1, dr, old_my - my])
            if op >= old_my - my and dr == -1:
                won.append([old_my, op, di + 1, 1, old_my - my])
        for my_bet in range(1, my + 1):
            for op_bet in range(my_bet, 101 - op):
                if op_bet == my_bet and dr == 1:
                        lost.append([my, op + op_bet, di - 1, -1, my_bet])
                elif op_bet > my_bet:
                    lost.append([my, op + op_bet, di - 1, dr, my_bet])
    return [won, lost]

def outcome_count(s, move):
    my, op, di, dr = s
    if dr == -1 and not move == 1:
        op +=1
    count = 1 + op - move
    return max(1, count)

def cnt():
    c = 0
    for i in game_outcomes:
        if i == 1:
            c += 1
    print(c)

game_outcomes = [-2 for i in range(224422)]
unsure_outcomes = [-1 for i in range(224422)]
won_indexes = []
won_states = []

for my in range(101):
    for op in range(101):
        for di in range(11):
            for dr in (-1,1):
                index = convert(my, op, di, dr)
                unsure_outcomes[index] = [0 for i in range(my)]

for my in range(101):
    for op in range(101):
        for dr in (-1,1):
            index1 = convert(my, op, 0, dr)
            index2 = convert(op, my, 10, -dr)
            won_indexes.append(index1)
            won_states.append([my, op, 0, dr])
            game_outcomes[index1] = 1
            game_outcomes[index2] = -1

for di in range(1, 10):
    for my in range(101):
        for dr in (-1,1):
            index1 = convert(my, 0, di, dr)
            index2 = convert(0, my, 10 - di, -dr)
            if my >= di:
                won_indexes.append(index1)
                won_states.append([my, 0, di, dr])
                game_outcomes[index1] = 1
                game_outcomes[index2] = -1
            else:
                game_outcomes[index1] = 0
                game_outcomes[index2] = 0
                
while not won_indexes == []:
    index = won_indexes[0]
    state = won_states[0]
    if state[1] == 50:
        print(state)
    won, lost = old_states(state)
    for won_state in won:
        s = won_state[:4]
        move = won_state[4]
        my, op, di, dr = s
        index = convert(my, op, di, dr)
        move_index = move - 1
        unsure_outcomes[index][move_index] += 1
        count = unsure_outcomes[index][move_index]
        if count == outcome_count(s, move):
            won_indexes.append(index)
            won_states.append(s)
            game_outcomes[index] = 1
            index2 = convert(s[1], s[0], 10 - s[2], -s[3])
            game_outcomes[index2] = -1
    for lost_state in lost:
        s = lost_state[:4]
        move = lost_state[4]
        my, op, di, dr = s
        index = convert(my, op, di, dr)
        move_index = move - 1
        unsure_outcomes[index][move_index] += 1
        count = unsure_outcomes[index][move_index]
        if count == outcome_count(s, move):
            won_indexes.append(index)
            won_states.append(s)
            game_outcomes[index] = 1
            index2 = convert(s[1], s[0], 10 - s[2], -s[3])
            game_outcomes[index2] = -1
    won_indexes = won_indexes[1:]
    won_states = won_states[1:]
    #cnt()

cnt()
print(game_outcomes[convert(50,2,1,1)])


