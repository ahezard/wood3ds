/*
    expwnd.h
    Copyright (C) 2007 Acekard, www.acekard.com
    Copyright (C) 2007-2009 somebody
    Copyright (C) 2009 yellow wood goblin

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __EXPWND_H__
#define __EXPWND_H__

#include "form.h"
#include "formdesc.h"
#include "spinbox.h"
#include "statictext.h"
#include "message.h"
#include <string>
#include "dsrom.h"

class cExpWnd: public akui::cForm
{
  public:
    cExpWnd(s32 x, s32 y, u32 w, u32 h, cWindow * parent, const std::string & text );
    ~cExpWnd();
  public:
    void draw();
    bool process( const akui::cMessage & msg );
    cWindow& loadAppearance(const std::string& aFileName );
  protected:
    bool processKeyMessage( const akui::cKeyMessage & msg );
    void onOK();
    void onCancel();
    void onRAM();
    void onSRAM();
    void onShow();
    akui::cButton _buttonOK;
    akui::cButton _buttonCancel;
    akui::cButton _buttonRAM;
    akui::cButton _buttonSRAM;
    akui::cSpinBox _Rumble;
    akui::cStaticText _Label;
    akui::cFormDesc _renderDesc;
    std::string _romName;
};




#endif//_HELPWND_H_
