def fibonbacci(n):
    a, b = 0, 1
    for _ in range(n):
        yield a
        a, b = b, a  + b

fib = fibonbacci(10)
for element in list(fib):
    print(element)
