def deprecated(func):
    def wrapper(*args, **kwargs):
        print("Attenzione! funzione deprecata")
        func(*args, **kwargs)
        return
    return wrapper

@deprecated
def hi(name):
    print(f"Ciao {name}")


hi("Davide")
