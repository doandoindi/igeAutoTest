#include "AutoTestImpl.h"
#include "AutoTest.h"

#include "SDL.h"

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

NSURL* resultsDir;
extern "C" id GetWindowContext();

AutoTestImpl::AutoTestImpl()
{
    NSFileManager* manager = [NSFileManager defaultManager];
    NSURL* url = [[manager URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask] lastObject];
    
    resultsDir = [url URLByAppendingPathComponent:@"GameLoopResults" isDirectory:YES];
    [manager createDirectoryAtURL:resultsDir withIntermediateDirectories:NO attributes:nil error:nil];
}

AutoTestImpl::~AutoTestImpl()
{
}

void AutoTestImpl::FinishLoop()
{
    UIApplication *app = [UIApplication sharedApplication];
    [app openURL:[NSURL URLWithString:@"firebase-game-loop-complete://"] options:@{} completionHandler:^(BOOL success) {}];
}

void AutoTestImpl::Screenshots(const char *name)
{
    UIView* view = GetWindowContext();
    if ([[UIScreen mainScreen] respondsToSelector:@selector(scale)]) {
        UIGraphicsBeginImageContextWithOptions(view.bounds.size, YES, [UIScreen mainScreen].scale);
    } else {
        UIGraphicsBeginImageContext(view.bounds.size);
    }
    [view drawViewHierarchyInRect:view.bounds afterScreenUpdates:YES];
    UIImage *image = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    NSData *imageData = UIImageJPEGRepresentation(image, 0.95);
    if (imageData) {
        auto _name = [NSString stringWithFormat:@"%s%s", name, ".jpg"];
        NSURL* resultFile = [resultsDir URLByAppendingPathComponent:_name];
        
        [imageData writeToURL:resultFile atomically:NO];
    } else {
        NSLog(@"error while taking screenshot");
    }
}

const char* AutoTestImpl::GetResultPath()
{
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDir = [paths objectAtIndex:0];
    NSString *resultsDir = [documentsDir stringByAppendingString:@"/GameLoopResults/"];
    NSString* resultFile = [resultsDir stringByAppendingString:@"result.json"];
    return ([resultFile UTF8String]);
}
