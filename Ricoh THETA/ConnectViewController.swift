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
        
        ad.ptpConnection = PtpConnection()
        ad.ptpConnection.setTargetIp("192.168.1.1")
        
        ad.load("receivedData")
        ad.load("receivedHandler")
        ad.load("receivedInfo")
        
        func a(connected:Bool) {
            if connected {
                print("接続完了")
                //UI処理のためメインスレッドで実行
                dispatch_async(dispatch_get_main_queue(), {
                    self.connectedLabel.textColor = UIColor.blueColor()
                })
                ad.ptpConnection.operateSession(b)
                
            } else {
                print("接続失敗")
                dispatch_async(dispatch_get_main_queue(), {
                    self.notConnectedLabel.textColor = UIColor.blueColor()
                })
                return
            }
        }
        
        ad.ptpConnection.connect(a)
        
        
        
        func b(session:PtpIpSession!) {
            dispatch_async(dispatch_get_main_queue(), {
                self.batteryLevelLabel.text = "バッテリーレベル: " + session.getBatteryLevel().description + "%"
            })
        }
        
    
        // Do any additional setup after loading the view, typically from a nib.
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }


}

