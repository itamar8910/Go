
class InvalidMoveException(Exception):
    def __init__(self, message = None):
        super().__init__("Invalid Move" if not message else message)

class KoException(InvalidMoveException):
    def __init__(self):
        super().__init__("Invalid Move: KO")

class SuicideException(InvalidMoveException):
    def __init__(self):
        super().__init__("Invalid Move: Suicide")

class SpotOccupiedException(InvalidMoveException):
     def __init__(self):
        super().__init__("Invalid Move: Spot Occupied")