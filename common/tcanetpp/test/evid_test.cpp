
#include <stdint.h>
#include <iostream>
#include <string>
#include <set>


namespace foo {

typedef uint16_t myid_t;
typedef std::set<myid_t>  EventSet;




EventSet evset;


myid_t  getNewId ( myid_t & id, myid_t & lastid)
{
    id = lastid + 1;

    EventSet::iterator  sIter;

    while ( id != lastid ) {
        if ( (sIter = evset.find(id)) == evset.end() && id != 0 )
            break;
        id++;
    }

    if ( id == lastid || id == 0 ) {
        std::cout << "NO MORE ID's" << std::endl;
        return 0;
    } else {
        evset.insert(id);
        lastid = id;
    }

    return id;
}


} // namespace

using namespace foo;



int main() 
{
    myid_t  lastid = 0;
    myid_t  myid   = 0;

    std::cout << "id starting is: " << myid << " lastid: " << lastid
        << std::endl;

    myid = getNewId(myid, lastid);
    
    std::cout << "id is: " << myid << " lastid: " << lastid
        << std::endl;

    uint16_t max = 0;
    max--;

    for ( short i = 0; i < max; i++ ) {
        myid = getNewId(myid, lastid );

        if ( myid == 0 ) 
            break;
    }
    
    std::cout << "id is: " << myid << " lastid: " << lastid
        << std::endl;
    std::cout << "set size is " << evset.size() << std::endl;

    EventSet::iterator  sIter;
    myid_t  fr, bk;
    
    sIter  = evset.begin();
    fr     = *sIter;
    sIter  = evset.end();
    bk     = *sIter;
   
    std::cout << "id front is: " << fr << " id back: " << bk << std::endl;

    fr = 300;
    evset.erase(fr);

    std::cout << "set size is " << evset.size() << std::endl;
    
    myid = getNewId(myid, lastid);

    std::cout << "id is: " << myid << " lastid: " << lastid
        << std::endl;
    std::cout << "set size is " << evset.size() << std::endl;
    return 0;
}

