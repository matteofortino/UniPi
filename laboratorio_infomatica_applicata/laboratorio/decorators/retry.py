def retry(n = 1):
    def decorator(func):
        def wrapper(*args, **kwargs): 
            for _ in range(n):
                try: 
                    return func(*args, **kwargs)
                except Exception: 
                    print("Eccezzione ricevuta")
        return wrapper
    return decorator

@retry(n=3)
def hello():
    raise TypeError

hello()
