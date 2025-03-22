//
//  ContentView.swift
//  NOGF
//
//  Created by henry on 2025/3/1.
//

import SwiftUI

struct ContentView: View {
    @State var role: Role = .none
    @State var isSender: Bool = false
    @State var isReceiver: Bool = false

    var body: some View {
        VStack{
            HStack {
                ButtonView(title: "Sender", isActive: $isSender) {
                    role = .Sender
                    isSender = true
                    isReceiver = false
                }
                
                ButtonView(title: "Receiver", isActive: $isReceiver) {
                    role = .Receive
                    isSender = false
                    isReceiver = true
                }
            }
            .padding()
            
            if role == .Receive {
                ReceiverContentView()
            } else if role == .Sender {
                SenderContentView()
            }
        }
    }
}

#Preview {
    ContentView()
}
