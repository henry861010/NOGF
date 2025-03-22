#include "NOGFUtiles.h"
#include <math.h>

// Convert degrees to radians
double toRadians(double degree) {
    return degree * M_PI / 180.0;
}

// Calculate bearing between two GPS coordinates
double cal_bearing(double base_lat, double base_lon, double target_lat, double target_lon) {
    base_lat = toRadians(base_lat);
    base_lon = toRadians(base_lon);
    target_lat = toRadians(target_lat);
    target_lon = toRadians(target_lon);

    double deltaLon = target_lon - base_lon;

    double y = sin(deltaLon) * cos(target_lat);
    double x = cos(base_lat) * sin(target_lat) - sin(base_lat) * cos(target_lat) * cos(deltaLon);
    double bearing = atan2(y, x);

    // Convert from radians to degrees and normalize
    bearing = fmod((bearing * 180.0 / M_PI) + 360.0, 360.0);
    
    return bearing;
}

// Haversine formula to calculate distance between two GPS coordinates
double calculateDistance(double lat1, double lon1, double lat2, double lon2) {
    lat1 = toRadians(lat1);
    lon1 = toRadians(lon1);
    lat2 = toRadians(lat2);
    lon2 = toRadians(lon2);

    double deltaLat = lat2 - lat1;
    double deltaLon = lon2 - lon1;

    double a = pow(sin(deltaLat / 2), 2) +
               cos(lat1) * cos(lat2) * pow(sin(deltaLon / 2), 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));

    return EARTH_RADIUS_KM * c;  // Distance in km
}

// decode the message from the ble to (lon, lat)
// the message format: $base_lat%base_lon|target_lat%target_lon|velocity|angle, length 10
bool decode_ble_message(String message, double &base_lat, double &base_lon, double &target_lat, double &target_lon, double &velocity, int &direction, double &angle) {
    if (message.startsWith("$")) {
        int index, index_next, index_temp;

        // base location
        index = 0;
        index_temp = message.indexOf("%");
        index_next = message.indexOf("|");
        if(index_temp==-1 || index_next==-1) return 0;

        base_lat = message.substring(index+1, index_temp).toFloat();
        base_lon = message.substring(index_temp+1, index_next).toFloat();
        
        // target location
        index = index_next;
        index_temp = message.indexOf("%", index_next+1);
        index_next = message.indexOf("|", index_next+1);
        if(index_temp==-1 || index_next==-1) return 0;

        target_lat = message.substring(index+1, index_temp).toFloat();
        target_lon = message.substring(index_temp+1, index_next).toFloat();

        // velocity
        index = message.indexOf("|", index+1);
        if(index_temp==-1 || index_next==-1) return 0;

        velocity = message.substring(index+1).toFloat();

        // direction
        index = message.indexOf("|", index+1);
        if(index_temp==-1 || index_next==-1) return 0;

        direction = message.substring(index_next+1).toInt();

        // angle
        index = message.indexOf("|", index+1);
        if(index_temp==-1 || index_next==-1) return 0;

        angle = message.substring(index_next+1).toFloat();

        return 1;
    }
    return 0;
}

double cal_angle_gap(double base_angle, double target_angle) {
    double angle_gap = target_angle - base_angle;
    if(angle_gap > 180) {
        return angle_gap - 360;
    } else if(angle_gap < -180) {
        return 360 + angle_gap;
    } else {
        return angle_gap;
    }
}

double cal_velocityToRpm(double velocity) {
    return velocity;
}
