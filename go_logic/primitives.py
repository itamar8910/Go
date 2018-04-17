from collections import namedtuple

Position = namedtuple('Position', ['row', 'col'])
Move = namedtuple('Move', ['player', 'pos'])