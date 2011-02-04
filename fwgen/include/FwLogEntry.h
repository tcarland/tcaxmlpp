#ifndef _FWGEN_FWLOGENTRY_H_
#define _FWGEN_FWLOGENTRY_H_

#include <string>
#include <vector>

#include "tcanetpp_types.h"


namespace fwgen {


/*
 * Jan 30 08:31:26 nebula kernel: FW:
 *   1      2     3       4                   5              6      7         8        9      10        11        12        13      14
 * IN=ppp0 OUT= MAC= SRC=146.201.161.125 DST=75.214.27.0 LEN=39 TOS=0x00 PREC=0x00 TTL=113 ID=22852 PROTO=UDP SPT=10885 DPT=4324 LEN=19 
 */

#define FWLOG_KEYMATCH    " kernel: FW: "
#define FWLOG_FIELD_COUNT 14


typedef std::vector<std::string>  StringFields;


class FwLogEntry {

  public:
    std::string  host;
    std::string  date;

    std::string  inf;
    std::string  outf;
    std::string  mac;
    std::string  src;
    std::string  dst;
    std::string  proto;
    std::string  spt;
    std::string  dpt;

    uint64_t     count;

    // metric names 
    std::string  absname;
    std::string  protom;
    std::string  matchstr;

  public:

    FwLogEntry();
    FwLogEntry ( const std::string & match );
    ~FwLogEntry() {}

    void          setMatch        ( const std::string & match );
    std::string   getMatch() const;

    static bool   ParseLogEntry   ( const std::string & line,
                                    FwLogEntry        & fwe,
                                    bool                debug = false );

    static void   SetHostname     ( FwLogEntry & fwe );

    static void   SetDefaultMatch ( const std::string & match_ );


  private:

    static void   SplitFields     ( StringFields      & fwv );

  private:

    static std::string DEFAULT_MATCH_STRING;

};

} // namespace


#endif //  _FWGEN_FWLOGENTRY_H_

