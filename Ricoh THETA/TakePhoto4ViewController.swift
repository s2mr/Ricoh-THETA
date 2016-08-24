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
//    var views = [UIView]()
    var checkArray:NSMutableArray = []
    
    @IBOutlet weak var collectionView: UICollectionView!
    
    override func viewDidLoad() {
        super.viewDidLoad()
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    override func viewWillAppear(animated: Bool) {
        super.viewWillAppear(animated)
        self.navigationController?.navigationBarHidden = true
        collectionView.reloadData()
    }
    
    @IBAction func doneButtonTapped(sender: AnyObject) {
        if checkArray.count > 0 {
            performSegueWithIdentifier("to6", sender: self)
        } else {
            let ac = UIAlertController(title: "エラー", message: "画像を選択してください", preferredStyle: .Alert)
            let okButton = UIAlertAction(title: "OK", style: .Default, handler:{ (action: UIAlertAction) in
                print("OK")
            })
            ac.addAction(okButton)
            presentViewController(ac, animated: true, completion: nil)
        }
    }
    
    
    func collectionView(collectionView: UICollectionView, numberOfItemsInSection section: Int) -> Int {
        return ad.receivedData.count
    }
    
    func collectionView(collectionView: UICollectionView, cellForItemAtIndexPath indexPath: NSIndexPath) -> UICollectionViewCell {
        let cell = collectionView.dequeueReusableCellWithReuseIdentifier("cell", forIndexPath: indexPath) as! CustomCell
    
        for subview in cell.contentView.subviews{
            if subview.tag == 100 {
                subview.removeFromSuperview()
            }
        }
        
        
        let img = UIImage(data: ad.receivedData[indexPath.row])
        cell.imageView.image = img
    
        if checkArray.containsObject(indexPath){
            let checkImg = UIImage(named: "ok")
            let checkView = UIImageView(frame: CGRect(x: 0, y: 0, width: (checkImg?.size.width)!, height: (checkImg?.size.height)!))
            checkView.backgroundColor = UIColor.grayColor()
            checkView.tag = 100
            checkView.image = checkImg
            print("セル作成！")
            cell.contentView.addSubview(checkView)
        }else{
            print("何も表示しません")
        }
        return cell
    }
    
    
    func collectionView(collectionView: UICollectionView, didSelectItemAtIndexPath indexPath: NSIndexPath) {
        
        print("セル選択:" + indexPath.row.description)
        //選択状態：１　、非選択状態：０
        if checkArray.containsObject(indexPath){
            print("チェックArrayから削除！")
            checkArray.removeObject(indexPath)
        }else{
            print("チェックArrayに追加！")
            checkArray.addObject(indexPath)
        }
        collectionView.reloadData()
        
    }
    

    
    // MARK: - Navigation

    // In a storyboard-based application, you will often want to do a little preparation before navigation
    override func prepareForSegue(segue: UIStoryboardSegue, sender: AnyObject?) {
        // Get the new view controller using segue.destinationViewController.
        // Pass the selected object to the new view controller.
        if segue.identifier == "to6"{
            //初期化
            ad.toShowImage.removeAll()

//            古いものが最初に表示されるようにソート
            let sortedArray = checkArray.sort {$0.row < $1.row}
            
            for index in sortedArray {
                ad.toShowImage.append(ad.receivedData[index.row])
            }
            
            print("receivedDataの個数: " + ad.receivedData.count.description)
            print("toShowImageの個数: " + ad.toShowImage.count.description)
        }
    }
}

class CustomCell:UICollectionViewCell {
    @IBOutlet weak var imageView: UIImageView!
    @IBOutlet weak var dateLabel: UILabel!
}
