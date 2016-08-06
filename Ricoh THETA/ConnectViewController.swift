//
//  ViewController.swift
//  Ricoh THETA
//
//  Created by 下村一将 on 2016/08/04.
//  Copyright © 2016年 kazu. All rights reserved.
//

import UIKit

class ConnectViewController: UIViewController{

    var ptpConnection :PtpConnection!
    var session :PtpIpSession!
    
    @IBOutlet weak var notConnectedLabel: UILabel!
    @IBOutlet weak var connectedLabel: UILabel!
    
    
    override func viewDidLoad() {
        super.viewDidLoad()
        ptpConnection = PtpConnection()
        ptpConnection.setTargetIp("192.168.1.1")
        
        func a(connected:Bool) {
            if connected {
                print("接続完了")
                
                //UI処理のためメインスレッドで実行
                dispatch_async(dispatch_get_main_queue(), {
                    self.connectedLabel.textColor = UIColor.blueColor()
                })
                
            } else {
                print("接続失敗")
                dispatch_async(dispatch_get_main_queue(), {
                    self.notConnectedLabel.textColor = UIColor.blueColor()
                })
            
            }
        }
        
        ptpConnection.connect(a)
        
        func b(session:PtpIpSession!) {
            print("セッション")
            session.initiateCapture()
        }
        
//        ptpConnection.operateSession(b)
    
        // Do any additional setup after loading the view, typically from a nib.
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }


}

