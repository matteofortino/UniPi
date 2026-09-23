tup = (1,2,3,'s',5,'b',7,'k',9,10,'d',12)

print(f"tup: {tup}")

new_tup = [num if type(num) is int else idx + 1 for idx, num in enumerate(tup)]
print(f"new tup: {new_tup}")
