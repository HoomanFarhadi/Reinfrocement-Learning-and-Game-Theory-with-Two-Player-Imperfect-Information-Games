#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <vector>

//There could be problems with arrays declared inside functions
//It is often small hard to find bugs that are causing problems and making it look like the model is wrong

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
    int iterations = 200;
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

int convert(int my, int op, int di, int dr){
    di *= 10201;
    my *= 101;
    op *= 1;
    return di + my + op;
}

int_vector calculated(112211, 0);
double_vector cache(112211, 0);
void init() {
    int index1, index2 = 0;
    for (int my = 0; my <= 100; my++) {
        for (int op = 0; op <= 100; op++) {
            index1 = convert(my, op, 0, 1);
            index2 = convert(my, op, 10, 1);
            calculated[index1] = 1;
            cache[index1] = 1;
            calculated[index2] = 1;
            cache[index2] = -1;
        }
    }

    for (int di = 1; di <= 9; di++) {
        for (int my = 0; my <= 100; my++) {
            index1 = convert(my, 0, di, 1);
            index2 = convert(0, my, 10 - di, 1);
            if (my >= di) {
                calculated[index1] = 1;
                cache[index1] = 1;
                calculated[index2] = 1;
                cache[index2] = -1;
            } else {
                calculated[index1] = 1;
                cache[index1] = 0;
                calculated[index2] = 1;
                cache[index2] = 0;
            }
        }
    }

    double proportions[] = {0.3731343283582091, 0.5952380952380955, 0.8173418621179818, 1.0394456289978682, 1.2615493958777546};
    for (int index = 0; index < 5; index++) {
        for (int op = 30; op <= 100; op++) {
            int low = ceil(op * proportions[index]);
            if (low <= 100) {
                for (int my = low; my <= 100; my++) {
                    index1 = convert(my, op, index + 1, 1);
                    //cout << my << " " << op << " " << index + 1 << " " << 1 << endl;
                    index2 = convert(op, my, 9 - index, 1);
                    calculated[index1] = 1;
                    cache[index1] = 1;
                    calculated[index2] = 1;
                    cache[index2] = -1;
                }
            }
        }
    }

    int distances[] = {1, 2, 3, 4, 5};
    for (int index = 0; index < 5; index++) {
        int distance = distances[index];
        int bound = (10 - distance) * 2;
        for (int op = 1; op < bound; op++) {
            for (int my = op + distance; my <= 100; my++) {
                index1 = convert(my, op, distance, 1);
                index2 = convert(op, my, 10 - distance, 1);
                calculated[index1] = 1;
                cache[index1] = 1;
                calculated[index2] = 1;
                cache[index2] = -1;
            }
        }
    }
}

double value(int my, int op, int di, int dr, bidding_game new_game){
    int_vector s{my, op, di, dr};

    if (dr == -1) {
        return -value(op, my, new_game.length - di - 1, 1, new_game);
    }

    double result = calculated[convert(my, op, di, dr)];
    if (result == 1){
        return cache[convert(my, op, di, dr)];
    }

    if (new_game.ended(s)){
        return new_game.getResult(s);
    }

    double_vector element(op, 0);
    vector < double_vector > values(my, element);
    for (int my_action = 0; my_action < my; my_action++){
        for (int op_action = 0; op_action < op; op_action++){
            int_vector next_s = new_game.nextState(s, my_action, op_action);
            int my_money = next_s[0];
            int opponent_money = next_s[1];
            int distance = next_s[2];
            int draw_advantage = next_s[3];
            values[my_action][op_action] = value(my_money, opponent_money, distance, draw_advantage, new_game);
        }
    }
    result = solve(values)[2][0];
    //cout << my << " " << op << " " << di << " " << dr << endl;
    //cout << result << endl;
    calculated[convert(my, op, di, dr)] = 1;
    cache[convert(my, op, di, dr)] = result;
    return result;
}

int predict(int_vector s, bidding_game new_game){
    int my = s[0];
    int op = s[1];
    int di = s[2];
    int dr = s[3];
    double org_my = my;
    double org_op = op;
    double scl_factor = 1;
    //double bound = 26 ** 2
    double bound = 100;

    if (my + op > bound){
        //scl_factor = ((my * op) / bound) ** 0.5
        scl_factor = bound / (my + op);
        my = my * scl_factor;
        op = op * scl_factor;
        s = {my, op, di, dr};
    }

    double_vector element(op, 0);
    vector < double_vector > values(my, element);
    for (int my_action = 0; my_action < my; my_action++){
        for (int op_action = 0; op_action < op; op_action++){
            int_vector next_s = new_game.nextState(s, my_action, op_action);
            int my_money = next_s[0];
            int opponent_money = next_s[1];
            int distance = next_s[2];
            int draw_advantage = next_s[3];
            values[my_action][op_action] = value(my_money, opponent_money, distance, draw_advantage, new_game);
        }
    }
    //print2DArray(values);
    //cout << value(54, 0, 10, 1, new_game) << endl;
    //cout << solve(values)[2][0] << endl;
    double_vector result = solve(values)[0];
    scl_factor = org_my / result.size();
    int pick = findMax(result);
    int scaled = round((pick + 1) * scl_factor - 1);
    //cout << min((int)(org_my - 1), scaled) + 1 << endl;
    return min((int)(org_my - 1), scaled);
}

int predict_random(int_vector s, bidding_game new_game){
    int my = s[0];
    int op = s[1];
    int di = s[2];
    int dr = s[3];
    double org_my = my;
    double org_op = op;
    double scl_factor = 1;
    double bound = 200;

    if (my + op > bound){
        scl_factor = bound / (my + op);
        my = my * scl_factor;
        op = op * scl_factor;
        s = {my, op, di, dr};
    }

    double_vector element(op, 0);
    vector < double_vector > values(my, element);
    for (int my_action = 0; my_action < my; my_action++){
        for (int op_action = 0; op_action < op; op_action++){
            int_vector next_s = new_game.nextState(s, my_action, op_action);
            int my_money = next_s[0];
            int opponent_money = next_s[1];
            int distance = next_s[2];
            int draw_advantage = next_s[3];
            values[my_action][op_action] = value(my_money, opponent_money, distance, draw_advantage, new_game);
        }
    }
    double_vector result = solve(values)[0];
    scl_factor = org_my / result.size();
    int pick = findRand(result);
    int scaled = round((pick + 1) * scl_factor - 1);
    return min((int)(org_my - 1), scaled);
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
        cout << my_action << endl;
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

void pit(bidding_game new_game) {
    init();
    int game_count = 100;
    double won = 0;
    double drawn = 0;

    for (int i = 0; i < game_count; i++) {
        int_vector s = new_game.startState();
        cout << i << endl;
        while (true) {
            int computer_money = s[0];
            int your_money = s[1];
            int distance = s[2];
            int draw_advantage = s[3];

            int my_action = predict(s, new_game);
            int opponent_action = predict_random(s, new_game);
            s = new_game.nextState(s, my_action, opponent_action);

            if (new_game.ended(s)) {
                int result = new_game.getResult(s);
                if (result == 1) {
                    won++;
                } else if (result == 0) {
                    drawn++;
                }
                break;
            }
        }
    }
    cout << "Won " << 100 * (won / game_count) << "% of the games" << endl;
    cout << "Drew " << 100 * (drawn / game_count) << "% of the games" << endl;
    cout << "Lost " << 100 * ((game_count - won - drawn) / game_count) << "% of the games" << endl;
}

int main() {
    srand(time(0));
    cout.precision(4);
    bidding_game new_game;
    //pit(new_game);
    init();
    while(true){
        play_game(new_game);
    }
    return 0;
}
