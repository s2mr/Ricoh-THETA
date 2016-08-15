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
    
    override func viewDidLoad() {
        super.viewDidLoad()

        // Do any additional setup after loading the view.
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    @IBAction func stopButtonPushed(sender: AnyObject) {
        
        func a(session:PtpIpSession!) {
//            session.terminateOpenCapture(0xFFFFFFFF)
        }
        
        ad.ptpConnection.operateSession(a)
        
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
