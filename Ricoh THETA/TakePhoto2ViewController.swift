//
//  TakePhoto2ViewController.swift
//  Ricoh THETA
//
//  Created by 下村一将 on 2016/08/05.
//  Copyright © 2016年 kazu. All rights reserved.
//

import UIKit

class TakePhoto2ViewController: UIViewController {

    let ad = UIApplication.sharedApplication().delegate as! AppDelegate
    var interbal:Int!
    var limitedSec:Int = 0
    var timer: NSTimer!
    
    @IBOutlet weak var secLabel: UILabel!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        limitedSec = interbal
        // Do any additional setup after loading the view.
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    override func viewWillAppear(animated: Bool) {
        super.viewWillAppear(animated)
        secLabel.text = interbal.description
        timer = NSTimer.scheduledTimerWithTimeInterval(1, target: self, selector: #selector(dec), userInfo: nil, repeats: true)
    }
    
    @IBAction func stopButtonPushed(sender: AnyObject) {
        
        func a(session:PtpIpSession!) {
            session.terminateOpenCapture(0xFFFFFFFF)
        }
        timer.invalidate()
        if ad.ptpConnection.connected(){
            ad.ptpConnection.operateSession(a)
        }
        
    }
    
    var isOnce = true
    
    func dec() {
        if limitedSec > 0 {
            
            if limitedSec == 1 {
                //            if isOnce {
                func a(session:PtpIpSession!) {
                    //                    session.setTimelapseInterval(interbal * 1000)
                    //                    session.setStillCaptureMode(0x0003)
                    session.initiateCapture()
                }
                ad.ptpConnection.operateSession(a)
                isOnce = false
            }
        limitedSec -= 1
            dispatch_async(dispatch_get_main_queue(), {
                self.secLabel.text = self.limitedSec.description
            })
        }else {
//            }
            limitedSec = interbal
            dispatch_async(dispatch_get_main_queue(), {
                self.secLabel.text = self.interbal.description
            })
        }
    }
    
    
    

    /*
    // MARK: - Navigation

    // In a storyboard-based application, you will often want to do a little preparation before navigation
    override func prepareForSegue(segue: UIStoryboardSegue, sender: AnyObject?) {
        // Get the new view controller using segue.destinationViewController.
        // Pass the selected object to the new view controller.
    }
    */

}
