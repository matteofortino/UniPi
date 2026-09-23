v = []
print("Inserisci 5 numeri")
for i in range(5):
    v.append(int(input(f"Numero {i + 1}: ")))

print(f"lista originale v: {v}")

print("Inizio ordinamento con bubble sort")

# bubble sort
n = len(v);
for i in range(n - 1):
    for j in range(n - 1 - i):
        if v[j] > v[j+1]:
            v[j], v[j+1] = v[j+1], v[j]
    print(f"Passagio {i + 1}: {v}")

print("Fine ordinamento")
