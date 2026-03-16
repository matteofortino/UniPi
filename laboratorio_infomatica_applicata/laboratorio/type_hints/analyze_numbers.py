def analyze_numbers(numbers: list[float]):
    total = sum(numbers)
    average = total / len(numbers) if numbers else 0
    return {"sum": total, "average": average}
