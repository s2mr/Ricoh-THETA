//
//  VRViewController.swift
//  Ricoh THETA
//
//  Created by 下村一将 on 2016/08/18.
//  Copyright © 2016年 kazu. All rights reserved.
//

import UIKit

class VRViewController: UIViewController {
    
    @IBOutlet var panoView: GCSPanoramaView!
    let ad = UIApplication.sharedApplication().delegate as! AppDelegate

    override func viewDidLoad() {
        super.viewDidLoad()
        panoView.enableFullscreenButton = true
        panoView.enableFullscreenButton = true
        panoView.loadImage(UIImage(data: ad.toShowImage.first!))
        // Do any additional setup after loading the view.
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
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
