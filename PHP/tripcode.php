<?php

/*
 *  tripcode.php
 *  tripcode
 *
 *  Created by Jean-François Ducarroz on 1/8/11.
 *  Copyright 2011 Outspring, Inc. All rights reserved.
 *
 */

class Tripcode
{
    private $m_tripcodeDigits = NULL;
    private $m_baseTable = array();
    private $m_reverseTable = array();
    private $m_accuracy = 0;

    function __construct()
    {
        define('TC_NBR_DIGITS', 10);
        define('TC_MAX_DEGREE', 360);

        // Convert the tripcode digits into an array
        $this->m_tripcodeDigits = preg_split('//', "23456789BCDFGHJKLMNPQRSTVWXZ", -1, PREG_SPLIT_NO_EMPTY);

        // Initialize the base table
        $baseNbr = count($this->m_tripcodeDigits);

        for ($i = 0, $factor = 1; $i < (int)((TC_NBR_DIGITS + 1) / 2); $i ++, $factor *= $baseNbr)
            $this->m_baseTable[$i] = $factor;

        // Determine the accuracy
        $this->m_accuracy = ((double)TC_MAX_DEGREE) / ((double)$factor - 1.0);
        
        // We need to truncate the accuracy to minimize disparity between various system and potential rouding error
        $factor = (int)pow(10.0, (int)(-log10($this->m_accuracy)) + 2);
        $this->m_accuracy = ceil($this->m_accuracy * (double)$factor) / (double)$factor;
        
        // Initialize the reverse digits table
        $this->m_reverseTable = array_flip($this->m_tripcodeDigits);
    }

    public function tripcodeForCoordinate($latitude, $longitude)
    {
        if ($latitude < -90.0 || $latitude > 90.0 || $longitude < -180.0 || $longitude > 180.0)
            return NULL;

        $llatitude = round(($latitude + 90.0) / ($this->m_accuracy / 2.0));
        $llongitude = round(($longitude + 180.0 ) / $this->m_accuracy);
        
        // Encode the latitude and longitude using the base table and tripcode digits
        $tripcode = '';
        for ($i = (int)((TC_NBR_DIGITS + 1) / 2) - 1, $j = 0; $i >= 0; $i --)
        {
            $tripcode .= $this->m_tripcodeDigits[$llatitude / $this->m_baseTable[$i]];
            $llatitude %= $this->m_baseTable[$i];
            
            if (++ $j == 3)
            {
                $tripcode .= '-';
                $j ++;
            }

            $tripcode .= $this->m_tripcodeDigits[$llongitude / $this->m_baseTable[$i]];
            $llongitude %= $this->m_baseTable[$i];
            
            if (++ $j == 7)
            {
                $tripcode .= '-';
                $j ++;
            }
        }

        return $tripcode;
    }
    
    public function coordinateForTripcode($tripcode)
    {
        $location = array();
        
        $llatitude = 0;
        $llongitude = 0;
        $digit;
        
        $tripcodeX = $tripcode.$this->m_tripcodeDigits[0].$this->m_tripcodeDigits[0];
        $len = strlen($tripcodeX);

        for ($i = (int)((TC_NBR_DIGITS + 1) / 2) - 1, $j = 0; $i >= 0 && $j < $len;)
        {
            while (($digit = $tripcodeX[$j]) == '-')
                $j++;
            if (ord($digit) >= 127)
                return $location;
            
            if (isset($this->m_reverseTable[$digit]))
                $digit = $this->m_reverseTable[$digit];
            else
                return $location;
            
            $llatitude += $digit * $this->m_baseTable[$i];
            $j ++;
            
            while (($digit = $tripcodeX[$j]) == '-')
                $j++;
            if (ord($digit) >= 127)
                return $location;
            
            if (isset($this->m_reverseTable[$digit]))
                $digit = $this->m_reverseTable[$digit];
            else
                return $location;

            $llongitude += $digit * $this->m_baseTable[$i];
            $j ++;

            $i --;
        }

        $location['latitude'] = ((double)$llatitude * ($this->m_accuracy / 2.0)) - 90.0;
        $location['longitude'] = ((double)$llongitude * $this->m_accuracy) - 180.0;

        return $location;
    }
    
    public function tripcodeWithOffset($tripcode, $offset)
    {
        $location = array();
        
        $llatitude = 0;
        $llongitude = 0;
        $digit;
        
        $tripcodeX = $tripcode.$this->m_tripcodeDigits[0].$this->m_tripcodeDigits[0];
        $len = strlen($tripcodeX);

        for ($i = (int)((TC_NBR_DIGITS + 1) / 2) - 1, $j = 0; $i >= 0 && $j < $len;)
        {
            while (($digit = $tripcodeX[$j]) == '-')
                $j++;
            if (ord($digit) >= 127)
                return $location;
            
            if (isset($this->m_reverseTable[$digit]))
                $digit = $this->m_reverseTable[$digit];
            else
                return $location;
            
            $llatitude += $digit * $this->m_baseTable[$i];
            $j ++;
            
            while (($digit = $tripcodeX[$j]) == '-')
                $j++;
            if (ord($digit) >= 127)
                return $location;
            
            if (isset($this->m_reverseTable[$digit]))
                $digit = $this->m_reverseTable[$digit];
            else
                return $location;

            $llongitude += $digit * $this->m_baseTable[$i];
            $j ++;

            $i --;
        }
        
        $llatitude += $offset;
        $llongitude += $offset;
        
        // Encode the latitude and longitude using the base table and tripcode digits
        $tripcode = '';
        for ($i = (int)((TC_NBR_DIGITS + 1) / 2) - 1, $j = 0; $i >= 0; $i --)
        {
            $tripcode .= $this->m_tripcodeDigits[$llatitude / $this->m_baseTable[$i]];
            $llatitude %= $this->m_baseTable[$i];
            
            if (++ $j == 3)
            {
                $tripcode .= '-';
                $j ++;
            }

            $tripcode .= $this->m_tripcodeDigits[$llongitude / $this->m_baseTable[$i]];
            $llongitude %= $this->m_baseTable[$i];
            
            if (++ $j == 7)
            {
                $tripcode .= '-';
                $j ++;
            }
        }

        return $tripcode;
    }
}

?>
