from keras.layers import Input, Dense
from keras.models import Model
from keras import regularizers
from keras.utils import to_categorical
import numpy as np
from collections import Counter
import math
import random
import copy
import pickle

#Hyperparameters
reg_cof = 0.0001
epochs = 500
batch_size = 32
hidden_nodes = 20
exp_cof = 1.4
iterations = 1000000
episodes = 20
simulations_train = 100
simulations_play = 500
game_count = 65
threshold = 0.55

class new_network:
    def __init__(self):
        inputs = Input(shape=(4,))

        x = Dense(hidden_nodes, activation='relu', kernel_regularizer=regularizers.l2(reg_cof))(inputs)
        x = Dense(hidden_nodes, activation='relu', kernel_regularizer=regularizers.l2(reg_cof))(x)
        policy_output = Dense(30, activation='softmax')(x)
        value_output = Dense(1, activation='tanh')(x)

        self.model = Model(inputs=inputs, outputs=[policy_output, value_output])
        self.model.compile(optimizer='adam',
                      loss=['categorical_crossentropy', 'mean_squared_error'],
                      loss_weights = [1,1],
                      metrics=['accuracy'])
        
    def train(self, input_data, policy_outputs, val_outputs):
        self.model.fit(input_data, [policy_outputs, val_outputs], epochs=epochs, batch_size=batch_size,verbose=0)
        
    def predict(self, datum):
        return self.model.predict(datum)

def save_network(network):
    with open("my_network.pkl", 'wb') as output:
        pickle.dump(network, output, pickle.HIGHEST_PROTOCOL)

def load_network():
    with open('my_network.pkl', 'rb') as input:
        return pickle.load(input)

class bidding_game:
    def __init__(self):
        self.starting_money = 30
        self.length = 7
        self.win_reward = 6
        self.draw_reward = -1
        self.loss_reward = -5
        
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
    
    def getReward(self, s):
        my_money, opponent_money, distance, draw_advantage = s
        if distance == 0:
            return self.win_reward
        if distance == self.length - 1:
            return self.loss_reward
        if my_money >= distance:
            return self.win_reward
        if opponent_money >= self.length - 1 - distance:
            return self.loss_reward
        return self.draw_reward

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

class MCTS:
    def __init__(self):
        self.visited = {}
        self.Q = {}
        self.N = {}
        self.P = {}
        
    def search(self, s, game, neural_net):
        if game.ended(s):
            return game.getReward(s)

        if not s in self.visited:
            self.visited[s] = 1
            predictions = neural_net.predict(np.array([s]))
            self.P[s] = predictions[0][0]
            self.Q[s] = np.zeros(game.starting_money)
            self.N[s] = np.zeros(game.starting_money)
            v = predictions[1][0][0]
            return v

        self.visited[s] += 1

        max_u, best_a = -float("inf"), -1
        for a in range(len(game.getActions(s))):
            u = self.Q[s][a] + exp_cof * self.P[s][a] * math.sqrt(sum(self.N[s])) / (1 + self. N[s][a])
            #u = self.Q[s][a] + self.P[s][a] * (math.sqrt(sum(self.N[s])) / (1 + self. N[s][a])) ** exp_cof
            if u > max_u:
                max_u = u
                best_a = a
                
        a = best_a
        opponent_state = (s[1], s[0], game.length - 1 - s[2], -1 * s[3])
        actions = game.getActions(opponent_state)
        polocies = normalize(neural_net.predict(np.array([opponent_state]))[0][0][:len(actions)])
        opponent_action = np.random.choice(actions, p = polocies)
        sp = game.nextState(s, a, opponent_action)
        v = self.search(sp, game, neural_net)

        self.Q[s][a] =  (self.N[s][a] * self.Q[s][a] + v) / (self.N[s][a] + 1)
        self.N[s][a] += 1
        return v

    def improved_policy(self, s):
        policy = []
        total = sum(self.N[s])
        for a in range(len(self.N[s])):
            policy.append(self.N[s][a] / total)
        return policy

def normalize(arr):
    Sum = sum(arr)
    for index in range(len(arr)):
        arr[index] /= Sum
    return arr

def contest(new_net, old_net, game_count):
    won = 0
    lost = 0
    new_game = bidding_game()
    for play_game in range(game_count):
        s = new_game.startState()
        
        while True:
            actions = new_game.getActions(s)
            polocies = normalize(new_net.predict(np.array([s]))[0][0][:len(actions)])
            my_action = np.random.choice(actions, p = polocies)
            opponent_state = (s[1], s[0], new_game.length - 1 - s[2], -1 * s[3])
            actions = new_game.getActions(opponent_state)
            polocies = normalize(old_net.predict(np.array([opponent_state]))[0][0][:len(actions)])
            opponent_action = np.random.choice(actions, p = polocies)
            s = new_game.nextState(s, my_action, opponent_action)

            if new_game.ended(s):
                if new_game.getResult(s) == 1:
                    won += 1
                elif new_game.getResult(s) == -1:
                    lost += 1
                break

    if won + lost == 0:
        return 0
    return won / (won + lost)

def policy_iteration(game, iterations, episodes, simulations,saved_model=True):
    if saved_model == True:
        neural_net = load_network()
    else:
        neural_net = new_network()

    input_examples = []
    policy_outputs = []
    value_outputs = []
    
    for iteration in range(iterations):
        print("Iteration: " + str(iteration))
        for episode in range(episodes):
            print("Episode: " + str(episode))
            i, p, v = execute_episode(game, neural_net, simulations)
            input_examples += i
            policy_outputs += p
            value_outputs += v
            
        new_net = copy.deepcopy(neural_net)
        input_samples, policy_samples, value_samples = zip(*random.sample(list(zip(input_examples, policy_outputs, value_outputs)), min(len(value_outputs), 500)))
        new_net.train(np.array(input_samples), np.array(policy_samples), np.array(value_samples))
        frac_won = contest(new_net, neural_net, game_count)

        if frac_won > threshold:
            print("Saving...")
            save_network(neural_net)
            neural_net = new_net
    
    return neural_net

#Simulate a Monte Carlo Tree until a terminal leaf is reached
def execute_episode(game, neural_net, simulations):
    states = []
    action_polocies = []
    values = []
    
    s = game.startState()
    mcts = MCTS()

    while True:
        for simulation in range(simulations):
            mcts.search(s, game, neural_net)
        states.append(list(s))
        action_polocies.append(mcts.improved_policy(s))
        values.append(None)
        a = np.random.choice(len(mcts.improved_policy(s)), p = mcts.improved_policy(s))
        opponent_state = (s[1], s[0], game.length - 1 - s[2], -1 * s[3])
        actions = game.getActions(opponent_state)
        polocies = normalize(neural_net.predict(np.array([opponent_state]))[0][0][:len(actions)])
        opponent_action = np.random.choice(actions, p = polocies)
        s = game.nextState(s, a, opponent_action)

        if game.ended(s):
            values = [game.getResult(s) for i in values]
            return [states, action_polocies, values]

#Train a model. Trained parameters are saved every time the best neural network is updated
def train(Continue=True):
    new_game = bidding_game()
    policy_iteration(new_game, iterations, episodes, simulations_train,Continue)

#Load a trained model from the save file to make predictions with
def predict_one(state, neural_net, use_mcts=True):
    new_game = bidding_game()
    if use_mcts == False:
        actions = new_game.getActions(state)
        polocies = normalize(neural_net.predict(np.array([state]))[0][0][:len(actions)])
        return np.random.choice(actions, p = polocies)
    mcts = MCTS()
    for simulation in range(simulations_play):
        mcts.search(state, new_game, neural_net)
    a = np.random.choice(len(mcts.improved_policy(state)), p = mcts.improved_policy(state))
    return a

#Play a game with a trained model
def play_game(random_model=False):
    if random_model == True:
        model = new_network()
    else:
        model = load_network()
    new_game = bidding_game()
    s = new_game.startState()
    
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
        my_action = predict_one(s, model)
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

train()
print("How many games would you like to play?")
num_games = int(input(": "))
for play in range(num_games):
    play_game()
