//
//  TakePhoto4ViewController.swift
//  Ricoh THETA
//
//  Created by 下村一将 on 2016/08/05.
//  Copyright © 2016年 kazu. All rights reserved.
//

import UIKit

class TakePhoto4ViewController: UIViewController, UICollectionViewDelegate, UICollectionViewDataSource {
    
    let ad = UIApplication.sharedApplication().delegate as! AppDelegate
    var views = [UIView]()
    
    @IBOutlet weak var collectionView: UICollectionView!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view.
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    override func viewWillAppear(animated: Bool) {
        super.viewWillAppear(animated)
        collectionView.reloadData()
    }
    
    func collectionView(collectionView: UICollectionView, numberOfItemsInSection section: Int) -> Int {
        for _ in 1...ad.receivedData.count {
        views.append(UIView(frame: CGRect(x: 0,y: 0,width: 30,height: 30)))
        }
        return ad.receivedData.count
    }
    
    func collectionView(collectionView: UICollectionView, cellForItemAtIndexPath indexPath: NSIndexPath) -> UICollectionViewCell {
        let cell = collectionView.dequeueReusableCellWithReuseIdentifier("cell", forIndexPath: indexPath) as! CustomCell
        let img = UIImage(data: ad.receivedData[indexPath.row])
        cell.tag = 0
        cell.imageView.image = img
//        cell.dateLabel.text = ad.receivedInfo[indexPath.row].capture_date.decription
        return cell
    }
    
    
    func collectionView(collectionView: UICollectionView, didSelectItemAtIndexPath indexPath: NSIndexPath) {
        
        print("セル選択:" + indexPath.row.description)
        //選択状態：１　、非選択状態：０
        if collectionView.cellForItemAtIndexPath(indexPath)?.tag == 0 {
            views[indexPath.row].backgroundColor = UIColor.cyanColor()
            collectionView.cellForItemAtIndexPath(indexPath)?.tag = 1
            collectionView.cellForItemAtIndexPath(indexPath)?.addSubview(views[indexPath.row])
        }else {
            collectionView.cellForItemAtIndexPath(indexPath)?.tag = 0
            views[indexPath.row].removeFromSuperview()
        }
    }
    

    
    // MARK: - Navigation

    // In a storyboard-based application, you will often want to do a little preparation before navigation
    override func prepareForSegue(segue: UIStoryboardSegue, sender: AnyObject?) {
        // Get the new view controller using segue.destinationViewController.
        // Pass the selected object to the new view controller.
        if segue.identifier == "to5"{
            
            //先頭から削除すると要素数が変わるため後ろから削除
            for i in (0..<ad.receivedData.count).reverse(){
                let cell = collectionView.cellForItemAtIndexPath(NSIndexPath(forRow: i, inSection: 0))
                if cell?.tag == 0 {
                    ad.receivedData.removeAtIndex(i)
                }
            }
            print("receivedDataの個数: " + ad.receivedData.count.description)
        }
    }
}

class CustomCell:UICollectionViewCell {
    @IBOutlet weak var imageView: UIImageView!
    @IBOutlet weak var dateLabel: UILabel!
}
