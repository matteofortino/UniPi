def memoize(func):
    cache = {}

    def wrapper(*args):
        if args in cache:
            return cache[args]
        
        result = func(*args)
        cache[args] = result
        return result

    return wrapper
@memoize
def fibonacci(n):
    if n == 1 or n == 0:
        return n
    return fibonacci(n - 1) + fibonacci(n - 2)

@memoize
def factorial(n): 
    if n == 1:
        return 1
    return n * factorial(n - 1)


for i in range(1000):
     print(f"{i + 1}: {fibonacci(i)}")

