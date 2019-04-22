#include <iostream>
#include <fstream>
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
    int iterations = 3000;
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

int_vector calculated(224422, 0);
double_vector cache(224422, 0);
void init() {
    int index1, index2, index3, index4 = 0;
    for (int my = 0; my <= 100; my++) {
        for (int op = 0; op <= 100; op++) {
            index1 = convert(my, op, 0, 1);
            index2 = convert(my, op, 10, 1);
            index3 = convert(my, op, 0, -1);
            index4 = convert(my, op, 10, -1);
            calculated[index1] = 1;
            cache[index1] = 1;
            calculated[index2] = 1;
            cache[index2] = -1;
            calculated[index3] = 1;
            cache[index3] = 1;
            calculated[index4] = 1;
            cache[index4] = -1;
        }
    }

    for (int di = 1; di <= 9; di++) {
        for (int my = 0; my <= 100; my++) {
            index1 = convert(my, 0, di, 1);
            index2 = convert(0, my, 10 - di, 1);
            index3 = convert(my, 0, di, -1);
            index4 = convert(0, my, 10 - di, -1);
            if (my >= di) {
                calculated[index1] = 1;
                cache[index1] = 1;
                calculated[index2] = 1;
                cache[index2] = -1;
                calculated[index3] = 1;
                cache[index3] = 1;
                calculated[index4] = 1;
                cache[index4] = -1;
            } else {
                calculated[index1] = 1;
                cache[index1] = 0;
                calculated[index2] = 1;
                cache[index2] = 0;
                calculated[index3] = 1;
                cache[index3] = 0;
                calculated[index4] = 1;
                cache[index4] = 0;
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
                    index2 = convert(op, my, 9 - index, 1);
                    index3 = convert(my, op, index + 1, -1);
                    index4 = convert(op, my, 9 - index, -1);
                    calculated[index1] = 1;
                    cache[index1] = 1;
                    calculated[index2] = 1;
                    cache[index2] = -1;
                    calculated[index3] = 1;
                    cache[index3] = 1;
                    calculated[index4] = 1;
                    cache[index4] = -1;
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
                index3 = convert(my, op, distance, -1);
                index4 = convert(op, my, 10 - distance, -1);
                calculated[index1] = 1;
                cache[index1] = 1;
                calculated[index2] = 1;
                cache[index2] = -1;
                calculated[index3] = 1;
                cache[index3] = 1;
                calculated[index4] = 1;
                cache[index4] = -1;
            }
        }
    }
}

double value(int my, int op, int di, int dr, bidding_game new_game){
    int_vector s{my, op, di, dr};
    int converted = convert(my, op, di, dr);

    double result = calculated[converted];
    if (result == 1){
        return cache[converted];
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
    calculated[converted] = 1;
    cache[converted] = result;
    return result;
}

void write_to_file() {
    ofstream MyFile;
    MyFile.open("indexes.txt");

    for (int index = 0; index < 224422; index++) {
        MyFile << cache[index] << ", ";
    }
    MyFile.close();
}

int main() {
    srand(time(0));
    cout.precision(4);
    bidding_game new_game;
    init();
    int found = 0;
    for (int my = 0; my <= 100; my++) {
        for (int op = 0; op <= 100; op++) {
            for (int di = 0; di <= 10; di++) {
                for (int dr = -1; dr < 2; dr += 2) {
                    value(my, op, di, dr, new_game);
                    found++;
                    if (found % 100 == 0) {
                        cout << found << endl;
                    }
                }
            }
        }
    }
    write_to_file();
    return 0;
}
