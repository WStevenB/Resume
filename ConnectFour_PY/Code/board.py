#contains data about the connect-four board and moves made during game
#accepts a new move and updates the board
#checks if either player has won
class Board:
    
    #the board is simply a string a length 42, the first 7 characters are the bottom row, next 7 are the second row, so on
    #0 is empty, w is player 1, and b is player 2
    def __init__(self):
        self.permBoard = "000000000000000000000000000000000000000000"
        self.moveList = []

	#finds valid columns for a turn
    def generate_moves(self):
        end = len(self.permBoard) - 1
        goodList = []
        for i in range(0,7):
            ind = 6 - i
            if self.permBoard[end-ind:end-ind+1] == "0":
                goodList.append(i)
        return goodList

	#accepts a column number and updates the board with that player's token
    def make_move(self, move):
        letter = "w"
        if (len(self.moveList) + 2) % 2 == 1: letter = "b"
        for i in range(0,6):
            if self.permBoard[7*i+move : 7*i+move+1] == "0":
                self.permBoard = self.permBoard[0 : 7*i+move] + letter + self.permBoard[7*i+move+1:]
                break
        self.moveList.append(move)

	#opposite of above function
    def unmake_last_move(self):
        if len(self.moveList) > 0:
            lastMove = self.moveList.pop(len(self.moveList)-1)
            for i in reversed(range(0,6)):
                if self.permBoard[7*i+lastMove : 7*i+lastMove+1] != "0":
                    self.permBoard = self.permBoard[0 : 7*i+lastMove] + "0" + self.permBoard[7*i+lastMove+1:]
                    break
	
	#for debugging purposes
    def __str__(self):
        return self.permBoard

	#searches a given row for 4 w's or 4 b's
    def checkRows(self, row):
        for i in range(0,4):
            i = i + row * 7
            section = self.permBoard[i:i+4]
            if section.count("b") == 4 or section.count("w") == 4:
                return True
        return False

	#searches a given column for 4 w's or 4 b's
    def checkColumns(self, column):
        for i in range(0,3):
            i = i * 7 + column
            section = self.permBoard[i:i+1] + self.permBoard[i+7:i+8] + self.permBoard[i+14:i+15] + self.permBoard[i+21:i+22]
            if section.count("b") == 4 or section.count("w") == 4:
                return True
        return False

	#searches a given row at an upwards right angle for 4 w's or 4 b's
    def checkRightDiagonals(self, row):
        for i in range(0,4):
            i = i + row * 7
            section = self.permBoard[i:i+1] + self.permBoard[i+8:i+9] + self.permBoard[i+16:i+17] + self.permBoard[i+24:i+25]
            if section.count("b") == 4 or section.count("w") == 4:
                return True
        return False

	#searches a given row at an upwards left angle for 4 w's or 4 b's
    def checkLeftDiagonals(self, row):
        for i in range(0,4):
            i = 6 - i + (row * 7)
            section = self.permBoard[i:i+1] + self.permBoard[i+6:i+7] + self.permBoard[i+12:i+13] + self.permBoard[i+18:i+19]
            if section.count("b") == 4 or section.count("w") == 4:
                return True
        return False

	#checks if the previous move ended the game
	#iterates through the rows/columns of board and calls the above four functions on each
    def last_move_won(self):
        for i in range(0,6):
            if self.permBoard[7*i:7*i+7] == "0000000": break
            elif self.checkRows(i) == True:
                return True
        for i in range(0,7):
            if self.checkColumns(i) == True:
                return True
        for i in range(0,3):
            if self.permBoard[7*i:7*i+7] == "0000000": break
            else:
                if self.checkRightDiagonals(i) == True:
                    return True
                elif self.checkLeftDiagonals(i) == True:
                    return True
        return False







