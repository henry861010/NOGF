//
//  LocationManager.swift
//  NOGF
//
//  Created by henry on 2025/3/1.
//


import CoreLocation

class LocationManager: NSObject, ObservableObject, CLLocationManagerDelegate {
    @Published var location: CLLocationCoordinate2D?
    @Published var heading: Double?
    @Published var speed: Double = 0.0
    
    private var locationManager = CLLocationManager()
    
    private var addLocationStream: ((CLLocationCoordinate2D) -> Void)?
    lazy var locationStream: AsyncStream<CLLocationCoordinate2D> = {
        AsyncStream { continuation in
            addLocationStream = { location in
                continuation.yield(location)
            }
        }
    }()
    
    private var addHeadingStream: ((Double) -> Void)?
    lazy var headingStream: AsyncStream<Double> = {
        AsyncStream { continuation in
            addHeadingStream = { heading in
                continuation.yield(heading)
            }
        }
    }()
    
    override init() {
        super.init()
        self.locationManager.delegate = self
        self.locationManager.requestWhenInUseAuthorization()
        self.locationManager.startUpdatingLocation()
    }
    
    
    func locationManager(_ manager: CLLocationManager, didUpdateLocations locations: [CLLocation]) {
        guard let location = locations.last else { return }
        // Update published variable with user location coordinates
        self.location = location.coordinate
        addLocationStream?(location.coordinate)
        speed = location.speed
    }

    func locationManager(_ manager: CLLocationManager, didChangeAuthorization status: CLAuthorizationStatus) {
        if status == .authorizedWhenInUse || status == .authorizedAlways {
            // If authorization status has changed to authorized
            // start updating location
            locationManager.startUpdatingLocation()
        }
    }
    
    func locationManager(_ manager: CLLocationManager, didUpdateHeading newHeading: CLHeading) {
        // Use the true heading if available, otherwise use magnetic heading
        if newHeading.headingAccuracy > 0 {
            heading = newHeading.trueHeading
        } else {
            heading = newHeading.magneticHeading
        }
        
        guard let heading = heading else { return }
        addHeadingStream?(heading)
    }
}
