words = ["hello", "python", "set", "unique"]

unique = { word for word in words if len(word) == len(set(word)) }
print(unique)
