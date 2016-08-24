//
//  CollectionViewController.swift
//  Ricoh THETA
//
//  Created by 下村一将 on 2016/08/16.
//  Copyright © 2016年 kazu. All rights reserved.
//

import UIKit
import Photos
import CoreMedia
import AVFoundation
import AVKit

class CollectionViewController: UIViewController, UICollectionViewDelegate, UICollectionViewDataSource, AVPlayerViewControllerDelegate,  UIGestureRecognizerDelegate {

    let ad = UIApplication.sharedApplication().delegate as! AppDelegate
    var player:AVPlayer!
    var controller: AVPlayerViewController!
    var assets = [PHAsset?]()
    var collectionImage = [UIImage?]()
//    var selectedAsset:PHAsset?
    @IBOutlet weak var collectionView: UICollectionView!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        pickMovie()
//        pickMovie()
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
    
    /*Delegate Method */
    
    func collectionView(collectionView: UICollectionView, numberOfItemsInSection section: Int) -> Int {
        print("CollectionView Reload!!")
        print("collectionImage.count : " + collectionImage.count.description)
        print("assets.count : " + assets.count.description)
        return collectionImage.count
    }
    
    func collectionView(collectionView: UICollectionView, cellForItemAtIndexPath indexPath: NSIndexPath) -> UICollectionViewCell {
        print(indexPath.row.description + "番目のセルを生成！")
        let cell = collectionView.dequeueReusableCellWithReuseIdentifier("cell", forIndexPath: indexPath) as! CustomCellForCollection
        cell.imageView.image = collectionImage[indexPath.row]
        let date =  assets[indexPath.row]?.creationDate
        let formatter = NSDateFormatter()
        formatter.timeStyle = .NoStyle
        formatter.dateStyle = .ShortStyle
        cell.dateLabel.text = formatter.stringFromDate(date!)
        return cell
    }
    
    func collectionView(collectionView: UICollectionView, didSelectItemAtIndexPath indexPath: NSIndexPath) {
        let manager = PHImageManager()
        let options = PHVideoRequestOptions()
        options.deliveryMode = .Automatic
        options.networkAccessAllowed = true
        manager.requestAVAssetForVideo(assets[indexPath.row]!, options: options, resultHandler: { avAsset, audioMix, info in
          self.play(avAsset!)
        })
    }
    
    // Screenサイズに応じたセルサイズを返す
    // UICollectionViewDelegateFlowLayoutの設定が必要
    func collectionView(collectionView: UICollectionView, layout collectionViewLayout: UICollectionViewLayout, sizeForItemAtIndexPath indexPath: NSIndexPath) -> CGSize {
        let cellSize:CGFloat = self.view.frame.size.width/2-6
        let scale = UIScreen.mainScreen().scale
        return CGSizeMake(cellSize, cellSize * scale)
    }
    
    func playerViewControllerDidStartPictureInPicture(playerViewController: AVPlayerViewController) {
//        self.dismissMoviePlayerViewControllerAnimated()
        
    }
    
    func playerViewControllerDidStopPictureInPicture(playerViewController: AVPlayerViewController) {
        print("playerViewControllerDidStopPictureInPicture(playerViewController: AVPlayerViewController)")
        playerViewController.dismissViewControllerAnimated(true, completion: nil)
    }
    
    func playerViewControllerShouldAutomaticallyDismissAtPictureInPictureStart(playerViewController: AVPlayerViewController) -> Bool {
        return true
    }
    
//    func dismissMoviePlayerViewControllerAnimated() {
//        <#code#>
//    }
    
    /*Delegate Method*/
    
    func pickMovie() {
        
        print("width : " + UIScreen.mainScreen().bounds.width.description)
        collectionView.backgroundColor = UIColor.whiteColor()
        
        //全てのカメラロールの動画を取得する。
        print("配列リセットします")
        collectionImage = [UIImage]()
        self.assets = [PHAsset]()
            
        let assets:PHFetchResult = PHAsset.fetchAssetsWithMediaType(PHAssetMediaType.Video, options:nil)
        print(assets.debugDescription)
        assets.enumerateObjectsUsingBlock({ obj, idx, stop in
            
            if obj is PHAsset {
                let asset:PHAsset = obj as! PHAsset
                //                解像度なので　* ２
                if asset.pixelWidth == Int(UIScreen.mainScreen().bounds.width) * 2 {
                    /*
                     print("Asset IDX:\(idx)")
                     print("mediaType:\(asset.mediaType)")
                     print("mediaSubtypes:\(asset.mediaSubtypes)")
                     print("pixelWidth:\(asset.pixelWidth)")
                     print("pixelHeight:\(asset.pixelHeight)")
                     print("creationDate:\(asset.creationDate)")
                     print("modificationDate:\(asset.modificationDate)")
                     print("duration:\(asset.duration)")
                     print("favorite:\(asset.favorite)")
                     print("hidden:\(asset.hidden)")
                     */
                    self.assets.append(asset)
                    
                    let option = PHImageRequestOptions()
                    option.resizeMode = .Exact
                    //                    option.deliveryMode = .HighQualityFormat
                    
                    let phimgr:PHImageManager = PHImageManager()
                    phimgr.requestImageForAsset(asset,
                        targetSize: CGSize(width:1500, height: 2668),
                    contentMode: .AspectFill, options: option) { image, info in
                        //ここでUIImageを取得します。
                        //self.photoImageView.image = image
                        let isDegraded = info![PHImageResultIsDegradedKey]?.boolValue
                        
                        //２回目の高画質画像が返ってきた時のみ配列に追加
                        if  isDegraded == true { return }
                        if image != nil {
                            print("UIImage get!")
                            self.collectionImage.append(image!)
                            print("self.ad.collectionImage.count : " + self.collectionImage.count.description)
                        }
                        
                    }
                    
                }
            }
        })
        dispatch_async(dispatch_get_main_queue(), {
            self.collectionView.reloadData()
        })
    }
    
    func play(asset : AVAsset) {
        let item = AVPlayerItem(asset: asset)
        //        player = AVPlayer(playerItem: item)
        
        let avPlayer = AVPlayer(playerItem: item)
        controller = AVPlayerViewController()
        //        let items = UIBarButtonItem(title: "Back", style: .Plain, target: self, action: nil)
//        controller.delegate = self
//        controller.setToolbarItems([items], animated: true)
        controller.view.frame = CGRectMake(0, 0, self.view.bounds.width, self.view.bounds.height)
//        controller.showsPlaybackControls = true
        controller.player = avPlayer
//        controller.view.frame = self.view.frame
        

        
        // single swipe down
        let swipeDownGesture: UISwipeGestureRecognizer = UISwipeGestureRecognizer(target: self, action: Selector("handleSwipeDown:"))
        swipeDownGesture.delegate = self
        swipeDownGesture.numberOfTouchesRequired = 1
        swipeDownGesture.direction = UISwipeGestureRecognizerDirection.Down
        controller.view.addGestureRecognizer(swipeDownGesture)
        
        /*
        
        // Viewを生成.
        let videoPlayerView = AVPlayerView(frame: self.view.bounds)
        
        // UIViewのレイヤーをAVPlayerLayerにする.
        let layer = videoPlayerView.layer as! AVPlayerLayer
        layer.videoGravity = AVLayerVideoGravityResizeAspect
        layer.player = player
        
 */
//        let modalPlayer = UIViewController()
//        
//        modalPlayer.view.backgroundColor = UIColor.whiteColor()
//        modalPlayer.view = UIView(frame: self.view.frame)
////        modalPlayer.view.layer.addSublayer(layer)
//        modalPlayer.addChildViewController(controller)
//        dispatch_async(dispatch_get_main_queue(), {
//            self.presentViewController(modalPlayer, animated: true, completion: nil)
//        }
        
        self.presentViewController(controller, animated: true, completion: nil)
//        controller.view.frame = self.view.frame
        controller.player?.play()
        
        NSNotificationCenter.defaultCenter().addObserver(self, selector: #selector(playerItemDidReachEnd), name: AVPlayerItemDidPlayToEndTimeNotification, object: avPlayer.currentItem)

    }
    
    func playerItemDidReachEnd() {
        controller.dismissViewControllerAnimated(true, completion: nil)
    }
    
    func handleSwipeDown(sender: UITapGestureRecognizer) {
        print("Swipe Down!!!")
        controller.player?.pause()
        controller.dismissViewControllerAnimated(true, completion: nil)
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

// レイヤーをAVPlayerLayerにする為のラッパークラス.
//class AVPlayerView : UIView{
//    
//    required init?(coder aDecoder: NSCoder) {
//        super.init(coder: aDecoder)
//    }
//    
//    override init(frame: CGRect) {
//        super.init(frame: frame)
//    }
//    
//    override class func layerClass() -> AnyClass{
//        return AVPlayerLayer.self
//    }
//}

class CustomCellForCollection:UICollectionViewCell {
    @IBOutlet weak var imageView: UIImageView!
    @IBOutlet weak var dateLabel: UILabel!
}
