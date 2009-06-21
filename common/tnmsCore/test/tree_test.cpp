
#include <string>
#include <vector>
#include <iostream>

#include "tnmsCore.h"
#include "HeirarchicalStringTree.hpp"
#include "HeirarchicalIntTree.hpp"
using namespace tnmsCore;



struct foo
{
    int f1;
    int f2;
    
    foo() 
        : f1(0), f2(0) 
    {}
};


int main()
{
    HeirarchicalStringTree<foo>         tree;
    HeirarchicalStringTree<foo>::Node * node = NULL;

    std::vector<HeirarchicalStringTree<foo>::Node*>  addlist;

    std::string  name1 = "test/foo/bar/joy";
    std::string  name2 = "test/foo";
    std::string  name3 = "/test/foo/";
    std::string  name4 = "/test/foo/bar/joy/beer1/";

    std::cout << "----------------------------" << std::endl
              << "   HeirarchicalStringTree Test"   << std::endl
              << std::endl;

    //-----------------------------------------------------------------------

    node = tree.insert(name1, std::inserter(addlist, addlist.begin()));

    std::cout << "List size for adding '" << name1 << "' is " << addlist.size()
              << std::endl;
    std::cout << "tree size is " << tree.size() << std::endl;

    node->getValue().f1 = 4;
    node->getValue().f2 = 4;

    //-----------------------------------------------------------------------

    node = tree.find(name2);

    if ( node == NULL ) {
        std::cout << "ERROR! node not found for branch " << name2 << std::endl;
        return -1;
    }

    std::cout << "Retrieved node " << node->getAbsoluteName() 
        << std::endl << std::endl;
    node->getValue().f1 = 2;
    node->getValue().f2 = 2;

    //-----------------------------------------------------------------------

    addlist.clear();
    std::cout << "Attempting duplicate insert of " << name3 << std::endl;

    node = tree.insert(name3, std::inserter(addlist, addlist.begin()));

    if ( node != NULL ) {
        std::cout << "ERROR! Duplicate insert not detected for " << name3 
            << std::endl;
        return -1;
    }

    std::cout << "duplicate name properly detected" << std::endl << std::endl;

    //-----------------------------------------------------------------------

    node = tree.insert(name4, std::inserter(addlist, addlist.begin()));

    std::cout << "Add of " << name4 << " resulted in " << addlist.size() 
        << " adds." << std::endl;

    if ( addlist.size() != 1 ) {
        std::cout << "ERROR! too many adds for " << name4 << std::endl;
        return -1;
    }

    //-----------------------------------------------------------------------

    node = tree.find(name1);

    if ( node == NULL ) {
        std::cout << "ERROR node not found " << std::endl;
        return -1;
    }

    std::cout << "Retrieved node for name '" << node->getName() << "' of path '"
        << node->getAbsoluteName() << std::endl;
    std::cout << " f1 = " << node->getValue().f1 << " f2 = " 
        << node->getValue().f2 << std::endl << std::endl;

    //-----------------------------------------------------------------------

    std::cout << std::endl << "Size before erase: " << tree.size() << std::endl;
    std::cout << "  erasing '" << name2 << "'" << std::endl;

    std::set<std::string> strings;
    //tree.erase(name2, std::inserter(strings, strings.begin()));
    node = tree.find(name2);
    tree.erase(node, std::inserter(strings, strings.begin()));

    std::set<std::string>::iterator  sIter;
    for ( sIter = strings.begin(); sIter != strings.end(); ++sIter )
        std::cout << "  Erased node " << *sIter << std::endl;


    std::cout << std::endl << "Size after erase: " << tree.size()
              << std::endl << std::endl;


    //-----------------------------------------------------------------------
    //   HeirarchicalIntTree
    //-----------------------------------------------------------------------

    std::cout << "----------------------------" << std::endl
              << "   HeirarchicalIntTree "   << std::endl
              << std::endl;

    HeirarchicalIntTree<foo>  inttree;

    TnmsOid oid1("1.2.3.4");
    TnmsOid oid2("1.2");
    TnmsOid oid3("1.2.");
    TnmsOid oid4("1.2.3.4.5");
    TnmsOid oid5("1.2.3.5.1");


    std::set<HeirarchicalIntTree<foo>::Node*>  iaddlist;

    HeirarchicalIntTree<foo>::Node * inode = NULL;

    std::cout << "Testing first OID: '" << oid1.toString() << "'" << std::endl;

    OidList::size_type  bi;
    for ( bi = 0; bi < oid1.size(); ++bi )
        std::cout << " index " << bi << " = '" << oid1[bi] << "'" << std::endl;

    bi = 2;
    TnmsOid * noid = TnmsOid::OidFromOidIndex(oid1, bi);
    std::cout << "OidFromIndex() indx: " << bi << " for oid1: "
              << noid->toString() << std::endl;

    //-----------------------------------------------------------------------

    inode = inttree.insert(oid1, std::inserter(iaddlist, iaddlist.begin()));

    std::cout << "list size for adding '" << oid1.toString() << "' is " 
        << iaddlist.size() << std::endl;
    std::cout << "tree size is " << inttree.size() << std::endl;

    inode->getValue().f1 = 4;
    inode->getValue().f2 = 4;

    //-----------------------------------------------------------------------

    inode = inttree.find(oid2);

    if ( inode == NULL ) {
        std::cout << "ERROR! node not found for branch " << oid2.toString() 
            << std::endl;
        return -1;
    }

    TnmsOid oid2out = inode->getAbsoluteName();

    //std::cout << "Retrieved node " << inode->getAbsoluteName().toString() 
        //<< "' "
    std::cout << "Retrieved node '" << oid2out.toString() << "' "
			  << std::endl << std::endl;

    inode->getValue().f1 = 2;
    inode->getValue().f2 = 2;

    //-----------------------------------------------------------------------

    iaddlist.clear();
    std::cout << "Attempting duplicate insert of " << oid3.toString() 
        << std::endl;

    inode = inttree.insert(oid3, std::inserter(iaddlist, iaddlist.begin()));

    if ( inode != NULL ) {
        std::cout << "ERROR! Duplicate insert not detected for " 
            << name3 << std::endl;
        return -1;
    }

    std::cout << "duplicate name properly detected" << std::endl << std::endl;

    //-----------------------------------------------------------------------

    inode = inttree.insert(oid4, std::inserter(iaddlist, iaddlist.begin()));

    std::cout << "Add of " << oid4.toString() << " resulted in " 
        << iaddlist.size() << " adds." << std::endl;

    if ( iaddlist.size() != 1 ) {
        std::cout << "ERROR! too many adds for " << oid4.toString() 
            << std::endl;
        return -1;
    }

    //-----------------------------------------------------------------------

    inode = inttree.find(oid1);

    if ( inode == NULL ) {
        std::cout << "ERROR node not found " << std::endl;
        return -1;
    }

    std::cout << "Retrieved node for name '" << inode->getName() << "' of path '"
        << inode->getAbsoluteName().toString() << std::endl;
    std::cout << " f1 = " << inode->getValue().f1 << " f2 = " << inode->getValue().f2
        << std::endl << std::endl;

    //-----------------------------------------------------------------------

    std::cout << std::endl << "Size before erase: " << inttree.size
        () << std::endl;
    std::cout << "  erasing '" << oid2.toString() << "'" << std::endl;

    std::set<TnmsOid>   oidset;
    //inttree.erase(name2, std::inserter(oidset, oidset.begin()));
    inode = inttree.find(oid2);
    inttree.erase(inode, std::inserter(oidset, oidset.begin()));

    std::set<TnmsOid>::iterator  oIter;
    for ( oIter = oidset.begin(); oIter != oidset.end(); ++oIter )
        std::cout << "  Erased node " << oIter->toString() << std::endl;

    std::cout << std::endl << "Size after erase: " << inttree.size() 
        << std::endl;

    return 0;
}


