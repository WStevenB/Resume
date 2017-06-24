import random
import math
import sys
import datetime as dt

#contains data about the connect four board, how much advantage each player has
#player one is doing well with positive advantage, player two with negative
#manually entered badPositions are used to avoid some common mistakes of this player
#start time is used to stay under the move time limit of 3 seconds
class Player:
    def __init__(self):
        self.permBoard = "000000000000000000000000000000000000000000"
        self.advantage = 0
        self.gameOver = False
        self.centerFull = False
        self.moveList = []
        self.startTime = ""
        self.badPositions = [["00wwb00000b000000w000000b000000w000000b000", 4], ["00wwb00000bw00000wb00000b000000w000000b000",4], ["0bwwwb000wb00000bw000000b000000w000000b000", 2], ["00wb000000w000000b000000w000000b000000w000", 2], ["bwwwb000w0bw00000wb00000b00000000000000000",1], ["bwwwb000w0b000000w000000b00000000000000000",1], ["000wbw0000bw00000bw00000w000000b0000000000",5],["wbwwb000bwbb0000bww0000wb00000ww000000b000",0],["wbwwb000bwbb000bbww000wwb0000bww00000wb000",5], ["00wwb00000bw00000wb00000bw00000w000000b000", 4], ["0bwwb0w0wwbw0000bww0000bbb0000wwb0000bbw00", 6], ["bwwwbwwwwbbwb00b0wb00000bw00000wb00000bb00",1], ["00bw000000b000000w000000b000000w0000000000",2], ["wbwwbw0bbwbb00b0bww0000wb00000ww00000wb000",5]]

	#called during gameplay to show who each player is
    def name(self):
        return 'Player_One'

	#actually updates the board given a column number of a move
	#used during runGame.py to make other player's moves
    def make_move(self, move):
        letter = "w"
        if (len(self.moveList) + 2) % 2 == 1: letter = "b"
        for i in range(0,6):
            if self.permBoard[7*i+move : 7*i+move+1] == "0":
                self.permBoard = self.permBoard[0 : 7*i+move] + letter + self.permBoard[7*i+move+1:]
                break
        self.moveList.append(move)

	#determines the best move to make
	#records current time and then avoids bad positions
	#depth defaults to 12, but may be less towards the end of the game
	#findBestMove is a recursive, depth-first search function
	#its idea is to find all attacks for both players and calculate a score for each
	#that is, if a player has three tokens in a row at a location, then their advantage is improved
	#if a player has two tokens in a row, their advantage is improved to a lesser degree
    def get_move(self):
        self.startTime = dt.datetime.now()
        for mistake in self.badPositions:
            if self.permBoard == mistake[0]: return mistake[1]
        depth = self.permBoard.count("0") - 10
        if depth > 10: depth = 10
        elif depth < 0: depth = 0
        elif depth % 2 != 0: depth -= 1
        bestMove = self.findBestMove(0, self.permBoard, depth)
        number = bestMove[1]
        return number


	#analyzes the attacks of each player in a given row
    def checkRows2(self, board, row):
        for i in range(0,4):
            i = i + row * 7
            section = board[i:i+4]
            if i != 3:
            	#checks for special case where a player has achieved "0www0" or "0bbb0"
            	#this is often an unstoppable attack
                section2 = board[i:i+5]
                if section2[:1] == "0" and section2[-1:] == "0":
                    if i-3 < 0 or (board[i-7:i-6] != "0" and board[i-3:i-2] != "0"):
                        if section2.count("b") == 2 and section2.count("w") == 0:
                            if board.count("w") != board.count("b"): self.advantage -= 100000000000
                        elif section2.count("w") == 2 and section2.count("b") == 0:
                            if board.count("w") == board.count("b"): self.advantage += 100000000000
            #finds count of tokens in a section of four spaces
            b = section.count("b")
            w = section.count("w")
            
            #this section has a w token but no b token
            #good for player one
            if section.find("b") == -1 and section.find("w") != -1:
            	#extra advantage is given if also player one's turn
            	#even further advantage if the space under the empty slot is filled, leads to win
                if w == 3 and board.count("w") == board.count("b"):
                    w += 2
                    beneath = section.find("0") + i - 7
                    if beneath < 0 or board[beneath:beneath+1] != "0":
                        w += 3
                elif w == 4:
                    self.gameOver = True
                    w += 7
                #increases advantage for player one
                self.advantage += math.pow(10, w)
                
            #this section has a b token but no w token
            #good for player two
            elif section.find("w") == -1 and section.find("b") != -1:
                #extra advantage is given if also player two's turn
            	#even further advantage if the space under the empty slot is filled, leads to win
                if b == 3 and board.count("w") != board.count("b"):
                    b += 2
                    beneath = section.find("0") + i - 7
                    if beneath < 0 or board[beneath:beneath+1] != "0": b += 3
                elif b == 4:
                    self.gameOver = True
                    b += 7
                #decreases advantage, good for player two
                self.advantage -= math.pow(10, b)


	#analyzes attacks for each player in a given column
    def checkColumns2(self, board, column):
        for i in range(0,3):
            i = i * 7 + column
            section = board[i:i+1] + board[i+7:i+8] + board[i+14:i+15] + board[i+21:i+22]
            #counts number of tokens in a board section of four
            b = section.count("b")
            w = section.count("w")
            
            #this section has a w token but no b token
            #good for player one
            if section.find("b") == -1 and section.find("w") != -1:
                if w == 3 and board.count("w") == board.count("b"): w += 5
                elif w == 4:
                    self.gameOver = True
                    w += 7
                #increases advantage for player one
                self.advantage += math.pow(10, w)
                
        	#this section has a b token but no w token
            #good for player two
            elif section.find("w") == -1 and section.find("b") != -1:
                if b == 3 and board.count("w") != board.count("b"): b += 5
                elif b == 4:
                    self.gameOver = True
                    b += 7
                #decreases advantage, good for player two
                self.advantage -= math.pow(10, b)


	#analyzes right diagonal attacks of each player given a particular row
    def checkRightDiagonals2(self, board, row):
        for i in range(0,4):
            i = i + row * 7
            section = board[i:i+1] + board[i+8:i+9] + board[i+16:i+17] + board[i+24:i+25]
            #counts number of tokens in a board section of four
            b = section.count("b")
            w = section.count("w")
            
            #this section has a w token but no b token
            #good for player one
            if section.find("b") == -1 and section.find("w") != -1:
                if w == 3 and board.count("w") == board.count("b"):
                    w += 2
                    beneath = section.find("0")*8 + i - 7
                    if beneath < 0 or board[beneath:beneath+1] != "0":
                        w += 3
                elif w == 4:
                    self.gameOver = True
                    w += 7
                #increases advantage for player one
                self.advantage += math.pow(10, w)
                
            #this section has a b token but no w token
            #good for player two
            elif section.find("w") == -1 and section.find("b") != -1:
                if b == 3 and board.count("w") != board.count("b"):
                    b += 2
                    beneath = section.find("0")*8 + i - 7
                    if beneath < 0 or board[beneath:beneath+1] != "0":
                        b += 3
                elif b == 4:
                    self.gameOver = True
                    b += 7
                #decreases advantage, good for player two
                self.advantage -= math.pow(10, b)


	#analyzes left diagonal attacks of each player given a particular row
    def checkLeftDiagonals2(self, board, row):
        for i in range(0,4):
            i = 6 - i + (row * 7)
            section = board[i:i+1] + board[i+6:i+7] + board[i+12:i+13] + board[i+18:i+19]
            #counts number of tokens in a board section of four
            b = section.count("b")
            w = section.count("w")
            
            #this section has a w token but no b token
            #good for player one
            if section.find("b") == -1 and section.find("w") != -1:
                if w == 3 and board.count("w") == board.count("b"):
                    w += 2
                    beneath = section.find("0")*8 + i - 7
                    if beneath < 0 or board[beneath:beneath+1] != "0":
                        w += 3
                elif w == 4:
                    self.gameOver = True
                    w += 7
                #increases advantage for player one
                self.advantage += math.pow(10, w)
                
            #this section has a b token but no w token
            #good for player two
            elif section.find("w") == -1 and section.find("b") != -1:
                if b == 3 and board.count("w") != board.count("b"):
                    b += 2
                    beneath = section.find("0")*8 + i - 7
                    if beneath < 0 or board[beneath:beneath+1] != "0":
                        b += 3
                elif b == 4:
                    self.gameOver = True
                    b += 7
                #decreases advantage, good for player two
                self.advantage -= math.pow(10, b)


	#iterates through rows/columns of board and calls above functions on each
	#returns total advantage - positive is good for player one, negative for player two
    def calcPosition(self, board):
        self.advantage = 0
        for i in range(0,6):
            if board[7*i:7*i+7] == "0000000": break
            else: self.checkRows2(board, i)
        for i in range(0,7):
            self.checkColumns2(board, i)
        for i in range(0,3):
            if board[7*i:7*i+7] == "0000000": break
            else:
                self.checkRightDiagonals2(board, i)
                self.checkLeftDiagonals2(board, i)
        return self.advantage


	#makes a move on a temporary board given a color of token and column to play
	#looks for first 0 in a column, if never finds one then will return na
    def makeMove(self, column, color, sweepBoard):
        for i in range(0,6):
            if sweepBoard[7*i + column:7*i + column + 1] == "0":
                sweepBoard = sweepBoard[0:7*i + column] + color + sweepBoard[7*i + column + 1:]
                return sweepBoard
        return "na"

	#finds valid moves given a temporary board
	#a temporary board contains some theoretical position to consider, not the actual gameplay
    def getLegalMoves(self, board):
        end = len(board) - 1
        goodList = []
        for i in range(0,7):
            ind = 6 - i
            if board[end-ind:end-ind+1] == "0":
                goodList.append(i)
        return goodList


	#accepts a current level of depth search, a temporary board position, and max search depth
    def findBestMove(self, sweepNum, sweepBoard, depth):
    	#for first two depths, four moves are considered
    	#afterwards, only two moves
        numNodes = 4
        if sweepNum > 1: numNodes = 2
        optimalAdvantage = 100099
        optimalMove = -1
        
        #player one's turn
        if sweepBoard.count("w") == sweepBoard.count("b"):
            prunedList = []
            goodMoves = self.getLegalMoves(sweepBoard)
            #calculates advantage for each legal move
            for m in goodMoves:
                self.gameOver = False
                tb = self.makeMove(m, "w", sweepBoard)
                adv = self.calcPosition(tb)
                
                #found a winning move
                if self.gameOver == True:
                    return [1000000000000000, m]
                    
                #takes the center column if available, and not disastrous
                if m == 3 and adv >= -100: return [1000, 3]
                
                #else will place in list by order of advantage
                if len(prunedList) == 0: prunedList.append([adv,m])
                else:
                    for ii in range(0,len(prunedList)):
                        if adv > prunedList[ii][0]:
                            prunedList.insert(ii, [adv, m])
                            break
                    else:
                        if len(prunedList) < numNodes: prunedList.append([adv, m])
                        
            #pops back of list until desired depth size from top of function
            while len(prunedList) > numNodes: prunedList.pop(len(prunedList)-1)
            
            #now the move list should contain the best and desired number of moves to consider
            for i in range(0, len(prunedList)):
            	#if short on time, will return current best move instead of continuing search
                if sweepNum == 0 and (dt.datetime.now()-self.startTime).total_seconds() > 2.3:
                    return [optimalAdvantage, optimalMove]
                    
                pm = prunedList[i]
                #makes potential move on a new temporary board
                tb = self.makeMove(pm[1], "w", sweepBoard)
                
                #continues search if current depth is less than max depth
                if sweepNum < depth:
                	
                	#CRITICAL LOC - recursive function call
                	#increases current depth level and passes new board position
                	#checks if resulting advantage is best
                    tempAdv = self.findBestMove(sweepNum + 1, tb, depth)
                    if tempAdv[0] > optimalAdvantage or optimalAdvantage == 100099:
                        optimalAdvantage = tempAdv[0]
                        optimalMove = pm[1]
                        
                #search should stop, checks if current move has greater advantage than previously considered moves
                elif pm[0] > optimalAdvantage or optimalAdvantage == 100099:
                    optimalAdvantage = pm[0]
                    optimalMove = pm[1]
                    
            #returns the best advantage and best move for this position
            return [optimalAdvantage, optimalMove]
            
        #player two's turn
        else:
            prunedList = []
            goodMoves = self.getLegalMoves(sweepBoard)
            #calculates advantage for each legal move
            for m in goodMoves:
                self.gameOver = False
                tb = self.makeMove(m, "b", sweepBoard)
                adv = self.calcPosition(tb)
                
                #found a winning move
                if self.gameOver == True:
                    return [-1000000000000000, m]
                    
                #takes the center column if available, and not disastrous
                if m == 3 and adv <= 100: return [-1000, 3]
                
                #else will place in list by order of advantage
                if len(prunedList) == 0: prunedList.append([adv,m])
                else:
                    for ii in range(0,len(prunedList)):
                        if adv < prunedList[ii][0]:
                            prunedList.insert(ii, [adv, m])
                            break
                    else:
                        if len(prunedList) < numNodes: prunedList.append([adv, m])
                        
            #pops back of list until desired depth size from top of function
            while len(prunedList) > numNodes: prunedList.pop(len(prunedList)-1)
			
			#now the move list should contain the best and desired number of moves to consider
            for i in range(0, len(prunedList)):
            	#if short on time, will return current best move instead of continuing search
                if sweepNum == 0 and (dt.datetime.now()-self.startTime).total_seconds() > 2.3:
                    return [optimalAdvantage, optimalMove]
                    
                pm = prunedList[i]
                #makes potential move on a new temporary board
                tb = self.makeMove(pm[1], "b", sweepBoard)
                
                #continues search if current depth is less than max depth
                if sweepNum < depth:
                	
                	#CRITICAL LOC - recursive function call
                	#increases current depth level and passes new board position
                	#checks if resulting advantage is best
                    tempAdv = self.findBestMove(sweepNum + 1, tb, depth)
                    if tempAdv[0] < optimalAdvantage or optimalAdvantage == 100099:
                        optimalAdvantage = tempAdv[0]
                        optimalMove = pm[1]
                        
                #search should stop, checks if current move has greater advantage than previously considered moves
                elif pm[0] < optimalAdvantage or optimalAdvantage == 100099:
                    optimalAdvantage = pm[0]
                    optimalMove = pm[1]
                    
            #returns the best advantage and best move for this position
            return [optimalAdvantage, optimalMove]




    








