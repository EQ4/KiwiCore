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

#include "Attribute.h"
#include "Dico.h"

namespace Kiwi
{
    
    // ================================================================================ //
    //                                  ATTRIBUTE FACTORY                               //
    // ================================================================================ //
    
    AttributeFactory::AttributeFactory() noexcept
    {
        
    }
    
    AttributeFactory::~AttributeFactory()
    {
        m_attributes.clear();
    }
    
    
    void AttributeFactory::receive(ElemVector const& elements)
    {
		if(elements.size() && elements[0].isTag())
		{
			auto it = m_attributes.find((sTag)elements[0]);
			if(it != m_attributes.end())
			{
				ElemVector nelements;
				nelements.assign(elements.begin()+1, elements.end());
				
				sAttribute attr = it->second;
				
				if(!attr->isOpaque())
				{
					attr->set(nelements);
					
					if(attr->shouldNotifyChanges())
					{
						notify(attr);
					}
				}
			}
		}
    }
    
    void AttributeFactory::write(shared_ptr<Dico> dico) const noexcept
    {
        for(auto it = m_attributes.begin(); it != m_attributes.end(); ++it)
        {
            if(it->second->isSaveable())
            {
                it->second->write(dico);
            }
        }
    }
    
    void AttributeFactory::read(shared_ptr<const Dico> dico) noexcept
    {
        for(auto it = m_attributes.begin(); it != m_attributes.end(); ++it)
        {
            it->second->read(dico);
        }
    }
    
    sAttribute AttributeFactory::getAttribute(sTag name) const noexcept
    {
        auto it = m_attributes.find(name);
        if(it != m_attributes.end() && !(it->second->isInvisible()))
            return it->second;
        else
            return nullptr;
    }
    
	void AttributeFactory::setAttributeAppearance(sTag name, string const& label, Attribute::Style style, string const& category)
    {
        auto it = m_attributes.find(name);
        if(it != m_attributes.end())
        {
            it->second->setLabel(label);
            it->second->setStyle(style);
            it->second->setCategory(category);
        }
    }
    
	void AttributeFactory::setAttributeBehavior(sTag name, Attribute::Behavior behavior)
    {
        auto it = m_attributes.find(name);
        if(it != m_attributes.end())
        {
            it->second->setBehavior(behavior);
        }
    }
    
    // ================================================================================ //
    //                                      ATTRIBUTE                                   //
    // ================================================================================ //
    
	Attribute::Attribute(sTag name,
						 ElemVector defaultValue,
						 string const& label,
						 Style style,
						 string const& category,
						 long behavior) :
    m_name(name),
	m_default_value(defaultValue),
    m_label(label),
    m_style(style),
    m_category(category),
    m_behavior(0 | behavior),
	m_frozen(false)
    {
		;
    }
    
    Attribute::~Attribute()
    {
        m_frozen_value.clear();
    }
	
	void Attribute::setBehavior(Behavior behavior)
	{
		if(m_behavior != behavior)
		{
			m_behavior = behavior;
		}
	}
	
	void Attribute::setInvisible(const bool b) noexcept
	{
		if (b)
			m_behavior &= ~Invisible;
		else
			m_behavior |= Invisible;
	}
	
	void Attribute::setOpaque(const bool b) noexcept
	{
		if (b)
			m_behavior &= ~Opaque;
		else
			m_behavior |= Opaque;
	}
	
	void Attribute::setDisabled(const bool b) noexcept
	{
		if (!b)
			m_behavior &= ~Disabled;
		else
			m_behavior |= Disabled;
	}

	void Attribute::setSaveable(const bool b) noexcept
	{
		if (!b)
			m_behavior &= ~NotSaveable;
		else
			m_behavior |= NotSaveable;
	}
	
	void Attribute::setNotifyChanges(const bool b) noexcept
	{
		if (!b)
			m_behavior &= ~NotNotifyChanges;
		else
			m_behavior |= NotNotifyChanges;
	}
	
	void Attribute::freeze(const bool frozen)
	{
		m_frozen = frozen;
		
		if (m_frozen)
			get(m_frozen_value);
		else
			m_frozen_value.clear();
	}
	
	void Attribute::reset()
	{
		set(m_default_value);
	}
	
    void Attribute::write(shared_ptr<Dico> dico) const noexcept
    {
		if(!(m_behavior & Behavior::NotSaveable) || m_frozen)
		{
			ElemVector elements;
			
			if(!m_frozen)
				get(elements);
			else
				elements = m_frozen_value;
				
			dico->set(m_name, elements);
		}
    }
    
    void Attribute::read(shared_ptr<const Dico> dico)
    {
        ElemVector elements;
        dico->get(m_name, elements);
        set(elements);
    }
    
    // ================================================================================ //
    //                                      ATTRIBUTE TYPED                             //
    // ================================================================================ //
    
	AttributeLong::AttributeLong(sTag name,
								 long defaultValue,
								 string const& label,
								 string const& category,
								 Attribute::Style style,
								 long behavior)
		: Attribute(name, {defaultValue}, label, style, category, behavior)
    {
    }
    
    void AttributeLong::set(ElemVector const& elements)
    {
        if(elements.size() && (elements[0].isLong() || elements[0].isDouble()))
        {
            m_value = elements[0];
        }
    }
    
    void AttributeLong::get(ElemVector& elements) const noexcept
    {
        elements = {m_value};
    }
    
	AttributeDouble::AttributeDouble(sTag name,
									 double defaultValue,
									 string const& label,
									 string const& category,
									 Attribute::Style style,
									 long behavior) :
		Attribute(name, {defaultValue}, label, style, category, behavior)
    {
    }
    
    void AttributeDouble::set(ElemVector const& elements)
    {
        if(elements.size() && elements[0].isNumber())
        {
            m_value = elements[0];
        }
    }
    
    void AttributeDouble::get(ElemVector& elements) const noexcept
    {
        elements = {m_value};
    }
	
	
	AttributeBool::AttributeBool(sTag name,
								 int defaultValue,
								 string const& label,
								 string const& category,
								 Attribute::Style style,
								 long behavior) :
	Attribute(name, {defaultValue}, label, style, category, behavior)
	{
		
	}
	
	void AttributeBool::set(ElemVector const& elements)
	{
		if(elements.size() && elements[0].isNumber())
		{
			m_value = ((int)elements[0] != 0);
		}
	}
	
	void AttributeBool::get(ElemVector& elements) const noexcept
	{
		elements = {m_value};
	}

	AttributeTag::AttributeTag(sTag name,
							   string defaultValue,
							   string const& label,
							   string const& category,
							   Attribute::Style style,
							   long behavior) :
		Attribute(name, {defaultValue}, label, style, category, behavior)
    {
    }
    
    void AttributeTag::set(ElemVector const& elements)
    {
        if(elements.size() && elements[0].isTag())
        {
            m_value = elements[0];
        }
    }
    
    void AttributeTag::get(ElemVector& elements) const noexcept
    {
        elements = {m_value};
    }
	
	AttributeColor::AttributeColor(sTag name,
								   ElemVector defaultValue,
								   string const& label,
								   string const& category,
								   long behavior)
	: Attribute(name, defaultValue, label, Attribute::Style::Color, category, behavior)
	{
	}
	
	void AttributeColor::set(ElemVector const& elements)
	{
		const size_t nelems = elements.size();
		for(size_t i = 0; i < 4; i++)
		{
			if (nelems > i && elements[i].isNumber())
				m_value[i] = clip((double)elements[i], 0., 1.);
			else
				m_value[i] = (i < 3) ? 0.f : 1.f; // default is black with full alpha color
		}
	}
	
	void AttributeColor::get(ElemVector& elements) const noexcept
	{
		elements = {m_value[0], m_value[1], m_value[2], m_value[3]};
	}
	
	
	AttributeRect::AttributeRect(sTag name,
								 ElemVector defaultValue,
								 string const& label,
								 string const& category,
								 long behavior)
	: Attribute(name, defaultValue, label, Attribute::Style::NumberList, category, behavior)
	{
	}
	
	void AttributeRect::set(ElemVector const& elements)
	{
		const size_t nelems = elements.size();
		
		for(size_t i = 0; i < 4; i++)
		{
			if (nelems > i && elements[i].isNumber())
				m_value[i] = (double)elements[i];
			else
				m_value[i] = 0.f;
		}
	}
	
	void AttributeRect::get(ElemVector& elements) const noexcept
	{
		elements = {m_value[0], m_value[1], m_value[2], m_value[3]};
	}
}
















