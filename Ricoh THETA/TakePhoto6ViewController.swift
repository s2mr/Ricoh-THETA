//
//  TakePhoto6ViewController.swift
//  Ricoh THETA
//
//  Created by 下村一将 on 2016/08/05.
//  Copyright © 2016年 kazu. All rights reserved.
//

import UIKit
import SceneKit
import ReplayKit
import MediaPlayer

class TakePhoto6ViewController: UIViewController, RPPreviewViewControllerDelegate, MPMediaPickerControllerDelegate {

    let ad = UIApplication.sharedApplication().delegate as! AppDelegate
    var cameraNode:SCNNode!
    var sphereGeometry:SCNSphere!
    var scene:SCNScene!
    var player :AVAudioPlayer!
    var timer:NSTimer!
    
    @IBOutlet weak var toolBar: UIToolbar!
    
    
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
        setupCamera()
    }
    
    override func viewWillDisappear(animated: Bool) {
        super.viewWillDisappear(animated)
//        音楽を止める
        if let _ = player {
            player.stop()
        }
        if let _ = timer {
            timer.invalidate()
        }
    }
    
    @IBAction func startButtonPushed(sender: AnyObject) {
        let recorder = RPScreenRecorder.sharedRecorder()
        recorder.startRecordingWithMicrophoneEnabled(false) {[unowned self] (error) in
            if let unwrappedError = error {
                print(unwrappedError.localizedDescription)
            } else {
                self.setHidden()
            }
        }
    }
    
    @IBAction func stopButtonPushed(sender: AnyObject) {
        let recorder = RPScreenRecorder.sharedRecorder()
        recorder.stopRecordingWithHandler { [unowned self] (preview, error) in
            if let unwrappedPreview = preview {
                unwrappedPreview.previewControllerDelegate = self
                self.presentViewController(unwrappedPreview, animated: true, completion: nil)
            }
        }
    }
    
    @IBAction func musicPickButtonPushed(sender: AnyObject) {
        // MPMediaPickerControllerのインスタンスを作成
        let picker = MPMediaPickerController()
        // ピッカーのデリゲートを設定
        picker.delegate = self
        // 複数選択を不可にする。（trueにすると、複数選択できる）
        picker.allowsPickingMultipleItems = false
        
        // ピッカーを表示する
        presentViewController(picker, animated: true, completion: nil)
    }
    
    @IBAction func viewTapped(sender: AnyObject) {
        print("View Tapped!!")
//        タブバーが表示されている時
        if !(tabBarController?.tabBar.hidden)! {
            setHidden()
        }else {
           setShown()
        }
    }
    
    @IBAction func replayButtonTapped(sender: AnyObject) {
    }
    
    
    func setHidden() {
        tabBarController?.tabBar.hidden = true
        navigationController?.navigationBarHidden = true
        toolBar.hidden = true
        UIApplication.sharedApplication().setStatusBarHidden(true, withAnimation: .None)
    }
    
    func setShown() {
        stopButtonPushed(self)
        tabBarController?.tabBar.hidden = false
        navigationController?.navigationBarHidden = false
        toolBar.hidden = false
        UIApplication.sharedApplication().setStatusBarHidden(false, withAnimation: .None)
    }
    
    
    func setupCamera() {
        scene = SCNScene()
        guard let scnView = self.view as? SCNView else { return }
        scnView.scene = scene
        
        scnView.allowsCameraControl = true
        
        //SCNNodeを作りそこにカメラを与えます。
        cameraNode = SCNNode()
        cameraNode.camera = SCNCamera()
        cameraNode.position = SCNVector3Zero
        cameraNode.camera?.automaticallyAdjustsZRange = true
        cameraNode.position.z = 15
        cameraNode.camera?.xFov = 80
        cameraNode.camera?.yFov = 80
        scene.rootNode.addChildNode(cameraNode)
        
        //環境光を設定します。
        let ambientLightNode = SCNNode()
        ambientLightNode.light = SCNLight()
        ambientLightNode.light?.type = SCNLightTypeAmbient
        ambientLightNode.light?.color = UIColor.whiteColor()
        scene.rootNode.addChildNode(ambientLightNode)
        
//        
//        if let _ = timer {
//            timer.fire()
//        }else {
            timer =  NSTimer.scheduledTimerWithTimeInterval(0.5, target: self, selector: #selector(change), userInfo: nil, repeats: true)
//        }
        
        //球体を作り中にカメラを入れます。
        sphereGeometry = SCNSphere(radius: 20)
        let material = SCNMaterial()
        material.doubleSided = true
        material.diffuse.contents = UIImage(data: ad.toShowImage.first!)?.flipHorizontal()
        sphereGeometry.firstMaterial = material
        
//        var materials = [SCNMaterial]()
//        for j in 1..<ad.toShowImage.count {
//            print("マテリアル追加")
//            let material = SCNMaterial()
//            material.doubleSided = true
//            material.diffuse.contents = UIImage(data:ad.toShowImage[j])?.flipHorizontal()
//            materials.append(material)
//        }
//        sphereGeometry.materials = materials
        
        let boxNode = SCNNode(geometry: sphereGeometry)
        scene.rootNode.addChildNode(boxNode)

    }
    
    var i = 1
    func change() {
        if i < ad.toShowImage.count {
            print("よばれた")
//            dispatch_async(dispatch_get_main_queue(), {
            self.sphereGeometry.firstMaterial?.diffuse.contents = UIImage(data: self.ad.toShowImage[self.i])
//            })
            i += 1
        }else {
//            timer.invalidate()
            i = 0
            self.sphereGeometry.firstMaterial?.diffuse.contents = UIImage(data: self.ad.toShowImage[self.i])
            i += 1
//            play()
        }
    }
    
    func play() {
        setupCamera()
    }
    
    func previewControllerDidFinish(previewController: RPPreviewViewController) {
        dismissViewControllerAnimated(true, completion: nil)
    }
    
    func mediaPickerDidCancel(mediaPicker: MPMediaPickerController) {
        dismissViewControllerAnimated(true, completion: nil)
    }
    
    // メディアアイテムピッカーでアイテムを選択完了したときに呼び出される
    func mediaPicker(mediaPicker: MPMediaPickerController, didPickMediaItems mediaItemCollection: MPMediaItemCollection) {
        
        let url = mediaItemCollection.representativeItem?.assetURL
        
        // 選択した曲情報がmediaItemCollectionに入っているので、これをplayerにセット。
        player = try! AVAudioPlayer(contentsOfURL: url!)
        // 再生開始
        player.enableRate = true
        player.numberOfLoops = -1
        // ピッカーを閉じ、破棄する
        dismissViewControllerAnimated(true, completion: nil)
        player.play()
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

class MakeMaterial {
    let material = SCNMaterial()
    
    init(data:NSData) {
        material.doubleSided = true
       self.material.diffuse.contents = UIImage(data: data)?.flipHorizontal()
    }
}

extension TakePhoto6ViewController: RPScreenRecorderDelegate {
    func screenRecorderDidChangeAvailability(screenRecorder: RPScreenRecorder) {
        print("screen recorder did change availability")
    }
    
    func screenRecorder(screenRecorder: RPScreenRecorder, didStopRecordingWithError error: NSError, previewViewController: RPPreviewViewController?) {
        print("screen recorder did stop recording : \(error.localizedDescription)")
    }
}

extension UIImage {
    func flipHorizontal() -> UIImage {
        UIGraphicsBeginImageContextWithOptions(size, false, 0.0)
        let imageRef = self.CGImage
        let context = UIGraphicsGetCurrentContext()
        CGContextTranslateCTM(context, size.width, size.height)
        CGContextScaleCTM(context, -1.0, -1.0)
        CGContextDrawImage(context, CGRect(x: 0, y: 0, width: size.width, height: size.height), imageRef)
        let flipHorizontalImage = UIGraphicsGetImageFromCurrentImageContext()
        UIGraphicsEndImageContext()
        return flipHorizontalImage
    }
}
