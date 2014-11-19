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

#ifndef __DEF_KIWI_ATTRIBUTE__
#define __DEF_KIWI_ATTRIBUTE__

#include "Defs.h"
#include "Tag.h"
#include "Element.h"
#include "Dico.h"
#include "Event.h"
#include "Doodle.h"

namespace Kiwi
{
    class Instance;
    
    // ================================================================================ //
    //                                      ATTRIBUTE                                   //
    // ================================================================================ //
    
    //! The attribute is an abstract class that holds a set of values of differents kinds and differents sizes.
	/** The attribute manages a set of values that should be displayed in an inspector. The setter and getter must be override.
     */
    class Attr
    {
    public:
		/** Flags describing the behavior of the attribute.
		 @see setInvisible, setDisabled, setSaveable, setNotifyChanges
		 */
        enum Behavior
        {
            Invisible			= (1<<0),///< Indicates that the attribute is invisible.
			Disabled			= (1<<1),///< Indicates that the attribute can't be changed.
			NotFreezable		= (1<<2),///< Indicates that the attribute can not be frozen.
            NotSaveable			= (1<<3),///< Indicates that the attribute is not saveable.
            NotNotifyChanges	= (1<<4) ///< Indicates that the attribute should not notify its changes.
        };
        
		/** Flags describing the display style of the attribute.
		 @see setStyle
		 */
		enum Style
		{
			Default = 0,	///< Indicates that the attribute should be displayed in a default style depending on its Type.
			Text,			///< Indicates that the attribute should be displayed in a text style
			List,			///< Indicates that the attribute should be displayed in a list of same or mixed elements style
			Enum,			///< Indicates that the attribute should be displayed in an enum style
			Number,			///< Indicates that the attribute should be displayed in a number style
			Toggle,			///< Indicates that the attribute should be displayed in a onoff toggle style
			Color,			///< Indicates that the attribute should be displayed in a color style
			Filepath		///< Indicates that the attribute should be displayed in a filepath style
		};
		
    private:
        static const sTag frozen_attributes;
        
        const sTag          m_name;				///< The name of the attribute.
        const sTag          m_label;			///< The label of the attribute.
        const sTag          m_category;			///< The category of the attribute.
        const Style         m_style;			///< The style of the attribute.
		const ElemVector    m_default_values;	///< The default value of the attribute.
        atomic_long         m_behavior;			///< The behavior of the attribute.
		ElemVector          m_frozen_values;    ///< The frozen value of the attribute.
        
    public:
		
        //! Constructor.
        /** Allocate and initialize the member values.
		 @param name		The name of the attribute.
		 @param label		A short description of the attribute in a human readable style.
         @param category	A named category that the attribute fits into.
		 @param style		The style of the attribute specified in the Attr::Style enum.
         @param defaultValues The default values.
		 @param behavior	A combination of the flags specified in the Attr::Behavior enum,
							which define the attribute's behavior.
         */
        Attr(sTag name,  sTag label, sTag category, Style style = Style::Default, ElemVector defaultValues = {}, long behavior = 0);
        
        //! Destructor.
        /** Clear the attribute.
         */
        virtual ~Attr();
        
        //! Attribute maker.
		/** The function creates an attribute with arguments.
		 */
		template<class AttrClass, class ...Args> static shared_ptr<AttrClass> create(Args&& ...arguments)
		{
            sAttr attr = make_shared<AttrClass>(forward<Args>(arguments)...);
            if(attr)
            {
                attr->setDefaultValues();
            }
			return attr;
        }
        
        //! Attribute maker.
		/** The function creates an attribute.
		 */
		template<class AttrClass> static sAttr create()
		{
			sAttr attr = make_shared<AttrClass>();
            if(attr)
            {
                attr->setDefaultValues();
            }
			return attr;
		}
		
		//! Retrieve the name of the attribute.
		/** The function retrieves the name of the attribute.
		 @return The name of the attribute.
		 @see getLabel, getStyle, getCategory
		 */
		inline sTag getName() const noexcept
        {
            return m_name;
        }
		
		//! Retrieve the attribute label.
		/** The function retrieves the attribute label.
		 @return The attribute label.
		 @see getName, getStyle, getCategory
		 */
		inline sTag getLabel() const noexcept
        {
            return m_label;
        }
        
        //! Retrieve the attribute category.
		/** The function retrieves the attribute category.
		 @return The attribute category.
		 @see setCategory, getName, getLabel, getStyle
		 */
		inline sTag getCategory() const noexcept
        {
            return m_category;
        }
		
		//! Retrieve the attribute style.
		/** The function retrieves the attribute style.
		 @return The attribute style.
		 @see getName, getLabel, getCategory
		 */
		inline Style getStyle() const noexcept
        {
            return m_style;
        }
        
        //! Retrieve if the attribute is invisible.
		/** The function retrieves if the attribute is invisible.
		 @return True if the attribute is invisible otherwise false.
		 @see setInvisible, isDisabled, isSaveable, shouldNotifyChanges, getBehavior
		 */
		inline bool isInvisible() const noexcept
        {
            return m_behavior & Invisible;
        }
		
		//! Retrieve if the attribute is disable.
		/** The function retrieves if the attribute is disable.
		 @return True if the attribute is disabled otherwise false.
		 @see setDisabled, isInvisible, isSaveable, shouldNotifyChanges, getBehavior
		 */
		inline bool isDisabled() const noexcept
        {
            return m_behavior & Disabled;
        }
		
		//! Retrieve if the attribute is saveable.
		/** The function retrieves if the attribute is saveable.
		 @return True if the attribute is saveable otherwise false.
		 @see setDisabled, isInvisible, isSaveable, shouldNotifyChanges, getBehavior
		 */
		inline bool isSaveable() const noexcept
        {
            return !(m_behavior & NotSaveable);
        }
		
		//! Retrieve if the attribute should notify changes.
		/** The function retrieves if the attribute should notify changes.
		 @return True if the attribute should notify changes otherwise false.
		 @see setDisabled, isInvisible, isSaveable, shouldNotifyChanges, getBehavior
		 */
		inline bool shouldNotifyChanges() const noexcept
        {
            return !(m_behavior & NotNotifyChanges);
        }
		
        //! Retrieves the whole behavior flags field of the attribute.
		/** The function retrieves the whole behavior flags field of the attribute.
		 @param behavior	A combination of the flags specified in the Attr::Behavior enum,
		 which define the attribute's behaviors.
		 @see setInvisible, setDisabled, setSaveable, setNotifyChanges
		 */
		inline long getBehavior() const noexcept
        {
            return m_behavior;
        }
		
		//! Is the attribute currently frozen ?
		/** Is the attribute currently frozen ?
		 @return True if the attribute is frozen, false otherwise.
		 @see freeze, getFrozenValue
		 */
		bool isFrozen() const noexcept
        {
            return !m_frozen_values.empty();
        }
        
        //! Retrieves the default values.
		/** Retrieve the default values.
		 @param elements A vector of elements to fill.
		 @see set
		 */
		inline void getDefaultValues(ElemVector& elements) const noexcept
        {
            elements = m_default_values;
        }
        
		//! Retrieve the frozen value.
		/** Retrieve the frozen value, if the attribute is not frozen the vector will be empty.
		 @param elements A vector of Element to be replaced by the frozen value.
		 @see freeze, isFrozen
		 */
		void getFrozenValues(ElemVector& elements) const noexcept
        {
            elements = m_frozen_values;
        }
        
        //! Retrieves the enum values.
		/** The Attr subclasses than want to be displayed in an enum style must implement this function to retrieve enum values.
		 @param elements A vector of elements to fill.
		 @see AttrEnum
		 */
		virtual void getEnumValues(ElemVector& elements) const
        {
            elements.clear();
        }
		
		//! Attempts to write the attribute in a dico.
		/** The function attempts to write the attribute in a dico.
		 If the behavior of the attribute is to not save the value and the attribute is not frozen, nothing will happen
		 @param dico The dico to write into.
		 @see read
		 */
		void write(sDico dico) const noexcept;
        
        //! Retrieves the values.
		/** The Attr subclasses must implement this function to retrieve the values.
		 @param elements A vector of elements to fill.
		 @see set
		 */
		virtual void get(ElemVector& elements) const noexcept = 0;
	
	protected:
		
		//! Sets the values with a vector of elements.
		/** The function sets the values with a vector of elements and resize the values if necessary.
		 Attr subclasses must implement this function
		 @param elements The vector of elements.
		 @see get
		 */
		virtual void set(ElemVector const& elements) = 0;
		
		//! Resets the attribute values to default values.
		/** Resets the attribute values to its default values.
		 @see set
		 */
		void setDefaultValues();
        
        //! Resets the attribute values to frozen values.
		/** Resets the attribute values to its frozen values.
		 @see set
		 */
		void setFrozenValues();
		
        /** An easy way to set the whole behavior flags field of the attribute.
		 @param behavior	A combination of the flags specified in the Attr::Behavior enum,
         which define the attribute's behaviors.
		 @see setInvisible, setDisabled, setSaveable, setNotifyChanges
		 */
		void setBehavior(long behavior) noexcept;
        
		/** An easy way to set or remove the Invisible bit in the attribute behavior flags field.
		 @param isVisible If true, the attribute will be invisible, if false it will be visible.
		 @see setDisabled, setSaveable, setNotifyChanges, setBehavior
		 */
		void setInvisible(const bool isInvisible) noexcept;
		
		/** An easy way to set or remove the Disabled bit in the attribute behavior flags field.
		 @param isDisabled If true, the attribute will be disabled, if false it will not.
		 @see setInvisible, setSaveable, setNotifyChanges, setBehavior
		 */
		void setDisabled(const bool isDisabled) noexcept;
		
		/** An easy way to set or remove the NotSaveable bit in the attribute behavior flags field.
		 @param saveable If true, the attribute will be saved, if false it will not.
		 @see setInvisible, setDisabled, setNotifyChanges, setBehavior
		 */
		void setSaveable(const bool saveable) noexcept;
		
		/** An easy way to set or remove the NotNotifyChanges bit in the attribute behavior flags field.
		 @param shouldNotify If true, the attribute will broadcast a notify message to its listeners when changed, if false it will not.
		 @see setInvisible, setDisabled, setSaveable, setBehavior
		 */
		void setNotifyChanges(const bool shouldNotify) noexcept;
        
        //! Read the attribute in a dico.
        /** The function reads the attribute in a dico.
         @param dico The dico.
		 @see write
         */
        virtual void read(scDico dico);
		
		//! Freezes or unfreezes the attribute.
		/** If you freeze an attribute, it will stores its current value as the saved value.
		 When an attribute is frozen it can still be changed,
		 but when the attribute will be saved it will take the frozen value rather than the current one.
		 @param frozen If true the attribute will be frozen, if false it will be unfrozen.
		 @see isFrozen, getFrozenValue
		 */
		void freeze(const bool frozen);
        
    public:
        // ================================================================================ //
        //                                  ATTRIBUTE MANAGER                               //
        // ================================================================================ //
        
        //! The attribute manager manages a set of attributes.
        /** The attribute manager manages a set of attributes, it allows the setting and the getting of their values and to retrieve them by name or by category.
         @see Attr
         */
        class Manager
        {
        private:
            unordered_map<sTag, sAttr>          m_attrs;
            mutable mutex                       m_attrs_mutex;
            
        public:
            
            //! Constructor.
            /** Creates a new attribute manager.
             */
            Manager() noexcept;
            
            //! Descrutor.
            /** Free the attributes.
             */
            ~Manager();
            
        protected:
            
            //! Add an attribute.
            /** The function adds an attribute .
             @param attr the attribute to add.
             */
            void addAttribute(sAttr attr);
            
            //! Remove an attribute.
            /** The function removes an attribute.
             @param attr The attribute to remove.
             */
            void removeAttribute(sAttr attr);
            
            //! The an attribute.
            /** The function removes an attribute.
             @param name The name of the attribute to remove.
             */
            void removeAttribute(sTag name);
            
            //! Set the attribute behavior.
            /** The function sets the attribute behaviors.
             @param name The name of the attribute.
             @param behavior The behavior of the attribute.
             */
            void setAttributeBehavior(sTag name, Attr::Behavior behavior);
            
            //! Set the attributes values with a dico.
            /** The function sets the attributes values with a dico.
             @param dico A dico.
             */
            void read(scDico dico) noexcept;
            
        public:
            
            //! Retrieve the number of attributes.
            /** The function retrieves the numbers of attributes. The attributes invisibles won't be counted.
             @return The number of attributes.
             */
            unsigned long getNumberOfAttributes() const noexcept;
            
            //! Retrieve the names of the attributes.
            /** The function retrieves the names of the attributes. The name attributes invisibles won't be retrieved.
             @param names A vector of tags that will contain the names of the attributes;
             */
            void getAttributeNames(vector<sTag>& names) const noexcept;
            
            //! Check if a given attribute exist.
            /** The function checks if a given attribute exist. If the attribute is invisible the function returns false.
             @param name The name of the attribute.
             @return true if an attribute exist, otherwise false.
             */
            bool hasAttribute(sTag name) const noexcept;
            
            //! Retrieve an attribute.
            /** The function retrieves an attribute. If the attribute is invisible the function returns a pointer null.
             @param name The name of the attribute.
             @return The attribute or null if the attribute doesn't exist.
             */
            sAttr getAttribute(sTag name) const noexcept;
            
            //! Set the values of an attribute.
            /** The function sets the value of an attribute.
             @param name		The name of the attribute.
             @param elements    A vector of elements to pass.
             @return true if the attribute value has setted its values, otherwise false.
             @see getAttributeValue
             */
            bool setAttributeValue(sTag name, ElemVector const& elements);
            
            //! Get the values of an attribute.
            /** The function gets the values of an attribute.
             @param name        The name of the attribute.
             @param elements    A vector of elements to pass.
             @return true if the attribute value retrieved its values, otherwise false.
             @see setAttrValue
             */
            bool getAttributeValue(sTag name, ElemVector& elements);
            
            //! Retrieve the number of attribute categories.
            /** The function retrieves the number of attribute categories. If a category have only invisibles attributes, the category won't be counted.
             @return The number of attribute categories.
             */
            unsigned long getNumberOfCategories() const noexcept;
            
            //! Retrieve the names of the categories.
            /** The function retrieves the names of the categories. If a category have only invisibles attributes, the name of the category won't be retrieved.
             @param A vector of tag containing the names of the categories.
             */
            void getCategoriesNames(vector<sTag>& names) const noexcept;
            
            //! Check if a category exist.
            /** The function checks if a category exist.  If a category have only invisibles attributes, the function returns false.
             @param name The name of the category.
             @return true if the category exist, otherwise false.
             */
            bool hasCategory(sTag name) const noexcept;
            
            //! Retrieve the attributes of a category.
            /** The function retrieves the attributes of a category. If an attribute is invisible, the attribute won't be retrieved.
             @param name The name of the category.
             @param attrs A vector of attributes to pass.
             */
            void getCategory(sTag name, vector<sAttr>& attrs) const;
            
            //! Write the attributes in a dico.
            /** The function writes the attributes in a dico.
             @param dico A dico.
             */
            void write(sDico dico) const noexcept;
            
            //! Notify the manager that an attribute has created.
            /** The function notifies the manager that an attribute has created.
             @param dico A dico.
             */
            virtual void attributeHasCreated(){};
            
            //! Notify the manager that an attribute has removed.
            /** The function notifies the manager that an attribute has removed.
             @param dico A dico.
             */
            virtual void attributeHasRemoved(){};
            
            //! Notify the manager that the values of an attribute has changed.
            /** The function notifies the manager that the values of an attribute has changed.
             @param dico A dico.
             */
            virtual void attributeValuesHasChanged(){};
            
            //! Notify the manager that the behavior of an attribute has changed.
            /** The function notifies the manager that the behavior of an attribute has changed.
             @param dico A dico.
             */
            virtual void attributeBehaviorHasChanged(){};
            
            // ================================================================================ //
            //								ATTRIBUTE MANAGER LISTENER                          //
            // ================================================================================ //
            
            //! The attribute manager listener is a virtual class that can be binded to an attribute manager to be notified of various changes.
            /** The attribute manager listener is a very light class that allows to be notified of the attributes modification.
             */
            class Listener
            {
            public:
                virtual ~Listener() {}
                
                //! Receive the notification that an attribute has changed.
                /** Sublass of Attr::Manager::Listener must implement this virtual function to receive notifications when an attribute is added or removed, or when its value, appearance or behavior changes.
                 @param manager		The Attr::Manager that manages the attribute.
                 @param attr		The attribute that has been modified.
                 @param type		The type of notification as specified in the Attr::Manager::NotificationType enum,
                 */
                virtual void attributeChanged(Manager* manager, sAttr attr) = 0;
            };
        };
    };
    
    // ================================================================================ //
    //                                      ATTRIBUTE TYPED                             //
    // ================================================================================ //
    
    //! The bool attribute is an attribute that holds a boolean value.
    /** The bool attribute holds a boolean value. Its default display style is obviously an Attr::Style::Toggle.
     @see Attr.
     */
    class AttrBool : public Attr
    {
    private:
        bool m_value;
    public:
        AttrBool(sTag name, sTag label, sTag category, bool default_value = 0, long behavior = 0) :
        Attr(name, label, category, Attr::Style::Toggle, {(long)default_value}, behavior) {;}
        virtual ~AttrBool() {};
        virtual void get(ElemVector& elements) const noexcept;
        virtual void set(ElemVector const& elements) override;
    };

    //! The bool attribute is an attribute that holds a long value.
    /** The bool attribute holds a long value. Its default display style is obviously an Attr::Style::Number
	 @see Attr.
	 */
    class AttrLong : public Attr
    {
    private:
        long m_value;
    public:
		AttrLong(sTag name, sTag label, sTag category, long default_value = 0, long behavior = 0) :
        Attr(name, label, category, Attr::Style::Number, {default_value}, behavior) {;}
		virtual ~AttrLong() {};
        virtual void get(ElemVector& elements) const noexcept;
        virtual void set(ElemVector const& elements) override;
    };
	
    //! The bool attribute is an attribute that holds a double value.
    /** The bool attribute holds a double value. Its default display style is obviously an Attr::Style::Number
	 @see Attr.
	 */
    class AttrDouble : public Attr
    {
    private:
        double m_value;
    public:
		AttrDouble(sTag name, sTag label, sTag category, double default_value = 0, long behavior = 0) :
        Attr(name, label, category, Attr::Style::Number, {default_value}, behavior) {;}
		virtual ~AttrDouble() {};
        virtual void get(ElemVector& elements) const noexcept;
        virtual void set(ElemVector const& elements) override;
    };
	
	//! The bool attribute is an attribute that holds a tag.
    /** The bool attribute holds a tag. Its default display style is obviously an Attr::Style::Text
	 @see Attr.
	 */
    class AttrTag : public Attr
    {
    private:
        sTag m_value;
    public:
		AttrTag(sTag name, sTag label, sTag category, sTag default_value = 0, long behavior = 0) :
        Attr(name, label, category, Attr::Style::Text, {default_value}, behavior) {;}
		virtual ~AttrTag() {};
        virtual void get(ElemVector& elements) const noexcept;
        virtual void set(ElemVector const& elements) override;
    };
	
	//! The enum attribute is an attribute that represent an enumeration.
	/** The enum attribute holds a long value and represents a position in an enumeration. The value can be set either by a number or a Tag, and will be clipped between 0 and the size of the enumeration. Its default display style is obviously an Attr::Style::Enum.
     @see Attr.
	 */
	class AttrEnum : public Attr
	{
	private:
        ElemVector::size_type m_value;
		ElemVector            m_enum_values;
	public:
		AttrEnum(sTag name, sTag label, sTag category, ElemVector const& enum_vals, Element const& default_value = 0, long behavior = 0) :
        Attr(name, label, category, Attr::Style::Enum, {default_value}, behavior), m_enum_values(enum_vals) {;}
		virtual ~AttrEnum() {};
		virtual void get(ElemVector& elements) const noexcept;
		virtual void set(ElemVector const& elements) override;
		virtual void getEnumValues(ElemVector& elements) const
        {
            elements = m_enum_values;
        }
	};
	
	//! The color attribute is an attribute that holds a color value.
	/** The color attribute holds a four double values suitable to represent a RGBA type color. Its default display style will obviously be a Attr::Style::Color. Each value of the vector will be clipped between 0 and 1.
	 @see Attr.
	 */
	class AttrColor : public Attr
	{
	private:
		double m_value[4];
	public:
		AttrColor(sTag name, sTag label, sTag category, ElemVector const& default_value = {0., 0., 0., 1.}, long behavior = 0) :
        Attr(name, label, category, Attr::Style::Color, {default_value}, behavior) {;}
		virtual ~AttrColor(){};
		virtual void get(ElemVector& elements) const noexcept;
		virtual void set(ElemVector const& elements) override;
	};
	
	//! The rectangle attribute is an attribute that is particulary suitable to represent a position and a size.
	/** The rectangle attribute holds four double values suitable to represent a rectangle, its default display style will obviously be a Attr::Style::List.
	 @see Attr
	 */
	class AttrRect : public Attr
	{
	private:
		double m_value[4];
	public:
		AttrRect(sTag name, sTag label, sTag category, ElemVector const& default_value = {0., 0., 0., 0.}, long behavior = 0) :
		Attr(name, label, category, Attr::Style::List, {default_value}, behavior) {;}
		virtual ~AttrRect() {};
		virtual void get(ElemVector& elements) const noexcept;
		virtual void set(ElemVector const& elements) override;
	};
	
	//! The point attribute is an attribute that is particulary suitable to represent a position.
	/** The point attribute holds two double values suitable to represent a point, its default display style will obviously be a Attr::Style::List.
	 @see Attr
	 */
	class AttrPoint : public Attr
	{
	private:
		double m_value[2];
	public:
		AttrPoint(sTag name, sTag label, sTag category, ElemVector const& default_value = {0., 0.}, long behavior = 0) :
		Attr(name, label, category, Attr::Style::List, {default_value}, behavior) {;}
		virtual ~AttrPoint() {};
		virtual void get(ElemVector& elements) const noexcept;
		virtual void set(ElemVector const& elements) override;
	};
    
    //! The font attribute is the default attributes for the font.
	/** Holds a vector of two double values suitable to represent a position or a size,
	 */
    class AttrFont
    {
    private:
        static const sTag Font;
        static const sTag fontname;
        static const sTag Font_Name;
        static const sTag Arial;
        static const sTag fontsize;
        static const sTag Font_Size;
        static const sTag fontface;
        static const sTag Font_Face;
        static const sTag normal;
        static const sTag bold;
        static const sTag italic;
        static const sTag bold_italic;
        static const sTag fontjustification;
        static const sTag Font_Justification;
        static const sTag left;
        static const sTag center;
        static const sTag right;
    public:
        class Name : public AttrTag
        {
        public:
            Name() : AttrTag(fontname, Font_Name, Font, Arial){};
            ~Name(){};
        };
        
        class Size : public AttrDouble
        {
        public:
            Size() : AttrDouble(fontsize, Font_Size, Font, 12){};
            ~Size(){};
        };
        
        class Face : public AttrEnum
        {
        public:
            Face() : AttrEnum(fontface, Font_Face, Font, {normal, bold, italic, bold_italic}, 0){};
            ~Face(){};
        };
        
        class Justification : public AttrEnum
        {
        public:
            Justification() : AttrEnum(fontjustification, Font_Justification, Font, {left, center, right}, 0){};
            ~Justification(){};
        };
    private:
        Kiwi::Font                  m_font;
        shared_ptr<Name>            m_name;
        shared_ptr<Face>            m_face;
        shared_ptr<Size>            m_size;
        shared_ptr<Justification>   m_just;
    public:
        
        AttrFont();
        ~AttrFont();
        
        Kiwi::Font getFont();
        Justification getJustification();
    };
    
    class AttrAppearance
    {
    public:
        class Hidden : public AttrBool
        {
        public:
            Hidden() : AttrBool(Tag::create("hidden"), Tag::create("Hide on Lock"), Tag::create("Appearance"), false){};
            ~Hidden(){};
        };
        
        class Presentation : public AttrBool
        {
        public:
            Presentation() : AttrBool(Tag::create("presentation"),  Tag::create("Include in Presentation"), Tag::create("Appearance"), false){};
            ~Presentation(){};
        };
        
        class Position : public AttrPoint
        {
        public:
            Position() : AttrPoint(Tag::create("position"), Tag::create("Position"), Tag::create("Appearance"), {0., 0.}){};
            ~Position(){};
        };
        
        class Size : public AttrPoint
        {
        public:
            Size() : AttrPoint(Tag::create("size"), Tag::create("Size"), Tag::create("Appearance"), {100., 20.}){};
            ~Size(){};
        };
        
        class PresentationPosition : public AttrPoint
        {
        public:
            PresentationPosition() : AttrPoint(Tag::create("presentation_pos"), Tag::create("Presentation Position"), Tag::create("Appearance"), {0., 0.}){};
            ~PresentationPosition(){};
        };
        
        class PresentationSize : public AttrPoint
        {
        public:
            PresentationSize() : AttrPoint(Tag::create("presentation_size"), Tag::create("Presentation Size"), Tag::create("Appearance"), {0., 0.}){};
            ~PresentationSize(){};
        };
    };
}


#endif


