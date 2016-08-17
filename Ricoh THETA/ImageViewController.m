/*
 * Copyright Ricoh Company, Ltd. All rights reserved.
 */

#import "ImageViewController.h"
#import "RicohEXIF.h"
#import "ExifTags.h"
#import "glkViewController.h"
#import "GLRenderView.h"

@interface ImageViewController ()
{
    PtpObject* _ptpObject;
    NSMutableData *_imageData;
    int imageWidth;
    int imageHeight;
    GlkViewController *glkViewController;
    float _yaw;
    float _roll;
    float _pitch;
    UIView *_configView;
    UIButton *_configButton1;
    UIButton *_configButton2;
    UIButton *_configButton3;
}
@end

@implementation ImageViewController

- (void)appendLog:(NSString*)text
{
    [_textView setText:[NSString stringWithFormat:@"%@%@\n", _textView.text, text]];
    [_textView scrollRangeToVisible:NSMakeRange([_textView.text length], 0)];
}

#pragma mark - UI events

- (void)onCloseClicked:(id)sender
{
    [self dismissViewControllerAnimated:YES completion:nil];
}

- (void)myCloseClicked:(id)sender
{
    [self dismissViewControllerAnimated:YES completion:nil];
}

- (IBAction)onConfig:(id)sender {
}

- (void)myConfig:(id)sender {
    NSLog(@"myConfig");
    _configView = [[UIView alloc] init];
    _configView.backgroundColor = [UIColor colorWithRed:1.0 green:1.0 blue:1.0 alpha:1.0];
    _configView.frame = _imageView.frame;
    [self.view addSubview:_configView];
    
    CGRect rect = _closeButton.frame;
    _configButton1 = [UIButton buttonWithType:UIButtonTypeRoundedRect];
    _configButton1.frame = rect;
    [_configButton1 setTitle:KSTR_NONE_INERTIA forState:UIControlStateNormal];
    [_configButton1 addTarget:self action:@selector(noInertia:) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:_configButton1];

    _configButton2 = [UIButton buttonWithType:UIButtonTypeRoundedRect];
    rect.origin.y += KINT_HIGHT_INTERVAL_BUTTON;
    _configButton2.frame = rect;
    [_configButton2 setTitle:KSTR_SHORT_INERTIA forState:UIControlStateNormal];
    [_configButton2 addTarget:self action:@selector(shortInertia:) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:_configButton2];
    
    _configButton3 = [UIButton buttonWithType:UIButtonTypeRoundedRect];
    rect.origin.y += KINT_HIGHT_INTERVAL_BUTTON;
    _configButton3.frame = rect;
    [_configButton3 setTitle:KSTR_LONG_INERTIA forState:UIControlStateNormal];
    [_configButton3 addTarget:self action:@selector(longInertia:) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:_configButton3];
}

- (void)noInertia:(id)sender    {
    NSLog(@"noInertia");
    [glkViewController.glRenderView setInertia:NoneInertia];
    [self deleteView];
}

- (void)shortInertia:(id)sender    {
    NSLog(@"shortInertia");
    [glkViewController.glRenderView setInertia:ShortInertia];
    [self deleteView];
}

- (void)longInertia:(id)sender    {
    NSLog(@"longInertia");
    [glkViewController.glRenderView setInertia:LongInertia];
    [self deleteView];
}

-(void)deleteView   {
    [_configButton3 removeFromSuperview];
    [_configButton2 removeFromSuperview];
    [_configButton1 removeFromSuperview];
    [_configView removeFromSuperview];
}

#pragma mark - PTP/IP Operation

- (void)getObject:(PtpConnection *)ptpConnection ptpObject:(PtpObject *)ptpObject
{
    _ptpObject = ptpObject;
    dispatch_async(dispatch_get_main_queue(), ^{
        _progressView.progress = 0.0;
        _progressView.hidden = NO;
    });
    
    [ptpConnection operateSession:^(PtpIpSession *session) {
        // This block is running at PtpConnection#gcd thread.
        
        NSMutableData* imageData = [NSMutableData data];
        uint32_t objectHandle = (uint32_t)_ptpObject.objectInfo.object_handle;
        __block float total = 0.0;
        
        // Get primary image that was resized to 2048x1024.
        imageWidth = 2048;
        imageHeight = 1024;
        BOOL result = [session getResizedImageObject:objectHandle
                                               width:imageWidth
                                              height:imageHeight
                                         onStartData:^(NSUInteger totalLength) {
                                             // Callback before object-data reception.
                                             NSLog(@"getObject(0x%08x) will received %zd bytes.", objectHandle, totalLength);
                                             total = (float)totalLength;
                                             
                                         } onChunkReceived:^BOOL(NSData *data) {
                                             // Callback for each chunks.
                                             [imageData appendData:data];
                                             
                                             // Update progress.
                                             dispatch_async(dispatch_get_main_queue(), ^{
                                                 _progressView.progress = (float)imageData.length / total;
                                             });
                                             
                                             // Continue to receive.
                                             return YES;
                                         }];
        _progressView.progress = 1.0;
        if (!result) {
            dispatch_async(dispatch_get_main_queue(), ^{
                _progressView.hidden = YES;
            });
            return;
        }
        _imageData = imageData;

        // Parse EXIF data, it contains the data to correct the tilt.
        RicohEXIF* exif = [[RicohEXIF alloc] initWithNSData:imageData];
        
        // If there is no information, yaw, pitch and roll method returns NaN.
        NSString* tiltInfo = [NSString stringWithFormat:@"yaw:%0.1f pitch:%0.1f roll:%0.1f",
                              exif.yaw,
                              exif.pitch,
                              exif.roll];
        
        if (isnan(exif.yaw)) {
            _yaw = 0.0f;
        } else {
            _yaw = exif.yaw;
        }
        if (isnan(exif.pitch)) {
            _pitch = 0.0f;
        } else {
            _pitch = exif.pitch;
        }
        if (isnan(exif.roll)) {
            _roll = 0.0f;
        } else {
            _roll = exif.roll;
        }

        dispatch_async(dispatch_get_main_queue(), ^{
            _progressView.hidden = YES;
            [self appendLog:tiltInfo];
            [self startGLK];
        });
    }];
}

#pragma mark - Life cycle

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
}


- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    _textView.text = nil;
    _imageView.image = nil;
    _ptpObject = nil;
}

#pragma make - operation

- (void)startGLK
{
    glkViewController = [[GlkViewController alloc] init:_imageView.frame image:_imageData width:imageWidth height:imageHeight yaw:_yaw roll:_roll pitch:_pitch];
    glkViewController.view.frame = _imageView.frame;
 
    
    NSLog(@"startGLK imageData: %@", [[NSString alloc] initWithData:_imageData encoding:NSUTF8StringEncoding]);
    NSLog(@"startGLK: frame %f %f %f %f", _imageView.frame.origin.x, _imageView.frame.origin.y, _imageView.frame.size.width, _imageView.frame.size.height);
    
    [self.view addSubview:glkViewController.view];
    
    UIButton *myButton = [UIButton buttonWithType:UIButtonTypeRoundedRect];
    myButton.frame = _closeButton.frame;
    [myButton setTitle:_closeButton.currentTitle forState:UIControlStateNormal];
    [myButton addTarget:self action:@selector(myCloseClicked:) forControlEvents:UIControlEventTouchUpInside];
    [glkViewController.view addSubview:myButton];

    UIButton *myConfigButton = [UIButton buttonWithType:UIButtonTypeRoundedRect];
    myConfigButton.frame = _configButton.frame;
    [myConfigButton setTitle:_configButton.currentTitle forState:UIControlStateNormal];
    [myConfigButton addTarget:self action:@selector(myConfig:) forControlEvents:UIControlEventTouchUpInside];
    [glkViewController.view addSubview:myConfigButton];
    
    [self addChildViewController:glkViewController];
    [glkViewController didMoveToParentViewController:self];
}


@end
