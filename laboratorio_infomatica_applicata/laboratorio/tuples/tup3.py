tup1 = (3,6,9)
tup2 = (1,1,1)
tup3 = (6,3,0)

sum_tup = tuple(sum(elements) for elements in zip(tup1, tup2, tup3))
print(f"sum_tup: {sum_tup}")


