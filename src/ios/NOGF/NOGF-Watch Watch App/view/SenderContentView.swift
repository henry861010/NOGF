//
//  SenderContentView.swift
//  NOGF
//
//  Created by henry on 2025/3/19.
//


//
//  SenderContentView.swift
//  NOGF
//
//  Created by henry on 2025/3/1.
//


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
                        .foregroundColor(.black)
                }
            }
            .padding()

            Text("message: \(model.ws_message ?? "---")")
                .padding()

            Text("connect to server successfully")
                .focusable()
                .digitalCrownRotation(
                    $crownValue, from: 0, through: 10, by: 1,
                    sensitivity: .low
                )
                .onChange(of: crownValue) { oldValue, newValue in
                    if newValue > 5 {
                        model.setPeriodSendLocation()
                    } else {
                        model.stopPeriodSendLocation()
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
