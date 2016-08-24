//
//  ViewController.swift
//  Ricoh THETA
//
//  Created by 下村一将 on 2016/08/04.
//  Copyright © 2016年 kazu. All rights reserved.
//

import UIKit

class ConnectViewController: UIViewController{

    let ad = UIApplication.sharedApplication().delegate as! AppDelegate
    
    @IBOutlet weak var notConnectedLabel: UILabel!
    @IBOutlet weak var connectedLabel: UILabel!
    @IBOutlet weak var batteryLevelLabel: UILabel!    
    
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        navigationItem.title = "接続"

        let myViewControllers = self.tabBarController!.viewControllers
        _ = (myViewControllers![3] as! HowToViewController).view // 1...B TAB index
        _ = (myViewControllers![2] as! CollectionViewController).view // 1...B TAB index
        ad.ptpConnection = PtpConnection()
        ad.ptpConnection.setTargetIp("192.168.1.1")
        
        ad.load("receivedData")
        ad.load("receivedHandler")
        ad.load("receivedInfo")
    
        // Do any additional setup after loading the view, typically from a nib.
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    override func viewWillAppear(animated: Bool) {
        super.viewWillAppear(animated)
        ad.ptpConnection.connect(a)
    }
    
    func a(connected:Bool) {
        if connected {
            print("接続完了")
            //UI処理のためメインスレッドで実行
            dispatch_async(dispatch_get_main_queue(), {
                self.connectedLabel.textColor = UIColor(red: 80/255, green: 203/255, blue: 134/255, alpha: 1)
                self.notConnectedLabel.textColor = UIColor(red: 80/255, green: 203/255, blue: 134/255, alpha: 0.1)
            })
            ad.ptpConnection.operateSession(b)
            
        } else {
            print("接続失敗")
            dispatch_async(dispatch_get_main_queue(), {
                self.notConnectedLabel.textColor = UIColor(red: 80/255, green: 203/255, blue: 134/255, alpha: 1)
                self.connectedLabel.textColor = UIColor(red: 80/255, green: 203/255, blue: 134/255, alpha: 0.1)
            })
            return
        }
    }
    
    func b(session:PtpIpSession!) {
        dispatch_async(dispatch_get_main_queue(), {
            self.batteryLevelLabel.hidden = false
            self.batteryLevelLabel.text = "Battery Level : " + session.getBatteryLevel().description + "%"
        })
    }
    
}

