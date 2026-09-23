import time
from matplotlib import pyplot as plt
from queens_solver import solve_queens, solve_queens_mod

X = [4,6,8,10,12,14,16,18,20,22,24,26,28,30,32]
Y = [0.]*15
for i,x in enumerate(X):
    time_tot = 0
    start_time = time.time()
    solution = solve_queens(x)
    elapsed = time.time() - start_time
    time_tot = time_tot + elapsed
    Y[i] = (time_tot)*1000

Y_mod = [0.]*15
for i,x in enumerate(X):
    time_tot = 0
    start_time = time.time()
    solution = solve_queens_mod(x)
    elapsed = time.time() - start_time
    time_tot = time_tot + elapsed
    #Y[i] = round(time_tot/4, 3)
    Y_mod[i] = (time_tot)*1000

plt.plot(X,Y)
plt.plot(X,Y_mod)
plt.title("Average time of solution found")
plt.xlabel("N")
plt.ylabel("time to complete (ms)")
plt.grid(True, linestyle='--'
, alpha=0.7)
#plt.yscale('log')
#plt.legend()
plt.show()
