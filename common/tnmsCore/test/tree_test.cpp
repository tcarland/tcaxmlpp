
#include <string>
#include <set>
#include <iostream>

#include "HeirarchicalStringTree.hpp"
using namespace tnmsCore;



struct foo
{
    foo() : f1(0), f2(0) {}
    int f1;
    int f2;
};


int main()
{
    HeirarchicalStringTree<foo>  tree;

    std::string  name1 = "test/foo/bar/joy";
    std::string  name2 = "test/foo";
    std::string  name3 = "/test/foo/";
    std::string  name4 = "/test/foo/bar/joy/beer1/";
    
    std::set<HeirarchicalStringTree<foo>::Node*>  addlist;
    
    HeirarchicalStringTree<foo>::Node * node = NULL;
    
    std::cout << std::endl;
    
    //-----------------------------------------------------------------------
    
    node = tree.insert(name1, std::inserter(addlist, addlist.begin()));
    
    std::cout << "list size for adding '" << name1 << "' is " << addlist.size() 
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
    
    std::cout << "Retrieved node " << node->getAbsoluteName() << std::endl << std::endl;
    node->getValue().f1 = 2;
    node->getValue().f2 = 2;
    
    //-----------------------------------------------------------------------
    
    addlist.clear();
    std::cout << "Attempting duplicate insert of " << name3 << std::endl;
    
    node = tree.insert(name3, std::inserter(addlist, addlist.begin()));
    
    if ( node != NULL ) {
        std::cout << "ERROR! Duplicate insert not detected for " << name3 << std::endl;
        return -1;
    }
    
    std::cout << "duplicate name properly detected" << std::endl << std::endl;
    
    //-----------------------------------------------------------------------
    
    node = tree.insert(name4, std::inserter(addlist, addlist.begin()));
    
    std::cout << "Add of " << name4 << " resulted in " << addlist.size() << " adds." << std::endl;
    
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
    std::cout << " f1 = " << node->getValue().f1 << " f2 = " << node->getValue().f2 
        << std::endl << std::endl;
    
    //-----------------------------------------------------------------------
    
    std::cout << std::endl << "Size before erase: " << tree.size() << std::endl;
    std::cout << "  erasing '" << name2 << "'" << std::endl;
    
    std::set<std::string> strings;
    //tree.erase(name2, std::inserter(strings, strings.begin()));
    node = tree.find(name2);
    tree.erase(node, std::inserter(strings, strings.begin()));
    
    std::cout << std::endl << "Size after erase: " << tree.size() << std::endl;
    
    std::set<std::string>::iterator  sIter;
    for ( sIter = strings.begin(); sIter != strings.end(); ++sIter ) 
        std::cout << "  Erased node " << *sIter << std::endl;
    
    //-----------------------------------------------------------------------
    
    return 0;
}


