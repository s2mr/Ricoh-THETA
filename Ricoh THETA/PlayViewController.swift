//
//  PlayViewController.swift
//  Ricoh THETA
//
//  Created by 下村一将 on 2016/08/17.
//  Copyright © 2016年 kazu. All rights reserved.
//

import UIKit

class PlayViewController: UIViewController {
    
    let ad = UIApplication.sharedApplication().delegate as! AppDelegate
    var glkViewController: GlkViewController!
    @IBOutlet var imageView: UIImageView!

    override func viewDidLoad() {
        super.viewDidLoad()
        startGlk()
        // Do any additional setup after loading the view.
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    func startGlk() {
        let image = UIImage(data:ad.toShowImage.first!)
        glkViewController = GlkViewController(imageView.frame, image: ad.toShowImage.first, width: Int32((image?.size.width)!), height: Int32((image?.size.width)!), yaw: 0, roll: 0, pitch: 0)
        glkViewController.view.frame = self.view.frame
        glkViewController.glRenderView.setInertia(100)
        self.view.addSubview(glkViewController.view)
        addChildViewController(glkViewController)
        glkViewController.didMoveToParentViewController(self)
        
//        NSTimer.scheduledTimerWithTimeInterval(3, target: self, selector: #selector(interbal), userInfo: nil, repeats: false)
    }
    
    let i = 1
    func interbal() {
        while i < ad.toShowImage.count {
            let image = UIImage(data:ad.toShowImage[i])
            dispatch_async(dispatch_get_main_queue(), {
                self.glkViewController.glRenderView.setTexture(self.ad.toShowImage[self.i], width: Int32((image?.size.width)!), height: Int32((image?.size.height)!), yaw: 0, pitch: 0, roll: 0)
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
