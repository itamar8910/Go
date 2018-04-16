from go_logic.GoLogic import BoardState, Position


def test_liberties1():
    board = [
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 1, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
     ]
    
    state = BoardState(board)
    group, liberties = state.get_group_and_liberties(Position(4, 4))
    # print(group, liberties)
    assert len(group) == 1 and list(group)[0] == Position(4, 4) and liberties == 4

if __name__ == "__main__":
    test_liberties()