from tkinter import Tk, Canvas
from go_logic.GoLogic import BoardState
from go_logic.Exceptions import InvalidMoveException
from go_logic.primitives import Position
from go_logic.sgf_parser import SGFParser

class GUI:
    def __init__(self):
        self.BOARD_SIZE = BoardState.BOARD_SIZE
        self.SQUARE_SIZE = 50
        self.STONE_RADIUS = 20
        self.state = BoardState()

        self.PADDING = 100
        self.turn = 'W'
        self.canvas_width = self.SQUARE_SIZE * self.BOARD_SIZE  + self.PADDING*2
        self.canvas_height = self.SQUARE_SIZE * self.BOARD_SIZE + self.PADDING*2
        self.top = Tk()
        self.canvas = Canvas(self.top, 
                width=self.canvas_width ,
                height=self.canvas_height)
        self.canvas.pack()
        self.canvas.bind('<Button-1>', self.mouse_click)

    def draw_state(self):
        def circle(canvas,x,y, r, color):
            id = canvas.create_oval(x-r,y-r,x+r,y+r, fill=color)
            return id
        self.canvas.delete('all')
        # draw grid
        board_size = self.SQUARE_SIZE * self.BOARD_SIZE  + self.PADDING*2
        for i in range(self.BOARD_SIZE):
            self.canvas.create_line(i * self.SQUARE_SIZE + self.PADDING, self.PADDING, i * self.SQUARE_SIZE + self.PADDING, board_size - self.PADDING - self.SQUARE_SIZE, fill='#000000', width=3)
            self.canvas.create_line(self.PADDING, i * self.SQUARE_SIZE + self.PADDING, board_size - self.PADDING - self.SQUARE_SIZE, i * self.SQUARE_SIZE + self.PADDING, fill='#000000', width=3)
        # draw stones
        for r in range(self.BOARD_SIZE):
            for c in range(self.BOARD_SIZE):
                if self.state.board[r][c] != ' ':
                    color = '#000000' if self.state.board[r][c] == 'B' else '#FFFFFF'
                    x = self.PADDING + c * self.SQUARE_SIZE
                    y = self.PADDING + r * self.SQUARE_SIZE
                    circle(self.canvas, x, y, self.STONE_RADIUS, color)

    def start(self):
        self.draw_state()
        self.top.mainloop()

    def mouse_click(self, event):
        click_row = round((event.y - self.PADDING) / self.SQUARE_SIZE)
        click_col = round((event.x - self.PADDING) / self.SQUARE_SIZE)
        try:
            self.state.move(self.turn, Position(click_row, click_col))
            self.turn = BoardState.other_player(self.turn)
            self.draw_state()
        except InvalidMoveException as e:
            print(e)
        

def run():
    parser = SGFParser('games/13/go13/2015-03-06T16:25:13.507Z_k5m7o9gtv63k.sgf')
    state = BoardState.init_from_moves(parser.moves)
    gui = GUI()
    gui.state = state
    gui.start()

