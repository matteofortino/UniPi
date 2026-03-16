class TypedList(list): 
    type = None
    def __init__(self, type):
        self.type = type
        super().__init__();


    def append(self, item):
        if type(item) is not self.type:
            raise TypeError
        super().append(item)


def main():
    l = TypedList(int);
    l.append("A")
    print(l)
    l.append(1)
    print(l)

main()
