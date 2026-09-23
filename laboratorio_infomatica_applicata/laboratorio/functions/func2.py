def is_over(thold, *args): 
    n_over = list(filter(lambda x: x > thold, args))
    if len(n_over) == 0: 
        print("No numbe above threshold")
    return n_over

print(is_over(int(input("Insert a threshold: ")), 1, 2, 3, 4, 5, 6,))


