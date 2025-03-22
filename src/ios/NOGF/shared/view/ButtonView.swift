//
//  ButtonView.swift
//  NOGF
//
//  Created by henry on 2025/3/1.
//

import SwiftUI

struct ButtonView: View {
    var title: String
    @Binding var isActive: Bool
    var action: () -> Void
    
    var body: some View {
        Button(action: action) {
            Text(title)
                .padding()
                .frame(maxWidth: .infinity) // Make it expand if needed
                .background(isActive ? Color.blue.opacity(0.1) : Color.gray.opacity(0.1)) // Light blue background
                .foregroundColor(.blue)
                .overlay(
                    RoundedRectangle(cornerRadius: 10)
                        .stroke(Color.blue, lineWidth: 2)
                )
                //.cornerRadius(10)
        }
    }
}
/*
 #Preview {
 ButtonView(title: "Login") {
 print("aaaa")
 }
 }
 */
