//
//  SenderContentView.swift
//  NOGF
//
//  Created by henry on 2025/3/1.
//

//
//  SenderContentView.swift
//  NOGF_iphone
//
//  Created by henry on 2025/2/28.
//

import SwiftUI

struct SenderContentView: View {
    @StateObject private var model = NOGFSenderModel()
    @State private var crownValue: Double = 0.0

    var body: some View {
        VStack {
            VStack {
                if let location = model.location {
                    Text("Latitude: \(location.latitude)")
                    Text("Longitude: \(location.longitude)")
                } else {
                    Text("Waiting for location...")
                        .foregroundColor(.gray)
                }
            }
            .padding()

            Text("message: \(model.ws_message ?? "---")")
                .padding()

            Text("connect to server successfully")
            Button {
                if model.ifSession {
                    model.stopPeriodSendLocation()
                } else {
                    model.setPeriodSendLocation()
                }
            } label: {
                if model.ifSession {
                    Image(systemName: "stop")
                } else {
                    Image(systemName: "restart")
                }
            }

        }
        .padding()
        .frame(maxWidth: .infinity, maxHeight: .infinity)
        .background(model.ifSession ? Color.red : Color.gray)  // Change background color
        .edgesIgnoringSafeArea(.all)  // Extend background to full screen
    }
}

#Preview {
    SenderContentView()
}
