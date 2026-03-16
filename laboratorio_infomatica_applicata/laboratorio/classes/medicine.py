class Patient: 
    def __init__(self, name: str, age: int, allergens: set[str]): 
        self.name = name
        self.age = age
        self.allergens = allergens

    def check(self, medicine: Medicine):
        if len(self.allergens.intersection(medicine.allergens)) != 0:
            print(f"{self.name} e' allergico al farmaco {medicine.name}")


class Medicine: 
    def __init__(self, name: str, allergens: set[str]): 
        self.name = name
        self.allergens = allergens

# Pazienti
p1 = Patient("Marco Rossi", 34, {"penicillina", "glutine"})
p2 = Patient("Luca Bianchi", 28, {"lattosio"})
p3 = Patient("Anna Verdi", 45, {"ibuprofene", "arachidi"})
p4 = Patient("Giulia Neri", 60, {"aspirina"})
p5 = Patient("Paolo Galli", 52, {"glutine", "soia"})

patients = [p1,p2,p3,p4,p5]

# Medicine
m1 = Medicine("Tachipirina", {"lattosio"})
m2 = Medicine("Brufen", {"ibuprofene"})
m3 = Medicine("Augmentin", {"penicillina"})
m4 = Medicine("Aspirinetta", {"aspirina"})
m5 = Medicine("VitaminaC", {"soia"})

medicines = [m1,m2,m3,m4,m5]

[p.check(m) for p in patients for m in medicines]

