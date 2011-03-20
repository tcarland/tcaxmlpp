#define _TNMSCONSOLE_TNMSCONSOLEAPP_CPP_

#include <time.h>
#include <sstream>
#include <list>

#include "TnmsConsoleApp.h"

#include "StringUtils.h"
using namespace tcanetpp;



namespace tnmsConsole {


struct NodeDumpPredicate {
    std::list<TnmsTree::Node*> nodes;
    void operator() ( TnmsTree::Node * node )
    {
        nodes.push_front(node);
    }
};


TnmsConsoleApp::TnmsConsoleApp() 
    : _mainPanel(NULL),
      _statPanel(NULL),
      _consPanel(NULL),
      _mtree(new ClientTreeMutex()),
      _iomgr(new ClientIOThread(_mtree)),
      _alarm(false),
      _stop(false),
      _tht(1),
      _mainht(0),
      _statht(0),
      _consht(3)
{
    _showI = _apis.end();
    _iomgr->start();
}


TnmsConsoleApp::~TnmsConsoleApp()
{
    delete _iomgr;
    delete _mtree;
}


void
TnmsConsoleApp::resize()
{
    int staty, consy;

    int ht  = this->height();
    _statht = (ht * .33) - _consht;
    _mainht = ht - _statht - _consht - _tht;
    staty   = ht - _statht - _consht;
    consy   = ht - _consht;

    _mainPanel->resize(_mainht, this->width());

    _statPanel->resize(_statht, this->width());
    _statPanel->erase();
    _statPanel->moveWindow(staty, 0);

    _consPanel->resize(_consht, this->width());
    _consPanel->erase();
    _consPanel->moveWindow(consy, 0);

    return;
}


void
TnmsConsoleApp::run()
{
    int  conht  = 3;
    int  statht = (this->height() * .33) - conht;
    int  ch;

    LineInputHandler * conin;

    _prompt    = "[tnms]> ";
    _title     = " tnms-console ";
    _mainPanel = this->createPanel(" main ", (this->height() - statht - conht - 1), this->width(), 1, 0);
    _statPanel = this->createPanel(" status ", statht, this->width(), (this->height() - statht - conht), 0);
    _consPanel = this->createPanel("console", conht, this->width(), (this->height() - conht), 0);

    _mainPanel->enableScroll(true);
    _mainPanel->setBorderColor(HEX_MAGENTA);

    _statPanel->enableScroll(true);
    _statPanel->setBorderColor(HEX_MAGENTA);
    _statPanel->setTextColor(HEX_GREEN);
    //_statPanel->setTextColor(HEX_RED);
    
    _consPanel->enableScroll(true);
    _consPanel->drawBorder(false);
    _consPanel->drawTitle(false);
    _consPanel->setInputHandler(new LineInputHandler());
    _consPanel->setOutputHandler(new LineOutputHandler());
    _consPanel->addText(_prompt);

    this->setTopPanel(_consPanel);
    this->print(0, 1, _title, HEX_GREEN, HEX_BOLD);
    this->setCursor(0);

    conin   = (LineInputHandler*) _consPanel->getInputHandler();

    std::string  cmd;

    while ( ! _alarm )
    {
        this->draw();

        cmd = "";
        ch  = this->poll();

        while ( ! conin->isReady() ) {
            this->draw();
            ch = this->poll();
        }

        cmd = conin->getLine();
        _consPanel->setText(_prompt);

        if ( cmd.size() > 0 )
            this->processCmd(cmd);
    }

    _iomgr->stop();

    return;
}


bool
TnmsConsoleApp::processCmd ( const std::string & cmdstr )
{
    std::string   cmd, name, desc, agentname, cfgfile;
    uint64_t      val = 0;
    time_t        now, ts;
 
    std::ostringstream        msg;
    std::vector<std::string>  cmdlist;
    ApiMap::iterator          aIter;

    StringUtils::split(cmdstr, ' ', std::back_inserter(cmdlist));
    //_statPanel->addText(cmdstr);

    if ( cmdlist.size() > 0 )
        cmd = cmdlist.at(0);
    else
        cmd = "";

    StringUtils::toLowerCase(cmd);

    now = ::time(NULL);
    ts  = now;

    // CREATE
    if ( cmd.compare("create") == 0 )
    {   // create instance
        if ( cmdlist.size() < 3 ) {
            msg << "Error: invalid syntax ";
            _statPanel->addText(msg.str());
            return false;
        }

        name      = cmdlist.at(1);
        agentname = cmdlist.at(2);

        if ( _apis.find(name) != _apis.end() ) {
            msg << "Instance already exists for '" << name << "' ";
            _statPanel->addText(msg.str());
            return false;
        }

        TnmsAPI * api = new TnmsAPI(agentname);

        if ( cmdlist.size() == 4 ) {
            cfgfile = cmdlist.at(3);
            api->set_config(cfgfile);
        }
        
        ApiMapInsert  insertR = _apis.insert(ApiMap::value_type(name, api));

        if ( ! insertR.second ) {
            msg << "Error: Insert failed for '" << name << "' ";
            _statPanel->addText(msg.str());
            return false;
        }

        _showI  = insertR.first;
        _prompt = "[tnms : " + _showI->first + "]> ";
    } 
    else if ( cmd.compare("destroy") == 0 )  // DESTROY
    {
        if ( cmdlist.size() != 2 ) {
            msg << "Error: invalid syntax ";
            _statPanel->addText(msg.str());
            return false;
        }

        name  = cmdlist.at(1);
        aIter = _apis.find(name);

        if ( aIter == _apis.end() ) {
            msg << "Error: '" << name << "' not found. ";
            _statPanel->addText(msg.str());
            return false;
        }
        if ( aIter == _showI ) {
            _showI = _apis.end();
            _prompt = "[tnms : n/a]> ";
        }
        delete aIter->second;
        _apis.erase(aIter);
    }
    else if ( cmd.compare("list") == 0 )   // LIST
    {
        _mainPanel->addText("TnmsAPI List:");
        for ( aIter = _apis.begin(); aIter != _apis.end(); ++aIter ) 
            _mainPanel->addText(aIter->first);
        _mainPanel->addText(" --- ", HEX_GREEN, HEX_DIM);
    }
    else if ( cmd.compare("set") == 0 )  // SET
    {
        if ( cmdlist.size() != 2 ) {
            msg << "Error: invalid syntax ";
            _statPanel->addText(msg.str());
            return false;
        }

        name  = cmdlist.at(1);
        aIter = _apis.find(name);

        if ( aIter == _apis.end() ) {
            msg << "Error: API instance '" << name << "' not found. ";
            _statPanel->addText(msg.str());
            return false;
        }
        _showI  = aIter;
        _prompt = "[tnms : " + _showI->first + "]";
        _prompt.append("> ");
    }
    else if ( cmd.compare("add") == 0 )   // ADD
    {
        if ( cmdlist.size() < 2 ) {
            msg << "Error: invalid syntax.";
            _statPanel->addText(msg.str(), HEX_YELLOW);
            return false;
        }

        if ( _showI == _apis.end() ) {
            msg << "No valid instance in this context.";
            _statPanel->addText(msg.str(), HEX_YELLOW);
            return false;
        }

        name   = cmdlist.at(1);

        if ( cmdlist.size() == 3 )
            ts = StringUtils::fromString<time_t>(cmdlist.at(2));
        else
            ts = now;

        //  api->add
        if ( ! _showI->second->add(name, ts) ) {
            msg << "Error: add failed for '" << name << "'";
            _statPanel->addText(msg.str(), HEX_RED);
            return false;
        }
    }
    else if ( cmd.compare("update") == 0 )  // UPDATE
    {
        if ( cmdlist.size() < 3 ) {
            msg << "Error: invalid syntax ";
            _statPanel->addText(msg.str(), HEX_YELLOW);
            return false;
        }

        if ( _showI == _apis.end() ) {
            msg << "No valid instance in this context ";
            _statPanel->addText(msg.str(), HEX_YELLOW);
            return false;
        }

        name = cmdlist.at(1);
        val  = StringUtils::fromString<uint64_t>(cmdlist.at(2));

        if ( cmdlist.size() == 4 )
            ts = StringUtils::fromString<time_t>(cmdlist.at(3));
        else
            ts = now;

        msg << "Update: '" << name << "' val: " << val << " tstamp: " << ts; 

        // api->update
        if ( ! _showI->second->update(name, ts, val, TNMS_UINT64) ) {
            msg << "Error: update failed";
            _statPanel->addText(msg.str(), HEX_RED);
            return false;
        }
    }
    else if ( cmd.compare("update_s") == 0 )  // UPDATE_S
    {
        if ( cmdlist.size() < 3 ) {
            msg << "Error: invalid syntax ";
            _statPanel->addText(msg.str());
            return false;
        }
        if ( _showI == _apis.end() ) {
            msg << "No valid instance in this context ";
            _statPanel->addText(msg.str());
            return false;
        }
        
        name = cmdlist.at(1);
        desc = cmdlist.at(2);
        
        if ( StringUtils::startsWith(desc, "\"") ) {
            if ( ! GetQuotedString(2, cmdlist, desc) ) {
                msg << "Error: invalid syntax";
                _statPanel->addText(msg.str());
                return false;
            }
        }

        if ( cmdlist.size() == 4 )
            ts = StringUtils::fromString<time_t>(cmdlist[3]);
        else
            ts = ::time(NULL);

        if ( ! _showI->second->update(name, ts, desc) ) {
            msg << "Error: update failed ";
            _statPanel->addText(msg.str());
            return false;
        }
    }
    else if ( cmd.compare("remove") == 0 )  // REMOVE
    {
        if ( cmdlist.size() < 2 ) {
            msg << "Error: invalid syntax for remove";
            _statPanel->addText(msg.str());
            return false;
        }
        if ( _showI == _apis.end() ) {
            msg << "No valid API instance in this context.";
            _statPanel->addText(msg.str());
            return false;
        }

        name = cmdlist.at(1);

        // api->remove
        if ( ! _showI->second->remove(name) ) {
            msg << "Error: remove failed for '" << name << "'";
            _statPanel->addText(msg.str());
            return false;
        }
    }
    else if ( cmd.compare("send") == 0 ) // SEND
    {
        // send updates
        if ( _showI == _apis.end() ) {
            msg << "No valid instance in this context ";
            _statPanel->addText(msg.str());
            return false;
        }

        int ret = 0;
        if ( (ret = this->sendAPIUpdates(_showI->second, now)) != 0 ) {
            msg << "no connection ";
            //continue;
        }
        _prompt = "[tnms : " + _showI->first + "]> ";
    }
    else if ( (cmd.compare("help") == 0) || (cmd.compare("?") == 0) )
    {
        this->DisplayHelp();
    }
    else if ( cmd.compare("debug") == 0 )
    {
       if ( _showI == _apis.end() ) {
            msg << "No valid instance in this context ";
            _statPanel->addText(msg.str());
            return false;
        }
        // dump tree
    } 
    else if ( cmd.compare("quit") == 0 ) 
    {
        if ( this->_stop )
            this->startClientProcessing();
        this->setAlarm();
        return true;
    }
    else if ( cmd.compare("client") == 0 )  // CLIENT subcommands
    {
        if ( cmdlist.size() < 2 ) {
            msg << "Error: invalid syntax ";
            _statPanel->addText(msg.str());
            return false;
        }

        if ( cmdlist.at(1).compare("stop") == 0 ) {
            if ( this->_stop )
                this->startClientProcessing();
            else
                this->stopClientProcessing();
        } else if ( cmdlist.at(1).compare("new") == 0 ) {
            // get user / pw
            this->processClientCmd(cmdlist);
        } else {
            //this->addClientCommand(cmdlist);
            this->processClientCmd(cmdlist);
        }
    } 
    else if ( cmd.compare("stop") == 0 )
    {
        if ( this->_stop )
            this->startClientProcessing();
        else
            this->stopClientProcessing();
    }
    else if ( cmd.compare("start") == 0 ) 
    {
        if ( this->_stop )
            this->startClientProcessing();
    }
    else if ( cmd.compare("clear") == 0 )
    {
        _mainPanel->clear();
    }
    else if ( cmd.compare("version") == 0 )
    {
        std::string  hexv = "libhexes version: ";
        hexv.append(LIBHEXES_VERSION);
        _mainPanel->addText("tnms-console version: 0.12");
        _mainPanel->addText(hexv);
    }
    else 
    {
        msg << "Unrecognized command: '" << cmd << "'";
    }

    if ( ! msg.str().empty() )
        _mainPanel->addText(msg.str());
            
    return true;
}


void
TnmsConsoleApp::processClientCmd ( CommandList & cmdlist )
{
    std::string  cmd, tag, name, val;

    ClientMap::iterator  cIter;

    if ( cmdlist.size() < 2 || cmdlist.at(0).compare("client") != 0 ) {
        _statPanel->addText("Error in client command");
        return;
    }

    cmd = cmdlist.at(1);

    //  New Client
    if ( cmd.compare("new") == 0 ) 
    {
        if ( cmdlist.size() < 5 )
        {
            _statPanel->addText("Syntax error in client command");
            return;
        }

        tag  = cmdlist.at(2);
        name = cmdlist.at(3);
        val  = cmdlist.at(4);

        uint16_t port = StringUtils::fromString<uint16_t>(val);
        
        if ( ! this->createClient(tag, name, port) )
            _statPanel->addText("Failed to create new client");

    } // Delete Client
    else if ( StringUtils::startsWith(cmd, "del") )
    {
         _statPanel->addText(" client delete");

        if ( cmdlist.size() < 3 ) {
            _statPanel->addText("Error in client command");
            return;
        }

        tag = cmdlist.at(2);

        this->removeClient(tag);
    } 
    else if ( cmd.compare("list") == 0 ) 
    {
        if ( cmdlist.size() > 3 && cmdlist.at(2).compare("subs") == 0 )
        {

            tag    = cmdlist.at(3);
            cIter  = _clients.find(tag);

            if ( cIter == _clients.end() )
                return;

            SubscriptionList  &  subs = cIter->second->getSubscriptionList();
            SubscriptionList::iterator sIter;

            _mainPanel->addText("Subscription List for " + cIter->second->getHostStr());
            for ( sIter = subs.begin(); sIter != subs.end(); ++sIter )
                _mainPanel->addText("  Sub> " + sIter->getElementName());
        }
        else
        {
            _mainPanel->addText("Current clients:");
            for ( cIter = _clients.begin(); cIter != _clients.end(); ++cIter )
            {
                std::ostringstream  sstr;
                TnmsClient * client = cIter->second;
                sstr << "  '" << cIter->first << "' : " << client->getHostStr();
                if ( client->isConnected() )
                    sstr << " <connected>";
                else
                    sstr << " <not connected>";
                _mainPanel->addText(sstr.str());
            }
            if ( _clients.size() == 0 )
                _mainPanel->addText( " -- no clients --");
        }
    }
    else if ( StringUtils::startsWith(cmd, "sub") )
    {
        if ( cmdlist.size() < 4 ) {
            _statPanel->addText("Syntax error in client subscribe");
            return;
        }
        tag    = cmdlist.at(2);
        name   = cmdlist.at(3);

        _statPanel->addText("client subscribe " + name);

        cIter = _clients.find(tag);
        if ( cIter != _clients.end() )
            cIter->second->subscribe(name);
    }
    else if ( StringUtils::startsWith(cmd, "unsub") )
    {
        if ( cmdlist.size() < 4 ) {
            _statPanel->addText("Syntax error in client unsubscribe");
            return;
        }

        tag    = cmdlist.at(2);
        name   = cmdlist.at(3);
        cIter  = _clients.find(tag);

        _statPanel->addText("client unsubscribe");
        if ( cIter != _clients.end() )
            cIter->second->unsubscribe(name);
    }
    else if ( cmd.compare("browse") == 0 ) 
    {
         if ( cmdlist.size() < 3 )
             return;
         
         name = cmdlist.at(2);
         //tree->debugDump(name);
    }
    else if ( cmd.compare("dump") == 0 ) 
    {
        if ( cmdlist.size() < 3 )
            name = "";
        else
            name = cmdlist.at(2);

        TnmsTree::StringList  strlist;
        TnmsTree::StringList::iterator sIter;
        TnmsTree * tree = _mtree->acquireTree();

        tree->debugDump(name, strlist);

        _mainPanel->addText("dump " + name);
        for ( sIter = strlist.begin(); sIter != strlist.end(); ++sIter )
            _mainPanel->addText(*sIter);
    }
    else if ( cmd.compare("show") == 0 ) 
    {
        if ( cmdlist.size() < 3 )
            return;

        TnmsMetric  metric;
        TnmsTree   * tree  = _mtree->acquireTree();

        if ( tree == NULL )
            return;

        name = cmdlist.at(2);
        
        if ( ! tree->request(name, metric) ) {
            _mainPanel->addText(" Metric not found for " + name);
        } else {
            _mainPanel->addText(" Node: " + metric.getElementName());
            if ( metric.getValueType() == TNMS_STRING ) {
                const std::string & valstr = metric.getValue();
                _mainPanel->addText("    Value = " + valstr);
            } else {
                uint64_t val = metric.getValue<uint64_t>();
                _mainPanel->addText("    Value = " + StringUtils::toString(val));
            }
        }
        _mtree->releaseTree();
    }
    else
    {
        _statPanel->addText("client command not recognized: " + cmd);
    }

    return;
}

bool
TnmsConsoleApp::createClient ( const std::string & name, const std::string & host, uint16_t port )
{
    TnmsTree   * tree   = _mtree->acquireTree();

    if ( tree == NULL )
        return false;

    TnmsClient * client = new TnmsClient(tree);

    if ( client->openConnection(host, port) < 0 )
        return false;

    _mtree->releaseTree();

    client->setClientLogin("tnms-console", "tnmsconsole11b");

    _clients[name] = client;
    _iomgr->addClient(client);
    _mainPanel->addText("Created new client: " + client->getHostStr());

    return true;
}


bool
TnmsConsoleApp::removeClient ( const std::string & name )
{
    ClientMap::iterator cIter;

    cIter = _clients.find(name);
    if ( cIter == _clients.end() )
        return false;

    _iomgr->removeClient(cIter->second);
    _clients.erase(cIter);

    return true;
}


void
TnmsConsoleApp::startClientProcessing()
{
    _mtree->notifyAll();
}


void
TnmsConsoleApp::stopClientProcessing()
{
    _mtree->lock();
}

void
TnmsConsoleApp::setAlarm()
{
    _alarm = true;
    if ( _iomgr )
        _iomgr->setAlarm();
}


int
TnmsConsoleApp::sendAPIUpdates ( TnmsAPI * api, const time_t & now )
{
    int     retval = 0;
    int     errcnt = 0;
    size_t  qsize  = 0;
    bool    conn   = false;

    if ( api == NULL )
        return -1;

    //LogFacility::LogMessage("Sending API updates");
    //LogFacility::Message  logmsg;
    std::ostringstream  logmsg;
    
    do {
#       ifndef WIN32
        struct tms  clocktms;
        clock_t     clk1, clk2;
        clk1 = times(&clocktms);
#       endif

        retval = api->send(now);

#       ifndef WIN32
        clk2 = times(&clocktms);
        if ( retval == 0 )
            logmsg << "send() took " << (clk2 - clk1) << " tick(s)";
#       endif

        this->sleeps(1);
       
        if ( retval == 1 ) {
            logmsg << "Invalid configuration";
            break;
        } else if ( retval > 0 ) {
            retval = api->send(now + 1);
        } else 
            conn = true;
        
        if ( conn ) {
            qsize  = api->flushsize();
            if ( qsize > 0 )
                logmsg << "Bytes queued: " << qsize;
        }
        
        if ( retval > 0 ) {
            if ( ! conn ) {
                logmsg << ".";
            } else if ( retval == TNMSERR_CONN_DENIED ) {
                logmsg << "Not authorized.";
                conn = false;
                break;
            }
            errcnt++;
        } else {
            if ( ! conn )
                logmsg << "  Connected.";
            conn = true;
        }
    } while ( (retval > 0 && errcnt < MAX_SENDERR_CNT) );

    _statPanel->addText(logmsg.str());

    return retval;
}



void
TnmsConsoleApp::sleeps ( int secs )
{
#ifdef WIN32
    Sleep(secs);
#else
    ::sleep(secs);
#endif
}


void
TnmsConsoleApp::DisplayHelp()
{
    _mainPanel->addText(" ");
    _mainPanel->addText(" tnms_console - a command-line tnms agent/client", HEX_GREEN, HEX_BOLD);
    _mainPanel->addText(" ");
    _mainPanel->addText("TnmsAPI Instance commands:", 0, HEX_BOLD);
    _mainPanel->addText(" create [tag] [agent-name] [cfg]    =  Creates a new instance.");
    _mainPanel->addText(" destroy [tag]                      =  Destroys an instance.");
    _mainPanel->addText(" list                               =  Lists available instances.");
    _mainPanel->addText(" set [tag]                          =  Switches the current instance.");
    _mainPanel->addText(" quit                               =  Terminate all instances and exit.");
    _mainPanel->addText(" ");
    _mainPanel->addText("Agent source commands:", 0, HEX_BOLD);
    _mainPanel->addText(" add [name] <epoch>                 =  adds a new metric of 'name'.");
    _mainPanel->addText(" remove   [name]                    =  removes a metric of 'name'.");
    _mainPanel->addText(" update   [name] [value] <epoch>    =  Updates a metric.");
    _mainPanel->addText(" update_s [name] [value] <epoch>    =  Updates a metric with a string value.");
    _mainPanel->addText(" send                               =  Give time to the API instance.");
    _mainPanel->addText(" ");
    _mainPanel->addText("Client commands:", 0, HEX_BOLD);
    _mainPanel->addText(" client new [tag] [server] [port]   =  creates a new client connection ");
    _mainPanel->addText(" client del [tag]                   =  removes a client connection  ");
    _mainPanel->addText(" client list                        =  lists current clients  ");
    _mainPanel->addText(" client subscribe [tag] [name]      =  subscribes client 'tag' to 'name'  ");
    _mainPanel->addText(" client unsubscribe [tag] [name]    =  unsubscribes client 'tag' to 'name'  ");
    _mainPanel->addText(" client browse [name]               =  list subtree by 'name' ");
    _mainPanel->addText(" client dump   [name]               =  dump subtree by 'name' ");
    _mainPanel->addText(" client show   [name]               =  display the current metric for 'name' ");
    //_mainPanel->redraw();
}

bool
TnmsConsoleApp::GetQuotedString ( size_t indx, CommandList & cmdlist, std::string & str )
{
    std::ostringstream  ostrm;
    std::string         word  = "";
    bool                valid = false;

    if ( cmdlist.size() < indx )
        return valid;

    word = cmdlist[indx];

    if ( StringUtils::startsWith(word, "\"") )
    {
        word = word.substr(1);
    } else
        return valid;

    do {
        if ( StringUtils::endsWith(word, "\"") ) {
            word = word.substr(0, word.length() - 1);
            valid = true;
        }

        ostrm << word;

        if ( ++indx == cmdlist.size() )
            break;

        if ( ! valid ) {
            ostrm << " ";
            word = cmdlist[indx];
        }
    } while ( ! valid );

    if ( valid )
        str = ostrm.str();

    return valid;
}

}  // namespace


