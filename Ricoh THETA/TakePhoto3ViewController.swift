//
//  TakePhoto3ViewController.swift
//  Ricoh THETA
//
//  Created by 下村一将 on 2016/08/05.
//  Copyright © 2016年 kazu. All rights reserved.
//

import UIKit

class TakePhoto3ViewController: UIViewController {

    let ad = UIApplication.sharedApplication().delegate as! AppDelegate
    
    @IBOutlet weak var progressView: UIProgressView!
    @IBOutlet weak var progressLabel: UILabel!
    @IBOutlet var partLabel: UILabel!
    @IBOutlet weak var imageView: UIImageView!
    

    override func viewDidLoad() {
        super.viewDidLoad()
        
        if ad.ptpConnection.connected() {
            ad.ptpConnection.operateSession(load)
        }else {
//            アラートを閉じる
//            self.dismissViewControllerAnimated(true, completion: nil)
            self.performSegueWithIdentifier("receiveFinished", sender: self)
        }

        // Do any additional setup after loading the view.
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    var totalLength:UInt!

    
    func load(session:PtpIpSession!) {
        let objectHandles = session.getObjectHandles()
        print(objectHandles)
        
        
//        for handle in objectHandles as! [Int] {
//            let objectInfo = session.getObjectInfo(handle)
//            print("Date:::: "+objectInfo.capture_date.description + "fileName:::"+objectInfo.filename)
//        }
        
        var receivedData = NSMutableData()
        
        //呼ばれるまで実行されない
        func a(data : NSData!) -> Bool {
            receivedData.appendData(data)
        
//            print("totalLength : "  + totalLength.description)
//            print("receivedData : " + receivedData.length.description)
//            print(Float(receivedData.length) / Float(totalLength))
            
//            if UIImage(data: receivedData) != nil {
                dispatch_async(dispatch_get_main_queue(), {
                    self.progressView.setProgress(Float(receivedData.length) / Float(self.totalLength), animated: true)
                })
//            }
            return true
        }
        //呼ばれるまで実行されない
        func b(totalLength:UInt) {
            self.totalLength = totalLength
        }
        
        var isOnce = false
        var total:Int!
        for (index, handle) in objectHandles.enumerate() {
            let handle = handle as! Int
            
            //受信済みハンドラーにhandleが含まれていない場合に取得、受信
            if !ad.receivedHandler.contains(handle) {
                if !isOnce {
                    //合計枚数
                    total = objectHandles.count - index
                    isOnce = true
                }
                let objectInfo = session.getObjectInfo(handle)
                //JPEGの場合
                if objectInfo.object_format == 0x3801 {
                    session.getResizedImageObject(handle, width: 2048, height: 1024, onStartData: b, onChunkReceived: a)
                    //画像取得後に配列に追加
                    //残り枚数
                    let nokori = objectHandles.count - index
                    ad.receivedData.append(receivedData)
                    ad.receivedHandler.append(handle)
//                    ad.receivedInfo.append(objectInfo)
                    
                    dispatch_async(dispatch_get_main_queue(), {
                        self.partLabel.text = "残り\(nokori)枚"
                        self.imageView.image = UIImage(data: self.ad.receivedData.last!)
                        self.progressLabel.text = "".stringByAppendingFormat("%.2f",(Float(index) / Float(objectHandles.count) * 100)) + "%"
                        self.progressView.setProgress(0, animated: false)
                    })
                    
                    receivedData = NSMutableData()
                }
            }
            print("".stringByAppendingFormat("%.2f",(Float(index) / Float(objectHandles.count) * 100)) + "%")
        }
    
//        let ac = UIAlertController(title: "お待ちください", message: "画像をアプリ内に保存しています", preferredStyle: .Alert)
//        self.presentViewController(ac, animated: true, completion: nil)
        
        //遷移する直前
        print("データの個数: " + ad.receivedData.count.description)
        print("ハンドルの個数: " + ad.receivedHandler.count.description)
        ad.save(ad.receivedData, key: "receivedData")
        ad.save(ad.receivedHandler, key: "receivedHandler")
        print("保存完了")
//        self.dismissViewControllerAnimated(true, completion: {
            self.performSegueWithIdentifier("receiveFinished", sender: self)
//        })
//        NSTimer.scheduledTimerWithTimeInterval(0.5, target: self, selector: #selector(dismiss), userInfo: nil, repeats: false)
        //        ad.save(ad.receivedInfo, key: "receivedInfo")
        
    }
    
    func dismiss() {
        self.dismissViewControllerAnimated(true, completion: {
            self.performSegueWithIdentifier("receiveFinished", sender: self)
        })
    }
    
    
    

    
    // MARK: - Navigation

    // In a storyboard-based application, you will often want to do a little preparation before navigation
    override func prepareForSegue(segue: UIStoryboardSegue, sender: AnyObject?) {
        // Get the new view controller using segue.destinationViewController.
        // Pass the selected object to the new view controller.
        if segue.identifier == "receiveFinished" {
//            self.dismissViewControllerAnimated(true, completion: nil)
        }
    }
 

}
