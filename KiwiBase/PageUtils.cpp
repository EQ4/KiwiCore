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

#include "PageUtils.h"
#include "Page.h"

namespace Kiwi
{
    
    // ================================================================================ //
    //                                      KNOCK                                       //
    // ================================================================================ //
    
    Knock::Knock(sPage page) noexcept :
    m_page(page)
    {
        ;
    }
    
    Knock::~Knock()
    {
        ;
    }
    
    void Knock::knockReset() noexcept
    {
        m_box.reset();
        m_link.reset();
        m_target= Nothing;
        m_part  = Outside;
		m_border = None;
        m_index = 0;
    }
    
    void Knock::knockAll(Gui::Point const& point, const bool presentation) noexcept
    {
        knockReset();
        if(sPage page = m_page.lock())
        {
            if(Page::sController pagectrl = page->getController())
            {
                vector<Box::sController> boxes;
                pagectrl->getBoxes(boxes);
                for(vector<Box::sController>::size_type i = boxes.size(); i; i--)
                {
					if(Box::sController boxctrl = boxes[i-1])
					{
						if(sBox box = boxctrl->getBox())
						{
							if(!presentation || (presentation && box->isInPresentation()))
							{
								if(boxctrl->contains(point, *this, presentation))
								{
									m_target = Box;
									return;
								}
							}
						}
					}
                }
				
				if(!presentation)
				{
					vector<Link::sController> links;
					pagectrl->getLinks(links);
					for(vector<Link::sController>::size_type i = links.size(); i; i--)
					{
						Link::sController link = links[i-1];
						if(link && link->contains(point, *this))
						{
							m_target = Link;
							return;
						}
					}
				}
				
                m_target = Page;
                m_part   = Inside;
                return;

            }
        }
    }
    
    void Knock::knockBoxes(Gui::Point const& point, const bool presentation) noexcept
    {
        knockReset();
        if(sPage page = m_page.lock())
        {
            if(Page::sController ctrl = page->getController())
            {
                vector<Box::sController> boxes;
                ctrl->getBoxes(boxes);
                for(vector<Box::sController>::size_type i = boxes.size(); i; i--)
                {
                    Box::sController box = boxes[i-1];
                    if(box && box->contains(point, *this, presentation))
                    {
                        return;
                    }
                }
            }
        }
    }
    
    void Knock::knockLinks(Gui::Point const& point) noexcept
    {
        knockReset();
        if(sPage page = m_page.lock())
        {
            if(Page::sController ctrl = page->getController())
            {
                vector<Link::sController> links;
                ctrl->getLinks(links);
                for(vector<Link::sController>::size_type i = links.size(); i; i--)
                {
                    Link::sController link = links[i-1];
                    if(link && link->contains(point, *this))
                    {
                        return;
                    }
                }
            }
        }
    }
    

    void Knock::knockAll(Gui::Rectangle const& rect, vector<Box::sController>& boxes, vector<Link::sController>& links, const bool presentation) noexcept
    {
        knockBoxes(rect, boxes, presentation);
        knockLinks(rect, links);
    }
    
    void Knock::knockBoxes(Gui::Rectangle const& rect, vector<Box::sController>& boxes, const bool presentation) noexcept
    {
        boxes.clear();
        if(sPage page = m_page.lock())
        {
            if(Page::sController ctrl = page->getController())
            {
                vector<Box::sController> vboxes;
                ctrl->getBoxes(vboxes);
                for(vector<Box::sController>::size_type i = 0; i < vboxes.size(); i++)
                {
                    if(vboxes[i] && vboxes[i]->overlaps(rect, presentation))
                    {
                        boxes.push_back(vboxes[i]);
                    }
                }
            }
        }
    }
    
    void Knock::knockLinks(Gui::Rectangle const& rect, vector<Link::sController>& links) noexcept
    {
        links.clear();
        if(sPage page = m_page.lock())
        {
            if(Page::sController ctrl = page->getController())
            {
                vector<Link::sController> vlinks;
                ctrl->getLinks(vlinks);
                for(vector<Link::sController>::size_type i = 0; i < vlinks.size(); i++)
                {
                    if(vlinks[i] && vlinks[i]->overlaps(rect))
                    {
                        links.push_back(vlinks[i]);
                    }
                }
            }
        }
    }
    
    // ================================================================================ //
    //										IOLET MAGNET                                //
    // ================================================================================ //
    
    IoletMagnet::IoletMagnet(sPage page) noexcept : m_page(page)
    {
        
    }
    
    IoletMagnet::~IoletMagnet()
    {
        
    }
    
    bool IoletMagnet::magnetFindIolet(Gui::Point const& point, sBox box, bool inlet, double const distance)
	{
        m_box.reset();
        sPage page = m_page.lock();
		if(page && box)
		{
            double ndistance = distance;
            vector<sBox> boxes;
            page->getBoxes(boxes);
            for(vector<sBox>::size_type i = 0; i < boxes.size(); i++)
            {
                if(boxes[i] && boxes[i] != box)
                {
                    Box::sController boxctrl = boxes[i]->getController();
                    if(boxctrl && inlet)
                    {
                        for(ulong j = 0; j <  boxes[i]->getNumberOfInlets(); j++)
                        {
                            const double cdistance = point.distance(boxctrl->getInletPosition(j));
                            if(cdistance < ndistance)
                            {
                                m_box   = boxes[i];
                                m_index = j;
                                ndistance = cdistance;
                            }
                        }
                    }
                    else if(boxctrl)
                    {
                        for(ulong j = 0; j <  boxes[i]->getNumberOfOutlets(); j++)
                        {
                            const double cdistance = point.distance(boxctrl->getOutletPosition(j));
                            if(cdistance < ndistance)
                            {
                                m_box   = boxes[i];
                                m_index = j;
                                ndistance = cdistance;
                            }
                        }
                    }
                }
            }
		}
		return !m_box.expired();
	}
    
    // ================================================================================ //
    //										LASSO                                       //
    // ================================================================================ //
    
    Lasso::Lasso(sPage page) noexcept : m_page(page)
	{
        ;
	}
    
    Lasso::~Lasso()
    {
        m_boxes.clear();
		m_links.clear();
    }
    
	void Lasso::begin(Gui::Point const& point, const bool preserve)
	{
        if(m_dragging)
        {
            m_boxes.clear();
            m_links.clear();
        }
		
        if(sPage page = m_page.lock())
        {
            if(Page::sController ctrl = page->getController())
            {
                if(!preserve)
                {
                    ctrl->unselectAll();
                }
                else
                {
                    ctrl->getSelection(m_boxes);
                    ctrl->getSelection(m_links);
                }
            }
        }
		
        m_start = point;
		m_bounds = Gui::Rectangle(m_start, Gui::Point(0., 0));
		m_dragging = true;
	}
	
	void Lasso::perform(Gui::Point const& point, bool boxes, bool links, const bool preserve, const bool presentation)
	{
        if(m_start.x() < point.x())
        {
            if(m_start.y() < point.y())
            {
                m_bounds = Gui::Rectangle(m_start, point - m_start);
            }
            else
            {
                m_bounds = Gui::Rectangle(m_start.x(), point.y(), point.x() - m_start.x(), m_start.y() - point.y());
            }
        }
        else
        {
            if(m_start.y() < point.y())
            {
                m_bounds = Gui::Rectangle(point.x(), m_start.y(), m_start.x() - point.x(), point.y() - m_start.y());
            }
            else
            {
                m_bounds = Gui::Rectangle(point, m_start - point);
            }
        }
		boundsHasChanged();
		
        if(sPage page = m_page.lock())
        {
            if(Page::sController ctrl = page->getController())
            {
				bool selectionHasChanged = false;
				
                if(preserve)
                {
                    if(boxes)
                    {
						vector<Box::sController> allBoxes;
						ctrl->getBoxes(allBoxes);
						vector<Box::sController> lassoBoxes;
						ctrl->knockBoxes(m_bounds, lassoBoxes, presentation);
						
						for(vector<Box::sController>::size_type i = 0; i < allBoxes.size(); i++)
						{
							const bool isSelected = allBoxes[i]->isSelected();
							const bool wasSelected = m_boxes.find(allBoxes[i]) != m_boxes.end();
							const bool inLasso = find(lassoBoxes.begin(), lassoBoxes.end(), allBoxes[i]) != lassoBoxes.end();
							
							if (!isSelected && (wasSelected != inLasso))
							{
								ctrl->select(allBoxes[i], false);
								selectionHasChanged = true;
							}
							else if(isSelected && (wasSelected == inLasso))
							{
								ctrl->unselect(allBoxes[i], false);
								selectionHasChanged = true;
							}
						}
                    }
					if(links)
					{
						vector<Link::sController> allLinks;
						ctrl->getLinks(allLinks);
						vector<Link::sController> lassoLinks;
						ctrl->knockLinks(m_bounds, lassoLinks);
						
						for(vector<Link::sController>::size_type i = 0; i < allLinks.size(); i++)
						{
							const bool isSelected = allLinks[i]->isSelected();
							const bool wasSelected = m_links.find(allLinks[i]) != m_links.end();
							const bool inLasso = find(lassoLinks.begin(), lassoLinks.end(), allLinks[i]) != lassoLinks.end();
							
							if (!isSelected && (wasSelected != inLasso))
							{
								ctrl->select(allLinks[i], false);
								selectionHasChanged = true;
							}
							else if(isSelected && (wasSelected == inLasso))
							{
								ctrl->unselect(allLinks[i], false);
								selectionHasChanged = true;
							}
						}
					}
					
					if (selectionHasChanged)
					{
						ctrl->selectionChanged();
					}
                }
                else
                {
                    ctrl->unselectAll(false);
                    if(boxes)
                    {
                        vector<Box::sController> nBoxes;
                        ctrl->knockBoxes(m_bounds, nBoxes, presentation);
                        ctrl->select(nBoxes);
                    }
                    if(links)
                    {
                        vector<Link::sController> nLinks;
                        ctrl->knockLinks(m_bounds, nLinks);
                        ctrl->select(nLinks);
                    }
                }
            }
        }
	}
    
	void Lasso::end()
	{
		m_dragging = false;
        m_boxes.clear();
		m_links.clear();
	}
	
    void Lasso::draw(Gui::Doodle &d)
	{
        d.setColor(Gui::Color(0.96, 0.96, 0.96, 0.5));
        d.fillAll();
        d.setColor(Gui::Color(0.96, 0.96, 0.96, 1.));
        d.drawRectangle(0., 0., d.getWidth(), d.getHeight(), 1.);
	}
    
    // ================================================================================ //
    //                                 IOLET HIGHLIGHTER                                //
    // ================================================================================ //
    
    IoletHighlighter::IoletHighlighter() noexcept
	{
		;
	}
    
    IoletHighlighter::~IoletHighlighter()
	{
		;
	}
	
	void IoletHighlighter::highlightInlet(sBox box, ulong index)
	{
		if(box)
		{
			if(Box::sController ctrl = box->getController())
			{
				if(sInlet inlet = box->getInlet(index))
				{
					const Gui::Point pos = ctrl->getInletPosition(index);
					m_polarity = inlet->getPolarity();
                    m_bounds = Gui::Rectangle(pos.x() - 8.,  pos.y() - 8., 16., 16.);
                    boundsHasChanged();
				}
			}
		}
	}
	
	void IoletHighlighter::highlightOutlet(sBox box, ulong index)
	{
		if(box)
		{
			if(Box::sController ctrl = box->getController())
			{
				if(sInlet inlet = box->getInlet(index))
				{
					const Gui::Point pos = ctrl->getOutletPosition(index);
					m_polarity = Iolet::Hot;
                    m_bounds = Gui::Rectangle(pos.x() - 8.,  pos.y() - 8., 16., 16.);
                    boundsHasChanged();
				}
			}
		}
	}
	
	void IoletHighlighter::draw(Gui::Doodle &d)
	{
		const Gui::Point size = d.getSize();
		const Gui::Color color = (m_polarity == Iolet::Cold) ? Gui::Color(0.28, 0.28, 0.88, 1) : Gui::Color(0.88, 0.28, 0.28, 1);
		d.setColor(color.brighter(0.3));
        d.fillEllipse(4.5, 4.5, size.x() - 9., size.y() - 9.);
        d.setColor(color);
        d.drawEllipse(4.5, 4.5, size.x() - 9., size.y() - 9., 1.5);
	}
}
