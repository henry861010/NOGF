//
//  BluetoothManager.swift
//  NOGF
//
//  Created by henry on 2025/3/1.
//


//
//  BluetoothController.swift
//  BLEConnection
//
//  Created by Caio Gomes Piteli on 24/03/24.
//

import CoreBluetooth
import Foundation

class BluetoothManager: NSObject, ObservableObject {

    private var centralManager: CBCentralManager!

    @Published var connectedPeripheral: CBPeripheral?
    @Published var discoveredPeripherals = [CBPeripheral]()
    @Published var isConnected = false
    @Published var bluetoothStatus: BluetoothStatus = .off
    var writableCharacteristic: CBCharacteristic?
    
    // the receive message stream
    private var addMessageStream: ((String) -> Void)?
    lazy var messageStream: AsyncStream<String> = {
        AsyncStream { continuation in
            addMessageStream = { message in
                continuation.yield(message)
            }
        }
    }()

    override init() {
        super.init()
        centralManager = CBCentralManager(delegate: self, queue: nil)
        centralManagerDidUpdateState(centralManager)
    }

}

extension BluetoothManager: CBCentralManagerDelegate, CBPeripheralDelegate {

    func centralManagerDidUpdateState(_ central: CBCentralManager) {
        switch central.state {
        case .poweredOn:
            centralManager.scanForPeripherals(withServices: nil, options: nil)
            bluetoothStatus = BluetoothStatus.on

        case .poweredOff:
            resetReferences()
            bluetoothStatus = BluetoothStatus.off

        case .resetting:
            // Wait for next state update and consider logging interruption of Bluetooth service
            bluetoothStatus = BluetoothStatus.resetting

        case .unauthorized:
            // Alert user to enable Bluetooth permission in app Settings
            bluetoothStatus = BluetoothStatus.unathorized

        case .unsupported:
            // Alert user their device does not support Bluetooth and app will not work as expected
            bluetoothStatus = BluetoothStatus.unsupported

        case .unknown:
            // Wait for next state update
            bluetoothStatus = BluetoothStatus.unknown

        @unknown default:
            print("---Unknown default bluetooth state---")
        }
    }

    func centralManager(
        _ central: CBCentralManager, didDiscover peripheral: CBPeripheral,
        advertisementData: [String: Any], rssi RSSI: NSNumber
    ) {
        if !discoveredPeripherals.contains(peripheral) {
            if peripheral.name != nil {
                discoveredPeripherals.append(peripheral)
            }
        }
    }

    func connect(peripheral: CBPeripheral) {
        centralManager.connect(peripheral, options: nil)
    }

    func centralManager(
        _ central: CBCentralManager, didConnect peripheral: CBPeripheral
    ) {
        self.connectedPeripheral = peripheral
        self.isConnected = true

        peripheral.delegate = self
        peripheral.discoverServices(nil)

    }

    func centralManager(
        _ central: CBCentralManager, didFailToConnect peripheral: CBPeripheral,
        error: Error?
    ) {
        print("WARNING: Connection failed!!!!!!!!!!!!!!!!!!!!!!!!!")
    }

    func disconnect() {
        guard let peripheral = connectedPeripheral else {
            return
        }

        centralManager.cancelPeripheralConnection(peripheral)
    }

    func resetReferences() {
        self.connectedPeripheral = nil
        self.discoveredPeripherals = []
        self.isConnected = false
        self.writableCharacteristic = nil
    }

    func centralManager(
        _ central: CBCentralManager,
        didDisconnectPeripheral peripheral: CBPeripheral, error: Error?
    ) {
        resetReferences()
        centralManager.scanForPeripherals(withServices: nil, options: nil)
    }

    func peripheral(
        _ peripheral: CBPeripheral, didDiscoverServices error: Error?
    ) {
        guard peripheral.services != nil else {
            return
        }

        guard let services = peripheral.services else {
            return
        }

        // discover the service of the peripheral, then discover the characteristics of each service
        for service in services {
            peripheral.discoverCharacteristics(nil, for: service)
        }
    }

    func peripheral(
        _ peripheral: CBPeripheral,
        didDiscoverCharacteristicsFor service: CBService, error: Error?
    ) {

        guard let characteristics = service.characteristics else {
            return
        }

        for characteristic in characteristics {
            if characteristic.properties.contains(.notify) {
                peripheral.setNotifyValue(true, for: characteristic)
            }
            
            /*
            if characteristic.uuid == CBUUID(string: "FFE1") {
                if characteristic.properties.contains(.writeWithoutResponse) {
                    self.writableCharacteristic = characteristic
                }

                //self.connectedPeripheral?.readValue(for: characteristic)
                //break
            }
             */
            if characteristic.properties.contains(.writeWithoutResponse) {
                self.writableCharacteristic = characteristic
                break;
            }
        }
    }

    func peripheral(
        _ peripheral: CBPeripheral,
        didUpdateValueFor characteristic: CBCharacteristic, error: Error?
    ) {
        if let value = characteristic.value {

            if let stringValue = String(data: value, encoding: .utf8) {
                addMessageStream?(stringValue)
            }
        }
    }

    func write(_ msg: String) {
        guard let peripheral = connectedPeripheral,
            let characteristic = writableCharacteristic
        else {
            return
        }
        let data = Data(msg.utf8)
        peripheral.writeValue(data, for: characteristic, type: .withoutResponse)
    }
}

enum BluetoothStatus: String {
    case off = "OFF"
    case on = "ON"
    case resetting = "RESETTING"
    case unathorized = "UNATHORIZED"
    case unsupported = "UNSUPPORTED"
    case unknown = "UNKNOWN"
}
