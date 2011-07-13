Speech - READ ME 

Copyright (c) Microsoft Corporation. All rights reserved.
This code is licensed under the terms of the Microsoft Kinect for
Windows SDK (Beta) from Microsoft Research License Agreement:
http://research.microsoft.com/KinectSDK-ToU


=============================
OVERVIEW  
.............................
This module provides sample code used to demonstrate how to perform speech recognition
on audio captured from a Kinect device.
Hypothesized and recognized text are written to console output.

=============================
SAMPLE LANGUAGE IMPLEMENTATIONS     
.............................
This sample is available in C#


=============================
FILES   
.............................
- Program.cs: Defines Speech.Program class and associated executable Main function

=============================
PREREQUISITES   
.............................
- Speech Platform Runtime (v10.2) x86. Even on x64 platforms the x86 needs to be used because the MSR Kinect SDK runtime is x86
  http://www.microsoft.com/downloads/en/details.aspx?FamilyID=bb0f72cb-b86b-46d1-bf06-665895a313c7
- Speech Platform SDK (v10.2) 
  http://www.microsoft.com/downloads/en/details.aspx?FamilyID=1b1604d3-4f66-4241-9a21-90a294a5c9a4&displaylang=en
- Kinect English Language Pack: MSKinectLangPack_enUS.msi (available in the same location as the Kinect For Windows SDK)

=============================
BUILDING THE SAMPLE   
.............................

To build the sample using Visual Studio:
-----------------------------------------------------------
1. In Windows Explorer, navigate to the Speech\CS directory.
2. Double-click the icon for the .sln (solution) file to open the file in Visual Studio.
3. In the Build menu, select Build Solution. The application will be built in the default \Debug or \Release directory.


=============================
RUNNING THE SAMPLE   
.............................

To run the sample:
------------------
1. Navigate to the directory that contains the new executable, using the command prompt or Windows Explorer.
2. Type Speech at the command line, or double-click the icon for Speech.exe to launch it from Windows Explorer.

