from typing import List, Optional
from random import choice


class Playground:
    def __init__(self, rows: int, cols: int):
        self.plan = [[" " for _ in range(cols)] for _ in range(rows)]
        self.rows = rows
        self.cols = cols


def new_playground(height: int, width: int) -> Playground:
    return Playground(height, width)


def get(playground: Playground, row: int, col: int) -> str:
    return playground.plan[row][col]


def drop(playground: Playground, col: int, symbol: str) -> bool:
    for i in range(playground.rows - 1, -1, -1):
        if playground.plan[i][col] == " ":
            playground.plan[i][col] = symbol
            return True
    return False


def print_sep(size: int) -> None:
    print("   +", end="")
    for _ in range(size):
        print("---+", end="")
    print("")
    return None


def print_line(row: List[str]) -> None:
    for i in range(len(row)):
        print("| {} ".format(row[i]), end="")
    print("|")
    return None


def draw(playground: Playground) -> None:
    for i in range(playground.rows):
        print_sep(playground.cols)
        if i < 10:
            print(" {} ".format(i), end="")
        else:
            print("{} ".format(i), end="")
        print_line(playground.plan[i])
    print_sep(playground.cols)
    print("   ", end="")
    for j in range(playground.cols):
        print("  {} ".format(chr(ord("A") + j)), end="")
    print("")
    return None


def check_horizontal(playground: Playground,
                     row: int, col: int) -> bool:
    symbol = playground.plan[row][col]
    return col + 3 < playground.cols and\
        playground.plan[row][col + 1] == symbol and\
        playground.plan[row][col + 2] == symbol and\
        playground.plan[row][col + 3] == symbol


def check_vertical(playground: Playground,
                   row: int, col: int) -> bool:
    symbol = playground.plan[row][col]
    return row + 3 < playground.rows and\
        playground.plan[row + 1][col] == symbol and\
        playground.plan[row + 2][col] == symbol and\
        playground.plan[row + 3][col] == symbol


def check_diagonal(playground: Playground,
                   row: int, col: int) -> bool:
    if row + 3 >= playground.rows:
        return False
    symbol = playground.plan[row][col]
    left = col >= 3 and\
        playground.plan[row + 1][col - 1] == symbol and\
        playground.plan[row + 2][col - 2] == symbol and\
        playground.plan[row + 3][col - 3] == symbol
    right = col + 3 < playground.cols and\
        playground.plan[row + 1][col + 1] == symbol and\
        playground.plan[row + 2][col + 2] == symbol and\
        playground.plan[row + 3][col + 3] == symbol
    return left or right


def check_victory(playground: Playground,
                  row: int, col: int) -> bool:
    return check_horizontal(playground, row, col)\
        or check_vertical(playground, row, col)\
        or check_diagonal(playground, row, col)


def who_won(playground: Playground) -> Optional[str]:
    x_player = False
    o_player = False
    tie = True
    for i in range(playground.rows):
        for j in range(playground.cols):
            symbol = playground.plan[i][j]
            if symbol != " " and check_victory(playground, i, j):
                if symbol == "X":
                    x_player = True
                else:
                    o_player = True
            elif symbol == " ":
                tie = False
    if x_player and o_player:
        return "invalid"
    elif x_player:
        return "X"
    elif o_player:
        return "O"
    elif tie:
        return "tie"
    return None


def get_row(playground: Playground, col: int) -> int:
    for i in range(playground.rows - 1, -1, -1):
        if playground.plan[i][col] == " ":
            return i
    return 0


def legal_cols(playground: Playground) -> List[int]:
    result = []
    for i in range(playground.cols):
        if playground.plan[0][i] == " ":
            result.append(i)
    return result


def strategy(playground: Playground, symbol: str) -> int:
    possible_cols = legal_cols(playground)
    for col in possible_cols:
        row = get_row(playground, col)
        playground.plan[row][col] = symbol
        if who_won(playground) == symbol:
            playground.plan[row][col] = " "
            return col
        playground.plan[row][col] = " "
    return choice(possible_cols)


def wants_start() -> bool:
    answer = input("Do you want to start? (Y/N):\n")
    while answer != "Y" and answer != "N":
        answer = input("Do you want to start? (Type 'Y' if you want to start,"
                       " or type 'N' if you don't):\n")
    return answer == "Y"


def user_move(playground: Playground) -> None:
    possibles = []
    for col in legal_cols(playground):
        possibles.append(chr(ord("A") + col))
    move = input("It's your move. Type number of column"
                 " where you want to place your symbol."
                 " (possible columns are: {})\n".format(possibles))
    while move not in possibles:
        move = input("Type legal number of column where you want to place your"
                     " symbol. (possible columns are: {})\n".format(possibles))
    col_num = ord(move) - ord("A")
    drop(playground, col_num, "O")
    draw(playground)
    print("You played 'O' to the column {}.".format(move))
    return None


def computer_move(playground: Playground) -> None:
    move = strategy(playground, "X")
    drop(playground, move, "X")
    input("Type something to see opponent's move.\n")
    draw(playground)
    print("Your opponent played 'X' to the"
          " column {}.\n".format(chr(ord("A") + move)))
    return None


def game(height: int, width: int) -> None:
    playground = new_playground(height, width)
    print("This is the Connect Four game. Size of "
          "playground si {} x {}".format(height, width))
    user_starts = wants_start()
    print("Game starts")
    if not user_starts:
        computer_move(playground)
    while True:
        user_move(playground)
        if who_won(playground) is not None:
            break
        computer_move(playground)
        if who_won(playground) is not None:
            break
    result = who_won(playground)
    if result == "X":
        print("Your opponent won. Better luck next time...")
    elif result == "O":
        print("You won. CONGRATULATIONS!")
    else:
        print("No more possible moves. It's a draw.")
    return None


if __name__ == '__main__':
    pass
