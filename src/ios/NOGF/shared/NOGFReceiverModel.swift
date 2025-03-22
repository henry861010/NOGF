//
//  NOGFReceiverModel.swift
//  NOGF
//
//  Created by henry on 2025/3/1.
//

import CoreLocation

class NOGFReceiverModel: ObservableObject {
    let locationManager = LocationManager()
    let webSocketManager = WebSocketManager("receiver")
    let bluetoothManager = BluetoothManager()
    #if os(iOS)
        let cameraManager = CameraManager()
        var photoLibraryManager: PhotoLibraryManager?
    #endif

    @Published var location: CLLocationCoordinate2D?
    @Published var heading: Double?

    @Published var sender_location: CLLocationCoordinate2D?
    @Published var sender_speed: Double?
    @Published var sender_message: String?

    @Published var movieFileUrl: URL?

    init() {
        #if os(iOS)
            Task {
                self.photoLibraryManager = await PhotoLibraryManager()
            }

            Task {
                await handleCameraMovie()
            }
        #endif

        Task {
            await handleLocation()
        }

        Task {
            await handleMessage()
        }
    }

    #if os(iOS)
        // for movie record
        func handleCameraMovie() async {
            let fileUrlStream = cameraManager.movieFileStream

            for await url in fileUrlStream {
                Task { @MainActor in
                    //movieFileUrl = url
                    await photoLibraryManager?.saveVideo(fileUrl: url)
                }
            }
        }
    #endif

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

    func handleMessage() async {
        let messageStream = webSocketManager.messageStream

        for await packet in messageStream {
            Task { @MainActor in
                print("packet: \(packet)")

                //decode the message
                if let jsonData = packet.data(using: .utf8) {
                    do {
                        let decodedMessage = try JSONDecoder().decode(
                            WebsocketMessageToReceiver.self, from: jsonData)

                        sender_message =
                            decodedMessage.message ?? sender_message
                        sender_speed = decodedMessage.speed ?? sender_speed
                        if let lat = decodedMessage.latitude,
                            let lon = decodedMessage.longitude
                        {
                            sender_location = CLLocationCoordinate2D(
                                latitude: Double(lat), longitude: Double(lon))
                        }

                        // execute the command (record the vedio)
                        #if os(iOS)
                            if decodedMessage.command == "Start"
                                && !cameraManager.isRecordVedio
                            {
                                cameraManager.startRecordingVideo()
                            } else if decodedMessage.command == "Stop"
                                && cameraManager.isRecordVedio
                            {
                                cameraManager.stopRecordingVideo()
                            }
                        #endif

                    } catch {
                        print("Error decoding JSON:", error)
                    }
                }

            }
        }
    }
}
