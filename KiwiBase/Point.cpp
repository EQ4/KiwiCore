/*
 ==============================================================================
 
 This file is part of the KIWI library.
 Copyright (c) 2014 Pierre Guillot & Eliott Paris.
 
 Permission is granted to use this software under the terms of either:
 a) the GPL v2 (or any later version)
 b) the Affero GPL v3
 
 Details of these licenses can be found at: www.gnu.org/licenses
 
 KIWI is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 
 ------------------------------------------------------------------------------
 
 To release a closed-source product which uses KIWI, contact : guillotpierre6@gmail.com
 
 ==============================================================================
 */

#include "Point.h"

namespace Kiwi
{
    // ================================================================================ //
    //                                      POINT                                       //
    // ================================================================================ //
    
    Point::Point() noexcept :
    m_x(0.), m_y(0.)
    {
        ;
    }
    
    Point::Point(const double x, const double y) noexcept :
    m_x(x), m_y(y)
    {
        ;
    }
    
    Point::Point(ElemVector const& elements) noexcept :
    m_x(!elements.empty() ? (double)elements[0] : 0.), m_y(elements.size() > 1 ? (double)elements[1] : 0.)
    {
        ;
    }
    
    Point::Point(Point const& pt) noexcept :
    m_x(pt.m_x), m_y(pt.m_y)
    {
        ;
    }
    
    Point::~Point()
    {
        ;
    }
    
    double Point::distance(Point const& begin, Point const& end) const noexcept
    {
        const double line_distance = begin.distance(end);
        if(!line_distance)
        {
            return this->distance(begin);
        }
        else
        {
            const Point shidt_this = *this - begin;
            const double dotprod = shidt_this.dot(end - begin) / line_distance;
            if(dotprod < 0.)
            {
                return this->distance(begin);
            }
            else if(dotprod > 1.)
            {
                return this->distance(end);
            }
            else
            {
                return this->distance(begin + (end - begin) * dotprod);
            }
        }
    }
    
    double Point::distance(Point const& begin, Point const& ctrl, Point const& end) const noexcept
    {
		const Point A = ctrl - begin;
        const Point B = end - ctrl - A;
        const Point relPoint = begin - *this;
        
        double sol1, sol2, sol3;
        const unsigned long nresult = solve(B.distance(), 3 * A.dot(B), 2 * A.distance() + B.dot(relPoint), A.dot(relPoint), sol1, sol2, sol3);
        if(nresult)
        {
            double dist = sol1;
            if(nresult > 1 && sol2 < dist)
            {
                dist  = sol2;
            }
            if(nresult > 2 && sol3 < dist)
            {
                dist  = sol3;
            }
            return dist;
        }
        else
        {
            const double dist1 = this->distance(begin);
            const double dist2 = this->distance(end);
            if(dist1 < dist2)
            {
                return dist1;
            }
            else
            {
                return dist2;
            }
        }
    }
    
    bool Point::near(Point const& pt, double const distance) const noexcept
    {
        return this->distance(pt) <= distance;
    }
    
    bool Point::near(Point const& begin, Point const& end, double const distance) const noexcept
    {
        return this->distance(begin, end) <= distance;
    }
    
    bool Point::near(Point const& begin, Point const& ctrl, Point const& end, double const distance) const noexcept
    {
        return this->distance(begin, ctrl, end) <= distance;
    }
    
}