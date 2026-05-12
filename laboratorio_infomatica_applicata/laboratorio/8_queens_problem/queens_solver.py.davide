"""
8-Queens Problem Solver

This module implements functions to solve the classic 8-queens problem.
"""

cols_set = set()
diags_set = set()
antidiags_set = set()

def is_safe(board: list[int], row:int, col:int) -> bool:
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
    for i in range(row):
        placed_col = board[i] # controllo una colonna alla volta

        # controlla le colonne
        if placed_col == col:
            return False

        # Check diagonal conflict (difference in rows == difference in cols)
        # controlla diagonali
        if abs(placed_col - col) == abs(i - row):
            return False

    return True

def solve_queens(n:int=8) -> list[int] | None:
    """
    Solve the n-queens problem and return a solution if one exists.

    Parameters:
        n (int): The size of the board and number of queens to place

    Returns:
        list or None: A 1D array representing a solution, where solution[i] is the
                     column position of the queen in row i, or None if no solution exists
    """
    ret = _find_solutions(n,find_all=False)
    if ret == []:
        return None
    else:
        # so che ret è una lista di liste
        return ret[0]

def _find_solutions(n: int, find_all: bool) -> list[list[int]]:
    match n:
        case 0 | 2 | 3:
            return []
        case 1:
            return [[0]]

    solutions = []
    board = [-1] * n

    # array booleani
    cols_used = [False] * n
    diags_used = [False] * (2 * n - 1)
    antidiags_used = [False] * (2 * n - 1)

    col_range0 = range((n+1)//2 - 1, -1, -1) # conviene provare prima le colonne centrali: MIGLIORAMENTO NOTEVOLE

    # caching delle possibili liste di colonne per ogni valore di n
    base = list(range(n))
    liste = [base]
    for i in range(1,n):
        lista = base.copy()
        lista[0], lista[i] = lista[i], lista[0]
        liste.append(lista)

    # caching di n - 1, per non doverlo ricalcolare ogni volta
    n_meno = n - 1

    def backtrack(row: int) -> bool:
        if row == n:
            if find_all:
                solutions.append(board.copy())
                return False
            else:
                solutions.append(board)
                return True

        match row:
            case 0: # prima riga
                for col in col_range0:
                    board[row] = col
                    cols_used[col] = True
                    diags_used[col - row + n - 1] = True
                    antidiags_used[col + row] = True

                    if backtrack(row + 1):
                        return True

                    # backtrack
                    cols_used[col] = False
                    diags_used[col - row + n_meno] = False
                    antidiags_used[col + row] = False
            case _: # qualsiasi altra riga
                L = (board[row-1] + 2) % n
                for col in liste[L]:
                    if cols_used[col] or diags_used[col - row + n_meno] or antidiags_used[col + row]:
                        continue
                    else:
                        board[row] = col
                        cols_used[col] = True
                        diags_used[col - row + n_meno] = True
                        antidiags_used[col + row] = True

                        if backtrack(row + 1):
                            return True

                        # backtrack
                        cols_used[col] = False
                        diags_used[col - row + n_meno] = False
                        antidiags_used[col + row] = False
        return False

    backtrack(0)

    if find_all:
        mirrored = []
        # prima mi sono fermato per non riesplorare casi simmetrici, ora devo inserire artificialmente queste soluzioni
        for sol in solutions:
            mirror = [n-1-col for col in sol]
            if mirror not in solutions: # non è detto che mirror sia una nuova soluzione
                mirrored.append(mirror)
        solutions.extend(mirrored)

    return solutions

def find_all_solutions(n=8) -> list[list[int]]:
    """
    Find all solutions to the n-queens problem.

    Parameters:
        n (int): The size of the board and number of queens to place

    Returns:
        list: A list of solutions, where each solution is a 1D array where
              solution[i] is the column position of the queen in row i
    """
    return _find_solutions(n,find_all=True)

def board_to_string(board:list[int])->str:
    """
    Convert a board configuration to a string representation.

    Parameters:
        board (list): A 1D array where board[i] represents the column position
                     of the queen in row i

    Returns:
        str: A string representation of the board with 'Q' for queens and '.' for empty squares
    """
    stringa = ""
    n = len(board)
    for i in board: # per ogni riga
        for j in range(0,n): # per ogni colonna
            if j == i:
                stringa += "Q"
            else:
                stringa += "."
        stringa += '\n' # newline
    return stringa

def count_solutions(n=8) -> int:
    """
    Count the number of solutions to the n-queens problem.

    Parameters:
        n (int): The size of the board and number of queens to place

    Returns:
        int: The number of solutions
    """
    return len(find_all_solutions(n))

def is_valid_solution(board:list[int])->bool:
    """
    Check if a board configuration is a valid solution to the n-queens problem.

    Parameters:
        board (list): A 1D array where board[i] represents the column position
                     of the queen in row i

    Returns:
        bool: True if the board is a valid solution, False otherwise
    """
    # la condizione che ci sia una queen per riga è già soddisfatta
    # 1. controllo che la board sia > 3x3
    size = len(board)
    # -------
    match size:
        case 0:
            return False
        case 2:
            return False
        case 3:
            return False
        case 1:
            return not board[0]

    # 2. controlla che la board sia valida
    for i in board:
        if i >= size or i < 0: # ogni numero deve essere minore della dimensione della board [0:size-1]
            return False
    # 3. controlla che ci sia UNA SOLA queen per colonna
    if len(set(board)) != size: # controllo che ogni numero appaia al più una volta
        return False
    # 4. controllo le diagonali
    # O(n)
    dif_set = set()
    sum_set = set()
    for i, val in enumerate(board): # per ogni riga
        dif = val - i # questo numero può apparire una volta sola nel set delle dif
        sum = val + i # questo numero può apparire una volta sola nel set delle sum
        if dif in dif_set or sum in sum_set: # il numero è gia presente in lista
            return False
        dif_set.add(dif)
        sum_set.add(sum)
    return True
