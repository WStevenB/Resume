
//
//  AppDelegate.swift
//  Chess Puzzles
//
//  Created by PAR on 10/26/15.
//  Copyright © 2015 Babcock. All rights reserved.
//

import UIKit

@UIApplicationMain
class AppDelegate: UIResponder, UIApplicationDelegate {

    var window: UIWindow?


    func application(_ application: UIApplication, didFinishLaunchingWithOptions launchOptions: [UIApplicationLaunchOptionsKey: Any]?) -> Bool {
        // Override point for customization after application launch.
        
        //register background fetch capability
        UIApplication.shared.setMinimumBackgroundFetchInterval(UIApplicationBackgroundFetchIntervalMinimum)
        
        //register notifications - happens when new puzzles are downloaded by background fetch
        if(UIApplication.instancesRespond(to: #selector(UIApplication.registerUserNotificationSettings(_:))))
        {
            application.registerUserNotificationSettings(UIUserNotificationSettings(types: [.sound, .alert], categories: nil))
        }
        
        //load fron NSDefaults - see GlobalData.swift
        loadVariables()
        if(myVar.statList.count == 0) {
            getStats()
        }
        if(myVar.leaderData == "") {
            getLeaders()
        }
        
        //set up images for chess pieces
        variables.piecesList.append(UIImage(named:"wp")!)
        variables.piecesList.append(UIImage(named:"wn")!)
        variables.piecesList.append(UIImage(named:"wb")!)
        variables.piecesList.append(UIImage(named:"wr")!)
        variables.piecesList.append(UIImage(named:"wq")!)
        variables.piecesList.append(UIImage(named:"wk")!)
        variables.piecesList.append(UIImage(named:"bp")!)
        variables.piecesList.append(UIImage(named:"bn")!)
        variables.piecesList.append(UIImage(named:"bb")!)
        variables.piecesList.append(UIImage(named:"br")!)
        variables.piecesList.append(UIImage(named:"bq")!)
        variables.piecesList.append(UIImage(named:"bk")!)
        
        //if not true, means that this is the first time running the app
        //should read puzzles from text file
        if(myVar.loadedOnce != "2")
        {
        myVar.masterPuzzleList = [Puzzle]()
        myVar.divPuzzleLists = [PuzzleList]()
        myVar.loadedOnce = "2"
        do
        {
            let path = Bundle.main.path(forResource: "LibOne", ofType: "txt")
            let text = try NSString(contentsOfFile: path!, encoding: String.Encoding.utf8.rawValue)
            var t = (text as String + "#FINALEND#") as NSString
            while(indexOf(t, find: "#FINALEND#") > 0)
            {
                let id = (t as String).substring(to: getIndex(t as String, indexOf(t, find: "\t")))
                t = (t as String).substring(from: getIndex(t as String, indexOf(t as NSString, find: "\t") + 1)) as NSString
                let author = t.substring(to: indexOf(t, find: "\t"))
                t = (t as String).substring(from: getIndex(t as String, indexOf(t as NSString, find: "\t") + 1)) as NSString
                let turn = t.substring(to: indexOf(t, find: "\t"))
                t = (t as String).substring(from: getIndex(t as String, indexOf(t as NSString, find: "\t") + 1)) as NSString
                let board = t.substring(to: indexOf(t, find: "\t"))
                t = (t as String).substring(from: getIndex(t as String, indexOf(t as NSString, find: "\t") + 1)) as NSString
                let moves = t.substring(to: indexOf(t, find: "\t"))
                t = (t as String).substring(from: getIndex(t as String, indexOf(t as NSString, find: "\t") + 1)) as NSString
                let title = t.substring(to: indexOf(t, find: "\t"))
                t = (t as String).substring(from: getIndex(t as String, indexOf(t as NSString, find: "\t") + 1)) as NSString
                let diff = t.substring(to: indexOf(t, find: "\r"))
                t = (t as String).substring(from: getIndex(t as String, indexOf(t as NSString, find: "\r\n") + 1)) as NSString
                var fMoves = [Point]()
                let m1 = moves.components(separatedBy: "X")
                for i in 0..<m1.count
                {
                    let m2 = m1[i].components(separatedBy: "x")
                    fMoves.append(Point(xp: Int(m2[0])!, yp: Int(m2[1])!))
                }
                myVar.masterPuzzleList.append(Puzzle(l: 0, i: id, a: author, ft: turn, b: board, m: fMoves, t: title, d: diff))
            }
            updateVotes()
            saveVariables2()
            loadVariables()
        }
        catch
        {
           
        }
        }
        
        return true
    }
    
    //background fetch mandatory function
    func application(_ application: UIApplication, performFetchWithCompletionHandler completionHandler: @escaping (UIBackgroundFetchResult) -> Void) {
        fetch() {
            completionHandler(UIBackgroundFetchResult.newData)
            
        }
    }
    
    //used above to load any new puzzles in database
    func fetch(_ completionHandler: (() -> Void)!) {
        if(myVar.libraryList.count == 0)
        {
            loadVariables()
        }
        checkPuzzles()
        completionHandler()
    }
    

    func applicationWillResignActive(_ application: UIApplication) {
        // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
        // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
    }

    func applicationDidEnterBackground(_ application: UIApplication) {
        // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
        // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
        saveVariables()
    }

    func applicationWillEnterForeground(_ application: UIApplication) {
        // Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
    }

    func applicationDidBecomeActive(_ application: UIApplication) {
        // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
    }

    func applicationWillTerminate(_ application: UIApplication) {
        // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
        saveVariables()
    }
    
    struct checkVar
    {
        static var lastDateNoNew = ""
        static var lastNotification = ""
    }
    
    func checkPuzzles()
    {
        //disabled in demo version
    }
    
    func upToDate(_ t:String)
    {
        checkVar.lastDateNoNew = t
    }
    
    //parses downloaded puzzles in NSDefaults
    func newPuzzles(_ t:NSString, today:String)
    {
        var puzzleList = [Puzzle]()
        let defaults = UserDefaults.standard
        if(defaults.object(forKey: "masterPuzzleList") != nil)
        {
            let data = defaults.data(forKey: "masterPuzzleList")!
            puzzleList = NSKeyedUnarchiver.unarchiveObject(with: data) as! [Puzzle]
        }
        var t = t
        while(indexOf(t, find: "#FINALEND#") > 0)
        {
            let id = (t as String).substring(to: getIndex(t as String, indexOf(t, find: "\t")))
            t = (t as String).substring(from: getIndex(t as String, indexOf(t as NSString, find: "\t") + 1)) as NSString
            let author = t.substring(to: indexOf(t, find: "\t"))
            t = (t as String).substring(from: getIndex(t as String, indexOf(t as NSString, find: "\t") + 1)) as NSString
            let turn = t.substring(to: indexOf(t, find: "\t"))
            t = (t as String).substring(from: getIndex(t as String, indexOf(t as NSString, find: "\t") + 1)) as NSString
            let board = t.substring(to: indexOf(t, find: "\t"))
            t = (t as String).substring(from: getIndex(t as String, indexOf(t as NSString, find: "\t") + 1)) as NSString
            let moves = t.substring(to: indexOf(t, find: "\t"))
            t = (t as String).substring(from: getIndex(t as String, indexOf(t as NSString, find: "\t") + 1)) as NSString
            let title = t.substring(to: indexOf(t, find: "\t"))
            t = (t as String).substring(from: getIndex(t as String, indexOf(t as NSString, find: "\t") + 1)) as NSString
            let diff = t.substring(to: indexOf(t, find: "\r"))
            t = (t as String).substring(from: getIndex(t as String, indexOf(t as NSString, find: "\r\n") + 1)) as NSString
            var fMoves = [Point]()
            let m1 = moves.components(separatedBy: "X")
            for i in 0..<m1.count
            {
                let m2 = m1[i].components(separatedBy: "x")
                fMoves.append(Point(xp: Int(m2[0])!, yp: Int(m2[1])!))
            }
            puzzleList.append(Puzzle(l: 0, i: id, a: author, ft: turn, b: board, m: fMoves, t: title, d: diff))
        }
        let saveData = NSKeyedArchiver.archivedData(withRootObject: puzzleList)
        UserDefaults.standard.set(saveData, forKey: "masterPuzzleList")
        
        
        if(today != checkVar.lastNotification)
        {
            checkVar.lastNotification = today
            let localNotification:UILocalNotification = UILocalNotification()
            localNotification.alertAction = "Slide to view latest puzzles!"
            localNotification.alertBody = "You have new puzzles!"
            localNotification.fireDate = Date(timeIntervalSinceNow: 10)
            localNotification.soundName = UILocalNotificationDefaultSoundName;
            UIApplication.shared.scheduleLocalNotification(localNotification)
        }
    }
    
    //used in converting between Swift 2 saved votes/solved format to Swift 3 data format
    func updateVotes() {
        var tempList = [Puzzle]()
        for i in 0..<myVar.libraryList.count {
            let defaults = UserDefaults.standard
            if(defaults.object(forKey: String(i)) != nil)
            {
                let data = defaults.data(forKey: String(i))!
                tempList = NSKeyedUnarchiver.unarchiveObject(with: data) as! [Puzzle]
                for ii in 0..<tempList.count {
                    for iii in 0..<myVar.masterPuzzleList.count {
                        if(myVar.masterPuzzleList[iii].id == tempList[ii].id) {
                            myVar.masterPuzzleList[iii].solved = tempList[ii].solved
                            myVar.masterPuzzleList[iii].voted = tempList[ii].voted
                            break
                        }
                    }
                }
            }
        }
    }


}

