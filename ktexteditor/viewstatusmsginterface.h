/* This file is part of the KDE libraries
   Copyright (C) 2001 Christoph Cullmann <cullmann@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __ktexteditor_viewstatusmsginterface_h__
#define __ktexteditor_viewstatusmsginterface_h__

class QString;
class QCString;

namespace KTextEditor
{

/**
*  This is an interface for retrieving status bar messages from the \ref Document class.
*/
class ViewStatusMsgInterface
{
  friend class PrivateViewStatusMsgInterface;

  public:
    ViewStatusMsgInterface();
    virtual ~ViewStatusMsgInterface();

    unsigned int viewStatusMsgInterfaceNumber () const;
    
  protected:  
    void setViewStatusMsgInterfaceDCOPSuffix (const QCString &suffix); 

  //
  // signals !!!
  //  
  public:
    virtual void viewStatusMsg (const class QString &msg) = 0;
    

  private:
    class PrivateViewStatusMsgInterface *d;
    static unsigned int globalViewStatusMsgInterfaceNumber;
    unsigned int myViewStatusMsgInterfaceNumber;
};
                  
ViewStatusMsgInterface *viewStatusMsgInterface (class View *view);

}

#endif
