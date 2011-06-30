// AudioDeviceInfoのサンプル
using System;
using Microsoft.Research.Kinect.Audio;

namespace audio_device_info
{
    class Program
    {
        static void Main( string[] args )
        {
            KinectAudioSource kinectAudio = new KinectAudioSource();
            foreach ( AudioDeviceInfo device in kinectAudio.FindCaptureDevices() ) {
                Console.WriteLine( "DeviceIndex:" + device.DeviceIndex + " " +
                                   "DeviceName :" + device.DeviceName + " " +
                                   "DeviceID   :" + device.DeviceID );
            }
        }
    }
}
