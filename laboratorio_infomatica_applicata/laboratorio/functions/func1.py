def n_max(thold, *args): 
    if max(args) < thold: 
        print("No numbers above threshold")
    return max(args)


print(n_max(int(input("Insert a threshold: ")), 1, 2, 3, 4, 5, 6))


