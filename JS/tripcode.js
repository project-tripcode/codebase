/*
 *  tripcode.js
 *  tripcode
 *
 *  Created by Jean-François Ducarroz on 1/8/11.
 *  Copyright 2011 Outspring, Inc. All rights reserved.
 *
 */

function Tripcode()
{
    const TC_NBR_DIGITS = 10;
    const TC_MAX_DEGREE = 360;

    var m_tripcodeDigits = "23456789BCDFGHJKLMNPQRSTVWXZ";
    var m_baseTable = new Array();
    var m_reverseTable = new Array();
    var m_accuracy = 0;

    var baseNbr = m_tripcodeDigits.length;
    
    var i, factor;
    for (i = 0, factor = 1; i < Math.floor((TC_NBR_DIGITS + 1) / 2); i ++, factor *= baseNbr)
        m_baseTable[i] = factor;

    // Determine the accuracy
    m_accuracy = TC_MAX_DEGREE / (factor - 1.0);

    // We need to truncate the accuracy to minimize disparity between various system and potential rouding error
    factor = Math.floor(Math.pow(10.0, Math.floor(-Math.log(m_accuracy) / Math.LN10) + 2));
    m_accuracy = Math.ceil(m_accuracy * factor) / factor;

    // Initialize the reverse digits table
    for (var key in m_tripcodeDigits)
        m_reverseTable[m_tripcodeDigits[key]] = key;
        
    this.tripcodeForCoordinate = function(latitude, longitude)
    {
        if (latitude < -90.0 || latitude > 90.0 || longitude < -180.0 || longitude > 180.0)
            return null;
        
        var llatitude = Math.round((latitude + 90.0) / (m_accuracy / 2.0));
        var llongitude = Math.round((longitude + 180.0 ) / m_accuracy);

        // Encode the latitude and longitude using the base table and tripcode digits
        var tripcode = '';
        var i, j;
        for (i = Math.floor((TC_NBR_DIGITS + 1) / 2) - 1, j = 0; i >= 0; i --)
        {
            tripcode += m_tripcodeDigits[Math.floor(llatitude / m_baseTable[i])];
            llatitude %= m_baseTable[i];
            
            if (++ j == 3)
            {
                tripcode += '-';
                j ++;
            }

            tripcode += m_tripcodeDigits[Math.floor(llongitude / m_baseTable[i])];
            llongitude %= m_baseTable[i];
            
            if (++ j == 7)
            {
                tripcode += '-';
                j ++;
            }
        }

        return tripcode;
    }
    
    this.coordinateForTripcode = function(tripcode)
    {
        var location = new Array();

        var llatitude = 0;
        var llongitude = 0;
        var digit;
        
        var tripcodeX = tripcode + m_tripcodeDigits[0] + m_tripcodeDigits[0];
        var len = tripcodeX.length;
        var i, j;
        for (i = Math.floor((TC_NBR_DIGITS + 1) / 2) - 1, j = 0; i >= 0 && j < len;)
        {
            while ((digit = tripcodeX[j]) == '-')
                j++;
            if (digit.charCodeAt(0) >= 127)
                return location;
            
            if (m_reverseTable[digit] != undefined)
                digit = m_reverseTable[digit];
            else
                return location;
            
            llatitude += digit * m_baseTable[i];
            j ++;
            
            while ((digit = tripcodeX[j]) == '-')
                j++;
            if (digit.charCodeAt(0) >= 127)
                return location;
            
            if (m_reverseTable[digit] != undefined)
                digit = m_reverseTable[digit];
            else
                return location;

            llongitude += digit * m_baseTable[i];
            j ++;

            i --;
        }

        location['latitude'] = (llatitude * (m_accuracy / 2.0)) - 90.0;
        location['longitude'] = (llongitude * m_accuracy) - 180.0;

        return location;
    }
}
