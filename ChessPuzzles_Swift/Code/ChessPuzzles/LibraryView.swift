//
//  SolveView.swift
//  Chess Puzzles
//
//  Created by PAR on 10/28/15.
//  Copyright © 2015 Babcock. All rights reserved.
//

import UIKit


class SolveView: UIViewController {
    
    //disabled in demo version
    @IBAction func HomeClick(_ sender: AnyObject) {
    }
    
    static var LibraryScroller: UIScrollView!
    static var PuzzleScroller: UIScrollView!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        let width = self.view.frame.width
        let height = self.view.frame.height
        SolveView.LibraryScroller = UIScrollView(frame: CGRect(x: 15, y: 70, width: width - 30, height: 30))
        self.view.addSubview(SolveView.LibraryScroller)
        SolveView.PuzzleScroller = UIScrollView(frame: CGRect(x: 40, y: 130, width: width - 80, height: height - 150))
        self.view.addSubview(SolveView.PuzzleScroller)
        self.view.backgroundColor = UIColorFromRGB2("e5ccb3")
        variables.libList = [UIView]()
        var xCoor = 0
        var scrollPosition = 0
        
        //dynamically adds libraries to top scrollview
        for i in 0 ..< myVar.divPuzzleLists.count
        {
            myVar.divPuzzleLists[i].setData()
            let square: UIView = UIView(frame: CGRect(x: xCoor, y: 0, width: 160, height: 30))
            square.layer.borderWidth = 2
            square.layer.borderColor = UIColor.clear.cgColor
            let square2: UILabel = UILabel(frame: CGRect(x: 0, y: 0, width: 160, height: 30))
            square2.textAlignment = NSTextAlignment.center
            square2.text = "Library #" + String(i + 1) + " (" + String(myVar.divPuzzleLists[i].totalSolved) + "/" + String(myVar.divPuzzleLists[i].total)
            var color = "996633"
            if(i % 2 == 1)
            {
                color = "ac7339"
            }
            if(myVar.selectedLibrary >= 100)
            {
                color = "99ff99"
                if(i % 2 == 1)
                {
                    color = "80ff80"
                }
            }
            square.layer.insertSublayer(createGradient(color,bounds: square.bounds,round: 5 as CGFloat), at:0)
            square.tag = i
            let tapGesture = UITapGestureRecognizer(target: self, action: #selector(SolveView.libraryClick(_:)))
            square.addGestureRecognizer(tapGesture)
            square.addSubview(square2)
            xCoor += 175
            if(i == myVar.selectedLibrary)
            {
                scrollPosition = xCoor - 175
            }
            SolveView.LibraryScroller.addSubview(square)
            variables.libList.append(square)
        }
        if(scrollPosition - 60 > 0)
        {
            scrollPosition -= 60
        }
        SolveView.LibraryScroller.contentSize = CGSize(width: CGFloat(xCoor + 120), height: CGFloat(30))
        SolveView.LibraryScroller.setContentOffset (CGPoint(x: scrollPosition,y: 0), animated: false)
        loadPuzzles(myVar.selectedLibrary)
    }
    
    //removes border from any selected library
    func clearBorders()
    {
        for i in 0 ..< variables.libList.count
        {
            variables.libList[i].layer.borderColor = UIColor.clear.cgColor
        }
    }
    
    //happens when user click a library
    //will display puzzles from that library
    func libraryClick(_ recognizer: UITapGestureRecognizer)
    {
        if(recognizer.view!.tag > 0 && myVar.hasCreatedPuzzle == "-1") {
            let alert = UIAlertController(title: "Unlock Library", message: "Please register an account, and create at least one puzzle of your own to unlock all libraries. Only the first one hundred problems will be available until then.", preferredStyle: UIAlertControllerStyle.alert)
            alert.addAction(UIAlertAction(title: "OK", style: UIAlertActionStyle.default, handler: {(UIAlertAction) in
            }))
            self.present(alert, animated: true, completion: nil)
        }
        else {
            loadPuzzles(recognizer.view!.tag)
        }
    }
    
    //places puzzles from a particular library in bottom scrollview
    func loadPuzzles(_ id:Int)
    {
        clearBorders()
        variables.libList[id].layer.borderColor = UIColor.black.cgColor
        let subViews = SolveView.PuzzleScroller.subviews
        for subview in subViews{
            subview.removeFromSuperview()
        }
        myVar.selectedLibrary = id
        myVar.puzzleList = myVar.divPuzzleLists[id].puzzles
        for i in 0 ..< myVar.puzzleList.count
        {
            if(myVar.puzzleList[i].solved != "1")
            {
                myVar.selectedPuzzle = i
                break
            }
        }
        var yCoor = 0
        var yPosition = 0
        for i in 0 ..< myVar.puzzleList.count
        {
            let square: UIView = UIView(frame: CGRect(x: 0, y: yCoor, width: Int(SolveView.PuzzleScroller.frame.width), height: 30))
            square.layer.borderWidth = 2
            square.layer.borderColor = UIColorFromRGB2("734d26").cgColor
            let square2: UILabel = UILabel(frame: CGRect(x: 0, y: 0, width: Int(SolveView.PuzzleScroller.frame.width), height: 30))
            square2.textAlignment = NSTextAlignment.center
            var diff = "◉"
            if(myVar.puzzleList[i].diff == "1")
            {
                diff = "◼︎"
            }
            else if(myVar.puzzleList[i].diff == "2")
            {
                diff = "♦︎"
            }
            var title = myVar.puzzleList[i].title
            if((title as NSString).length > 30)
            {
                title = (title as NSString).substring(to: 27) + "..."
            }
            square2.text = diff + " " + title
            var color = "d2a679"
            if(i % 2 == 1)
            {
                color = "c68c53"
            }
            if(myVar.puzzleList[i].solved == "1")
            {
                color = "99ff99"
                if(i % 2 == 1)
                {
                    color = "80ff80"
                }
                square.layer.borderColor = UIColor.black.cgColor
            }
            square.layer.insertSublayer(createGradient(color,bounds: square.bounds,round: 0 as CGFloat), at:0)
            square.tag = i
            let tapGesture = UITapGestureRecognizer(target: self, action: #selector(SolveView.puzzleClick(_:)))
            square.addGestureRecognizer(tapGesture)
            square.addSubview(square2)
            SolveView.PuzzleScroller.addSubview(square)
            yCoor += 45
            if(myVar.selectedPuzzle == i && i > 8)
            {
                yPosition = yCoor - 45
            }
        }
        if(yPosition - 150 > 0)
        {
            yPosition -= 150
        }
        SolveView.PuzzleScroller.contentSize = CGSize(width: SolveView.PuzzleScroller.frame.width, height: CGFloat(yCoor + 50))
        SolveView.PuzzleScroller.setContentOffset (CGPoint(x: 0,y: yPosition), animated: false)
    }
    
    //occurs when user clicks on a puzzle
    //opens solving puzzle view controller
    func puzzleClick(_ recognizer: UITapGestureRecognizer)
    {
        let id = recognizer.view!.tag
        myVar.selectedPuzzle = id
        let mainStoryboard = UIStoryboard(name: "Main", bundle: Bundle.main)
        let vc : UIViewController = mainStoryboard.instantiateViewController(withIdentifier: "Puzzle")
        self.present(vc, animated: true, completion: nil)
    }
    
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    
}




/*_______________________________________________________________________________________________________________________
_______________________________________________________________________________________________________________________
_______________________________________________________________________________________________________________________
_______________________________________________________________________________________________________________________
_______________________________________________________________________________________________________________________
_______________________________________________________________________________________________________________________*/

func solvedRequest(_ id: String, errors: String, username: String)
{
     //disabled in demo version
}

func attemptRequest(_ id: String, username: String)
{
    //disabled in demo version
}

func upvoteRequest(_ id:String)
{
    //disabled in demo version
}

func downvoteRequest(_ id:String)
{
    //disabled in demo version
}

func libraryRequest(_ name:String)
{
    //disabled in demo version
}


