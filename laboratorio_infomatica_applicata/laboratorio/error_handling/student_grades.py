students_grades = {
    "Alice": 28,
    "Marco": 24,
    "Giulia": 30,
    "Luca": 22,
    "Sofia": 27,
    "Matteo": 25,
    "Chiara": 29,
    "Davide": 21
}

def get_student_grade(students_grades, name):
    try:
        return students_grades[name]
    except KeyError: 
        print("Error: student not found")

print(get_student_grade(students_grades, "Marco"))
