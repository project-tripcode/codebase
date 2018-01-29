//
//  Tripcode.m
//  Tripcode iOS Objective-C
//
//  Created by pat on 12/13/17.
//  Copyright © 2017 Micromat, Inc. All rights reserved.
//

#import "Tripcode.h"

@implementation Tripcode

static const char * s_tripcodeDigits = "23456789BCDFGHJKLMNPQRSTVWXZ";

#define TC_NBR_DIGITS        10
#define TC_MAX_DEGREE        360

+ (NSString *)tripcodeForLocation:(CLLocation *)location
{
    return [self tripcodeForCoordinate: location.coordinate];
}

+ (NSString *)tripcodeForCoordinate:(CLLocationCoordinate2D)coordinate
{
    long baseTable[TC_NBR_DIGITS / 2];
    double accuracy = [self initValuesFor: baseTable
                                  reverse: NULL];
    
    double latitude = coordinate.latitude;
    double longitude = coordinate.longitude;
    
    if (latitude < -90.0 || latitude > 90.0 || longitude < -180.0 || longitude > 180.0)
        return nil;
    
    long llatitude = round((latitude + 90.0) / (accuracy / 2.0));
    long llongitude = round((longitude + 180.0 ) / accuracy);
    
    // Encode the latitude and longitude using the base table and tripcode digits
    NSMutableString * tripcode = [NSMutableString new];
    
    int i;
    for (i = (TC_NBR_DIGITS / 2) - 1; i >= 0; i --)
    {
        [tripcode appendFormat: @"%c", s_tripcodeDigits[llatitude / baseTable[i]]];
        llatitude %= baseTable[i];
        
        if (tripcode.length == 3)
            [tripcode appendString: @"-"];
        
        [tripcode appendFormat: @"%c", s_tripcodeDigits[llongitude / baseTable[i]]];
        llongitude %= baseTable[i];
        
        if (tripcode.length == 7)
            [tripcode appendString: @"-"];
    }
    
    return tripcode;
}

+ (CLLocation *)locationForTripcode:(NSString *)tripcode
{
    return [[CLLocation alloc] initWithCoordinate: [self coordinateForTripcode: tripcode]
                                         altitude: 0.0
                               horizontalAccuracy: 0.0
                                 verticalAccuracy: 0.0
                                        timestamp: [NSDate date]];
}

+ (double)initValuesFor:(long *)baseTable reverse:(char *)reverseTable
{
    // Initialize the base table
    unsigned long baseNbr = strlen(s_tripcodeDigits);
    unsigned long i, factor;
    
    for (i = 0, factor = 1; i < (TC_NBR_DIGITS / 2); i ++, factor *= baseNbr)
    {
        if (baseTable)
            baseTable[i] = factor;
    }
    
    // Determine the accuracy
    double accuracy = ((double)TC_MAX_DEGREE) / ((double)factor - 1.0);
    
    // We need to truncate the accuracy to minimize disparity between various system and potential rouding error
    factor = (long)pow(10.0, (int)(-log10(accuracy)) + 2);
    accuracy = ceil(accuracy * (double)factor) / (double)factor;
    
    if (!reverseTable)
        return accuracy;
    
    // Initialize the reverse digits table
    memset(reverseTable, -1, 1);
    for (i = 0; i < strlen(s_tripcodeDigits); i ++)
    {
        char digit = s_tripcodeDigits[i];
        reverseTable[digit] = i;
        if (digit >= 'A' && digit <= 'Z')
            reverseTable[digit + 0x20] = i;    // support for lower case
    }
    
    return accuracy;
}

+ (CLLocationCoordinate2D)coordinateForTripcode:(NSString *)tripcode
{
    long baseTable[(TC_NBR_DIGITS / 2)];
    char reverseTable[127];
    double accuracy = [self initValuesFor: baseTable
                                  reverse: reverseTable];

    long llatitude = 0;
    long llongitude = 0;
    long digit;
    
    char tripcodeX[TC_NBR_DIGITS * 2];
    char tc8extra[3] = {s_tripcodeDigits[0], s_tripcodeDigits[0], 0};
    strcpy (tripcodeX, tripcode.UTF8String);
    strcat (tripcodeX, tc8extra);
    
    CLLocationCoordinate2D invalidTripcode = {999.0, 999.0};
    
    for (int i = (TC_NBR_DIGITS / 2) - 1, j = 0; i >= 0 && tripcodeX[j];)
    {
        while ((digit = tripcodeX[j]) == '-')
            j++;
        
        if (digit >= 127)
            return invalidTripcode;
        
        digit = reverseTable[digit];
        if (digit == -1)
            return invalidTripcode;
        
        llatitude += digit * baseTable[i];
        j ++;
        
        while ((digit = tripcodeX[j]) == '-')
            j++;
        if (digit >= 127)
            return invalidTripcode;
        
        digit = reverseTable[digit];
        if (digit == -1)
            return invalidTripcode;
        
        llongitude += digit * baseTable[i];
        j ++;
        i --;
    }
    
    double latitude = ((double)llatitude * (accuracy / 2.0)) - 90.0;
    double longitude = ((double)llongitude * accuracy) - 180.0;
    
    return (CLLocationCoordinate2D){latitude, longitude};
}

@end
