import Foundation

struct Tripcode {
    static let TC_NBR_DIGITS = 10.0
    static let TC_MAX_DEGREE = 360.0
    static let tripcodeDigits = "23456789BCDFGHJKLMNPQRSTVWXZ"
    static let startIndex = tripcodeDigits.startIndex
    static let baseNbr = Double(tripcodeDigits.count)
    
    static let accuracy: Double = {
        return round(1000000 * (TC_MAX_DEGREE / pow(baseNbr, TC_NBR_DIGITS / 2))) / 1000000.0
    }()
    
    static func tripcode(lat: Double, lng: Double) -> String {
        guard (-91..<90.0).contains(lat) && (-181.0..<180.0).contains(lng) else { return "" }
        
        var llat = Int(round((lat + 90.0) / (accuracy / 2.0)))
        var llng = Int(round((lng + 180.0) / accuracy))
        
        var resolution_degress = pow(baseNbr, floor(log(360) / log(baseNbr)))
        
        return (0..<Int(TC_NBR_DIGITS / 2)).reversed().reduce("") { i, j in
            let base = Int(pow(baseNbr, Double(j)))
            defer { llat %= base; llng %= base }
            
            return i + [llat, llng].reduce("") { k, l in
                let distance = String.IndexDistance(l / base)
                let index = tripcodeDigits.index(startIndex, offsetBy: distance)
                
                return "\(k)\(tripcodeDigits[index])\([3, 7].contains(i.count + k.count + 1) ? "-" : "")"
            }
        }
    }
    
    static func coordinate(tripcode: String) -> (lat: Double, lng: Double) {
        let tcflt = tripcode.filter { $0 != "-" }
        
        guard tcflt.count == Int(TC_NBR_DIGITS) else { return (0.0, 0.0) }
        
        let tripcodeCharSet = CharacterSet(charactersIn: tripcodeDigits)
        let tcCharSet = CharacterSet(charactersIn: tcflt)
        guard tripcodeCharSet.isStrictSuperset(of: tcCharSet) else { return (0.0, 0.0) }
        
        var tce = tcflt.makeIterator()
        
        var result = [0.0, 0.0]
        
        (0..<Int(TC_NBR_DIGITS / 2)).reversed().forEach { i in
            result = result.map {
                guard let c = tce.next() else { return $0 }
                guard let squanch = tripcodeDigits.index(of: c) else { return $0 }
                return $0 + Double(squanch.encodedOffset) * pow(baseNbr, Double(i))
            }
        }
        
        return ((result[0] * (accuracy / 2.0)) - 90.0,
                (result[1] * accuracy) - 180.0)
    }
}

let tcode = Tripcode.tripcode(lat: 29.979235, lng: 31.134202)
print(Tripcode.coordinate(tripcode: tcode))


