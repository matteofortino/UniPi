
tup = (1,2,3,'s',5,6,7)
idx = 3

print(f"tup: {tup}")

new_tup = tup[:idx] + (idx + 1, ) + tup[idx + 1:]

print(f"new tup: {new_tup}")
