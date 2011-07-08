AudioCaptureRaw – READ ME
Copyright © Microsoft Corporation.  All rights reserved.  
This code is licensed under the terms of the Microsoft Kinect for
Windows SDK (Beta) from Microsoft Research License Agreement:
http://research.microsoft.com/KinectSDK-ToU

=============================
OVERVIEW  
.............................
This sample demonstrates capturing audio data using WASAPI, with no additional processing (such as mic array or AEC). As such, it'll produce
a 4 channel wav file when recording from a Kinect device.

The sample captures audio in shared mode using the timer driven programming model, saving the captured data to a file.

This sample was apapted from the CaptureSharedTimerDriver sample in the Platform SDK

=============================
Sample Language Implementations
.............................
    C++

=============================
FILES   
.............................
AudioCaptureRaw.vcproj
    This is the main project file for VC++ projects generated using an Application Wizard.
    It contains information about the version of Visual C++ that generated the file, and
    information about the platforms, configurations, and project features selected with the
    Application Wizard.

AudioCaptureRaw.cpp
    This is the main application source file.  It parses the command line and instantiates a
    CWASAPICapture object which actually performs the rendering.
    
WASAPICapture.cpp/WASAPICapture.h
    Implementation of a class which uses the WASAPI APIs to render a buffer containing audio data.
    

/////////////////////////////////////////////////////////////////////////////
Other standard files:

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named WASAPICaptureSharedTimerDriven.pch and a precompiled types file named StdAfx.obj.
targetver.h
    Specifies the OS target version for the sample (Vista+)


=============================
BUILDING THE SAMPLE
.............................

To build the sample using Visual Studio 2010:
-----------------------------------------------------------
    1. Open Windows Explorer and navigate to the directory.
    2. Double-click the icon for the .sln (solution) file to open the file in Visual Studio.
    3. In the Build menu, select Build Solution.  The application will be built in the default 
       \Debug or \Release directory

=============================
RUNNING THE SAMPLE   
.............................
    Type AudioCaptureRaw.exe at the command line
