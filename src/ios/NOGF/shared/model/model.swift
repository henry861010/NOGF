//
//  model.swift
//  NOGF
//
//  Created by henry on 2025/3/1.
//

import Foundation

enum Role {
    case none
    case Sender
    case Receive
}

//receiverMessageStruct
struct WebsocketMessageToReceiver: Codable {
    let latitude: Double?
    let longitude: Double?
    let speed: Double?
    let command: String  // Start, Stop
    let message: String?
}

//receiverMessageStruct
struct WebsocketMessageToSender: Codable {
    let latitude: Double?
    let longitude: Double?
    let command: String  // Start, Stop
    let message: String?
}
