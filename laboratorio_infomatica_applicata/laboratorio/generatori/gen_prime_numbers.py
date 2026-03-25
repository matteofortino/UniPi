def prime_numbers(limit):
    for i in range(1, limit, 2):
        is_prime = True
        for j in range(2, int(i**0.5)):
            if i % j == 0:
                is_prime = False
        if is_prime:
            yield i

print(list(prime_numbers(100000)))

