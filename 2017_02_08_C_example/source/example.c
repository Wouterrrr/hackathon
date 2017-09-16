#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <signal.h>
#endif // WIN32
#include "marvelmind.h"

bool terminateProgram=false;

#ifdef WIN32
BOOL CtrlHandler( DWORD fdwCtrlType )
{
    if ((fdwCtrlType==CTRL_C_EVENT ||
            fdwCtrlType==CTRL_BREAK_EVENT ||
            fdwCtrlType==CTRL_CLOSE_EVENT ||
            fdwCtrlType==CTRL_LOGOFF_EVENT ||
            fdwCtrlType==CTRL_SHUTDOWN_EVENT) &&
            (terminateProgram==false))
    {
        terminateProgram=true;
        return true;
    }
    else return false;
}
#else
void CtrlHandler(int signum)
{
    terminateProgram=true;
}
#endif

#ifdef WIN32
void sleep(unsigned int seconds)
{
    Sleep (seconds*1000);
}
#endif

int main (int argc, char *argv[])
{
    // get port name from command line arguments (if specified)
    const char * ttyFileName;
    if (argc==2) ttyFileName=argv[1];
    else ttyFileName=DEFAULT_TTY_FILENAME;

    // Init
    struct MarvelmindHedge * hedge=createMarvelmindHedge ();
    if (hedge==NULL)
    {
        puts ("Error: Unable to create MarvelmindHedge");
        return -1;
    }
    hedge->ttyFileName=ttyFileName;
    hedge->verbose=true; // show errors and warnings
    startMarvelmindHedge (hedge);

    // Set Ctrl-C handler
#ifdef WIN32
    SetConsoleCtrlHandler( (PHANDLER_ROUTINE) CtrlHandler, TRUE );
#else
    signal (SIGINT, CtrlHandler);
    signal (SIGQUIT, CtrlHandler);
#endif

    // Main loop
    while ((!terminateProgram) && (!hedge->terminationRequired))
    {
        //sleep (3);
        printPositionFromMarvelmindHedge (hedge, true);

        printStationaryBeaconsPositionsFromMarvelmindHedge (hedge, true);
    }

    // Exit
    stopMarvelmindHedge (hedge);
    destroyMarvelmindHedge (hedge);
    return 0;
}
