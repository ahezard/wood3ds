/*
    point.h
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

#ifndef _AKUI_POINT_H_
#define _AKUI_POINT_H_

#include <algorithm>

namespace akui
{
    template <typename T>
    class t_point
    {
    public:
        typedef T value_type;
        value_type x;
        value_type y;

        t_point():x(),y(){}
        t_point(value_type x,value_type y) :x(x),y(y) {}
        t_point(const t_point &p) { x=p.x,y=p.y; }
        t_point operator+(const t_point &p) const { return t_point(x+p.x,y+p.y); }
        t_point operator-(const t_point &p) const { return t_point(x-p.x,y-p.y); }
        bool operator==(const t_point &p) const { return (x==p.x) && (y==p.y); }
        bool operator!=(const t_point &p) const { return (x!=p.x) || (y!=p.y); }
        t_point& operator+=(const t_point &p) { x+=p.x,y+=p.y; return *this; }
        t_point& operator-=(const t_point &p) { x-=p.x,y-=p.y; return *this; }
        t_point& operator=(const t_point &p) { x=p.x,y=p.y; return *this; }
        //当前点是否在p的左侧
        bool is_left(const t_point &p) const { return x<p.x; }                            // 是否在点的左边
        //当前点是否在p的右侧
        bool is_right(const t_point &p) const { return x>p.x; }                            // 是否在点的右边
        //当前点是否在p的上方
        bool is_up(const t_point &p) const { return y<p.y; }                                // 是否在点的上边
        //当前点是否在p的下方
        bool is_down(const t_point &p) const { return y>p.y; }                            // 是否在点的下边
        t_point& operator () (value_type x_,value_type y_){x=x_;y=y_;return *this;}
    };

    //point
    typedef t_point<int> cPoint;
    typedef t_point<float> cPointf;
    //size
    typedef t_point<int> cSize;

}


#endif //_AKUI_POINT_H_
