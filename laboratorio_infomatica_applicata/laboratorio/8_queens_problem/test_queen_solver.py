import unittest
from queens_solver import (
    is_safe, 
    solve_queens, 
    find_all_solutions, 
    board_to_string, 
    count_solutions,
    is_valid_solution
)

class TestQueensSolver(unittest.TestCase):
    
    def test_is_safe(self):
        # Test an empty board
        board = []
        self.assertTrue(is_safe(board, 0, 0))
        
        # Test a safe position
        board = [1, 3, 0]  # Queens at (0,1), (1,3), (2,0)
        self.assertTrue(is_safe(board, 3, 2))
        
        # Test an unsafe position - same column
        board = [1, 3, 0]
        self.assertFalse(is_safe(board, 3, 1))
        
        # Test an unsafe position - same diagonal
        board = [1, 3, 0]
        self.assertFalse(is_safe(board, 3, 4))
    
    def test_solve_queens_4(self):
        # Test for 4x4 board - should find a solution
        solution = solve_queens(4)
        self.assertIsNotNone(solution)
        self.assertEqual(len(solution), 4)
        self.assertTrue(is_valid_solution(solution))
    
    def test_solve_queens_8(self):
        # Test for 8x8 board - should find a solution
        solution = solve_queens(8)
        self.assertIsNotNone(solution)
        self.assertEqual(len(solution), 8)
        self.assertTrue(is_valid_solution(solution))
    
    def test_solve_queens_3(self):
        # Test for 3x3 board - should not find a solution
        solution = solve_queens(3)
        self.assertIsNone(solution)
    
    def test_find_all_solutions_4(self):
        # Test for 4x4 board - should find 2 solutions
        solutions = find_all_solutions(4)
        self.assertEqual(len(solutions), 2)
        for solution in solutions:
            self.assertTrue(is_valid_solution(solution))
    
    def test_count_solutions(self):
        # Test known solution counts
        self.assertEqual(count_solutions(1), 1)
        self.assertEqual(count_solutions(4), 2)
        self.assertEqual(count_solutions(5), 10)
        self.assertEqual(count_solutions(6), 4)
    
    def test_board_to_string(self):
        # Test string representation
        board = [1, 3, 0, 2]  # 4x4 board with queens at (0,1), (1,3), (2,0), (3,2)
        expected = ".Q..\n...Q\nQ...\n..Q.\n"
        self.assertEqual(board_to_string(board), expected)
    
    def test_is_valid_solution(self):
        # Test valid solution
        self.assertTrue(is_valid_solution([1, 3, 0, 2]))
        
        # Test invalid solution - two queens in same row
        invalid_board = [1, 1, 0, 2]
        self.assertFalse(is_valid_solution(invalid_board))
        
        # Test invalid solution - two queens on same diagonal
        invalid_board = [0, 2, 3, 1]
        self.assertFalse(is_valid_solution(invalid_board))

        # Test invalid solution - a number out of the board
        invalid_board = [0, 2, 4, 1]
        self.assertFalse(is_valid_solution(invalid_board))

if __name__ == '__main__':
    unittest.main()