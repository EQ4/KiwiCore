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

#ifndef __DEF_KIWI_ARITHMETIC_TILDE__
#define __DEF_KIWI_ARITHMETIC_TILDE__

#include "../KiwiBase/Core.h"

namespace Kiwi
{
    // ================================================================================ //
    //                                  ARITHMETIC                                      //
    // ================================================================================ //
    
    // ================================================================================ //
    //                                      PLUS                                        //
    // ================================================================================ //
    
    class PlusTilde : public Box
    {
    private:
        typedef shared_ptr<PlusTilde> This;
        friend class ArithmeticTilde;
        sample          m_addend;
    public:
        PlusTilde(shared_ptr<Instance> kiwi);
        
        static shared_ptr<Object> create(shared_ptr<Instance> kiwi, const shared_ptr<Tag> name, vector<Element>& elements);
        static void receiveLong(This, long value);
        static void receiveDouble(This x, double value);
        static void dsp(This x, shared_ptr<DspNode> node);
        static void processBoth(This x, long nins, sample const** ins, long nouts, sample** outs, long vectorsize);
        static void processLeft(This x, long nins, sample const** ins, long nouts, sample** outs, long vectorsize);
        static void processRight(This x, long nins, sample const** ins, long nouts, sample** outs, long vectorsize);
        static void processNone(This x, long nins, sample const** ins, long nouts, sample** outs, long vectorsize);
    };
    
    // ================================================================================ //
    //                                      MINUS                                       //
    // ================================================================================ //
    /*
    class MinusTilde : public Box
    {
    private:
        friend class ArithmeticTilde;
        sample          m_subtrahend;
    public:
        KIWI_BOX_INIT(MinusTilde, "-~");
        
        shared_ptr<Object> create(shared_ptr<Instance> kiwi, const shared_ptr<Tag> name, vector<Element>& elements) const override;
        
        static void receiveLong(shared_ptr<MinusTilde> plus, long value);
        static void receiveDouble(shared_ptr<MinusTilde> plus, double value);
        static void dsp(shared_ptr<MinusTilde> plus, shared_ptr<DspNode> node);
        static void processBoth(shared_ptr<MinusTilde> plus, long nins, sample const** ins, long nouts, sample** outs, long vectorsize);
        static void processLeft(shared_ptr<MinusTilde> plus, long nins, sample const** ins, long nouts, sample** outs, long vectorsize);
        static void processRight(shared_ptr<MinusTilde> plus, long nins, sample const** ins, long nouts, sample** outs, long vectorsize);
        static void processNone(shared_ptr<MinusTilde> plus, long nins, sample const** ins, long nouts, sample** outs, long vectorsize);
    };
    
    */
    class ArithmeticTilde
    {
    public:
        static void load(shared_ptr<Instance> kiwi)
        {
            kiwi->addObjectPrototype(unique_ptr<Box>(new PlusTilde(kiwi)));
            /*
            kiwi->addObjectPrototype(unique_ptr<Box>(new MinusTilde(kiwi)));
            k->addObjectPrototype(unique_ptr<Object>(new Minus(k, string("-"), elements)));
            k->addObjectPrototype(unique_ptr<Object>(new Times(k, string("Times"), elements)));
            k->addObjectPrototype(unique_ptr<Object>(new Times(k, string("*"), elements)));
            k->addObjectPrototype(unique_ptr<Object>(new Divide(k, string("Divide"), elements)));
            k->addObjectPrototype(unique_ptr<Object>(new Divide(k, string("/"), elements)));
            k->addObjectPrototype(unique_ptr<Object>(new Modulo(k, string("Modulo"), elements)));
            k->addObjectPrototype(unique_ptr<Object>(new Modulo(k, string("%"), elements)));*/
        }
    };
    
}

#endif


