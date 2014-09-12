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

#include "Instance.h"
#include "../KiwiBoxes/Arithmetic.h"
#include "../KiwiBoxes/ArithmeticTilde.h"

namespace Kiwi
{
    // ================================================================================ //
    //                                      INSTANCE                                    //
    // ================================================================================ //
    
    Instance::Instance() : m_untitled_pages(0)
    {
        
    }
    
    Instance::~Instance()
    {
        m_pages.clear();
        m_prototypes.clear();
    }
    
    void Instance::init()
    {
        Arithmetic::load(enable_shared_from_this<Instance>::shared_from_this());
        ArithmeticTilde::load(enable_shared_from_this<Instance>::shared_from_this());
    }
    
    void Instance::addObjectPrototype(unique_ptr<Object> object)
    {
        map<shared_ptr<Tag>, unique_ptr<Object>>::iterator it = m_prototypes.find(object->name());
        if(it != m_prototypes.end())
        {
            string message = string("The object prototype ") + (string)*object->name() + string(" already exist !");
            error(message);
        }
        else
        {
            m_prototypes[object->name()] = move(object);
        }
    }
    
    shared_ptr<Object> Instance::createObject(string name, vector<Element> const& elements)
    {
        return createObject(createTag(name), elements);
    }
    
    shared_ptr<Object> Instance::createObject(string name, Element const& element)
    {
        vector<Element> elements = {element};
        return createObject(createTag(name), elements);
    }
    
    shared_ptr<Object> Instance::createObject(shared_ptr<Tag> name, Element const& element)
    {
        vector<Element> elements = {element};
        return createObject(name, elements);
    }
    
    shared_ptr<Object> Instance::createObject(string name)
    {
        vector<Element> elements;
        return createObject(createTag(name), elements);
    }
    
    shared_ptr<Object> Instance::createObject(shared_ptr<Tag> name)
    {
        vector<Element> elements;
        return createObject(name, elements);
    }
    
    shared_ptr<Object> Instance::createObject(shared_ptr<Tag> name, vector<Element> const& elements)
    {
        shared_ptr<Object> object;
        map<shared_ptr<Tag>, unique_ptr<Object>>::iterator it = m_prototypes.find(name);
        if(it != m_prototypes.end())
        {
            shared_ptr<Tag> cteTag = createTag("create");
            MethodCreate create = (MethodCreate)it->second->getMethod(cteTag);
            if(create)
            {
                if(it->second->getMethodType(cteTag) == T_NOTHING)
                {
                    if(elements.size() == 0)
                    {
                        return create(enable_shared_from_this<Instance>::shared_from_this(), name);
                    }
                    else
                    {
                        error("This object doesn't support creation args ect...");
                    }
                }
            
                if(elements.size() == 0)
                {
                    if(it->second->getMethodType(cteTag) == T_NOTHING)
                    {
                        return create(enable_shared_from_this<Instance>::shared_from_this(), name);
                    }
                }
                if(elements.size() == 1)
                {
                    if(it->second->getMethodType(cteTag) == T_LONG && (elements[0].isLong() || elements[0].isDouble()))
                    {
                       return create(enable_shared_from_this<Instance>::shared_from_this(), name, (long)elements[0]);
                    }
                    else if(it->second->getMethodType(cteTag) == T_DOUBLE && (elements[0].isDouble() || elements[0].isLong()))
                    {
                        return create(enable_shared_from_this<Instance>::shared_from_this(), name, (double)elements[0]);
                    }
                    else if(it->second->getMethodType(cteTag) == T_TAG && elements[0].isTag())
                    {
                        //return create(enable_shared_from_this<Instance>::shared_from_this(), name, (shared_ptr<Tag>)(elements[0]));
                    }
                        
                       
                }
                return create(enable_shared_from_this<Instance>::shared_from_this(), name, &elements);
            }
        }
        return object;
    }
    
    shared_ptr<Connection> Instance::createConnection(shared_ptr<Box> from, int oulet, shared_ptr<Box> to, int inlet)
    {
        return make_shared<Connection>(enable_shared_from_this<Instance>::shared_from_this(), from, oulet, to, inlet);
    }
    
    shared_ptr<Dico> Instance::createDico()
    {
        return make_shared<Dico>(enable_shared_from_this<Instance>::shared_from_this());
    }

    shared_ptr<Page> Instance::createPage(string file, string directory)
    {
        if(file.empty())
		{
			file = string("Untitled") + to_string(++m_untitled_pages);
		}
  
        shared_ptr<Page> newPage = make_shared<Page>(enable_shared_from_this<Instance>::shared_from_this(), file, directory);
        m_pages.push_back(newPage);
        return newPage;
    }
    
    void Instance::closePage(shared_ptr<Page> page)
    {
        for(int i = 0; i < m_pages.size(); i++)
        {
            if(m_pages[i] == page)
            {
                m_pages.erase(m_pages.begin()+i);
                break;
            }
        }
    }
    
    void Instance::startDsp(double samplerate, long vectorsize)
    {
        for(int i = 0; i < m_pages.size(); i++)
            m_pages[i]->startDsp(samplerate, vectorsize);
    }
    
    void Instance::tickDsp()
    {
        for(int i = 0; i < m_pages.size(); i++)
            m_pages[i]->tickDsp();
    }
    
    void Instance::stopDsp()
    {
        for(int i = 0; i < m_pages.size(); i++)
            m_pages[i]->stopDsp();
    }
}
















