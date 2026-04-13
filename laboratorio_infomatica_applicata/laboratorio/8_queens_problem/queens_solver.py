"""
8-Queens Problem Solver

This module implements functions to solve the classic 8-queens problem.
"""
class QueenSolver:
    def __init__(self, n=8): 
        self.board = [-1]*n
        self.n = n

    def is_safe(self, row, col):
        for prev_row, prev_col in enumerate(self.board): 
            if prev_col == -1:
                continue
            if prev_col == col: 
                return False
            if abs(prev_row - row) == abs(prev_col - col): 
                return False 
        return True
    def solve_queen(self):
        if self.n == 1:
            return [0]
        if self.n in (2,3):
            return None
        
        cols  = [False] * self.n
        diag1 = [False] * (2 * self.n)  # row - col, offset di n per evitare indici negativi
        diag2 = [False] * (2 * self.n)  # row + col

        def count_free(row):
            '''
                Parametri: 
                    - row (int): numero della righa da processare
                Ritorno: 
                    - numero di colonne libere in quella righa
            '''
            return sum(
                1 for col in range(self.n)
                if not cols[col] and not diag1[row-col+self.n] and not diag2[row+col]
            )
        def solve(remaining_rows):
                if not remaining_rows:
                    return True

                # scegli la riga con meno colonne libere
                row = min(remaining_rows, key=count_free)
                remaining_rows.remove(row)
                for col in range(self.n):
                    d1 = row - col + self.n
                    d2 = row + col
                    if not cols[col] and not diag1[d1] and not diag2[d2]:
                        self.board[row] = col
                        cols[col] = diag1[d1] = diag2[d2] = True
                        if solve(remaining_rows):
                            return True
                        cols[col] = diag1[d1] = diag2[d2] = False
                        self.board[row] = -1
                # se non trova una soluzione re-inserisco la riga nel set
                remaining_rows.add(row)
                return False

        return self.board if solve(set(range(self.n))) else None
    
    def find_all_solutions(self):
        '''
            Fa la stessa cosa di queen_solve ma
            la ricorsione non viene fermata
        '''

        all_solution = []
        self.board = [-1] * self.n
        cols = [False] * self.n
        diag1 = [False] * (2 * self.n)
        diag2 = [False] * (2 * self.n)

        def count_free(row):
            return sum(
                1 for col in range(self.n) if not cols[col] and not diag1[row-col+self.n] and not diag2[row+col]
            )

        def solve(remaining_rows): 
            if not remaining_rows:
                all_solution.append(self.board[:])
                return
            row = min(remaining_rows, key=count_free)
            remaining_rows.remove(row)
            for col in range(self.n):
                d1 = row - col + self.n
                d2 = row + col
                if not cols[col] and not diag1[d1] and not diag2[d2]: 
                    self.board[row] = col
                    cols[col] = diag1[d1] = diag2[d2] = True
                    solve(remaining_rows)
                    cols[col] = diag1[d1] = diag2[d2] = False 
                    self.board[row] = -1
            remaining_rows.add(row)
        solve(set(range(self.n)))
        return all_solution

    def board_to_string(self):

        self.n = len(self.board)
        rows = []
        for col in self.board: 
            rows.append("." * col + "Q" + "." * (self.n - col - 1) + "\n")
        return "".join(rows)

    def count_solution(self):
        return len(self.find_all_solutions())
    
    def is_valid_solution(self):
        copied_board = self.board.copy()
        n = self.n
        for row, col in enumerate(copied_board):
            if col < 0 or col >= n:
                return False
            for prev_row in range(row):
                prev_col = copied_board[prev_row]
                if prev_col == col or abs(prev_row - row) == abs(prev_col - col):
                    return False
        return True



def is_safe(board, row, col):

    """
    Check if a queen can be placed at position (row, col) without being threatened.
    
    A queen threatens another queen if they share the same row, column, or diagonal.
    
    Parameters:
        board (list): A 1D array where board[i] represents the column position 
                     of the queen in row i
        row (int): The row to check
        col (int): The column to check
    
    Returns:
        bool: True if it's safe to place a queen at position (row, col), False otherwise
    """
    qs = QueenSolver(len(board))
    qs.board = board
    return qs.is_safe(row, col)

def solve_queens(n=8):
    """
    Solve the n-queens problem and return a solution if one exists.
    
    Parameters:
        n (int): The size of the board and number of queens to place
        
    Returns:
        list or None: A 1D array representing a solution, where solution[i] is the 
                     column position of the queen in row i, or None if no solution exists
    """
    qs = QueenSolver(n)
    return qs.solve_queen()

def find_all_solutions(n=8):
    """
    Find all solutions to the n-queens problem.
    
    Parameters:
        n (int): The size of the board and number of queens to place
        
    Returns:
        list: A list of solutions, where each solution is a 1D array where
              solution[i] is the column position of the queen in row i
    """
    # TODO: Implement this function
    qs = QueenSolver(n)
    return qs.find_all_solutions()
    

    
def board_to_string(board):
    """
    Convert a board configuration to a string representation.
    
    Parameters:
        board (list): A 1D array where board[i] represents the column position 
                     of the queen in row i
                     
    Returns:
        str: A string representation of the board with 'Q' for queens and '.' for empty squares
    """
    # TODO: Implement this function
    qs = QueenSolver(len(board))
    qs.board = board
    return qs.board_to_string()


def count_solutions(n=8):
    """
    Count the number of solutions to the n-queens problem.
    
    Parameters:
        n (int): The size of the board and number of queens to place
        
    Returns:
        int: The number of solutions
    """
    # TODO: Implement this function
    # Hint: You can reuse find_all_solutions or implement a more efficient version
    qs = QueenSolver(n)
    return qs.count_solution()
def is_valid_solution(board):
    """
    Check if a board configuration is a valid solution to the n-queens problem.
    
    Parameters:
        board (list): A 1D array where board[i] represents the column position 
                     of the queen in row i
                     
    Returns:
        bool: True if the board is a valid solution, False otherwise
    """
    qs = QueenSolver(len(board))
    qs.board = board
    return qs.is_valid_solution()

