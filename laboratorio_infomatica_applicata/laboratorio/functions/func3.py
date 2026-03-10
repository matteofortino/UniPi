data = {
    1: 42,
    2: [1, 2, 3, 4, 5],
    3: {"a": 10, "b": 20, "c": 30},
    4: (7, 8, 9, 10),
    5: {100, 200, 300},
    6: {"numbers": [11, 22, 33], "total": 66},
    7: 10,
    8: {"x": 5, "y": 15, "z": 25},
    9: (1000, 2000, 3000),
    10: {"nested": {"k1": 1, "k2": 2, "k3": 3}},
    11: [99, 88, 77],
    12: {"values": (4, 8, 12, 16)},
    13: 2.14
}

def numerical_values_to_string(data): 
    l = list(str(n) for n in data.values() if type(n) in (int, float)) 
    return l
print(numerical_values_to_string(data))
print(isinstance(3.14, (int, float)))
