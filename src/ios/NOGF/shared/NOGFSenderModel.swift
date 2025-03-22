//
//  NOGFSenderModel.swift
//  NOGF
//
//  Created by henry on 2025/3/1.
//


//
//  NOGFSenderModel.swift
//  NOGF_iphone
//
//  Created by henry on 2025/2/28.
//

import Foundation

import Combine
import CoreLocation

class NOGFSenderModel: ObservableObject {
    let locationManager = LocationManager()
    //let httpManager = HttpManager("ws://57.181.243.204:8000/", "1")
    let httpManager = HttpManager("http://127.0.0.1:8000/sender", "1")
    
    private var cancellables = Set<AnyCancellable>()
    
    @Published var ifSession: Bool = false

    @Published var location: CLLocationCoordinate2D?
    @Published var heading: Double?
    
    @Published var ws_command: String?
    @Published var ws_message: String?

    init() {
        Task {
            await handleLocation()
        }
    }
    
    func handleLocation() async {
        let locationStream = locationManager.locationStream
        
        for await location in locationStream {
            Task { @MainActor in
                self.location = location
            }
        }
    }
    
    func handleHeading() async {
        let headingStream = locationManager.headingStream
        
        for await heading in headingStream {
            Task { @MainActor in
                self.heading = heading
            }
        }
    }
    
    private func sendLocation(command: String) {
        guard let location = locationManager.location else { return }

        let json: [String: Any] = [
            "latitude": location.latitude,
            "longitude": location.longitude,
            "speed": locationManager.speed,
            "timestamp": Date().formatted(),
            "command": command,
        ]

        if let jsonData = try? JSONSerialization.data(withJSONObject: json, options: []) {
            httpManager.send(jsonData)
        }
    }
    
    func setPeriodSendLocation() {
        if(!ifSession) {
            ifSession = true
            Timer.publish(every: 1, on: .main, in: .common)
                .autoconnect()
                .sink { [weak self] _ in
                    self?.sendLocation(command: "Start")
                }
                .store(in: &cancellables)
        }
    }
    
    func stopPeriodSendLocation() {
        if(ifSession) {
            ifSession = false
            cancellables.removeAll()
            sendLocation(command: "Stop")
        }
    }
}



