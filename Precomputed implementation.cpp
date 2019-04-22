#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <vector>

using namespace std;

typedef vector<int> int_vector;
typedef vector<double> double_vector;

void printArray(double_vector arr) {
    for (int i = 0; i < arr.size(); i++) {
        cout << arr[i] << " ";
    }
    cout << endl;
}

void printintVector(int_vector arr) {
    for (int i = 0; i < arr.size(); i++) {
        cout << arr[i] << " ";
    }
    cout << endl;
}

void print2DArray(vector < double_vector > arr) {
    for (int i = 0; i < arr.size(); i++) {
        printArray(arr[i]);
    }
}

int findMin(double_vector vec) {
    int index = 0;
    double Min = vec[0];
    for (int i = 0; i < vec.size(); i++) {
        if (vec[i] < Min) {
            Min = vec[i];
            index = i;
        }
    }
    return index;
}

int findMax(double_vector vec) {
    int index = 0;
    double Max = vec[0];
    for (int i = 0; i < vec.size(); i++) {
        if (vec[i] > Max) {
            Max = vec[i];
            index = i;
        }
    }
    return index;
}

int findRand(double_vector vec) {
    for (int i = 0; i < vec.size(); i++) {
        vec[i] /= 100;
    }
    for (int i = 1; i < vec.size(); i++) {
        vec[i] += vec[i-1];
    }
    double r = ((double)rand() / RAND_MAX);
    for (int index = 0; index < vec.size(); index++) {
        if (r < vec[index]) {
            return index;
        }
    }
}

class bidding_game {
public:
    int starting_money = 100;
    int length = 11;
    bool ended(int_vector s) {
        int my_money = s[0];
        int opponent_money = s[1];
        int distance = s[2];
        int draw_advantage = s[3];
        if (my_money == 0 || opponent_money == 0 || distance == 0 || distance == length - 1) {
            return true;
        }
        return false;
    }

    int_vector startState() {
        int draw_advantage = 1;
        if (((double)rand() / RAND_MAX) < 0.5) {
            draw_advantage *= -1;
        }
        int_vector state = {starting_money, starting_money, (length - 1) / 2, draw_advantage};
        return state;
    }

    int_vector nextState(int_vector s, int my_action, int opponent_action) {
        int my_money = s[0];
        int opponent_money = s[1];
        int distance = s[2];
        int draw_advantage = s[3];
        if (my_action > opponent_action) {
            my_money -= (my_action + 1);
            distance -= 1;
        } else if (my_action < opponent_action) {
            opponent_money -= (opponent_action + 1);
            distance += 1;
        } else if (my_action == opponent_action) {
            if (draw_advantage == 1) {
                my_money -= (my_action + 1);
                distance -= 1;
                draw_advantage *= -1;
            } else if (draw_advantage == -1) {
                opponent_money -= (opponent_action + 1);
                distance += 1;
                draw_advantage *= -1;
            }
        }
        int_vector state = {my_money, opponent_money, distance, draw_advantage};
        return state;
    }

    int getResult(int_vector s) {
        int my_money = s[0];
        int opponent_money = s[1];
        int distance = s[2];
        int draw_advantage = s[3];
        if (distance == 0) {
            return 1;
        }
        if (distance == length - 1 ) {
            return -1;
        }
        if (my_money >= distance) {
            return 1;
        }
        if (opponent_money >= length - 1 - distance) {
            return -1;
        }
        return 0;
    }

    int_vector getActions(int_vector s) {
        int_vector arr(s[0]);
        for (int i = 0; i < s[0]; i++) {
            arr[i] = i;
        }
        return arr;
    }
};

vector< double_vector > zip(vector< double_vector > matrix) {
    vector< double_vector > result(matrix[0].size());
    for (int i = 0; i < matrix[0].size(); i++) {
        double_vector element(matrix.size());
        for (int j = 0; j < matrix.size(); j++) {
            element[j] = matrix[j][i];
        }
        result[i] = element;
    }
    return result;
}

vector< double_vector > solve(vector< double_vector > payoff_matrix){
    int iterations = 100000;
    vector < double_vector > transpose = zip(payoff_matrix);
    int numrows = payoff_matrix.size();
    int numcols = transpose.size();
    double_vector row_cum_payoff(numrows, 0);
    double_vector col_cum_payoff(numcols, 0);
    double_vector colpos(numcols);
    for (int i = 0; i < numcols; i++) {
        colpos[i] = i;
    }
    double_vector rowpos(numrows);
    for (int i = 0; i < numrows; i++) {
        rowpos[i] = -i;
    }
    double_vector colcnt(numcols, 0);
    double_vector rowcnt(numrows, 0);
    int active = 0;
    for (int j = 0; j < iterations; j++) {
        rowcnt[active] += 1;
        for (int i = 0; i < col_cum_payoff.size(); i++) {
            col_cum_payoff[i] = col_cum_payoff[i] + payoff_matrix[active][i];
        }
        active = colpos[findMin(col_cum_payoff)];
        colcnt[active] += 1;
        for (int i = 0; i < row_cum_payoff.size(); i++) {
            row_cum_payoff[i] = row_cum_payoff[i] + transpose[active][i];
        }
        active = -rowpos[findMax(row_cum_payoff)];
    }
    double Max = row_cum_payoff[findMax(row_cum_payoff)];
    double Min = col_cum_payoff[findMin(col_cum_payoff)];
    double value_of_game = (Max + Min) / 2.0 / iterations;
    vector< double_vector > result(3);
    result[0] = rowcnt;
    result[1] = colcnt;
    result[2] = {value_of_game};
    return result;
}

int convert(int my, int op, int di, int dr) {
    if (dr == -1) {
        dr = 0;
    } else {
        dr = 112211;
    }
    my *= 1111;
    op *= 11;
    return dr + di + my + op;
}

int predict(int_vector s, bidding_game new_game){
    int my = s[0];
    int op = s[1];
    int di = s[2];
    int dr = s[3];

    double_vector element(op, 0);
    vector < double_vector > values(my, element);
    for (int my_action = 0; my_action < my; my_action++){
        for (int op_action = 0; op_action < op; op_action++){
            int_vector next_s = new_game.nextState(s, my_action, op_action);
            int my_money = next_s[0];
            int opponent_money = next_s[1];
            int distance = next_s[2];
            int draw_advantage = next_s[3];
            values[my_action][op_action] = cache[convert(my_money, opponent_money, distance, draw_advantage)];
        }
    }
    //cout << cache[convert(100, 100, 5, 1)] << endl;
    double_vector result = solve(values)[0];
    return findMax(result);
}

void play_game(bidding_game new_game){
    int_vector s = new_game.startState();

    while (true) {
        int computer_money = s[0];
        int your_money = s[1];
        int distance = s[2];
        int draw_advantage = s[3];
        cout << "Computer has: " << computer_money << " coins" << endl;
        cout << "You have: " << your_money << " coins" << endl;
        if (draw_advantage == 1) {
            cout << "Computer has the draw advantage" << endl;
        } else {
            cout << "You have the draw advantage" << endl;
        }
        cout << "P0";
        for (int i = 0; i < (distance - 1); i++) {
            cout << " __ ";
        }
        cout << " _B_ ";
        for (int i = 0; i < (new_game.length - 2 - distance); i++) {
            cout << " __ ";
        }
        cout << "P1" << endl;

        double t = time(0);
        int my_action = predict(s, new_game);
        double time_taken = time(0) - t;
        cout << time_taken << endl;
        int opponent_action;
        cout << "Your bet: ";
        cin >> opponent_action;
        opponent_action--;
        s = new_game.nextState(s, my_action, opponent_action);

        if (new_game.ended(s)) {
            int result = new_game.getResult(s);
            if (result == 1) {
                cout << "Computer has won" << endl;
            } else if (result == -1) {
                cout << "You have won" << endl;
            } else {
                cout << "The game was a draw" << endl;
            }
            break;
        }
    }
}

int main() {
    srand(time(0));
    cout.precision(4);
    bidding_game new_game;
    while(true){
        play_game(new_game);
    }
    return 0;
}