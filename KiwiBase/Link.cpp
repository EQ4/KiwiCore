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

#include "Link.h"
#include "Box.h"
#include "Page.h"

namespace Kiwi
{
    // ================================================================================ //
    //                                      LINK                                        //
    // ================================================================================ //
    
    sLink Link::create(Socket const& from, Socket const& to)
    {
        sLink link = make_shared<Link>(from, to);
        if(link && link->isValid())
        {
            return link;
        }
        return nullptr;
    }
    
    sLink Link::create(scPage page, scDico dico)
    {
        if(page && dico)
        {
            sBox from, to;
            unsigned long outlet, inlet;
            unsigned long from_id, to_id;
            
            ElemVector elements;
            dico->get(Tag::from, elements);
            if(elements.size() == 2 && elements[0].isLong() && elements[1].isLong())
            {
                from_id = elements[0];
                outlet  = elements[1];
            }
            else
            {
                return nullptr;
            }
            
            dico->get(Tag::to, elements);
            if(elements.size() == 2 && elements[0].isLong() && elements[1].isLong())
            {
                to_id   = elements[0];
                inlet   = elements[1];
            }
            else
            {
                return nullptr;
            }
        
            if(from_id != to_id)
            {
                vector<sBox> boxes;
                page->getBoxes(boxes);
                for(vector<sBox>::size_type i = 0; i < boxes.size(); i++)
                {
                    if(boxes[i]->getId() == from_id)
                    {
                        from = boxes[i];
                        if(from->getNumberOfOutlets() <=  outlet)
                        {
                            return nullptr;
                        }
                    }
                    else if(boxes[i]->getId() == to_id)
                    {
                        to = boxes[i];
                        if(to->getNumberOfInlets() <=  inlet)
                        {
                            return nullptr;
                        }
                    }
                }
                
                if(from && to)
                {
                    return Link::create({from, outlet}, {to, inlet});
                }
            }
            
        }
        return nullptr;
    }
    
    sLink Link::create(scLink link, const sBox oldbox, const sBox newbox)
    {
        if(link->getBoxFrom() == oldbox)
        {
            if(link->getOutletIndex() < newbox->getNumberOfOutlets())
            {
                return create({newbox, link->getOutletIndex()}, link->getTo());
            }
        }
        else if(link->getBoxTo() == oldbox)
        {
            if(link->getInletIndex() < newbox->getNumberOfInlets())
            {
                return create(link->getFrom(), {newbox, link->getInletIndex()});
            }
        }
        return nullptr;
    }
    
    bool Link::isValid() const noexcept
    {
        sBox from   = getBoxFrom();
        sBox to     = getBoxTo();
        return from && to && from != to && from->getPage() == to->getPage() && getOutletIndex() < from->getNumberOfOutlets() && getInletIndex() < to->getNumberOfInlets();
    }
    
    bool Link::isConnectable() const noexcept
    {
        sBox from   = getBoxFrom();
        sBox to     = getBoxTo();
        if(from && to && from != to && from->getPage() == to->getPage() && getOutletIndex() < from->getNumberOfOutlets() && getInletIndex() < to->getNumberOfInlets())
        {
            vector<Socket> sockets;
            from->getOutletSockets(getOutletIndex(), sockets);
            for(vector<Socket>::size_type i = 0; i < sockets.size(); i++)
            {
                sBox receiver = sockets[i].box.lock();
                if(receiver && receiver == to && sockets[i].index == getInletIndex())
                {
                    return false;
                }
            }
            return true;
        }
        return false;
    }
    
    bool Link::connect() const noexcept
    {
        sBox     from    = getBoxFrom();
        sBox     to      = getBoxTo();
        if(from && to)
        {
            if(from->connectOutlet(getOutletIndex(), to, getInletIndex()))
            {
                if(to->connectInlet(getInletIndex(), from, getOutletIndex()))
                {
                    return true;
                }
                else
                {
                    from->disconnectOutlet(getOutletIndex(), to, getInletIndex());
                    return false;
                }
            }
        }
        return false;
    }
    

    bool Link::disconnect() const noexcept
    {
        return false;
    }
    
    void Link::write(sDico dico) const noexcept
    {
        sBox     from    = getBoxFrom();
        sBox     to      = getBoxTo();
        if(from && to)
        {
            dico->set(Tag::from, {from->getId(), getOutletIndex()});
            dico->set(Tag::to, {to->getId(), getInletIndex()});
        }
        else
        {
            dico->clear(Tag::from);
            dico->clear(Tag::to);
        }
    }
}
















