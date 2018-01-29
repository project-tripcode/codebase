/*
 *  tripcode.h
 *  tripcode
 *
 *  Created by Jean-François Ducarroz on 1/8/11.
 *  Copyright 2011 Outspring, Inc. All rights reserved.
 *
 */


typedef struct
{
	double latitude;
	double longitude;
} TCCoordinate;

class Tripcode
{
	#define TC_NBR_DIGITS		10
	#define TC_MAX_DEGREE		360

	public:
		Tripcode();

		const char * tripcodeForCoordinate(TCCoordinate coordinate);
		const char * tripcodeForCoordinate(double latitude, double longitude);
		TCCoordinate coordinateForTripcode(const char * tripcode);
		
		const char * tripcodeToTripcode8(const char * tripcode);
		
		bool isValidCoordinate(TCCoordinate location);
	
	private:
		double m_accuracy;
		long m_baseTable[((TC_NBR_DIGITS + 1) / 2)];
		char m_reverseTable[127];
};