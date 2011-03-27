#ifndef _HEXES_HEXDIALOG_H_
#define _HEXES_HEXDIALOG_H_

#include <string>

#include "HexPanel.h"


namespace hexes {

class HexDialog : public HexPanel {

  public:

    HexDialog ( const std::string & title,
                const HexString   & dialog = HexString() );

    HexDialog ( const std::string & title,
                const HexString   & dialog,
                int   height, int   width,
                int   starty, int   startx );

    virtual ~HexDialog();


    virtual int  showDialog();


    std::string  getResult();
    void         getCharOnly();


  protected:

    void    initDialog();
    size_t  getLongestLine();


  private:

    std::string  _result;
    bool         _dynsz;
    bool         _char;

};

} // namespace 
#endif // _HEXES_HEXDIALOG_H_
