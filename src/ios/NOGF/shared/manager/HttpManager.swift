//
//  httpManager.swift
//  NOGF
//
//  Created by henry on 2025/3/21.
//
import SwiftUI

class HttpManager: ObservableObject {
    private var baseURL: URL
    private var token: String
    
    init(_ url: String, _ token: String) {
        self.baseURL = URL(string: url)!
        self.token = token
    }
    
    func send(_ jsonData: Data) {
        // Configure request
        var request = URLRequest(url: self.baseURL)
        request.httpMethod = "POST"
        request.setValue("application/json; charset=UTF-8", forHTTPHeaderField: "Content-Type")
        request.setValue("\(token)", forHTTPHeaderField: "Authorization")
        request.httpBody = jsonData

        // Send request
        URLSession.shared.dataTask(with: request) { data, response, error in
            if let error = error {
                DispatchQueue.main.async {
                    print("Error: \(error.localizedDescription)")
                }
                return
            }

            if let data = data, let responseStr = String(data: data, encoding: .utf8)
            {
                DispatchQueue.main.async {
                    //print("Response: \(responseStr)")
                }
            }
        }.resume()
    }
}
