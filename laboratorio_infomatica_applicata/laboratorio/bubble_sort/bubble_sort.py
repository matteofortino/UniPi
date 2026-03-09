def bubble_sort(v):
    n = len(v);
    for i in range(n - 1):
        for j in range(n - 1 - i):
            if v[j] > v[j+1]:
                v[j], v[j+1] = v[j+1], v[j]
        print(v)

def improved_bubble_sort(v):
    n = len(v);
    for i in range(n - 1):
        swapped = False
        for j in range(n - 1 - i):
            if v[j] > v[j+1]:
                swapped = True
                v[j], v[j+1] = v[j+1], v[j]
        print(v)
        if not swapped:
            break

def main(): 
    v = []
    print("Inserisci 5 numeri")
    for i in range(5):
        v.append(int(input(f"Numero {i + 1}: ")))

    print(f"lista originale v: {v}")
    print("Inizio ordinamento con bubble sort originale")
    bubble_sort(v.copy())
    print("Fine ordinamento")
    print("\n")
    print("Inizio ordinamento con bubble sort migliorato")
    improved_bubble_sort(v.copy())
    print("Fine ordinamento")

main()
