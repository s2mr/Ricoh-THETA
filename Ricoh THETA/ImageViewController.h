/*
 * Copyright Ricoh Company, Ltd. All rights reserved.
 */

#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>
#import "PtpConnection.h"
#import "PtpObject.h"

#define KSTR_NONE_INERTIA           @"none"
#define KSTR_SHORT_INERTIA          @"weak"
#define KSTR_LONG_INERTIA           @"strong"

#define KINT_HIGHT_INTERVAL_BUTTON  54

typedef enum : int {
    NoneInertia = 0,
    ShortInertia,
    LongInertia
} enumInertia;

@interface ImageViewController : UIViewController

@property (nonatomic, strong) IBOutlet UIImageView* imageView;
@property (nonatomic, strong) IBOutlet UITextView* textView;
@property (nonatomic, strong) IBOutlet UIProgressView* progressView;
@property (nonatomic, strong) IBOutlet UIButton *closeButton;
@property (nonatomic, strong) IBOutlet UIButton *configButton;

- (IBAction)onCloseClicked:(id)sender;
- (void)getObject:(PtpConnection*)ptpConnection ptpObject:(PtpObject*)ptpObject;
- (IBAction)onConfig:(id)sender;

@end
