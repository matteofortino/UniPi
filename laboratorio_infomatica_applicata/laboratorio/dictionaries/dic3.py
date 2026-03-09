my_set = {0.5, 0.7, "s", "c", 1, 3, True, False}
print(my_set)


my_list = list({type(el) for el in my_set})
print(my_list)

my_dict = {el : type(el) for el in my_set}
print(my_dict)



