values = [4, 8, 15, 16, 23, 42]
filtered = []
# for v in values:
#     doubled = v * 2
#     if doubled > 20: 
#         filtered.append(doubled)

filtered = [d for x in values if (d := x * 2) > 20]
print(filtered)
