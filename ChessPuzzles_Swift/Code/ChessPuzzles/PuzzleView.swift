//
//  PuzzleView.swift
//  Chess Puzzles
//
//  Created by Walter Babcock on 12/28/16.
//  Copyright © 2016 Babcock. All rights reserved.
//

import UIKit

class PuzzleView: UIViewController {
    
    @IBOutlet weak var AuthorView: UIView!
    @IBOutlet weak var TitleLabel: UILabel!
    @IBOutlet weak var AuthorLabel: UILabel!
    @IBOutlet weak var TurnLabel: UILabel!
    @IBOutlet weak var DownButton: UIImageView!
    @IBOutlet weak var VoteView: UIView!
    @IBOutlet weak var UpButton: UIImageView!
    
    //navigates back to library view controller
    @IBAction func LibraryClick(_ sender: AnyObject) {
        let mainStoryboard = UIStoryboard(name: "Main", bundle: Bundle.main)
        let vc : UIViewController = mainStoryboard.instantiateViewController(withIdentifier: "Solve")
        self.present(vc, animated: true, completion: nil)
    }
    
    //shows author and title of puzzle with fade in animation
    override func viewDidAppear(_ animated: Bool) {
        showAuthor()
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        //open voting capability
        let twoTap = UITapGestureRecognizer(target: self, action: #selector(PuzzleView.vote(_:)))
        twoTap.numberOfTouchesRequired = 2
        self.view.addGestureRecognizer(twoTap)
        
        //adds click listeners to voting buttons
        let upGesture = UITapGestureRecognizer(target: self, action: #selector(PuzzleView.upvote(_:)))
        UpButton.addGestureRecognizer(upGesture)
        let downGesture = UITapGestureRecognizer(target: self, action: #selector(PuzzleView.downvote(_:)))
        DownButton.addGestureRecognizer(downGesture)
        
        //if first attempt, updates database to reflect new attempt made
        myVar.errors = 0
        if(myVar.puzzleList[myVar.selectedPuzzle].solved == "0")
        {
            attemptRequest(myVar.puzzleList[myVar.selectedPuzzle].id, username: myVar.username.addingPercentEncoding(withAllowedCharacters: NSMutableCharacterSet.alphanumeric() as CharacterSet)!)
            myVar.puzzleList[myVar.selectedPuzzle].solved = "2"
            let saveData = NSKeyedArchiver.archivedData(withRootObject: myVar.puzzleList)
            UserDefaults.standard.set(saveData, forKey: String(myVar.selectedLibrary))
        }
        
        //will contain views for chess board
        variables.squareList = [UIView]()
        variables.chessList = [UIImageView]()
        variables.moveList = [UIView]()
        
        //gets data for which color moves first, resolves ambiguity
        variables.whiteFirst = myVar.puzzleList[myVar.selectedPuzzle].firstTurn
        if(variables.whiteFirst == "0")
        {
            variables.whiteTurn = "1"
        }
        else
        {
            variables.whiteTurn = "0"
        }
        //copies info to shorter variable name
        variables.startingPosition = myVar.puzzleList[myVar.selectedPuzzle].board
        variables.boardPosition = myVar.puzzleList[myVar.selectedPuzzle].board as NSString
        variables.moves = myVar.puzzleList[myVar.selectedPuzzle].moves
        
        //sets currentMove counter to beginning of puzzle
        //incremented as user makes a correct move
        myVar.currentMove = 0
        
        //dynamically adds views for chess board
        let width = self.view.frame.width / 8
        var x = 0 as CGFloat
        var y = width * 10
        var count = 0
        for i in 0 ..< 64
        {
            var color = "926239"
            if(count % 2 == 1)
            {
                color = "fbe3cf"
            }
            let square: UIView = UIView(frame: CGRect(x: x, y: y, width: width, height: width))
            square.layer.borderWidth = 2
            square.layer.borderColor = UIColor.clear.cgColor
            let square2: UIImageView = UIImageView(frame: CGRect(x: 0, y: 0, width: width, height: width))
            let square3: UIView = UIView(frame: CGRect(x: 10, y: 10, width: width - 20, height: width - 20))
            square.layer.insertSublayer(createGradient(color,bounds: square.bounds,round: 0 as CGFloat), at:0)
            square.tag = i
            variables.squareList.append(square)
            variables.chessList.append(square2)
            variables.moveList.append(square3)
            square.addSubview(square2)
            square.addSubview(square3)
            self.view.addSubview(square)
            x += width
            count += 1
            if((i+1) % 8 == 0)
            {
                x = 0
                y -= width
                count += 1
            }
        }
        
        //creates touch listener for chess board
        let touch = UILongPressGestureRecognizer(target: self, action: #selector(PuzzleView.startMove(_:)))
        touch.minimumPressDuration = 0.1
        self.view.addGestureRecognizer(touch)
        setBoard()
        
        //adds different views onto screen for improved graphic design
        let border: UIView = UIView(frame: CGRect(x: 0, y: (width*3)-(width/4), width: self.view.frame.width, height: width/4))
        border.layer.insertSublayer(createGradient4("666666",bounds: border.bounds,round: 0 as CGFloat), at:0)
        self.view.addSubview(border)
        let border2: UIView = UIView(frame: CGRect(x: 0, y: (width*11), width: self.view.frame.width, height: width/4))
        border2.layer.insertSublayer(createGradient("666666",bounds: border2.bounds,round: 0 as CGFloat), at:0)
        self.view.addSubview(border2)
        let border3: UIView = UIView(frame: CGRect(x: 0, y: 0, width: self.view.frame.width, height: (width*3)-(width/4)))
        border3.layer.insertSublayer(createGradient3("d3b891",bounds: border3.bounds,round: 0 as CGFloat), at:0)
        self.view.addSubview(border3)
        let border4: UIView = UIView(frame: CGRect(x: 0, y: (width*11)+(width/4), width: self.view.frame.width, height: self.view.frame.height - ((width*11)+(width/4))))
        border4.layer.insertSublayer(createGradient2("d3b891",bounds: border4.bounds,round: 0 as CGFloat), at:0)
        self.view.addSubview(border4)
        let border5: UIView = UIView(frame: CGRect(x: 0, y: 0, width: self.view.frame.width, height: 25))
        border5.layer.insertSublayer(createGradient4("5a3f26",bounds: border5.bounds,round: 0 as CGFloat), at:0)
        self.view.addSubview(border5)
        let border6: UIView = UIView(frame: CGRect(x: 0, y: self.view.frame.height - 25, width: self.view.frame.width, height: 25))
        border6.layer.insertSublayer(createGradient("5a3f26",bounds: border6.bounds,round: 0 as CGFloat), at:0)
        self.view.addSubview(border6)
        let border7: UIView = UIView(frame: CGRect(x: 0, y: 25, width: self.view.frame.width, height: 5))
        border7.layer.insertSublayer(createGradient4("90643c",bounds: border7.bounds,round: 0 as CGFloat), at:0)
        self.view.addSubview(border7)
        let border8: UIView = UIView(frame: CGRect(x: 0, y: self.view.frame.height - 30, width: self.view.frame.width, height: 5))
        border8.layer.insertSublayer(createGradient("90643c",bounds: border8.bounds,round: 0 as CGFloat), at:0)
        self.view.addSubview(border8)
        let border9: UIView = UIView(frame: CGRect(x: 0, y: 30, width: self.view.frame.width, height: 2))
        border9.layer.insertSublayer(createGradient4("bb8a5d",bounds: border9.bounds,round: 0 as CGFloat), at:0)
        self.view.addSubview(border9)
        let border10: UIView = UIView(frame: CGRect(x: 0, y: self.view.frame.height - 32, width: self.view.frame.width, height: 2))
        border10.layer.insertSublayer(createGradient("bb8a5d",bounds: border10.bounds,round: 0 as CGFloat), at:0)
        self.view.addSubview(border10)
        
        self.view.bringSubview(toFront: HomeButton)
        self.view.bringSubview(toFront: MenuButton)
    }
    
    @IBOutlet weak var HomeButton: UIButton!
    @IBOutlet weak var MenuButton: UIButton!
    
    //button click action, shows dialog
    @IBAction func MenuClick(_ sender: AnyObject) {
        let alert = UIAlertController(title: "Grandmaster Chess Puzzles", message: "Pick an Action", preferredStyle: UIAlertControllerStyle.alert)
        alert.addAction(UIAlertAction(title: "Next", style: UIAlertActionStyle.default, handler: {(UIAlertAction) in
            self.next()
        }))
        if(myVar.puzzleList[myVar.selectedPuzzle].voted == "0")
        {
            alert.addAction(UIAlertAction(title: "Upvote", style: UIAlertActionStyle.default, handler: {(UIAlertAction) in
                self.upvote2()
            }))
            alert.addAction(UIAlertAction(title: "Downvote", style: UIAlertActionStyle.default, handler: {(UIAlertAction) in
                self.downvote2()
            }))
        }
        alert.addAction(UIAlertAction(title: "Dismiss", style: UIAlertActionStyle.default, handler: {(UIAlertAction) in
            
        }))
        
        self.present(alert, animated: true, completion: nil)
    }
    
    //hides time and battery level
    override var prefersStatusBarHidden : Bool {
        return true
    }
    
    //bring up voting dialog, happen on double click of screen
    func vote(_ recognizer: UITapGestureRecognizer)
    {
        let alert = UIAlertController(title: "Grandmaster Chess Puzzles", message: "Pick an Action", preferredStyle: UIAlertControllerStyle.alert)
        alert.addAction(UIAlertAction(title: "Next", style: UIAlertActionStyle.default, handler: {(UIAlertAction) in
            self.next()
        }))
        if(myVar.puzzleList[myVar.selectedPuzzle].voted == "0")
        {
            alert.addAction(UIAlertAction(title: "Upvote", style: UIAlertActionStyle.default, handler: {(UIAlertAction) in
                self.upvote2()
            }))
            alert.addAction(UIAlertAction(title: "Downvote", style: UIAlertActionStyle.default, handler: {(UIAlertAction) in
                self.downvote2()
            }))
        }
        alert.addAction(UIAlertAction(title: "Dismiss", style: UIAlertActionStyle.default, handler: {(UIAlertAction) in
       
        }))
        
        self.present(alert, animated: true, completion: nil)
    }
    
    
    //touch listener for chess board
    //three phases: began, changed, and ended
    func startMove(_ recognizer: UILongPressGestureRecognizer)
    {
        let state = recognizer.state
        let p = recognizer.location(in: recognizer.view!.window)
        switch(state)
        {
        case UIGestureRecognizerState.began:
            variables.goodStart = "0"
            variables.okStart = "0"
            
            //loops through all squares
            for i in 0 ..< variables.squareList.count
            {
                //user touched down in this particular square
                //if it is a valid move - records this location for use in ended phase, and highlights border of square
                if(variables.squareList[i].frame.contains(p) && variables.boardPosition.character(at: i) != 48)
                {
                        if((variables.whiteTurn == "1" && variables.boardPosition.character(at: i) < 92) || (variables.whiteTurn == "0" && variables.boardPosition.character(at: i) > 92))
                        {
                            variables.okStart = "1"
                            variables.validList = getValidMoves(i, piece: Int(String(describing: String(variables.boardPosition.character(at: i)).utf8))!)
                            for ii in 0 ..< variables.validList.count
                            {
                                var color = "85b0be"
                                if((variables.validList[ii] + Int(variables.validList[ii]/8)) % 2 == 1)
                                {
                                    color = "b7e2f0"
                                }
                                variables.moveList[variables.validList[ii]].layer.insertSublayer(createGradient(color,bounds: variables.moveList[variables.validList[ii]].bounds, round: 8 as CGFloat), at:0)
                            }
                            variables.squareList[i].layer.borderColor = UIColorFromRGB2("85b0be").cgColor
                            variables.newPiece = String(describing: UnicodeScalar(variables.boardPosition.character(at: i)))
                            variables.oldSquare = i
                            
                        }
                        if(i == variables.moves[myVar.currentMove].x)
                        {
                            variables.goodStart = "1"
                        }
                    
                }
                else
                {
                    variables.squareList[i].layer.borderColor = UIColor.clear.cgColor
                    
                }
            }
            
            break
            
        case UIGestureRecognizerState.changed:
            //will move highlighted border to new square, remove any other highlighted borders
            if(variables.goodStart == "1" || variables.okStart == "1")
            {
                for i in 0 ..< variables.squareList.count
                {
                    if(variables.squareList[i].frame.contains(p))
                    {
                        if(variables.validList.contains(i))
                        {
                            variables.squareList[i].layer.borderColor = UIColorFromRGB2("85b0be").cgColor
                        }
                    }
                    else if(variables.oldSquare != i)
                    {
                        variables.squareList[i].layer.borderColor = UIColor.clear.cgColor
                        
                    }
                }
            }
            break
            
        case UIGestureRecognizerState.ended:
                var error = "1"
                //loops through all squares
                for i in 0 ..< variables.squareList.count
                {
                    //removes highlighted border from all squares
                    variables.squareList[i].layer.borderColor = UIColor.clear.cgColor
                    
                    //user ended on this square and was right if true
                    if(variables.squareList[i].frame.contains(p) && i == variables.moves[myVar.currentMove].y && variables.goodStart == "1")
                    {
                        //removes error flag
                        error = "0"
                        //shows animation of pieces from human player then computer player
                        if(myVar.currentMove < variables.moves.count - 1)
                        {
                            computerMove()
                        }
                        //if out of computer moves, then puzzle is solved
                        else
                        {
                            //show human player final move
                            finalMove()
                            
                            //updates database if puzzle hasn't been solved before
                            if(myVar.puzzleList[myVar.selectedPuzzle].solved != "1")
                            {
                                myVar.divPuzzleLists[myVar.selectedLibrary].puzzles[myVar.selectedPuzzle].solved = "1"
                                myVar.puzzleList[myVar.selectedPuzzle].solved = "1"
                                var count = 0;
                                for i in 0 ..< myVar.puzzleList.count
                                {
                                    if(myVar.puzzleList[i].solved == "1")
                                    {
                                        count += 1
                                    }
                                }
                                myVar.divPuzzleLists[myVar.selectedLibrary].totalSolved = count
                                if(myVar.divPuzzleLists[myVar.selectedLibrary].totalSolved >= 100)
                                {
                                    libraryRequest(prepareArg2(myVar.username as NSString))
                                }
                                myVar.puzzleList[myVar.selectedPuzzle].solved = "1"
                                let saveData = NSKeyedArchiver.archivedData(withRootObject: myVar.puzzleList)
                                UserDefaults.standard.set(saveData, forKey: String(myVar.selectedLibrary))
                                solvedRequest(myVar.puzzleList[myVar.selectedPuzzle].id, errors: String(myVar.errors), username: prepareArg2(myVar.username as NSString))
                            }
                            
                            
                        }
                        
                    }
            }
            clearBorders()
            if(error == "1" && variables.okStart == "1")
            {
                myVar.errors += 1
            }
                
            break
            
        default:
            break
        }
    }
    
    func upvote(_ recognizer: UITapGestureRecognizer)
    {

    }
    
    func downvote(_ recognizer: UITapGestureRecognizer)
    {

    }
    
    func upvote2()
    {

    }
    
    func downvote2()
    {

    }

    //animation for a human player's move, followed by animation of computer player
    func computerMove()
    {
        let qualityOfServiceClass = DispatchQoS.QoSClass.background
        let backgroundQueue = DispatchQueue.global(qos: qualityOfServiceClass)
        backgroundQueue.async(execute: {
            var i = myVar.currentMove
            let l = variables.boardPosition.substring(with: NSRange.init(location: variables.moves[i].x, length: 1))
            switchBoard(variables.moves[i].x, letter: "0")
            DispatchQueue.main.async(execute: { () -> Void in
                setBoard()
                self.movePiece(variables.moves[i].x, end: variables.moves[i].y, pic: numberForLetter(l))
            })
            sleep(2)
            i += 1
            let l2 = variables.boardPosition.substring(with: NSRange.init(location: variables.moves[i].x, length: 1))
            switchBoard(variables.moves[i].x, letter: "0")
            DispatchQueue.main.async(execute: { () -> Void in
                setBoard()
                self.movePiece(variables.moves[i].x, end: variables.moves[i].y, pic: numberForLetter(l2))
            })
            myVar.currentMove += 2
        })
    }
    
    //called when a puzzle is solved
    //shows animation of human player's last move
    //then shows dialog to let user decide what to do next
    func finalMove()
    {
        let m = "Nice job! You solved a puzzle!"
        let qualityOfServiceClass = DispatchQoS.QoSClass.background
        let backgroundQueue = DispatchQueue.global(qos: qualityOfServiceClass)
        backgroundQueue.async(execute: {
            let i = myVar.currentMove
            let l = variables.boardPosition.substring(with: NSRange.init(location: variables.moves[i].x, length: 1))
            switchBoard(variables.moves[i].x, letter: "0")
            DispatchQueue.main.async(execute: { () -> Void in
                setBoard()
                self.movePiece(variables.moves[i].x, end: variables.moves[i].y, pic: numberForLetter(l))
            })
            sleep(2)
            DispatchQueue.main.async(execute: { () -> Void in
                let alert = UIAlertController(title: "Grandmaster Chess Puzzles", message: m, preferredStyle: UIAlertControllerStyle.alert)
                alert.addAction(UIAlertAction(title: "Next", style: UIAlertActionStyle.default, handler: {(UIAlertAction) in
                    self.next()
                }))
                alert.addAction(UIAlertAction(title: "Review", style: UIAlertActionStyle.default, handler: {(UIAlertAction) in
                    self.review()
                }))
                if(myVar.puzzleList[myVar.selectedPuzzle].voted == "0")
                {
                alert.addAction(UIAlertAction(title: "Upvote", style: UIAlertActionStyle.default, handler: {(UIAlertAction) in
                    self.upvote2()
                    self.next()
                }))
                alert.addAction(UIAlertAction(title: "Downvote", style: UIAlertActionStyle.default, handler: {(UIAlertAction) in
                    self.downvote2()
                    self.next()
                }))
                }
                alert.addAction(UIAlertAction(title: "Libraries", style: UIAlertActionStyle.default, handler: {(UIAlertAction) in
                    let mainStoryboard = UIStoryboard(name: "Main", bundle: Bundle.main)
                    let vc : UIViewController = mainStoryboard.instantiateViewController(withIdentifier: "Solve")
                    self.present(vc, animated: true, completion: nil)
                }))
                
                self.present(alert, animated: true, completion: nil)
            })
        })
    }
    
    //moves to next puzzle in current library and sets data back to initial values
    func next()
    {
        myVar.errors = 0
        if(myVar.selectedPuzzle < myVar.puzzleList.count - 1)
        {
            myVar.selectedPuzzle += 1
        }
        else
        {
            myVar.selectedPuzzle = 0
        }
        variables.whiteFirst = myVar.puzzleList[myVar.selectedPuzzle].firstTurn
        if(variables.whiteFirst == "0")
        {
            variables.whiteTurn = "1"
        }
        else
        {
            variables.whiteTurn = "0"
        }
        variables.startingPosition = myVar.puzzleList[myVar.selectedPuzzle].board
        variables.boardPosition = myVar.puzzleList[myVar.selectedPuzzle].board as NSString
        variables.moves = myVar.puzzleList[myVar.selectedPuzzle].moves
        myVar.currentMove = 0
        setBoard()
        showAuthor()
        if(myVar.puzzleList[myVar.selectedPuzzle].solved == "0")
        {
            attemptRequest(myVar.puzzleList[myVar.selectedPuzzle].id, username: myVar.username.addingPercentEncoding(withAllowedCharacters: NSMutableCharacterSet.alphanumeric() as CharacterSet)!)
            myVar.puzzleList[myVar.selectedPuzzle].solved = "2"
            let saveData = NSKeyedArchiver.archivedData(withRootObject: myVar.puzzleList)
            UserDefaults.standard.set(saveData, forKey: String(myVar.selectedLibrary))
        }
    }
    
    //displays all animations for this puzzle
    func review()
    {
        variables.boardPosition = variables.startingPosition as NSString
        setBoard()
        let qualityOfServiceClass = DispatchQoS.QoSClass.background
        let backgroundQueue = DispatchQueue.global(qos: qualityOfServiceClass)
        backgroundQueue.async(execute: {
            for i in 0 ..< variables.moves.count
            {
                let l = variables.boardPosition.substring(with: NSRange.init(location: variables.moves[i].x, length: 1))
                switchBoard(variables.moves[i].x, letter: "0")
                DispatchQueue.main.async(execute: { () -> Void in
                    setBoard()
                    self.movePiece(variables.moves[i].x, end: variables.moves[i].y, pic: numberForLetter(l))
                })
                sleep(2)
            }
            DispatchQueue.main.async(execute: { () -> Void in
                let alert = UIAlertController(title: "Grandmaster Chess Puzzles", message: "You solved a puzzle!", preferredStyle: UIAlertControllerStyle.alert)
                alert.addAction(UIAlertAction(title: "Review", style: UIAlertActionStyle.default, handler: {(UIAlertAction) in
                    self.review()
                }))
                alert.addAction(UIAlertAction(title: "Next", style: UIAlertActionStyle.default, handler: {(UIAlertAction) in
                    self.next()
                }))
                if(myVar.puzzleList[myVar.selectedPuzzle].voted == "0")
                {
                    alert.addAction(UIAlertAction(title: "Upvote", style: UIAlertActionStyle.default, handler: {(UIAlertAction) in
                        self.upvote2()
                        self.next()
                    }))
                    alert.addAction(UIAlertAction(title: "Downvote", style: UIAlertActionStyle.default, handler: {(UIAlertAction) in
                        self.downvote2()
                        self.next()
                    }))
                }
                
                self.present(alert, animated: true, completion: nil)
            })
        })
    }
    
    
    //moves a chess piece from one square to another
    func movePiece(_ start: Int, end: Int, pic: Int)
    {
        CATransaction.begin()
        CATransaction.setCompletionBlock({
            variables.movingPiece.removeFromSuperview()
            switchBoard(end, letter: letterForSquare(pic))
            checkQueens()
            setBoard()
        })
        variables.movingPiece = UIView(frame: CGRect(x: variables.squareList[end].frame.origin.x, y: variables.squareList[end].frame.origin.y, width: self.view.frame.width / 8, height: self.view.frame.width / 8))
        let square: UIImageView = UIImageView(frame: CGRect(x: 0, y: 0, width: self.view.frame.width / 8, height: self.view.frame.width / 8))
        square.image = variables.piecesList[pic]
        variables.movingPiece.addSubview(square)
        self.view.addSubview(variables.movingPiece)
        let animation = CABasicAnimation(keyPath: "position")
        animation.fromValue = NSValue(cgPoint: CGPoint(x: variables.squareList[start].frame.origin.x + self.view.frame.width/16, y: variables.squareList[start].frame.origin.y + self.view.frame.width/16))
        animation.toValue = NSValue(cgPoint: CGPoint(x: variables.squareList[end].frame.origin.x + self.view.frame.width/16, y: variables.squareList[end].frame.origin.y + self.view.frame.width/16))
        animation.duration = 1
        variables.movingPiece.layer.add(animation, forKey: "moving")
        CATransaction.commit()
    }
    
    //fade in animation showing details of puzzle, then sleeps, and fades out
    func showAuthor()
    {
        VoteView.alpha = 0
        VoteView.isHidden = false
        AuthorView.layer.zPosition = 1
        TurnLabel.layer.zPosition = 2
        AuthorView.alpha = 0
        TurnLabel.alpha = 0
        TurnLabel.isHidden = false
        AuthorView.isHidden = false
        var diff = "◉  "
        if(myVar.puzzleList[myVar.selectedPuzzle].diff == "1")
        {
            diff = "◼︎  "
        }
        else if(myVar.puzzleList[myVar.selectedPuzzle].diff == "2")
        {
            diff = "♦︎  "
        }
        AuthorLabel.text = myVar.puzzleList[myVar.selectedPuzzle].author
        TitleLabel.text = diff + myVar.puzzleList[myVar.selectedPuzzle].title
        var turn = "White to Move"
        if(myVar.puzzleList[myVar.selectedPuzzle].firstTurn == "0")
        {
            turn = "Black to Move"
        }
        TurnLabel.text = turn
        UIView.animate(withDuration: 1.0, delay: 0.0, options: UIViewAnimationOptions.curveEaseIn, animations: {self.AuthorView.alpha = 1.0}, completion:nil)
        UIView.animate(withDuration: 1.0, delay: 0.0, options: UIViewAnimationOptions.curveEaseIn, animations: {self.TurnLabel.alpha = 1.0}, completion:nil)
        let backgroundQueue = DispatchQueue.global(qos: DispatchQoS.QoSClass.background)
        backgroundQueue.async(execute: {
                sleep(4)
                DispatchQueue.main.async(execute: { () -> Void in
                    UIView.animate(withDuration: 1.0, delay: 0.0, options: UIViewAnimationOptions.curveEaseIn, animations: {self.AuthorView.alpha = 0.0}, completion:{(Bool)  in
                    })
                    UIView.animate(withDuration: 1.0, delay: 0.0, options: UIViewAnimationOptions.curveEaseIn, animations: {self.TurnLabel.alpha = 0.0}, completion:nil)
                })
        })
    }
    
    func showVotes()
    {
        if(myVar.puzzleList[myVar.selectedPuzzle].voted == "0")
        {
            UIView.animate(withDuration: 1.0, delay: 0.0, options: UIViewAnimationOptions.curveEaseIn, animations: {self.VoteView.alpha = 1.0}, completion:nil)
        }
    }
    
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    
}

