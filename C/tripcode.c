//
//  main.c
//  tripcode
//
//  Created by pat on 12/15/17.
//  Copyright © 2017 Micromat, Inc. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct {
    double latitude;
    double longitude;
} TCCoordinate;


typedef struct {
    char name[256];
    TCCoordinate coordinate;
} Location;

Location locations[] = {
    {"Pyramids of Giza", {29.979235, 31.134202}},     // Pyramids of Giza
    {"Eiffel Tower", {48.858370, 2.294481}},      // Eiffel Tower
    {"Tower of London", {51.508112, -0.075949}},     // Tower of London
    {"Golden Gate Bridge", {37.819929, -122.478255}},   // Golden Gate Bridge
    {"Acropolis of Athens", {37.971532, 23.725749}},     // Acropolis of Athens
    {"Hermitage Museum", {59.939832, 30.314560}},     // Hermitage Museum
    {"The Louvre", {48.860611, 2.337644}},      // The Louvre
    {"Washington Monument", {38.889484, -77.035279}},    // Washington Monument
    {"Grand Canyon", {36.106965, -112.112997}},   // Grand Canyon
    {"Machu Picchu", {-13.163141, -72.544963}},   // Machu Picchu
    {"", {-999.0, -999.0}}
};

const char * tripcodeForCoordinate(TCCoordinate coordinate);
TCCoordinate coordinateForTripcode(const char * tripcode);

#define TC_NBR_DIGITS        10
#define TC_MAX_DEGREE        360

typedef struct {
    double accuracy;
    long baseTable[((TC_NBR_DIGITS + 1) / 2)];
    char reverseTable[127];
    TCCoordinate coordinate;
    char tripcode[TC_NBR_DIGITS + 3];
} TripcodeContext;

static const char * tripcodeDigits = "23456789BCDFGHJKLMNPQRSTVWXZ";

void resetContext(TripcodeContext * context)
{
    memset(context->tripcode, -1, TC_NBR_DIGITS + 3);
    context->coordinate = (TCCoordinate){999.0, 999.0};
}

void initContext(TripcodeContext * context)
{
    resetContext(context);
    
    // Initialize the base table
    unsigned long baseNbr = strlen(tripcodeDigits);
    unsigned long factor;
    int i;
    
    for (i = 0, factor = 1; i < ((TC_NBR_DIGITS + 1) / 2); i ++, factor *= baseNbr)
        context->baseTable[i] = factor;
    
    // Determine the accuracy
    double accuracy = ((double)TC_MAX_DEGREE) / ((double)factor - 1.0);
    
    // We need to truncate the accuracy to minimize disparity between various system and potential rouding error
    factor = (long)pow(10.0, (int)(-log10(accuracy)) + 2);
    accuracy = ceil(accuracy * (double)factor) / (double)factor;
    context->accuracy = accuracy;
    
    // Initialize the reverse digits table
    memset(context->reverseTable, -1, 1);
    for (i = 0; i < baseNbr; i ++)
    {
        char digit = tripcodeDigits[i];
        context->reverseTable[digit] = i;
        if (digit >= 'A' && digit <= 'Z')
            context->reverseTable[digit + 0x20] = i;    // support for lower case
    }
}

void tripcode(TripcodeContext * context)
{
    double latitude = context->coordinate.latitude;
    double longitude = context->coordinate.longitude;

    if (latitude < -90.0 || latitude > 90.0 || longitude < -180.0 || longitude > 180.0)
        return;

    bzero(context->tripcode, TC_NBR_DIGITS + 3);
    
    long llatitude = round((latitude + 90.0) / (context->accuracy / 2.0));
    long llongitude = round((longitude + 180.0 ) / context->accuracy);
    
    
    int i, j;
    for (i = ((TC_NBR_DIGITS + 1) / 2) - 1, j = 0; i >= 0; i --)
    {
        context->tripcode[j++] = tripcodeDigits[llatitude / context->baseTable[i]];
        llatitude %= context->baseTable[i];
        
        if (j == 3)
            context->tripcode[j++] = '-';
        
        context->tripcode[j++] = tripcodeDigits[llongitude / context->baseTable[i]];
        llongitude %= context->baseTable[i];
        
        if (j == 7)
            context->tripcode[j++] = '-';
    }
    
    context->tripcode[j] = 0;
}

void coordinates(TripcodeContext * context)
{
    if (context->tripcode[0] == -1)
        return;
    
    int i, j;
    long llatitude = 0;
    long llongitude = 0;
    long digit;
    
    char tripcodeX[TC_NBR_DIGITS * 2];
    char tc8extra[3] = {tripcodeDigits[0], tripcodeDigits[0], 0};
    strcpy (tripcodeX, context->tripcode);
    strcat (tripcodeX, tc8extra);
    
    for (i = ((TC_NBR_DIGITS + 1) / 2) - 1, j = 0; i >= 0 && tripcodeX[j];)
    {
        while ((digit = tripcodeX[j]) == '-')
            j++;
        if (digit >= 127)
            return;
        
        digit = context->reverseTable[digit];
        if (digit == -1)
            return;
        
        llatitude += digit * context->baseTable[i];
        j ++;
        
        while ((digit = tripcodeX[j]) == '-')
            j++;
        if (digit >= 127)
            return;
        
        digit = context->reverseTable[digit];
        if (digit == -1)
            return;
        
        llongitude += digit * context->baseTable[i];
        j ++;
        
        i --;
    }
    
    context->coordinate.latitude = ((double)llatitude * (context->accuracy / 2.0)) - 90.0;
    context->coordinate.longitude = ((double)llongitude * context->accuracy) - 180.0;
}

int main(int argc, const char * argv[]) {
    TripcodeContext context;
    initContext(&context);
    
    Location * locPtr = locations;
    Location location;

    while ((location = *locPtr++).coordinate.latitude != -999.0)
    {
        printf("\n");
        
        TCCoordinate coordinate = location.coordinate;
        
        resetContext(&context);
        context.coordinate = location.coordinate;

        printf("  *\t%s: %f, %f\n", location.name, coordinate.latitude, coordinate.longitude);

        tripcode(&context);
        
        printf("\t%s\n", context.tripcode);
        
        coordinates(&context);

        coordinate = context.coordinate;
        
        printf("\t%f, %f\n", coordinate.latitude, coordinate.longitude);
    }
    
    printf("\n");
    
    return 0;
}
