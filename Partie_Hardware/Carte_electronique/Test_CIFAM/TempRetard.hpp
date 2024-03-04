#ifndef __TempRetard_HPP__
#define __TempRetard_HPP__

#include "Mode.hpp"
#include "Cifam.hpp"

class TEMPRETARD : public MODE
{
    private:
        
    public:
        TEMPRETARD();

        void Chrono1();
        void Chrono2();
        void Chrono3();
        void Chrono4();

        ~TEMPRETARD();
};

#endif
