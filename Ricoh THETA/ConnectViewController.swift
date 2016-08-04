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
    
    
    
    override func viewDidLoad() {
        super.viewDidLoad()
        ptpConnection = PtpConnection()
        ptpConnection.setTargetIp("192.168.1.1")
        
        func a(connected:Bool) {
            if connected {
                print("接続完了")
            } else {
                print("接続失敗")
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

