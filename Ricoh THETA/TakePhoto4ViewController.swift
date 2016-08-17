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
//    var checkMark:UIView!
    var checkArray:NSMutableArray = []
    
    @IBOutlet weak var collectionView: UICollectionView!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        self.navigationController?.navigationBarHidden = true
//        checkMark = UIView(frame: CGRect(x: 0, y: 0, width: 30, height: 30))
//        checkMark.backgroundColor = UIColor.cyanColor()
        // Do any additional setup after loading the view.
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    override func viewWillAppear(animated: Bool) {
        super.viewWillAppear(animated)
//        views = [UIView]()
        collectionView.reloadData()
    }
    
    @IBAction func doneButtonTapped(sender: AnyObject) {
        if checkArray.count > 0 {
            performSegueWithIdentifier("to5", sender: self)
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
//        for _ in 0..<ad.receivedData.count {
//            views.append(UIView(frame: CGRect(x: 0,y: 0,width: 30,height: 30)))
////            ad.picTags.append(0)
//        }
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
            let checkImg = UIImage(named: "checkMark")
            let checkView = UIImageView(frame: CGRect(x: 0, y: 0, width: (checkImg?.size.width)!, height: (checkImg?.size.height)!))
            checkView.tag = 100
            checkView.image = checkImg
            
            print("セル作成！")
//            print(indexPath.row)
            
//            views[indexPath.row].backgroundColor = UIColor.cyanColor()
            cell.contentView.addSubview(checkView)
        }else{
            print("何も表示しません")
        }
        
//        cell.dateLabel.text = ad.receivedInfo[indexPath.row].capture_date.decription
        return cell
    }
    
    
    func collectionView(collectionView: UICollectionView, didSelectItemAtIndexPath indexPath: NSIndexPath) {
        
        print("セル選択:" + indexPath.row.description)
        //選択状態：１　、非選択状態：０
//        if ad.picTags[indexPath.row] == 0 {
//            views[indexPath.row].backgroundColor = UIColor.cyanColor()
//            ad.picTags[indexPath.row] = 1
//            collectionView.cellForItemAtIndexPath(indexPath)?.addSubview(views[indexPath.row])
//        }else {
//            ad.picTags[indexPath.row] = 0
//            views[indexPath.row].removeFromSuperview()
//        }
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
        if segue.identifier == "to5"{
            
            //先頭から削除すると要素数が変わるため後ろから削除
//            for i in (0..<ad.receivedData.count).reverse(){
//                print("Cellのタグ: " + ad.picTags[i].description)
//                if ad.picTags[i] == 0 {
//                    ad.receivedData.removeAtIndex(i)
//                }
//            }
            
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
