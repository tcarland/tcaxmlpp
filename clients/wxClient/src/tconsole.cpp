
extern "C" {
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
}
#include <string>
#include <sstream>
#include <vector>
#include <list>
#include <map>

#include "TnmsConsoleApp.h"
using namespace tnmsconsole;


void sigHandler ( int signal )
{
    if ( signal == SIGINT || signal == SIGTERM )
        std::cout << "Interrupt caught. Use 'quit' to exit." << std::endl;
    return;
}



int main ( int argc, char ** argv )
{
    std::list<std::string>  fileslist;
    std::list<std::string>::iterator  fIter;

    signal(SIGINT,  &sigHandler);
    signal(SIGTERM, &sigHandler);
    
    TnmsConsoleApp * mgr = NULL;

    if ( fileslist.empty() ) {
         mgr = new TnmsConsoleApp();
         mgr->run();
         delete mgr;
    }

    return 0;
}

