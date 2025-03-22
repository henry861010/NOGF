//
//  CameraManager.swift
//  NOGF
//
//  Created by henry on 2025/3/1.
//


//
//  CameraManager.swift
//  SwiftUIDemo2
//
//  Created by Itsuki on 2024/05/15.
//

import UIKit
import AVFoundation


class CameraManager: NSObject {
    private let captureSession = AVCaptureSession()
    
    @Published var isRecordVedio: Bool = false
    
    private var isCaptureSessionConfigured = false
    private var deviceInput: AVCaptureDeviceInput?
    private var photoOutput: AVCapturePhotoOutput?
    private var movieFileOutput: AVCaptureMovieFileOutput?
    
    // for preview
    private var videoOutput: AVCaptureVideoDataOutput?
    private var sessionQueue: DispatchQueue!
    
    private var captureDevice: AVCaptureDevice? {
        didSet {
            guard let captureDevice = captureDevice else { return }
            sessionQueue.async {
                self.updateSessionForCaptureDevice(captureDevice)
            }
        }
    }
        
    var isRunning: Bool {
        captureSession.isRunning
    }
    
    // photo stream
    private var addToPhotoStream: ((AVCapturePhoto) -> Void)?
    lazy var photoStream: AsyncStream<AVCapturePhoto> = {
        AsyncStream { continuation in
            addToPhotoStream = { photo in
                continuation.yield(photo)
            }
        }
    }()
    
    // movie stream
    private var addToMovieFileStream: ((URL) -> Void)?
    lazy var movieFileStream: AsyncStream<URL> = {
        AsyncStream { continuation in
            addToMovieFileStream = { fileUrl in
                continuation.yield(fileUrl)
            }
        }
    }()
    
    // preview device output stream
    var isPreviewPaused = false
    private var addToPreviewStream: ((CIImage) -> Void)?
    lazy var previewStream: AsyncStream<CIImage> = {
        AsyncStream { continuation in
            addToPreviewStream = { ciImage in
                if !self.isPreviewPaused {
                    continuation.yield(ciImage)
                }
            }
        }
    }()
    
    
    override init() {
        super.init()
        // The value of this property is an AVCaptureSessionPreset indicating the current session preset in use by the receiver. The sessionPreset property may be set while the receiver is running.
        captureSession.sessionPreset = .high
        
        sessionQueue = DispatchQueue(label: "session queue")
        //captureDevice = availableCaptureDevices.first ?? AVCaptureDevice.default(for: .video)
        captureDevice = AVCaptureDevice.default(.builtInWideAngleCamera, for: .video, position: .back)
    }
    

    func start() async {
        let authorized = await checkAuthorization()
        guard authorized else {
            print("Camera access was not authorized.")
            return
        }
        
        if isCaptureSessionConfigured {
            if !captureSession.isRunning {
                sessionQueue.async { [self] in
                    self.captureSession.startRunning()
                }
            }
            return
        }
        
        sessionQueue.async { [self] in
            self.configureCaptureSession { success in
                guard success else { return }
                self.captureSession.startRunning()
            }
        }
    }
    
    func stop() {
        guard isCaptureSessionConfigured else { return }
        
        if captureSession.isRunning {
            sessionQueue.async {
                self.captureSession.stopRunning()
            }
        }
    }
    
    // switch between available cameras
    func switchCaptureDevice() {
        // ???
    }
    
    
    func startRecordingVideo() {
        if !isRecordVedio {
            guard let movieFileOutput = self.movieFileOutput else {
                print("Cannot find movie file output")
                return
            }
            
            guard
                let directoryPath = FileManager.default.urls(for: .documentDirectory, in: .userDomainMask).first
            else {
                print("Cannot access local file domain")
                return
            }
            
            let fileName = UUID().uuidString
            let filePath = directoryPath
                .appendingPathComponent(fileName)
                .appendingPathExtension("mp4")
            
            movieFileOutput.startRecording(to: filePath, recordingDelegate: self)
            isRecordVedio = true
        }
    }
    
    func stopRecordingVideo() {
        if isRecordVedio {
            guard let movieFileOutput = self.movieFileOutput else {
                print("Cannot find movie file output")
                return
            }
            movieFileOutput.stopRecording()
            isRecordVedio = false
        }
    }


    
    func takePhoto() {
        guard let photoOutput = self.photoOutput else { return }
        
        sessionQueue.async {
            var photoSettings = AVCapturePhotoSettings()

            if photoOutput.availablePhotoCodecTypes.contains(.hevc) {
                photoSettings = AVCapturePhotoSettings(format: [AVVideoCodecKey: AVVideoCodecType.hevc])
            }
            
            let isFlashAvailable = self.deviceInput?.device.isFlashAvailable ?? false
            photoSettings.flashMode = isFlashAvailable ? .auto : .off
            if let previewPhotoPixelFormatType = photoSettings.availablePreviewPhotoPixelFormatTypes.first {
                photoSettings.previewPhotoFormat = [kCVPixelBufferPixelFormatTypeKey as String: previewPhotoPixelFormatType]
            }
            photoSettings.photoQualityPrioritization = .balanced
            
            if let photoOutputVideoConnection = photoOutput.connection(with: .video) {
                photoOutputVideoConnection.videoRotationAngle = RotationAngle.portrait.rawValue
            }
            
            photoOutput.capturePhoto(with: photoSettings, delegate: self)
        }
    }

    
    private func updateSessionForCaptureDevice(_ captureDevice: AVCaptureDevice) {
        guard isCaptureSessionConfigured else { return }
        
        captureSession.beginConfiguration()
        defer { captureSession.commitConfiguration() }

        for input in captureSession.inputs {
            if let deviceInput = input as? AVCaptureDeviceInput {
                captureSession.removeInput(deviceInput)
            }
        }
        
        if let deviceInput = deviceInputFor(device: captureDevice) {
            if !captureSession.inputs.contains(deviceInput), captureSession.canAddInput(deviceInput) {
                captureSession.addInput(deviceInput)
            }
        }
        
        updateVideoOutputConnection()
    }
    
    private func deviceInputFor(device: AVCaptureDevice?) -> AVCaptureDeviceInput? {
        guard let validDevice = device else { return nil }
        do {
            return try AVCaptureDeviceInput(device: validDevice)
        } catch let error {
            print("Error getting capture device input: \(error.localizedDescription)")
            return nil
        }
    }
    
    private func configureCaptureSession(completionHandler: (_ success: Bool) -> Void) {
        
        var success = false
        
        self.captureSession.beginConfiguration()
        
        defer {
            self.captureSession.commitConfiguration()
            completionHandler(success)
        }
        
        guard
            let captureDevice = captureDevice,
            let deviceInput = try? AVCaptureDeviceInput(device: captureDevice)
        else {
            print("Failed to obtain video input.")
            return
        }
        
        let movieFileOutput = AVCaptureMovieFileOutput()
        
        let photoOutput = AVCapturePhotoOutput()
        captureSession.sessionPreset = AVCaptureSession.Preset.photo
        
        let videoOutput = AVCaptureVideoDataOutput()
        videoOutput.setSampleBufferDelegate(self, queue: DispatchQueue(label: "VideoDataOutputQueue"))
        
        guard captureSession.canAddInput(deviceInput) else {
            print("Unable to add device input to capture session.")
            return
        }
        guard captureSession.canAddOutput(photoOutput) else {
            print("Unable to add photo output to capture session.")
            return
        }
        guard captureSession.canAddOutput(videoOutput) else {
            print("Unable to add video output to capture session.")
            return
        }
        
        captureSession.addInput(deviceInput)
        captureSession.addOutput(photoOutput)
        captureSession.addOutput(videoOutput)
        captureSession.addOutput(movieFileOutput)
        
        self.deviceInput = deviceInput
        self.photoOutput = photoOutput
        self.videoOutput = videoOutput
        self.movieFileOutput = movieFileOutput

        photoOutput.maxPhotoQualityPrioritization = .quality
        
        updateVideoOutputConnection()
        
        isCaptureSessionConfigured = true
        
        success = true
    }
    
    
    private func checkAuthorization() async -> Bool {
        switch AVCaptureDevice.authorizationStatus(for: .video) {
        case .authorized:
            print("Camera access authorized.")
            return true
        case .notDetermined:
            print("Camera access not determined.")
            sessionQueue.suspend()
            let status = await AVCaptureDevice.requestAccess(for: .video)
            sessionQueue.resume()
            return status
        case .denied:
            print("Camera access denied.")
            return false
        case .restricted:
            print("Camera library access restricted.")
            return false
        default:
            return false
        }
    }
    
    private func updateVideoOutputConnection() {
        if let videoOutput = videoOutput, let videoOutputConnection = videoOutput.connection(with: .video) {
            if videoOutputConnection.isVideoMirroringSupported {
                videoOutputConnection.isVideoMirrored = false
            }
        }
    }

    func zoom(_ factor: CGFloat) {
        guard let device = captureDevice else { return }
        
        do {
            try device.lockForConfiguration()
            let maxZoomFactor = device.activeFormat.videoMaxZoomFactor
            let clampedFactor = max(1.0, min(factor, maxZoomFactor))
            //device.videoZoomFactor = clampedFactor
            device.ramp(toVideoZoomFactor: clampedFactor, withRate: 1.0)
            device.unlockForConfiguration()
        } catch {
            print("Error setting zoom factor: \(error)")
        }
    }
}


extension CameraManager: AVCapturePhotoCaptureDelegate {
    
    func photoOutput(_ output: AVCapturePhotoOutput, didFinishProcessingPhoto photo: AVCapturePhoto, error: Error?) {
        
        if let error = error {
            print("Error capturing photo: \(error.localizedDescription)")
            return
        }
        
        addToPhotoStream?(photo)
    }
    
}

extension CameraManager: AVCaptureVideoDataOutputSampleBufferDelegate {
    
    func captureOutput(_ output: AVCaptureOutput, didOutput sampleBuffer: CMSampleBuffer, from connection: AVCaptureConnection) {
        guard let pixelBuffer = sampleBuffer.imageBuffer else { return }
        connection.videoRotationAngle = RotationAngle.portrait.rawValue
        addToPreviewStream?(CIImage(cvPixelBuffer: pixelBuffer))
    }
}

extension CameraManager: AVCaptureFileOutputRecordingDelegate {
    func fileOutput(_ output: AVCaptureFileOutput, didFinishRecordingTo outputFileURL: URL, from connections: [AVCaptureConnection], error: Error?) {
        if let error = error {
            print("Error capturing photo: \(error.localizedDescription)")
            return
        }
        addToMovieFileStream?(outputFileURL)
    }
}


private enum RotationAngle: CGFloat {
    case portrait = 90
    case portraitUpsideDown = 270
    case landscapeRight = 180
    case landscapeLeft = 0
}

