"""
Queens Solver Performance Demo

This script demonstrates the performance of the Queens problem solver for different board sizes.
It includes a scoring mechanism to compare and rank different implementations.
"""

import time
import platform
import sys
from queens_solver import (
    solve_queens, 
    board_to_string
)

def format_time(seconds):
    """Format time in human-readable format based on magnitude"""
    if seconds < 0.001:
        return f"{seconds*1000000:.2f} μs"
    elif seconds < 1:
        return f"{seconds*1000:.2f} ms"
    else:
        return f"{seconds:.4f} s"

def test_solution(n, verbose=True):
    """Test the olution and measure its performance"""
    start_time = time.time()
    solution = solve_queens(n)
    elapsed = time.time() - start_time
    
    if verbose:
        print(f"Board size {n}x{n}:")
        if solution:
            print(f"Solution found in {format_time(elapsed)}")
            if n <= 12:  # Only print the board for smaller sizes
                print(board_to_string(solution))
        else:
            print(f"No solution exists. Verified in {format_time(elapsed)}")
            elapsed = 60.0 # Penalty
    
    return elapsed

def calculate_performance_score(timings):
    """
    Calculate a performance score based on the timings.
    A lower score is better (faster).
    """
    # Base score is the sum of all timings
    base_score = sum(timings.values())
    
    # Add penalty for any timeout
    timeout_penalty = 10.0 * len([t for t in timings.values() if t >= 30.0])
    
    # Final score (rounded to 2 decimal places)
    score = round(base_score + timeout_penalty, 2)
    
    return score

def main():
    """Main function to run the performance tests"""
    print("=" * 70)
    print("Queens Solver Performance Test")
    print("=" * 70)
    print(f"Python version: {sys.version}")
    print(f"System: {platform.system()} {platform.release()}")
    print(f"CPU: {platform.processor()}")
    print("=" * 70)
    
    # Test finding a single solution
    print("\n\nTesting finding a single solution:")
    print("-" * 50)
    
    board_sizes = range(4, 33, 2)
    timings = {}
    
    for n in board_sizes:
        print(f"\nTesting solution for {n}x{n} board:")
        try:
            elapsed = test_solution(n)
            timings[f"solution_{n}"] = elapsed
        except Exception as e:
            print(f"Error in solution: {e}")
            timings[f"solution_{n}"] = 60.0  # Penalty
        except KeyboardInterrupt as e:
            print(f"Timeout!")
            timings[f"solution_{n}"] = 60.0  # Penalty
                
    # Calculate and display performance score
    print("\n" + "=" * 70)
    print("Performance Results")
    print("=" * 70)
    
    all_timings = {**timings}
    score = calculate_performance_score(all_timings)
    
    print("\nSummary of timings:")
    for test, timing in sorted(all_timings.items()):
        print(f"{test}: {format_time(timing)}")
    
    print("\n" + "=" * 70)
    print(f"Overall Performance Score: {score}")
    print("Lower score is better!")
    print("=" * 70)
    
    return score

if __name__ == "__main__":
    main()
