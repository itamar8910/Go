import sgf
from go_logic.primitives import Position, Move

class SGFParser:
    
    def __init__(self, path):
        assert path.endswith('.sgf')

        with open(path, 'r') as f:
            self.parser = sgf.parse(f.read())
        assert len(self.parser.children) > 0
        self.game = self.parser.children[0]
        
        self.board_size = self.parse_board_size()
        self.player_black = self.parse_player('B')
        self.player_white = self.parse_player('W')
        self.moves = self.parse_moves()

    def parse_board_size(self):
        return int(self.game.root.properties['SZ'][0])
    
    def parse_player(self, player):
        assert player == 'B' or player == 'W'
        return self.game.root.properties['P{}'.format(player)][0]

    def parse_moves(self):
        def node_properties_to_move(props):
            assert len(props.keys()) == 1
            player = list(props.keys())[0]
            assert player == 'B' or player == 'W'
            move_letters = props[player][0]
            if len(move_letters) == 0:
                return None
            col = ord(move_letters[0]) - ord('a')
            row = ord(move_letters[1]) - ord('a')
            return Move(player, Position(row, col))
        # moves_raw = [move for move in self.game.nodes[1:]]
        moves = [node_properties_to_move(node.properties) for node in self.game.nodes[1:]]
        return moves
    
        

if __name__ == "__main__":
    parser = SGFParser('../games/13/go13/2015-03-06T16:25:13.507Z_k5m7o9gtv63k.sgf')
    print(parser.board_size)
    print(parser.player_black)
    print(parser.player_white)
    print(parser.moves)