/*
 *  TreeAuthorizations.hpp
 *
 *  Copyright(c) 2008,2009  Timothy Charlton Arland
 *  Author: tca@charltontechnology.net
 */
#ifndef _TNMSCORE_TREEAUTHORIZATIONS_HPP_
#define _TNMSCORE_TREEAUTHORIZATIONS_HPP_

#include <list>
#include <string>

#include "StringUtils.h"
using namespace tcanetpp;


namespace tnmsCore {


typedef std::list<std::string>  TreeAuthList;


struct TreeAuthFunctor {
    TreeAuthList  & authlist;

    TreeAuthFunctor ( TreeAuthList & authlist_ )
        : authlist(authlist_)
    {}
    virtual ~TreeAuthFunctor() {}

    virtual bool operator() ( const std::string & metric_name ) = 0;
};


struct AuthAllFunctor : public TreeAuthFunctor {

    AuthAllFunctor ( TreeAuthList & authlist_ )
        : TreeAuthFunctor(authlist_)
    {}
    virtual ~AuthAllFunctor() {}

    bool operator() ( const std::string & metric_name )
    {
        return true;
    }
};


struct AuthIncludeFunctor : public TreeAuthFunctor {

    AuthIncludeFunctor ( TreeAuthList & authlist )
        : TreeAuthFunctor(authlist)
    {}
    virtual ~AuthIncludeFunctor() {}

    bool operator() ( const std::string & metric_name )
    {
        TreeAuthList::iterator  sIter;
        for ( sIter = authlist.begin(); sIter != authlist.end(); ++sIter )
            if ( StringUtils::startsWith(metric_name, *sIter) )
                return true;
        return false;
    }
};


struct AuthExcludeFunctor : public TreeAuthFunctor {

    AuthExcludeFunctor ( TreeAuthList & authlist_ )
        : TreeAuthFunctor(authlist_)
    {}
    virtual ~AuthExcludeFunctor() {}

    bool operator() ( const std::string & metric_name )
    {
        TreeAuthList::iterator  sIter;
        for ( sIter = authlist.begin(); sIter != authlist.end(); ++sIter )
            if ( StringUtils::startsWith(metric_name, *sIter) )
                return false;
        return true;
    }
};


} // namespace

#endif /* _TNMSCORE_TREEAUTHORIZATIONS_HPP_ */

