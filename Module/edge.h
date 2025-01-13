#ifndef KEDGE_H
#define KEDGE_H

#include "kfc.h"


struct KEDGEPIXEL{
    unsigned short   u;
    unsigned short   v;
    unsigned short   wAng;//0-360
    unsigned short   wDir;//0,1,2,3
    double  		 dMag;
};

class KEdge : public std::vector<KEDGEPIXEL>
{
    KMatrix         _mKernelX, _mKernelY;  //convolution mask
    KImageDouble    _idGradX, _idGradY;
    KImageDouble    _idMag, _idBuf;        //buffer for edge gradients and magnitudes
    KImageWord      _iwAng;                //0~360
    KImageGray      _igDir;                //0~3

    int             _nHalf, _nMaskLength;  //dimension of convolution mask
    int             _nWidth, _nHeight;     //image size
    KRect           _rcArea;
    double          _dSigma;               //edge stdev.

    //intermediates
    std::vector<KEDGEPIXEL> _lDefinite;
    unsigned int            _dwpSQR[111];       //store square of (10*index) for fast calculation

public:
    KEdge(){ }
    KEdge(double dSigma,int nLength=0,int nCapacity=2000){ Create(dSigma,nLength,nCapacity); }
    ~KEdge(){}
    void         Create(double dSigma,int nLength=0,int nCapacity=2000);

    //operations
public:
    void    Canny(double dLow,double dHigh,const KImageGray& igIn);
    void    Canny(double dLow,double dHigh,const KImageGray& igIn,  KImageGray& igOut);
    void    Canny(double dLow,double dHigh,const KImageDouble& idIn,KImageGray& igOut);

    void 	Canny(double dLow,double dHigh,const KRect& rcFrom,const KRect& rcTo,const KImageGray& igIn);
    void    Canny(double dLow,double dHigh,const KRect& rcWin,const KImageGray& igIn);
    void    Canny(double dLow,double dHigh,const KRect& rcWin,const KImageColor& icIn);
    void    Canny(double dLow,double dHigh,const KRect& rcWin,const KImageColor24& icIn);

    void    Gradient(const KRect& rcRegion,const KImageGray& igIn);
    void    Gradient(const KRect& rcRegion,const KImageDouble& afIn);
    void    Gradient(const KImageGray& igIn, KImageGray& igOut);
    void    Gradient(const KImageDouble& idIn);
    double  GradientEnergy(const KImageGray& igIn);
    void	DT(const double& dBW,KImageWord& iwDist,KImageWord& iwDir);
    //getting
public:
    int             Width()  const { return _nWidth; }
    int             Height() const { return _nHeight; }
    int             Half()   const { return _nHalf; }

    KImageDouble&   GradX(){ return _idGradX; }
    KImageDouble&   GradY(){ return _idGradY; }
    KMatrix&   		KernelX(){ return _mKernelX; }
    KMatrix&   		KernelY(){ return _mKernelY; }
};
//---------------------------------------------------------------------------


#endif // KEDGE_H
