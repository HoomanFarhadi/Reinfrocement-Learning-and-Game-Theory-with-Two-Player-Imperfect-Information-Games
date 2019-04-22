from math import ceil

def iterate():
    starting_value = 1
    p = 1 / 2.68
    iterations = 100
    ratio = 1
    pos = 4 - 1
    for i in range(iterations):
        next_value = starting_value / (1 - p)
        lst = [starting_value, next_value]
        for i in range(3):
            lst.append(lst[-1] + (lst[-1] - lst[-2]))
        if lst[pos] < ratio:
            break
        starting_value = next_value / (next_value + 1)
    return lst

l = iterate()
print(l)

Sum = 0
for i in l:
    for op in range(30,101):
        for my in range(ceil(i * op), 101):
            Sum += 1

for i in (1,2,3,4,5):
    bound = (10 - i) * 2
    for op in range(1, bound):
        for my in range(op + i, 101):
            Sum += 1
    

range(12,10)

    
