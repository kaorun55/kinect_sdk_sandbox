#include <windows.h>
#include <conio.h>
#include <iostream>
#include <MSR_NuiApi.h>

int GetDeviceCount()
{
    int count = 0;
    ::MSR_NUIGetDeviceCount( &count );
    return count;
}

void CALLBACK StatusProc( const NuiStatusData* data )
{
    std::cout << GetDeviceCount() << " KINECT(s)" << std::endl;
    std::wcout << "Name : " << data->instanceName <<
                  " Status : " << (SUCCEEDED( data->hrStatus ) ? " connect" : " disconnect") << std::endl;
}

void main()
{
    std::cout << GetDeviceCount() << " KINECT(s)" << std::endl;
    ::MSR_NuiSetDeviceStatusCallback( &::StatusProc );

    while ( ::_kbhit() == 0 ) {
        ::Sleep( 100 );
    }
}
