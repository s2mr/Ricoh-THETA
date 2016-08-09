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
    @IBOutlet weak var imageView: UIImageView!
    
    
    
    override func viewDidLoad() {
        super.viewDidLoad()
        ad.ptpConnection = PtpConnection()
        ad.ptpConnection.setTargetIp("192.168.1.1")
        
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
        
        ad.ptpConnection.connect(a)
        
        
        
        func b(session:PtpIpSession!) {
            
            
//            print("storageId:" + session.getStorageIDs().description)
//            print("storageInfo: " + session.getStorageInfo().description)
            
            
            
            let objectHandles = session.getObjectHandles()
            
            print(objectHandles)
            
            let objectInfo = session.getObjectInfo(objectHandles[2] as! Int)
            
            print("Date:::: "+objectInfo.capture_date.description + "   fileName:::"+objectInfo.filename)
        
//            let thumb0 = session.getThumb(objectHandles[36] as! Int)
//
//            func a(data : NSData!) -> Bool {
//                dispatch_async(dispatch_get_main_queue(), {
//                    //キャスト不可のためnil代入
//                    let image = UIImage(data: data)
//                    self.imageView.image = image
//                })
//                return true
//            }
//            
//            imageView.contentMode = .ScaleToFill
//            let thumb = session.getResizedImageObject(objectHandles[2] as! Int, width: 2048, height: 1024, onStartData: nil, onChunkReceived: a)
            
            let thumb2 = session.getObject(objectHandles[2] as! Int)
            print("FileName:   " + thumb2.filename)
            
            let data = thumb2.readBytes(1024)
            let image = UIImage(data: data)
            print("イメージの大きさ：　　" + (image?.size.height.description)!)
            
            dispatch_async(dispatch_get_main_queue(), {
                self.imageView.image = image
            })

            
            
            
//            let data:NSData = image.readBytes(1024)
//                session.getObject(objectHandle[0] as! Int)
            
//            let imageUI = UIImage(data: data)
//            let imageView = UIImageView(image: imageUI)
//            imageView.layer.position = CGPoint(x: self.view.frame.width * 0.5, y: self.view.frame.height * 0.5)
//            imageView.contentMode = .ScaleToFill
            dispatch_async(dispatch_get_main_queue(), {
                self.batteryLevelLabel.text = "バッテリーレベル: " + session.getBatteryLevel().description + "%"
//                self.imageView.image = imageUI

            })
        }
        
        ad.ptpConnection.operateSession(b)
    
        // Do any additional setup after loading the view, typically from a nib.
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }


}

