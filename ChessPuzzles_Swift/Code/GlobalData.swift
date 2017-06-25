//
//  ViewController.swift
//  Chess Puzzles
//
//  Created by PAR on 10/26/15.
//  Copyright © 2015 Babcock. All rights reserved.
//

import UIKit

//global variables, for both solving and creating puzzles
struct variables
{
    static var libList = [UIView]()
    static var squareList = [UIView]()
    static var chessList = [UIImageView]()
    static var moveList = [UIView]()
    static var middleOfMove = "0"
    static var whiteFirst = "1"
    static var boardPosition:NSString = "0000000000000000000000000000000000000000000000000000000000000000"
    static var piecesList = [UIImage]()
    static var newList = [UIView]()
    static var newPiece = "0"
    static var oldSquare = -1
    static var startingPosition = "0000000000000000000000000000000000000000000000000000000000000000"
    static var boardLocked = "0"
    static var moves = [Point]()
    static var validList = [Int]()
    static var whiteTurn = "1"
    static var goodStart = "0"
    static var okStart = "0"
    static var movingPiece = UIView()
    static var difficulty = "0"
    static var puzzleName = ""
    static var touchedBoard = "0"
}

//more global variables, for use in the whole app
struct myVar
{
    static var errors = 0
    static var loadedOnce = "0"
    static var signedIn = "0"
    static var email = ""
    static var username = ""
    static var goodToGo = "1"
    static var libraryList = [Int]()
    static var puzzleList = [Puzzle]()
    static var masterPuzzleList = [Puzzle]()
    static var divPuzzleLists = [PuzzleList]()
    static var statList = [Stat]()
    static var leaderData = ""
    static var selectedPuzzle = 0
    static var selectedLibrary = 0
    static var currentMove = 0
    static var requestList = [String]();
    static var seenVoteInstructions = "0"
    static var hasCreatedPuzzle = "0"
}

//save important data to NSDefaults
//simple variables directly written and complex data types use keyed archiver
//called when app goes into background
func saveVariables()
{
    let defaults = UserDefaults.standard
    defaults.setValue(myVar.libraryList, forKey: "libraryList")
    defaults.setValue(myVar.hasCreatedPuzzle, forKey: "hasCreatedPuzzle")
    defaults.setValue(myVar.signedIn, forKey: "signedIn")
    defaults.setValue(myVar.loadedOnce, forKey: "loadedOnce")
    defaults.setValue(myVar.email, forKey: "email")
    defaults.setValue(myVar.username, forKey: "username")
    defaults.setValue(myVar.seenVoteInstructions, forKey: "vote")
    defaults.setValue(myVar.selectedLibrary, forKey: "selectedLib")
    for i in 0..<myVar.divPuzzleLists.count {
        myVar.divPuzzleLists[i].setData()
    }
    let saveData = NSKeyedArchiver.archivedData(withRootObject: myVar.masterPuzzleList)
    UserDefaults.standard.set(saveData, forKey: "masterPuzzleList")
}

//older version of above function
func saveVariables2()
{
    let defaults = UserDefaults.standard
    defaults.setValue(myVar.libraryList, forKey: "libraryList")
    defaults.setValue(myVar.hasCreatedPuzzle, forKey: "hasCreatedPuzzle")
    defaults.setValue(myVar.signedIn, forKey: "signedIn")
    defaults.setValue(myVar.loadedOnce, forKey: "loadedOnce")
    defaults.setValue(myVar.email, forKey: "email")
    defaults.setValue(myVar.username, forKey: "username")
    defaults.setValue(myVar.seenVoteInstructions, forKey: "vote")
    defaults.setValue(myVar.selectedLibrary, forKey: "selectedLib")
    let saveData = NSKeyedArchiver.archivedData(withRootObject: myVar.masterPuzzleList)
    UserDefaults.standard.set(saveData, forKey: "masterPuzzleList")
}

//retrieves data from NSDefaults in applicationDidFinishLaunching()
func loadVariables()
{
    let defaults = UserDefaults.standard
    let em = defaults.value(forKey: "signedIn") as? String
    if(em != nil)
    {
        myVar.signedIn=em!
    }
    let hc = defaults.value(forKey: "hasCreatedPuzzle") as? String
    if(hc != nil)
    {
        myVar.hasCreatedPuzzle=hc!
    }
    let un = defaults.value(forKey: "username") as? String
    if(un != nil)
    {
        myVar.username=un!
    }
    let email = defaults.value(forKey: "email") as? String
    if(email != nil)
    {
        myVar.email=email!
    }
    let lo = defaults.value(forKey: "loadedOnce") as? String
    if(lo != nil)
    {
        myVar.loadedOnce=lo!
    }
    let ll = defaults.value(forKey: "libraryList") as? [Int]
    if(ll != nil)
    {
        myVar.libraryList=ll!
    }
    let v = defaults.value(forKey: "vote") as? String
    if(v != nil)
    {
        myVar.seenVoteInstructions=v!
    }
    let sl = defaults.value(forKey: "selectedLib") as? Int
    if(sl != nil)
    {
        myVar.selectedLibrary=sl!
    }
    if(defaults.object(forKey: "masterPuzzleList") != nil)
    {
        let data = defaults.data(forKey: "masterPuzzleList")!
        myVar.masterPuzzleList = NSKeyedUnarchiver.unarchiveObject(with: data) as! [Puzzle]
        var tempList = [Puzzle]()
        myVar.divPuzzleLists = [PuzzleList]()
        for i in 0..<myVar.masterPuzzleList.count {
            tempList.append(myVar.masterPuzzleList[i])
            if((i+1)%100==0) {
                myVar.divPuzzleLists.append(PuzzleList(p: tempList))
                tempList = [Puzzle]()
            }
        }
        if(tempList.count > 0) {
            myVar.divPuzzleLists.append(PuzzleList(p: tempList))
        }
    }
}







/* ______________________________________________________________________________________________________
______________________________________________________________________________________________________
______________________________________________________________________________________________________
______________________________________________________________________________________________________
______________________________________________________________________________________________________*/


//improved syntax
func getIndex(_ text: String,_ num:Int) -> String.Index {
        return text.index(text.startIndex, offsetBy: num)
}

//updates board with new move
func switchBoard(_ index:Int, letter:String)
{
    if(index < 63)
    {
        variables.boardPosition = variables.boardPosition.substring(to: index) + letter + variables.boardPosition.substring(from: index+1) as NSString
    }
    else
    {
        variables.boardPosition = ((variables.boardPosition as String).substring(to: getIndex(variables.boardPosition as String, index)) + letter) as NSString
    }
}

//verfies two kings are present when creating new puzzle
func checkKings() -> String
{
    var final = "1"
    if(variables.newPiece == "k" && variables.boardPosition.range(of: "k").toRange() != nil)
    {
        final = "0"
    }
    else if(variables.newPiece == "K" && variables.boardPosition.range(of: "K").toRange() != nil)
    {
        final = "0"
    }
    if(variables.touchedBoard == "1")
    {
        final = "1"
    }
    return final
}


//checks for pawn promotion, auto-queen is assumed
//room for improvement - let player choose piece
func checkQueens()
{
    if(variables.whiteFirst == "1")
    {
        for i in 0 ..< 8
        {
            if(variables.boardPosition.character(at: i) == 80)
            {
                variables.boardPosition = variables.boardPosition.substring(to: i) + "Q" + variables.boardPosition.substring(from: i+1) as NSString
            }
        }
        for i in 56 ..< 63
        {
            if(variables.boardPosition.character(at: i) == 112)
            {
                variables.boardPosition = variables.boardPosition.substring(to: i) + "q" + variables.boardPosition.substring(from: i+1) as NSString
            }
        }
        if(variables.boardPosition.character(at: 63) == 112)
        {
            variables.boardPosition = ((variables.boardPosition as String).substring(to: getIndex(variables.boardPosition as String, 63)) + "q") as NSString
        }
    }
    else
    {
        for i in 0 ..< 8
        {
            if(variables.boardPosition.character(at: i) == 112)
            {
                variables.boardPosition = variables.boardPosition.substring(to: i) + "q" + variables.boardPosition.substring(from: i+1) as NSString
            }
        }
        for i in 56 ..< 63
        {
            if(variables.boardPosition.character(at: i) == 80)
            {
                variables.boardPosition = variables.boardPosition.substring(to: i) + "Q" + variables.boardPosition.substring(from: i+1) as NSString
            }
        }
        if(variables.boardPosition.character(at: 63) == 80)
        {
            variables.boardPosition = ((variables.boardPosition as String).substring(to: getIndex(variables.boardPosition as String, 63)) + "Q") as NSString
        }
    }
}

//matches each letter to index of matching image in variables.piecesList
func letterForSquare(_ id:Int) -> String
{
    var final = "0"
    if(id == 0)
    {
        final = "p"
    }
    else if(id == 1)
    {
        final = "n"
    }
    else if(id == 2)
    {
        final = "b"
    }
    else if(id == 3)
    {
        final = "r"
    }
    else if(id == 4)
    {
        final = "q"
    }
    else if(id == 5)
    {
        final = "k"
    }
    else if(id == 6)
    {
        final = "P"
    }
    else if(id == 7)
    {
        final = "N"
    }
    else if(id == 8)
    {
        final = "B"
    }
    else if(id == 9)
    {
        final = "R"
    }
    else if(id == 10)
    {
        final = "Q"
    }
    else if(id == 11)
    {
        final = "K"
    }
    return final
}

//does opposite of above function
func numberForLetter(_ id:String) -> Int
{
    var final = 0
    if(id == "p")
    {
        final = 0
    }
    else if(id == "n")
    {
        final = 1
    }
    else if(id == "b")
    {
        final = 2
    }
    else if(id == "r")
    {
        final = 3
    }
    else if(id == "q")
    {
        final = 4
    }
    else if(id == "k")
    {
        final = 5
    }
    else if(id == "P")
    {
        final = 6
    }
    else if(id == "N")
    {
        final = 7
    }
    else if(id == "B")
    {
        final = 8
    }
    else if(id == "R")
    {
        final = 9
    }
    else if(id == "Q")
    {
        final = 10
    }
    else if(id == "K")
    {
        final = 11
    }
    return final
}


//removes border showing where user has touched a chess square
func clearBorders()
{
    for i in 0 ..< variables.squareList.count
    {
        variables.moveList[i].layer.sublayers?.first?.removeFromSuperlayer()
    }
}

//sets images for chess pieces onto board
func setBoard()
{
    for i in 0 ..< variables.chessList.count
    {
        if(variables.boardPosition.character(at: i) == 48) //empty
        {
            variables.chessList[i].image = nil
        }
        else if(variables.boardPosition.character(at: i) == 112) //white pawn
        {
            variables.chessList[i].image = variables.piecesList[0]
        }
        else if(variables.boardPosition.character(at: i) == 110) //white knight
        {
            variables.chessList[i].image = variables.piecesList[1]
        }
        else if(variables.boardPosition.character(at: i) == 98) //white bishop
        {
            variables.chessList[i].image = variables.piecesList[2]
        }
        else if(variables.boardPosition.character(at: i) == 114) //white rook
        {
            variables.chessList[i].image = variables.piecesList[3]
        }
        else if(variables.boardPosition.character(at: i) == 113) //white queen
        {
            variables.chessList[i].image = variables.piecesList[4]
        }
        else if(variables.boardPosition.character(at: i) == 107) //white king
        {
            variables.chessList[i].image = variables.piecesList[5]
        }
            
        else if(variables.boardPosition.character(at: i) == 80) //black pawn
        {
            variables.chessList[i].image = variables.piecesList[6]
        }
        else if(variables.boardPosition.character(at: i) == 78) //black knight
        {
            variables.chessList[i].image = variables.piecesList[7]
        }
        else if(variables.boardPosition.character(at: i) == 66) //black bishop
        {
            variables.chessList[i].image = variables.piecesList[8]
        }
        else if(variables.boardPosition.character(at: i) == 82) //black rook
        {
            variables.chessList[i].image = variables.piecesList[9]
        }
        else if(variables.boardPosition.character(at: i) == 81) //black queen
        {
            variables.chessList[i].image = variables.piecesList[10]
        }
        else if(variables.boardPosition.character(at: i) == 75) //black king
        {
            variables.chessList[i].image = variables.piecesList[11]
        }
    }
}


//returns a list of valid moves for a given piece on the board
func getValidMoves(_ square:Int, piece:Int) -> [Int]
{
    var goodList = [Int]()
    let coor = Point(xp: square % 8, yp: Int(square / 8))
    switch piece
    {
    case 112: //white pawn
        var sq = -1
        if(variables.whiteFirst == "1")
        {
            if(coor.y + 1 < 8)
            {
                sq = (coor.x) + ((coor.y + 1) * 8)
                if(Int(variables.boardPosition.character(at: sq)) ==  48)
                {
                    goodList.append(sq)
                    if(coor.y == 1)
                    {
                        sq = (coor.x) + ((coor.y + 2) * 8)
                        if(Int(variables.boardPosition.character(at: sq)) ==  48)
                        {
                            goodList.append(sq)
                        }
                    }
                }
            }
            if(coor.x + 1 < 8 && coor.y + 1 < 8)
            {
                sq = (coor.x + 1) + ((coor.y + 1) * 8)
                if(Int(variables.boardPosition.character(at: sq)) < 92 && Int(variables.boardPosition.character(at: sq)) !=  48)
                {
                    goodList.append(sq)
                }
            }
            if(coor.x - 1 >= 0 && coor.y + 1 < 8)
            {
                sq = (coor.x - 1) + ((coor.y + 1) * 8)
                if(Int(variables.boardPosition.character(at: sq)) < 92 && Int(variables.boardPosition.character(at: sq)) !=  48)
                {
                    goodList.append(sq)
                }
            }
        }
        else
        {
            if(coor.y - 1 >= 0)
            {
                sq = (coor.x) + ((coor.y - 1) * 8)
                if(Int(variables.boardPosition.character(at: sq)) ==  48)
                {
                    goodList.append(sq)
                    if(coor.y == 6)
                    {
                        sq = (coor.x) + ((coor.y - 2) * 8)
                        if(Int(variables.boardPosition.character(at: sq)) ==  48)
                        {
                            goodList.append(sq)
                        }
                    }
                }
            }
            if(coor.x + 1 < 8 && coor.y - 1 >= 0)
            {
                sq = (coor.x + 1) + ((coor.y - 1) * 8)
                if(Int(variables.boardPosition.character(at: sq)) < 92 && Int(variables.boardPosition.character(at: sq)) !=  48)
                {
                    goodList.append(sq)
                }
            }
            if(coor.x - 1 >= 0 && coor.y - 1 >= 0)
            {
                sq = (coor.x - 1) + ((coor.y - 1) * 8)
                if(Int(variables.boardPosition.character(at: sq)) < 92 && Int(variables.boardPosition.character(at: sq)) !=  48)
                {
                    goodList.append(sq)
                }
            }
        }
        break
    case 80: //black pawn
        var sq = -1
        if(variables.whiteFirst != "1")
        {
            if(coor.y + 1 < 8)
            {
                sq = (coor.x) + ((coor.y + 1) * 8)
                if(Int(variables.boardPosition.character(at: sq)) ==  48)
                {
                    goodList.append(sq)
                    if(coor.y == 1)
                    {
                        sq = (coor.x) + ((coor.y + 2) * 8)
                        if(Int(variables.boardPosition.character(at: sq)) ==  48)
                        {
                            goodList.append(sq)
                        }
                    }
                }
            }
            if(coor.x + 1 < 8 && coor.y + 1 < 8)
            {
                sq = (coor.x + 1) + ((coor.y + 1) * 8)
                if(Int(variables.boardPosition.character(at: sq)) > 92)
                {
                    goodList.append(sq)
                }
            }
            if(coor.x - 1 >= 0 && coor.y + 1 < 8)
            {
                sq = (coor.x - 1) + ((coor.y + 1) * 8)
                if(Int(variables.boardPosition.character(at: sq)) > 92)
                {
                    goodList.append(sq)
                }
            }
        }
        else
        {
            if(coor.y - 1 >= 0)
            {
                sq = (coor.x) + ((coor.y - 1) * 8)
                if(Int(variables.boardPosition.character(at: sq)) ==  48)
                {
                    goodList.append(sq)
                    if(coor.y == 6)
                    {
                        sq = (coor.x) + ((coor.y - 2) * 8)
                        if(Int(variables.boardPosition.character(at: sq)) ==  48)
                        {
                            goodList.append(sq)
                        }
                    }
                }
            }
            if(coor.x + 1 < 8 && coor.y - 1 >= 0)
            {
                sq = (coor.x + 1) + ((coor.y - 1) * 8)
                if(Int(variables.boardPosition.character(at: sq)) > 92)
                {
                    goodList.append(sq)
                }
            }
            if(coor.x - 1 >= 0 && coor.y - 1 >= 0)
            {
                sq = (coor.x - 1) + ((coor.y - 1) * 8)
                if(Int(variables.boardPosition.character(at: sq)) > 92)
                {
                    goodList.append(sq)
                }
            }
        }
        
        break
    case 98: //white bishop
        for i in 1 ..< 8
        {
            var sq = -1
            if(coor.x + i < 8 && coor.y + i < 8)
            {
                sq = (coor.x + i) + ((coor.y + i) * 8)
            }
            else
            {
                break
            }
            if(Int(variables.boardPosition.character(at: sq)) ==  48)
            {
                goodList.append(sq)
            }
            else if(Int(variables.boardPosition.character(at: sq)) < 92)
            {
                goodList.append(sq)
                break
            }
            else
            {
                break
            }
        }
        for i in 1 ..< 8
        {
            var sq = -1
            if(coor.x + i < 8 && coor.y - i >= 0)
            {
                sq = (coor.x + i) + ((coor.y - i) * 8)
            }
            else
            {
                break
            }
            if(Int(variables.boardPosition.character(at: sq)) ==  48)
            {
                goodList.append(sq)
            }
            else if(Int(variables.boardPosition.character(at: sq)) < 92)
            {
                goodList.append(sq)
                break
            }
            else
            {
                break
            }
        }
        for i in 1 ..< 8
        {
            var sq = -1
            if(coor.x - i >= 0 && coor.y + i < 8)
            {
                sq = (coor.x - i) + ((coor.y + i) * 8)
            }
            else
            {
                break
            }
            if(Int(variables.boardPosition.character(at: sq)) ==  48)
            {
                goodList.append(sq)
            }
            else if(Int(variables.boardPosition.character(at: sq)) < 92)
            {
                goodList.append(sq)
                break
            }
            else
            {
                break
            }
        }
        for i in 1 ..< 8
        {
            var sq = -1
            if(coor.x - i >= 0 && coor.y - i >= 0)
            {
                sq = (coor.x - i) + ((coor.y - i) * 8)
            }
            else
            {
                break
            }
            if(Int(variables.boardPosition.character(at: sq)) ==  48)
            {
                goodList.append(sq)
            }
            else if(Int(variables.boardPosition.character(at: sq)) < 92)
            {
                goodList.append(sq)
                break
            }
            else
            {
                break
            }
        }
        break
    case 66: //black bishop
        for i in 1 ..< 8
        {
            var sq = -1
            if(coor.x + i < 8 && coor.y + i < 8)
            {
                sq = (coor.x + i) + ((coor.y + i) * 8)
            }
            else
            {
                break
            }
            if(Int(variables.boardPosition.character(at: sq)) ==  48)
            {
                goodList.append(sq)
            }
            else if(Int(variables.boardPosition.character(at: sq)) > 92)
            {
                goodList.append(sq)
                break
            }
            else
            {
                break
            }
        }
        for i in 1 ..< 8
        {
            var sq = -1
            if(coor.x + i < 8 && coor.y - i >= 0)
            {
                sq = (coor.x + i) + ((coor.y - i) * 8)
            }
            else
            {
                break
            }
            if(Int(variables.boardPosition.character(at: sq)) ==  48)
            {
                goodList.append(sq)
            }
            else if(Int(variables.boardPosition.character(at: sq)) > 92)
            {
                goodList.append(sq)
                break
            }
            else
            {
                break
            }
        }
        for i in 1 ..< 8
        {
            var sq = -1
            if(coor.x - i >= 0 && coor.y + i < 8)
            {
                sq = (coor.x - i) + ((coor.y + i) * 8)
            }
            else
            {
                break
            }
            if(Int(variables.boardPosition.character(at: sq)) ==  48)
            {
                goodList.append(sq)
            }
            else if(Int(variables.boardPosition.character(at: sq)) > 92)
            {
                goodList.append(sq)
                break
            }
            else
            {
                break
            }
        }
        for i in 1 ..< 8
        {
            var sq = -1
            if(coor.x - i >= 0 && coor.y - i >= 0)
            {
                sq = (coor.x - i) + ((coor.y - i) * 8)
            }
            else
            {
                break
            }
            if(Int(variables.boardPosition.character(at: sq)) ==  48)
            {
                goodList.append(sq)
            }
            else if(Int(variables.boardPosition.character(at: sq)) > 92)
            {
                goodList.append(sq)
                break
            }
            else
            {
                break
            }
        }
        break
    case 110: //white knight
        var sq = -1
        if(coor.x + 1 < 8 && coor.y + 2 < 8)
        {
            sq = (coor.x + 1) + ((coor.y + 2) * 8)
            if(Int(variables.boardPosition.character(at: sq)) ==  48 || Int(variables.boardPosition.character(at: sq)) < 92)
            {
                goodList.append(sq)
            }
        }
        if(coor.x + 2 < 8 && coor.y - 1 >= 0)
        {
            sq = (coor.x + 2) + ((coor.y - 1) * 8)
            if(Int(variables.boardPosition.character(at: sq)) ==  48 || Int(variables.boardPosition.character(at: sq)) < 92)
            {
                goodList.append(sq)
            }
        }
        if(coor.x - 1 >= 0 && coor.y + 2 < 8)
        {
            sq = (coor.x - 1) + ((coor.y + 2) * 8)
            if(Int(variables.boardPosition.character(at: sq)) ==  48 || Int(variables.boardPosition.character(at: sq)) < 92)
            {
                goodList.append(sq)
            }
        }
        if(coor.x - 2 >= 0 && coor.y - 1 >= 0)
        {
            sq = (coor.x - 2) + ((coor.y - 1) * 8)
            if(Int(variables.boardPosition.character(at: sq)) ==  48 || Int(variables.boardPosition.character(at: sq)) < 92)
            {
                goodList.append(sq)
            }
        }
        if(coor.x + 2 < 8 && coor.y + 1 < 8)
        {
            sq = (coor.x + 2) + ((coor.y + 1) * 8)
            if(Int(variables.boardPosition.character(at: sq)) ==  48 || Int(variables.boardPosition.character(at: sq)) < 92)
            {
                goodList.append(sq)
            }
        }
        if(coor.x + 1 < 8 && coor.y - 2 >= 0)
        {
            sq = (coor.x + 1) + ((coor.y - 2) * 8)
            if(Int(variables.boardPosition.character(at: sq)) ==  48 || Int(variables.boardPosition.character(at: sq)) < 92)
            {
                goodList.append(sq)
            }
        }
        if(coor.x - 2 >= 0 && coor.y + 1 < 8)
        {
            sq = (coor.x - 2) + ((coor.y + 1) * 8)
            if(Int(variables.boardPosition.character(at: sq)) ==  48 || Int(variables.boardPosition.character(at: sq)) < 92)
            {
                goodList.append(sq)
            }
        }
        if(coor.x - 1 >= 0 && coor.y - 2 >= 0)
        {
            sq = (coor.x - 1) + ((coor.y - 2) * 8)
            if(Int(variables.boardPosition.character(at: sq)) ==  48 || Int(variables.boardPosition.character(at: sq)) < 92)
            {
                goodList.append(sq)
            }
        }
        break
    case 78: //black knight
        var sq = -1
        if(coor.x + 1 < 8 && coor.y + 2 < 8)
        {
            sq = (coor.x + 1) + ((coor.y + 2) * 8)
            if(Int(variables.boardPosition.character(at: sq)) ==  48 || Int(variables.boardPosition.character(at: sq)) > 92)
            {
                goodList.append(sq)
            }
        }
        if(coor.x + 2 < 8 && coor.y - 1 >= 0)
        {
            sq = (coor.x + 2) + ((coor.y - 1) * 8)
            if(Int(variables.boardPosition.character(at: sq)) ==  48 || Int(variables.boardPosition.character(at: sq)) > 92)
            {
                goodList.append(sq)
            }
        }
        if(coor.x - 1 >= 0 && coor.y + 2 < 8)
        {
            sq = (coor.x - 1) + ((coor.y + 2) * 8)
            if(Int(variables.boardPosition.character(at: sq)) ==  48 || Int(variables.boardPosition.character(at: sq)) > 92)
            {
                goodList.append(sq)
            }
        }
        if(coor.x - 2 >= 0 && coor.y - 1 >= 0)
        {
            sq = (coor.x - 2) + ((coor.y - 1) * 8)
            if(Int(variables.boardPosition.character(at: sq)) ==  48 || Int(variables.boardPosition.character(at: sq)) > 92)
            {
                goodList.append(sq)
            }
        }
        if(coor.x + 2 < 8 && coor.y + 1 < 8)
        {
            sq = (coor.x + 2) + ((coor.y + 1) * 8)
            if(Int(variables.boardPosition.character(at: sq)) ==  48 || Int(variables.boardPosition.character(at: sq)) > 92)
            {
                goodList.append(sq)
            }
        }
        if(coor.x + 1 < 8 && coor.y - 2 >= 0)
        {
            sq = (coor.x + 1) + ((coor.y - 2) * 8)
            if(Int(variables.boardPosition.character(at: sq)) ==  48 || Int(variables.boardPosition.character(at: sq)) > 92)
            {
                goodList.append(sq)
            }
        }
        if(coor.x - 2 >= 0 && coor.y + 1 < 8)
        {
            sq = (coor.x - 2) + ((coor.y + 1) * 8)
            if(Int(variables.boardPosition.character(at: sq)) ==  48 || Int(variables.boardPosition.character(at: sq)) > 92)
            {
                goodList.append(sq)
            }
        }
        if(coor.x - 1 >= 0 && coor.y - 2 >= 0)
        {
            sq = (coor.x - 1) + ((coor.y - 2) * 8)
            if(Int(variables.boardPosition.character(at: sq)) ==  48 || Int(variables.boardPosition.character(at: sq)) > 92)
            {
                goodList.append(sq)
            }
        }
        break
    case 114: //white rook
        for i in 1 ..< 8
        {
            var sq = -1
            if(coor.x + i < 8)
            {
                sq = (coor.x + i) + (coor.y * 8)
            }
            else
            {
                break
            }
            if(Int(variables.boardPosition.character(at: sq)) ==  48)
            {
                goodList.append(sq)
            }
            else if(Int(variables.boardPosition.character(at: sq)) < 92)
            {
                goodList.append(sq)
                break
            }
            else
            {
                break
            }
        }
        
        for i in 1 ..< 8
        {
            var sq = -1
            if(coor.x - i >= 0)
            {
                sq = (coor.x - i) + (coor.y * 8)
            }
            else
            {
                break
            }
            if(Int(variables.boardPosition.character(at: sq)) ==  48)
            {
                goodList.append(sq)
            }
            else if(Int(variables.boardPosition.character(at: sq)) < 92)
            {
                goodList.append(sq)
                break
            }
            else
            {
                break
            }
        }
        
        for i in 1 ..< 8
        {
            var sq = -1
            if(coor.y + i < 8)
            {
                sq = (coor.x) + ((coor.y + i) * 8)
            }
            else
            {
                break
            }
            if(Int(variables.boardPosition.character(at: sq)) ==  48)
            {
                goodList.append(sq)
            }
            else if(Int(variables.boardPosition.character(at: sq)) < 92)
            {
                goodList.append(sq)
                break
            }
            else
            {
                break
            }
        }
        
        for i in 1 ..< 8
        {
            var sq = -1
            if(coor.y - i >= 0)
            {
                sq = (coor.x) + ((coor.y - i) * 8)
            }
            else
            {
                break
            }
            if(Int(variables.boardPosition.character(at: sq)) ==  48)
            {
                goodList.append(sq)
            }
            else if(Int(variables.boardPosition.character(at: sq)) < 92)
            {
                goodList.append(sq)
                break
            }
            else
            {
                break
            }
        }
        break
    case 82: //black rook
        for i in 1 ..< 8
        {
            var sq = -1
            if(coor.x + i < 8)
            {
                sq = (coor.x + i) + (coor.y * 8)
            }
            else
            {
                break
            }
            if(Int(variables.boardPosition.character(at: sq)) ==  48)
            {
                goodList.append(sq)
            }
            else if(Int(variables.boardPosition.character(at: sq)) > 92)
            {
                goodList.append(sq)
                break
            }
            else
            {
                break
            }
        }
        
        for i in 1 ..< 8
        {
            var sq = -1
            if(coor.x - i >= 0)
            {
                sq = (coor.x - i) + (coor.y * 8)
            }
            else
            {
                break
            }
            if(Int(variables.boardPosition.character(at: sq)) ==  48)
            {
                goodList.append(sq)
            }
            else if(Int(variables.boardPosition.character(at: sq)) > 92)
            {
                goodList.append(sq)
                break
            }
            else
            {
                break
            }
        }
        
        for i in 1 ..< 8
        {
            var sq = -1
            if(coor.y + i < 8)
            {
                sq = (coor.x) + ((coor.y + i) * 8)
            }
            else
            {
                break
            }
            if(Int(variables.boardPosition.character(at: sq)) ==  48)
            {
                goodList.append(sq)
            }
            else if(Int(variables.boardPosition.character(at: sq)) > 92)
            {
                goodList.append(sq)
                break
            }
            else
            {
                break
            }
        }
        
        for i in 1 ..< 8
        {
            var sq = -1
            if(coor.y - i >= 0)
            {
                sq = (coor.x) + ((coor.y - i) * 8)
            }
            else
            {
                break
            }
            if(Int(variables.boardPosition.character(at: sq)) ==  48)
            {
                goodList.append(sq)
            }
            else if(Int(variables.boardPosition.character(at: sq)) > 92)
            {
                goodList.append(sq)
                break
            }
            else
            {
                break
            }
        }
        break
    case 107: //white king
        var sq = -1
        if(coor.x + 1 < 8 && coor.y + 1 < 8)
        {
            sq = (coor.x + 1) + ((coor.y + 1) * 8)
            if(Int(variables.boardPosition.character(at: sq)) ==  48 || Int(variables.boardPosition.character(at: sq)) < 92)
            {
                goodList.append(sq)
            }
        }
        if(coor.x + 1 < 8 && coor.y - 1 >= 0)
        {
            sq = (coor.x + 1) + ((coor.y - 1) * 8)
            if(Int(variables.boardPosition.character(at: sq)) ==  48 || Int(variables.boardPosition.character(at: sq)) < 92)
            {
                goodList.append(sq)
            }
        }
        if(coor.x - 1 >= 0 && coor.y + 1 < 8)
        {
            sq = (coor.x - 1) + ((coor.y + 1) * 8)
            if(Int(variables.boardPosition.character(at: sq)) ==  48 || Int(variables.boardPosition.character(at: sq)) < 92)
            {
                goodList.append(sq)
            }
        }
        if(coor.x - 1 >= 0 && coor.y - 1 >= 0)
        {
            sq = (coor.x - 1) + ((coor.y - 1) * 8)
            if(Int(variables.boardPosition.character(at: sq)) ==  48 || Int(variables.boardPosition.character(at: sq)) < 92)
            {
                goodList.append(sq)
            }
        }
        if(coor.x + 1 < 8)
        {
            sq = (coor.x + 1) + ((coor.y) * 8)
            if(Int(variables.boardPosition.character(at: sq)) ==  48 || Int(variables.boardPosition.character(at: sq)) < 92)
            {
                goodList.append(sq)
            }
        }
        if(coor.x - 1 >= 0)
        {
            sq = (coor.x - 1) + ((coor.y) * 8)
            if(Int(variables.boardPosition.character(at: sq)) ==  48 || Int(variables.boardPosition.character(at: sq)) < 92)
            {
                goodList.append(sq)
            }
        }
        if(coor.y + 1 < 8)
        {
            sq = (coor.x) + ((coor.y + 1) * 8)
            if(Int(variables.boardPosition.character(at: sq)) ==  48 || Int(variables.boardPosition.character(at: sq)) < 92)
            {
                goodList.append(sq)
            }
        }
        if(coor.y - 1 >= 0)
        {
            sq = (coor.x) + ((coor.y - 1) * 8)
            if(Int(variables.boardPosition.character(at: sq)) ==  48 || Int(variables.boardPosition.character(at: sq)) < 92)
            {
                goodList.append(sq)
            }
        }
        break
    case 75: //black king
        var sq = -1
        if(coor.x + 1 < 8 && coor.y + 1 < 8)
        {
            sq = (coor.x + 1) + ((coor.y + 1) * 8)
            if(Int(variables.boardPosition.character(at: sq)) ==  48 || Int(variables.boardPosition.character(at: sq)) > 92)
            {
                goodList.append(sq)
            }
        }
        if(coor.x + 1 < 8 && coor.y - 1 >= 0)
        {
            sq = (coor.x + 1) + ((coor.y - 1) * 8)
            if(Int(variables.boardPosition.character(at: sq)) ==  48 || Int(variables.boardPosition.character(at: sq)) > 92)
            {
                goodList.append(sq)
            }
        }
        if(coor.x - 1 >= 0 && coor.y + 1 < 8)
        {
            sq = (coor.x - 1) + ((coor.y + 1) * 8)
            if(Int(variables.boardPosition.character(at: sq)) ==  48 || Int(variables.boardPosition.character(at: sq)) > 92)
            {
                goodList.append(sq)
            }
        }
        if(coor.x - 1 >= 0 && coor.y - 1 >= 0)
        {
            sq = (coor.x - 1) + ((coor.y - 1) * 8)
            if(Int(variables.boardPosition.character(at: sq)) ==  48 || Int(variables.boardPosition.character(at: sq)) > 92)
            {
                goodList.append(sq)
            }
        }
        if(coor.x + 1 < 8)
        {
            sq = (coor.x + 1) + ((coor.y) * 8)
            if(Int(variables.boardPosition.character(at: sq)) ==  48 || Int(variables.boardPosition.character(at: sq)) > 92)
            {
                goodList.append(sq)
            }
        }
        if(coor.x - 1 >= 0)
        {
            sq = (coor.x - 1) + ((coor.y) * 8)
            if(Int(variables.boardPosition.character(at: sq)) ==  48 || Int(variables.boardPosition.character(at: sq)) > 92)
            {
                goodList.append(sq)
            }
        }
        if(coor.y + 1 < 8)
        {
            sq = (coor.x) + ((coor.y + 1) * 8)
            if(Int(variables.boardPosition.character(at: sq)) ==  48 || Int(variables.boardPosition.character(at: sq)) > 92)
            {
                goodList.append(sq)
            }
        }
        if(coor.y - 1 >= 0)
        {
            sq = (coor.x) + ((coor.y - 1) * 8)
            if(Int(variables.boardPosition.character(at: sq)) ==  48 || Int(variables.boardPosition.character(at: sq)) > 92)
            {
                goodList.append(sq)
            }
        }
        break
    case 113: //white queen
        for i in 1 ..< 8
        {
            var sq = -1
            if(coor.x + i < 8 && coor.y + i < 8)
            {
                sq = (coor.x + i) + ((coor.y + i) * 8)
            }
            else
            {
                break
            }
            if(Int(variables.boardPosition.character(at: sq)) ==  48)
            {
                goodList.append(sq)
            }
            else if(Int(variables.boardPosition.character(at: sq)) < 92)
            {
                goodList.append(sq)
                break
            }
            else
            {
                break
            }
        }
        
        for i in 1 ..< 8
        {
            var sq = -1
            if(coor.x + i < 8 && coor.y - i >= 0)
            {
                sq = (coor.x + i) + ((coor.y - i) * 8)
            }
            else
            {
                break
            }
            if(Int(variables.boardPosition.character(at: sq)) ==  48)
            {
                goodList.append(sq)
            }
            else if(Int(variables.boardPosition.character(at: sq)) < 92)
            {
                goodList.append(sq)
                break
            }
            else
            {
                break
            }
        }
        
        for i in 1 ..< 8
        {
            var sq = -1
            if(coor.x - i >= 0 && coor.y + i < 8)
            {
                sq = (coor.x - i) + ((coor.y + i) * 8)
            }
            else
            {
                break
            }
            if(Int(variables.boardPosition.character(at: sq)) ==  48)
            {
                goodList.append(sq)
            }
            else if(Int(variables.boardPosition.character(at: sq)) < 92)
            {
                goodList.append(sq)
                break
            }
            else
            {
                break
            }
        }
        
        for i in 1 ..< 8
        {
            var sq = -1
            if(coor.x - i >= 0 && coor.y - i >= 0)
            {
                sq = (coor.x - i) + ((coor.y - i) * 8)
            }
            else
            {
                break
            }
            if(Int(variables.boardPosition.character(at: sq)) ==  48)
            {
                goodList.append(sq)
            }
            else if(Int(variables.boardPosition.character(at: sq)) < 92)
            {
                goodList.append(sq)
                break
            }
            else
            {
                break
            }
        }
        
        for i in 1 ..< 8
        {
            var sq = -1
            if(coor.x + i < 8)
            {
                sq = (coor.x + i) + (coor.y * 8)
            }
            else
            {
                break
            }
            if(Int(variables.boardPosition.character(at: sq)) ==  48)
            {
                goodList.append(sq)
            }
            else if(Int(variables.boardPosition.character(at: sq)) < 92)
            {
                goodList.append(sq)
                break
            }
            else
            {
                break
            }
        }
        
        for i in 1 ..< 8
        {
            var sq = -1
            if(coor.x - i >= 0)
            {
                sq = (coor.x - i) + (coor.y * 8)
            }
            else
            {
                break
            }
            if(Int(variables.boardPosition.character(at: sq)) ==  48)
            {
                goodList.append(sq)
            }
            else if(Int(variables.boardPosition.character(at: sq)) < 92)
            {
                goodList.append(sq)
                break
            }
            else
            {
                break
            }
        }
        
        for i in 1 ..< 8
        {
            var sq = -1
            if(coor.y + i < 8)
            {
                sq = (coor.x) + ((coor.y + i) * 8)
            }
            else
            {
                break
            }
            if(Int(variables.boardPosition.character(at: sq)) ==  48)
            {
                goodList.append(sq)
            }
            else if(Int(variables.boardPosition.character(at: sq)) < 92)
            {
                goodList.append(sq)
                break
            }
            else
            {
                break
            }
        }
        
        for i in 1 ..< 8
        {
            var sq = -1
            if(coor.y - i >= 0)
            {
                sq = (coor.x) + ((coor.y - i) * 8)
            }
            else
            {
                break
            }
            if(Int(variables.boardPosition.character(at: sq)) ==  48)
            {
                goodList.append(sq)
            }
            else if(Int(variables.boardPosition.character(at: sq)) < 92)
            {
                goodList.append(sq)
                break
            }
            else
            {
                break
            }
        }
        break
    case 81: //black queen
        for i in 1 ..< 8
        {
            var sq = -1
            if(coor.x + i < 8)
            {
                sq = (coor.x + i) + (coor.y * 8)
            }
            else
            {
                break
            }
            if(Int(variables.boardPosition.character(at: sq)) ==  48)
            {
                goodList.append(sq)
            }
            else if(Int(variables.boardPosition.character(at: sq)) > 92)
            {
                goodList.append(sq)
                break
            }
            else
            {
                break
            }
        }
        
        for i in 1 ..< 8
        {
            var sq = -1
            if(coor.x - i >= 0)
            {
                sq = (coor.x - i) + (coor.y * 8)
            }
            else
            {
                break
            }
            if(Int(variables.boardPosition.character(at: sq)) ==  48)
            {
                goodList.append(sq)
            }
            else if(Int(variables.boardPosition.character(at: sq)) > 92)
            {
                goodList.append(sq)
                break
            }
            else
            {
                break
            }
        }
        
        for i in 1 ..< 8
        {
            var sq = -1
            if(coor.y + i < 8)
            {
                sq = (coor.x) + ((coor.y + i) * 8)
            }
            else
            {
                break
            }
            if(Int(variables.boardPosition.character(at: sq)) ==  48)
            {
                goodList.append(sq)
            }
            else if(Int(variables.boardPosition.character(at: sq)) > 92)
            {
                goodList.append(sq)
                break
            }
            else
            {
                break
            }
        }
        
        for i in 1 ..< 8
        {
            var sq = -1
            if(coor.y - i >= 0)
            {
                sq = (coor.x) + ((coor.y - i) * 8)
            }
            else
            {
                break
            }
            if(Int(variables.boardPosition.character(at: sq)) ==  48)
            {
                goodList.append(sq)
            }
            else if(Int(variables.boardPosition.character(at: sq)) > 92)
            {
                goodList.append(sq)
                break
            }
            else
            {
                break
            }
        }
        
        for i in 1 ..< 8
        {
            var sq = -1
            if(coor.x + i < 8 && coor.y + i < 8)
            {
                sq = (coor.x + i) + ((coor.y + i) * 8)
            }
            else
            {
                break
            }
            if(Int(variables.boardPosition.character(at: sq)) ==  48)
            {
                goodList.append(sq)
            }
            else if(Int(variables.boardPosition.character(at: sq)) > 92)
            {
                goodList.append(sq)
                break
            }
            else
            {
                break
            }
        }
        
        for i in 1 ..< 8
        {
            var sq = -1
            if(coor.x + i < 8 && coor.y - i >= 0)
            {
                sq = (coor.x + i) + ((coor.y - i) * 8)
            }
            else
            {
                break
            }
            if(Int(variables.boardPosition.character(at: sq)) ==  48)
            {
                goodList.append(sq)
            }
            else if(Int(variables.boardPosition.character(at: sq)) > 92)
            {
                goodList.append(sq)
                break
            }
            else
            {
                break
            }
        }
        
        for i in 1 ..< 8
        {
            var sq = -1
            if(coor.x - i >= 0 && coor.y + i < 8)
            {
                sq = (coor.x - i) + ((coor.y + i) * 8)
            }
            else
            {
                break
            }
            if(Int(variables.boardPosition.character(at: sq)) ==  48)
            {
                goodList.append(sq)
            }
            else if(Int(variables.boardPosition.character(at: sq)) > 92)
            {
                goodList.append(sq)
                break
            }
            else
            {
                break
            }
        }
        
        for i in 1 ..< 8
        {
            var sq = -1
            if(coor.x - i >= 0 && coor.y - i >= 0)
            {
                sq = (coor.x - i) + ((coor.y - i) * 8)
            }
            else
            {
                break
            }
            if(Int(variables.boardPosition.character(at: sq)) ==  48)
            {
                goodList.append(sq)
            }
            else if(Int(variables.boardPosition.character(at: sq)) > 92)
            {
                goodList.append(sq)
                break
            }
            else
            {
                break
            }
        }
        
        
        break
    default:
        break
    }
    return goodList
}

//returns a gradient for use in chess board squares and elsewhere
func createGradient(_ color:String, bounds:CGRect, round:CGFloat) -> CAGradientLayer
{
    let grad = CAGradientLayer()
    let pc = UIColorFromRGB2(color, alpha: 1)
    let lpc = UIColorFromRGB2(color, alpha: 0.75)
    grad.colors = [lpc.cgColor, pc.cgColor]
    grad.frame = bounds
    grad.cornerRadius = round
    return grad
}

func createGradient2(_ color:String, bounds:CGRect, round:CGFloat) -> CAGradientLayer
{
    let grad = CAGradientLayer()
    let pc = UIColorFromRGB2(color, alpha: 1)
    let lpc = UIColorFromRGB2(color, alpha: 0.25)
    grad.colors = [lpc.cgColor, pc.cgColor]
    grad.frame = bounds
    grad.cornerRadius = round
    return grad
}

func createGradient3(_ color:String, bounds:CGRect, round:CGFloat) -> CAGradientLayer
{
    let grad = CAGradientLayer()
    let pc = UIColorFromRGB2(color, alpha: 0.25)
    let lpc = UIColorFromRGB2(color, alpha: 1)
    grad.colors = [lpc.cgColor, pc.cgColor]
    grad.frame = bounds
    grad.cornerRadius = round
    return grad
}

func createGradient4(_ color:String, bounds:CGRect, round:CGFloat) -> CAGradientLayer
{
    let grad = CAGradientLayer()
    let pc = UIColorFromRGB2(color, alpha: 0.75)
    let lpc = UIColorFromRGB2(color, alpha: 1)
    grad.colors = [lpc.cgColor, pc.cgColor]
    grad.frame = bounds
    grad.cornerRadius = round
    return grad
}

//returns a color where hex string
func UIColorFromRGB2(_ colorCode: String, alpha: Float = 1.0) -> UIColor {
    var colorCode = colorCode
    
    colorCode = colorCode.replacingOccurrences(of: "#", with: "", options: NSString.CompareOptions.literal, range: nil)
    let scanner = Scanner(string:colorCode)
    var color:UInt32 = 0;
    scanner.scanHexInt32(&color)
    
    let mask = 0x000000FF
    let r = CGFloat(Float(Int(color >> 16) & mask)/255.0)
    let g = CGFloat(Float(Int(color >> 8) & mask)/255.0)
    let b = CGFloat(Float(Int(color) & mask)/255.0)
    
    return UIColor(red: r, green: g, blue: b, alpha: CGFloat(alpha))
}

//encodes strings as valid url parameters
func prepareArg2(_ text:NSString) -> String
{
    return (text as String).addingPercentEncoding(withAllowedCharacters: NSMutableCharacterSet.alphanumeric() as CharacterSet)!
}

//improved syntax
func indexOf(_ text:NSString, find:String) -> Int
{
    return text.range(of: find).toRange()!.lowerBound as Int
}

//improved syntax
func replace(_ text:NSString, old:String, newer:String) -> String
{
    return text.replacingOccurrences(of: old, with: newer)
}

func doRequests()
{
    //disabled in demo version
}

func getStats() {
    //disabled in demo version
}

func getLeaders() {
    //disabled in demo version
}

func getStatRecord(id:String) {
    //disabled in demo version
}

func parseStats(text:NSString) {
    //disabled in demo version
}

class Stat {
    var id: String
    var text: String
    
    init(i:String, err:String, sol:String) {
        id = i
        text = "Average Mistakes - " + err + "\r\nTimes Solved - " + sol + "%"
    }
}


//simple class for x/y coordinate with archiving methods
class Point : NSObject, NSCoding
{
    var x:Int
    var y:Int
    
    init(xp:Int, yp:Int)
    {
        x = xp
        y = yp
    }
    
    required init (coder decoder: NSCoder)
    {
        self.x = decoder.decodeObject(forKey: "x") as? Int ?? decoder.decodeInteger(forKey: "x")
        self.y = decoder.decodeObject(forKey: "y") as? Int ?? decoder.decodeInteger(forKey: "y")
    }
    
    func encode(with coder: NSCoder)
    {
        coder.encode(self.x, forKey: "x")
        coder.encode(self.y, forKey: "y")
    }
}

//holds several chess puzzles and how many have been solved
class PuzzleList {
    var puzzles: [Puzzle]
    var totalSolved:Int
    var total:Int
    
    init(p:[Puzzle]) {
        puzzles = p
        total = 0
        totalSolved = 0
        total = p.count
        for i in 0..<p.count {
            if(p[i].solved == "1") {
                totalSolved += 1
            }
        }
    }
    
    //updates self.totalSolved and the masterPuzzleList
    func setData() {
        total = puzzles.count
        totalSolved = 0
        for i in 0..<puzzles.count {
            if(puzzles[i].solved == "1") {
                totalSolved += 1
            }
            for ii in 0..<myVar.masterPuzzleList.count {
                if(puzzles[i].id == myVar.masterPuzzleList[ii].id) {
                    myVar.masterPuzzleList[ii].solved = puzzles[i].solved
                    myVar.masterPuzzleList[ii].voted = puzzles[i].voted
                    break
                }
            }
        }
    }
}



//main class for a chess puzzle
//id = database id, firstTurn = white or black, diff = difficulty
//solved = has it been solved by this user, voted = has it been voted on by this user
//board = string of length 64
//moves = list of correct moves - eg 3,4,6,7,8,9 means first move is A4 to A5, computer will move A7 to A8
//then final is B1 to B2
class Puzzle : NSObject, NSCoding
{
    var library:Int
    var id:String
    var author:String
    var firstTurn:String
    var board:String
    var moves:[Point]
    var title:String
    var diff:String
    var solved:String
    var voted:String
    
    
    init(l:Int, i:String, a:String, ft:String, b:String, m:[Point], t:String, d:String)
    {
        library = l
        id = i
        author = a
        firstTurn = ft
        board = b
        moves = m
        title = t
        diff = d
        solved = "0"
        voted = "0"
    }
    
    required init (coder decoder: NSCoder)
    {
        self.library = decoder.decodeObject(forKey: "library") as? Int ?? decoder.decodeInteger(forKey: "library")
        self.id = decoder.decodeObject(forKey: "id") as? String ?? ""
        self.author = decoder.decodeObject(forKey: "author") as? String ?? ""
        self.firstTurn = decoder.decodeObject(forKey: "firstTurn") as? String ?? ""
        self.board = decoder.decodeObject(forKey: "board") as? String ?? ""
        self.moves = decoder.decodeObject(forKey: "moves") as! [Point]
        self.title = decoder.decodeObject(forKey: "title") as? String ?? ""
        self.diff = decoder.decodeObject(forKey: "diff") as? String ?? ""
        self.solved = decoder.decodeObject(forKey: "solved") as? String ?? ""
        self.voted = decoder.decodeObject(forKey: "voted") as? String ?? ""
    }
    
    func encode(with coder: NSCoder)
    {
        coder.encode(self.library, forKey: "library")
        coder.encode(self.id, forKey: "id")
        coder.encode(self.author, forKey: "author")
        coder.encode(self.firstTurn, forKey: "firstTurn")
        coder.encode(self.board, forKey: "board")
        coder.encode(self.moves, forKey: "moves")
        coder.encode(self.title, forKey: "title")
        coder.encode(self.diff, forKey: "diff")
        coder.encode(self.solved, forKey: "solved")
        coder.encode(self.voted, forKey: "voted")
    }
}



