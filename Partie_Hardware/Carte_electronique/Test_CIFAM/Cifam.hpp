#ifndef __Cifam_HPP__
#define __Cifam_HPP__

#include "Injection.hpp"
#include "TempRetard.hpp"
#include "Mode.hpp"
#include "Cifam.h"

class CIFAM
{
    private:
        MODE *_mode;

    public:
        
        float _Gain1;
        float _Gain2;
        float _Gain3;
        float _Gain4;

        CIFAM();
        CIFAM(int mode);

        void Init();
        void DacWrite(unsigned long int data, unsigned long int channel);

        void GainOut1(float value);
        void GainOut2(float value);
        void GainOut3(float value);
        void GainOut4(float value);

        ~CIFAM();
};

#endif