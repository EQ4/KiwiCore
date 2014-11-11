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

#include "Box.h"
#include "Page.h"
#include "Instance.h"

namespace Kiwi
{
    map<sTag, unique_ptr<Box>>  Box::m_prototypes;
    
    // ================================================================================ //
    //                                      BOX                                         //
    // ================================================================================ //
    
    Box::Box(weak_ptr<Page> page, string const& name) :
	Attribute::Manager(),
    m_page(page),
    m_name(Tag::create(name)),
    m_stack_count(0)
    {
		//getInstance()->getBoxDefaultAttributes();
		
		// Font attributes
        addAttribute<AttributeTag>(Tag::create("fontname"), "Arial", "Font Name", "Font");
        addAttribute<AttributeDouble>(Tag::create("fontsize"), 12, "Font Size", "Font");
		
		ElemVector elems = {Tag::create("regular"), Tag::create("bold"), Tag::create("italic"), Tag::create("bold italic")};
		addAttribute<AttributeEnum>(Tag::create("fontface"), elems, 0, "Font Style", "Font");
		
		elems = {Tag::create("left"), Tag::create("center"), Tag::create("right")};
		addAttribute<AttributeEnum>(Tag::create("textjustification"), elems, 0, "Justification", "Font");
		
		// Appearance attributes
		addAttribute<AttributeBool>(Tag::create("hidden"), false, "Hide on Lock", "Appearance");
		addAttribute<AttributeBool>(Tag::create("presentation"), false, "Include in Presentation", "Appearance");
		
		elems = {0., 0.};
		addAttribute<AttributePoint>(Tag::create("position"), elems, "Position", "Appearance");
		elems = {100., 20.};
		addAttribute<AttributePoint>(Tag::create("page_size"), elems, "Size", "Appearance");
		
		elems = {0., 0.};
		addAttribute<AttributePoint>(Tag::create("presentation_pos"), elems, "Presentation Position", "Appearance");
		elems = {0., 0.};
		addAttribute<AttributePoint>(Tag::create("presentation_size"), elems, "Presentation Size", "Appearance");
		
		// Color attributes
		elems = {1., 1., 1, 1.};
		addAttribute<AttributePoint>(Tag::create("bgcolor"), elems, "Background Color", "Color");
		elems = {0., 0., 0, 1.};
		addAttribute<AttributePoint>(Tag::create("bdcolor"), elems, "Border Color", "Color");
		elems = {0., 0., 0, 1.};
		addAttribute<AttributePoint>(Tag::create("textcolor"), elems, "Text Color", "Color");
    }
    
    Box::~Box()
    {
        m_outlets.clear();
        m_inlets.clear();
        m_listeners.clear();
    }
    
    shared_ptr<Box> Box::create(sPage page, sDico dico)
    {
        sTag name = dico->get(Tag::name);
        if(name)
        {
            auto it = m_prototypes.find(name);
            if(it != m_prototypes.end())
            {
                sTag text = dico->get(Tag::text);
                if(text)
                {
                    sDico other = Dico::create();
                    other->read(toString(text));
                    ElemVector keys;
                    other->keys(keys);
                    for(size_t i = 0; i < keys.size(); i++)
                    {
                        ElemVector values;
                        other->get(keys[i], values);
                        dico->set(keys[i], values);
                    }
                }
                
                sBox box = it->second->allocate(page, dico);
                if(box)
                {
                    box->m_text = dico->get(Tag::text);
                    box->load(dico);
					box->Attribute::Manager::read(dico);
                    return box;
                }
            }
            else
            {
                Console::error("The box " + toString(name) + " doesn't exist !");
                return nullptr;
            }
        }
        Console::error("The dico isn't valid for box creation !");
        return nullptr;
    }
    
    sPage Box::getPage() const noexcept
    {
        return m_page.lock();
    }
    
    sInstance Box::getInstance() const noexcept
    {
        sPage page = m_page.lock();
        if(page)
        {
            return page->getInstance();
        }
        else
        {
            return nullptr;
        }
    }
    
    sTag Box::getName() const noexcept
    {
        return m_name;
    }
    
    sTag Box::getText() const noexcept
    {
        return m_text;
    }
    
    string Box::getExpression() const noexcept
    {
        return "error";
    }
    
    void Box::save(shared_ptr<Dico> dico) const
    {
        ;
    }
    
    void Box::load(shared_ptr<const Dico> dico)
    {
        ;
    }
    
    bool Box::receive(size_t index, ElemVector const& elements)
    {
        return false;
    }
    
    bool Box::receive(Event::Mouse const& events)
    {
        return false;
    }
    
    bool Box::receive(Event::Keyboard const& events)
    {
        return false;
    }
    
    bool Box::paint(Doodle& d) const
    {
        return false;
    }
    
    void Box::draw(Doodle& d, bool edit) const
    {
        d.setColor({1., 1., 1., 1.});
        d.fillRectangle(1., 1., d.getWidth() - 2., d.getHeight() - 2., 2.5);
        if(!paint(d))
        {
            d.setColor({0.3, 0.3, 0.3, 1.});
            d.drawText(toString(m_text), 3, 0, d.getWidth(), d.getHeight(), Doodle::Justification::CentredLeft);
        }
        d.setColor({0.4, 0.4, 0.4, 1.});
        d.drawRectangle(0., 0., d.getWidth(), d.getHeight(), 1., 2.5);
        
        if(edit)
        {
            size_t ninlet = m_inlets.size();
            size_t noutlet = m_outlets.size();
            d.setColor({0.3, 0.3, 0.3, 1.});
            if(ninlet)
            {
                d.fillRectangle(0., 0., 5, 3, 2.5);
            }
            if(ninlet > 1)
            {
                double ratio = (d.getWidth() - 5.) / (double)(ninlet - 1);
                for(size_t i = ninlet; i; i--)
                {
                    d.fillRectangle(ratio * i, 0., 5, 3, 2.5);
                }
            }
            
            if(noutlet)
            {
                d.fillRectangle(0., d.getHeight() - 3., 5, 3, 2.5);
            }
            if(noutlet > 1)
            {
                double ratio = (d.getWidth() - 5.) / (double)(noutlet - 1);
                for(size_t i = noutlet; i; i--)
                {
                    d.fillRectangle(ratio * i, d.getHeight() - 3., 5, 3, 2.5);
                }
            }
            
        }
    }
    
    void Box::redraw() const
    {
        
    }

    void Box::write(sDico dico) const
    {
        save(dico);
        Attribute::Manager::write(dico);
        dico->set(Tag::name, m_name);
        dico->set(Tag::ninlets, m_inlets.size());
        dico->set(Tag::noutlets, m_outlets.size());
        dico->set(Tag::text, m_text);
    }
    
    void Box::send(size_t index, ElemVector const& elements) const noexcept
    {
        if(index < m_outlets.size())
        {
            for(size_t i = 0; i < m_outlets[index]->m_conns.size(); i++)
            {
                sBox receiver   = m_outlets[index]->m_conns[i].m_box;
                size_t inlet    = m_outlets[index]->m_conns[i].m_index;
                if(++receiver->m_stack_count < 256)
                {
                    if(!receiver->receive(inlet, elements))
                    {
                        receiver->Attribute::Manager::receive(elements);
                    }
                }
                else if(receiver->m_stack_count  == 256)
                {
                    Console::error(receiver, "Stack overflow");
                    if(!receiver->receive(inlet, elements))
                    {
                        receiver->Attribute::Manager::receive(elements);
                    }
                }
                else
                {
                    Console::error(receiver, "Stack overflow");
                }
                receiver->m_stack_count--;
            }
        }
    }
    
    // ================================================================================ //
    //                                      INLETS                                      //
    // ================================================================================ //
    
    void Box::addInlet(Inlet::Type type, string const& description)
    {
        m_inlets.push_back(unique_ptr<Inlet>(new Inlet(type, description)));
    }
    
    void Box::insertInlet(size_t index, Inlet::Type type, string const& description)
    {
        if(index >= m_inlets.size())
        {
            m_inlets.push_back(unique_ptr<Inlet>(new Inlet(type, description)));
        }
        else
        {
            m_inlets.insert(m_inlets.begin()+index, unique_ptr<Inlet>(new Inlet(type, description)));
        }
    }
    
    void Box::removeInlet(size_t index)
    {
        if(index < m_inlets.size())
        {
            m_inlets.erase(m_inlets.begin()+index);
        }
    }
    
    string Box::getInletDescription(size_t index) const noexcept
    {
        if(index < m_inlets.size())
        {
            return m_inlets[index]->m_description;
        }
        else
        {
            return "";
        }
    }
    
    size_t Box::getNumberOfInlets() const noexcept
    {
        return m_inlets.size();
    }
    
    // ================================================================================ //
    //                                      OUTLETS                                     //
    // ================================================================================ //
    
    void Box::addOutlet(Outlet::Type type, string const& description)
    {
        m_outlets.push_back(unique_ptr<Outlet>(new Outlet(type, description)));
    }
    
    void Box::insertOutlet(size_t index, Outlet::Type type, string const& description)
    {
        if(index >= m_outlets.size())
        {
            m_outlets.push_back(unique_ptr<Outlet>(new Outlet(type, description)));
        }
        else
        {
            m_outlets.insert(m_outlets.begin()+index, unique_ptr<Outlet>(new Outlet(type, description)));
        }
    }
    
    void Box::removeOutlet(size_t index)
    {
        if(index < m_outlets.size())
        {
            m_outlets.erase(m_outlets.begin()+index);
        }
    }

    string Box::getOutletDescription(size_t index) const noexcept
    {
        if(index < m_outlets.size())
        {
            return m_outlets[index]->m_description;
        }
        else
        {
            return "";
        }
    }
    
    size_t Box::getNumberOfOutlets() const noexcept
    {
        return m_outlets.size();
    }
    
    bool Box::compatible(shared_ptr<Box> from, size_t outlet, shared_ptr<Box> to, size_t inlet) noexcept
    {
        if(from
           && to
           && from != to
           && outlet < from->m_outlets.size()
           && inlet < to->m_inlets.size()
           && from->m_page.lock() == to->m_page.lock())
        {
            for(size_t i = 0; i < from->m_outlets[outlet]->m_conns.size(); i++)
            {
                if(from->m_outlets[outlet]->m_conns[i].m_box == to
                   && from->m_outlets[outlet]->m_conns[i].m_index == inlet)
                {
                    return false;
                }
            }
            return true;
        }
        return false;
    }
    
    bool Box::connect(shared_ptr<Box> from, size_t outlet, shared_ptr<Box> to, size_t inlet) noexcept
    {
        if(compatible(from, outlet, to, inlet))
        {
            from->m_outlets[outlet]->m_conns.push_back({to, inlet});
            return true;
        }
        return false;
    }
    
    bool Box::disconnect(shared_ptr<Box> from, size_t outlet, shared_ptr<Box> to, size_t inlet) noexcept
    {
        if(from && outlet < from->getNumberOfOutlets())
        {
            for(size_t i = 0; i < from->m_outlets[outlet]->m_conns.size(); i++)
            {
                if(from->m_outlets[outlet]->m_conns[i].m_box == to && from->m_outlets[outlet]->m_conns[i].m_index == inlet)
                {
                    from->m_outlets[outlet]->m_conns.erase(from->m_outlets[outlet]->m_conns.begin()+i);
                    return true;
                }
            }
        }
        return false;
    }
    
    void Box::bind(weak_ptr<Box::Listener> listener)
    {
        m_listeners.insert(listener);
    }
    
    void Box::unbind(weak_ptr<Box::Listener> listener)
    {
        m_listeners.erase(listener);
    }
    
    void Box::addPrototype(unique_ptr<Box> box, string const& name)
    {
        sTag tname;
        if (name.empty())
        {
            tname = box->getName();
        }
        else
        {
            tname = Tag::create(name);
        }
        auto it = m_prototypes.find(tname);
        if(it != m_prototypes.end())
        {
            Console::error("The box " + toString(box->getName()) + " already exist !");
        }
        else
        {
            m_prototypes[tname] = move(box);
        }
    }
}
















