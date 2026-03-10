def op_calculator(op, a, b): 
    formula = f"{a}{op}{b}"

    try:
        result = eval(formula)
        return result 
    except ZeroDivisionError: 
        print("Error: cannot divide by zero")
    except TypeError: 
        print("Error: only numbers are supported")
    except NameError:
        print("Error: operation not permitted")


print(op_calculator("*", "10", "7"))


