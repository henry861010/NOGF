//
//  ReceiverContentView.swift
//  NOGF
//
//  Created by henry on 2025/3/19.
//


//
//  ReceiverContentView.swift
//  NOGF
//
//  Created by henry on 2025/3/1.
//


//
//  ReceiverContentView.swift
//  NOGF
//
//  Created by henry on 2025/3/1.
//


import SwiftUI

struct ReceiverContentView: View {
    @StateObject private var model = NOGFReceiverModel()
    @State var isRecording = false
    @State var zoomFactor: CGFloat = 0.0

    var body: some View {
        VStack {
            VStack {
                if let location = model.location {
                    Text("receiver lat: \(location.latitude)")
                    Text("receiver lon: \(location.longitude)")
                } else {
                    Text("receiver lat: ---")
                    Text("receiver long: ---")
                }
            }
            .padding()

            VStack {
                if let location = model.sender_location {
                    Text("sender lat: \(location.latitude)")
                    Text("sneder lon: \(location.longitude)")
                } else {
                    Text("sender lat: ---")
                    Text("sender lon: ---")
                }
            }
            .padding()
            
            Text("sender message: \(model.sender_message ?? "--")")
                .padding()

            if model.webSocketManager.ifConnected {
                Text("connect status: connected")
                    .padding()
            } else {
                Text("connect status: disconnected")
                    .padding()
            }

            if !model.bluetoothManager.isConnected {
                List(model.bluetoothManager.discoveredPeripherals, id: \.self) { peripheral in
                    Button(action: {
                        model.bluetoothManager.connect(peripheral: peripheral)
                    }) {
                        ZStack {
                            Rectangle()
                                .fill(.clear)

                            HStack {
                                Text(peripheral.name ?? "Unknown name")
                                    .font(.body)
                                    .listRowBackground(Color.clear)
                                    .padding(.horizontal)
                            }
                        }
                    }
                }
                .scrollContentBackground(.hidden)
                .padding()
            } else {
                HStack {
                    Image(systemName: "wifi")
                        .foregroundStyle(.white)
                    Text((model.bluetoothManager.connectedPeripheral?.name)!)
                    
                    Button {
                        model.bluetoothManager.disconnect()
                    } label: {
                        Image(systemName: "wifi.slash")
                            .foregroundStyle(.white)
                    }
                }
            }
            HStack {
                Button {
                    model.bluetoothManager.write("1")
                } label: {
                    Image(systemName: "1.circle.fill")
                        .foregroundStyle(.white)
                }
                .padding()
                
                Button {
                    model.bluetoothManager.write("2")
                } label: {
                    Image(systemName: "2.circle.fill")
                        .foregroundStyle(.white)
                }
                .padding()
            }

        }
        .padding()
        .frame(maxWidth: .infinity, maxHeight: .infinity)
        .background(isRecording ? Color.blue : Color.gray)
        //.edgesIgnoringSafeArea(.all)
    }
}
