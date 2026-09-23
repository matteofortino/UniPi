print("Esercizio 1")

l = [1, 2, 3]
l_inverted = []
for i in range(len(l) - 1, -1, -1):
    l_inverted.append(l[i])

print(f"lista: {l}")
print(f"lista invertita: {l_inverted}")


print("\nEsercizio 2")

l = [i for i in range(1, 11)]
# better is
# l = list(range(1, 11))
l_odd = [el for el in l if el % 2 == 1]

print(f"lista: {l}")
print(f"lista dispari: {l_odd}")

print("\nEsercizio 3")
a = [1, 9, 3, 6, 2, 5, 87, 1, 8, 1]
a_new = [i for i, num in enumerate(a) if num**2 > sum(a[:i])]

print(f"a: {a}")
print(f"a_new: {a_new}")

print("\nEsercizio 4")
A = [2,1,4,2,3,4,5,6,7,8,9,10]
B = []

for i in range(len(A)):
    s = 0; 
    s = sum([A[k] for k in range(i) if A[k] % 2 == 0])
    B.append(s)

print(f"A: {A}")
print(f"B: {B}")


print("\nEsercizio 5")
s = "python is not dead hard"
vocali = ["a", "e", "i", "o", "u"]

consonanti = [char for char in s if char not in vocali and not char.isspace()]



print(f"stringa: {s}")
print(f"consonanti: {''.join(consonanti)}")
