//
//  Tripcode.h
//  Tripcode iOS Objective-C
//
//  Created by pat on 12/13/17.
//  Copyright © 2017 Micromat, Inc. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>

@interface Tripcode : NSObject

+ (NSString *)tripcodeForLocation:(CLLocation *)location;
+ (NSString *)tripcodeForCoordinate:(CLLocationCoordinate2D)coordinate;

+ (CLLocation *)locationForTripcode:(NSString *)tripcode;
+ (CLLocationCoordinate2D)coordinateForTripcode:(NSString *)tripcode;

@end
