//
//  WebSocketManager.swift
//  NOGF
//
//  Created by henry on 2025/3/1.
//


import Combine
import Foundation
//
//  Websocket.swift
//  NOGF_applewatch
//
//  Created by henry on 2025/2/12.
//
import SwiftUI

class WebSocketManager: ObservableObject {
    @Published var ifConnected = false
    @Published var message: String = ""

    private let configuration: URLSessionConfiguration
    private let urlSession: URLSession
    private var webSocketTask: URLSessionWebSocketTask?
    private var baseURL: URL
    
    private var retryDelay: TimeInterval = 1.0
    private let maxRetryDelay: TimeInterval = 30.0
    
    // receive message stream
    private var addMessageStream: ((String) -> Void)?
    lazy var messageStream: AsyncStream<String> = {
        AsyncStream { continuation in
            addMessageStream = { message in
                continuation.yield(message)
            }
        }
    }()
    
    
    init(_ role: String) {
        self.baseURL = URL(string: "ws://57.181.243.204:8000/ws/1/\(role)")!
        self.configuration = URLSessionConfiguration.ephemeral
        //        configuration.allowsCellularAccess = true
        // configuration.waitsForConnectivity = true
        self.urlSession = URLSession(configuration: configuration)
        connect()
    }
    
    deinit {
        self.disconnect()
    }

    func connect() {
        disconnect()
        webSocketTask = urlSession.webSocketTask(with: baseURL)
        webSocketTask?.resume()

        ifConnected = true
        retryDelay = 2

        receiveMessage()
    }
    
    func disconnect() {
        webSocketTask?.cancel(with: .goingAway, reason: nil)
        webSocketTask = nil
        ifConnected = false
    }
    
    private func reconnect() {
        if ifConnected { return } // Already connected
        
        DispatchQueue.main.asyncAfter(deadline: .now() + retryDelay) { [weak self] in
            guard let self = self else { return }
            if !self.ifConnected {
                self.connect()
                self.retryDelay = min(self.retryDelay * 2, self.maxRetryDelay) // Exponential backoff
            }
        }
    }

    func sendMessage(_ message: String) {
        let message = URLSessionWebSocketTask.Message.string(message)
        webSocketTask?.send(message) { error in
            if let error = error {
                print("WebSocket couldn’t send message because: \(error)")
            }
        }
    }

    private func receiveMessage() {
        webSocketTask?.receive { [weak self] result in

            switch result {
            case .failure(let error):
                print("Error in receiving message: \(error)")
                DispatchQueue.main.async {
                    self?.ifConnected = false
                    self?.reconnect()
                }
            case .success(.string(let str)):
                DispatchQueue.main.async {
                    self?.message = str
                }
                self?.addMessageStream?(str)
                self?.receiveMessage()
            default:
                print("default")
            }
        }
    }
}
