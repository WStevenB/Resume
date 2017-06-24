import board
import player
import random_player
import random


def test_Q4():
  players = [player.Player(), random_player.Player()]
  random.shuffle(players)
  print(players[0].name() + " vs " + players[1].name())

  b = board.Board()
  i = 2
  legal_moves = b.generate_moves()
  while (not b.last_move_won()) and len(legal_moves) > 0 and i < 45:
    move = players[i%2].get_move()
    players[0].make_move(move)
    players[1].make_move(move)
    b.make_move(move)
    i+=1
    printBoard(i, b.permBoard, move)
    legal_moves = b.generate_moves()
  if b.last_move_won():
    print("VICTORY FOR PLAYER " + players[(i-1)%2].name())
  else:
    print("DRAW")

def printBoard(count, permBoard, move):
    print("Move #" + str(count-2))
    print("Column " + str(move+1))
    print("-------")
    print(permBoard[35:42])
    print(permBoard[28:35])
    print(permBoard[21:28])
    print(permBoard[14:21])
    print(permBoard[7:14])
    print(permBoard[0:7])
    print("")
    print("")

test_Q4()



