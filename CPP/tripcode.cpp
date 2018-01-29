/*
 *  tripcode.cpp
 *  tripcode
 *
 *  Created by Jean-François Ducarroz on 1/8/11.
 *  Copyright 2011 Outspring, Inc. All rights reserved.
 *
 */

#include "tripcode.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

static const char * tripcodeDigits = "23456789BCDFGHJKLMNPQRSTVWXZ";

Tripcode::Tripcode()
{
	// Initialize the base table
	int baseNbr = strlen(tripcodeDigits);
	unsigned long factor;
	int i;

	for (i = 0, factor = 1; i < ((TC_NBR_DIGITS + 1) / 2); i ++, factor *= baseNbr)
		m_baseTable[i] = factor;
	
	// Determine the accuracy
	m_accuracy = ((double)TC_MAX_DEGREE) / ((double)factor - 1.0);
	
	// We need to truncate the accuracy to minimize disparity between various system and potential rouding error
	factor = (long)pow(10.0, (int)(-log10(m_accuracy)) + 2);
	m_accuracy = ceil(m_accuracy * (double)factor) / (double)factor;
	
	// Initialize the reverse digits table
	memset(m_reverseTable, -1, 1);
	for (i = 0; i < baseNbr; i ++)
	{
		char digit = tripcodeDigits[i];
		m_reverseTable[digit] = i;
		if (digit >= 'A' && digit <= 'Z')
			m_reverseTable[digit + 0x20] = i;	// support for lower case
	}
}

const char * Tripcode::tripcodeForCoordinate(TCCoordinate location)
{
	return Tripcode::tripcodeForCoordinate(location.latitude, location.longitude);
}

#if TC_TESTING
static char s_tripcode[TC_NBR_DIGITS + 3];
#endif

const char * Tripcode::tripcodeForCoordinate(double latitude, double longitude)
{
	if (latitude < -90.0 || latitude > 90.0 || longitude < -180.0 || longitude > 180.0)
		return NULL;

	long llatitude = round((latitude + 90.0) / (m_accuracy / 2.0));
	long llongitude = round((longitude + 180.0 ) / m_accuracy);
	
	// Encode the latitude and longitude using the base table and tripcode digits
#if TC_TESTING
	char * tripcode = s_tripcode;
#else
	char * tripcode = (char *)malloc(TC_NBR_DIGITS + 3);		// add 3 bytes for formatting and c string ending
#endif
	
	int i, j;
	for (i = ((TC_NBR_DIGITS + 1) / 2) - 1, j = 0; i >= 0; i --)
	{
		tripcode[j++] = tripcodeDigits[llatitude / m_baseTable[i]];
		llatitude %= m_baseTable[i];
		
		if (j == 3)
			tripcode[j++] = '-';

		tripcode[j++] = tripcodeDigits[llongitude / m_baseTable[i]];
		llongitude %= m_baseTable[i];

		if (j == 7)
			tripcode[j++] = '-';
	}
	
	tripcode[j] = 0;

	return tripcode;
}

TCCoordinate Tripcode::coordinateForTripcode(const char * tripcode)
{
	int i, j;
	TCCoordinate location = {999.0, 999.0};
	
	long llatitude = 0;
	long llongitude = 0;
	long digit;
	
	char tripcodeX[TC_NBR_DIGITS * 2];
	char tc8extra[3] = {tripcodeDigits[0], tripcodeDigits[0], 0};
	strcpy (tripcodeX, tripcode);
	strcat (tripcodeX, tc8extra);
	
	for (i = ((TC_NBR_DIGITS + 1) / 2) - 1, j = 0; i >= 0 && tripcodeX[j];)
	{
		while ((digit = tripcodeX[j]) == '-')
			j++;
		if (digit >= 127)
			return location;
		
		digit = m_reverseTable[digit];
		if (digit == -1)
			return location;
		
		llatitude += digit * m_baseTable[i];
		j ++;
		
		while ((digit = tripcodeX[j]) == '-')
			j++;
		if (digit >= 127)
			return location;
		
		digit = m_reverseTable[digit];
		if (digit == -1)
			return location;
		
		llongitude += digit * m_baseTable[i];
		j ++;

		i --;
	}
	
	location.latitude = ((double)llatitude * (m_accuracy / 2.0)) - 90.0;
	location.longitude = ((double)llongitude * m_accuracy) - 180.0;

	return location;
}

const char * Tripcode::tripcodeToTripcode8(const char * tripcode)
{
	int baseNbr = strlen(tripcodeDigits);

	TCCoordinate location = this->coordinateForTripcode(tripcode);
	long llatitude = (location.latitude + 90.0) / (m_accuracy / 2.0);
	long llongitude =  (location.longitude + 180.0) / m_accuracy;
	
	llatitude = ((llatitude + (baseNbr / 2)) / baseNbr) * baseNbr;
	llongitude = ((llongitude + (baseNbr / 2)) / baseNbr) * baseNbr;
	
	char * tripcode8 = const_cast<char *>(tripcodeForCoordinate(((double)llatitude * (m_accuracy / 2.0)) - 90.0, ((double)llongitude * m_accuracy) - 180.0));
	if (strlen(tripcode8) == 12)
	{
		tripcode8[3] = tripcode8[4];
		tripcode8[4] = '-';
		tripcode8[7] = tripcode8[8];
		tripcode8[8] = tripcode8[9];
		tripcode8[9] = 0;
	}
	
	return tripcode8;
}

