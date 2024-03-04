#ifndef __Injection_HPP__
#define __Injection_HPP__

#include "Mode.hpp"
#include "Cifam.hpp"

class INJECTION : public MODE
{
    private:
        
    public:
        INJECTION();

        void Chrono1();
        void Chrono2();
        void Chrono3();
        void Chrono4();

        ~INJECTION();
};



#endif
