using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Research.Kinect.Audio;

namespace audio_device_info
{
    class Program
    {
        static void Main( string[] args )
        {
            KinectAudioSource kas = new KinectAudioSource();
            foreach ( AudioDeviceInfo device in kas.FindCaptureDevices() ) {
                Console.WriteLine( "DeviceIndex:" + device.DeviceIndex + " " +
                                   "DeviceName :" + device.DeviceName + " " +
                                   "DeviceID   :" + device.DeviceID );
            }
        }
    }
}
