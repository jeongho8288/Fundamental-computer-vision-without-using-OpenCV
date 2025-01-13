#ifndef KCORNER_H
#define KCORNER_H

#include "kfc.h"

using namespace std;

struct KCORNERPIXEL
{
    unsigned short   u;
    unsigned short   v;
    double  		 dR;
//    KVector          vD;
};

class KCorner : public vector<KCORNERPIXEL>
{
    KMatrix         _mKernelX, _mKernelY;   //convolution kernel
    KImageDouble    _idGradX2, _idGradY2, _idGradXY ;
    KImageDouble    _idR;
    int             _nKernelSize;           //dimension of convolution kernel

    int             _nBlockSize;
    KMatrix         _mBlockWeight;

public:
    KCorner( ){ }
    KCorner(const double& dSigma_a, const int& nBlockSize){ Create(dSigma_a,nBlockSize); }
    virtual ~KCorner(){ }

    void            Create(const double& dSigma_a, const int& nBlockSize);

    //operations
public:
    void            HarrisCorner(const double& dThresh, const KImageGray& igIn);
//    void            HarrisCorner2(const double& dThresh, const KImageGray& igIn, const KMatrix& mPca, const KVector& vMean);

    //getting
public:
    KImageDouble&   GradX2(){ return _idGradX2; }
    KImageDouble&   GradXY(){ return _idGradXY; }
    KImageDouble&   GradY2(){ return _idGradY2; }

};
//---------------------------------------------------------------------------


#endif // KCORNER_H
