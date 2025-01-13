#ifndef KOPTICALFLOW_H
#define KOPTICALFLOW_H

#include "kfc.h"

struct KOPTICALFLOW_INFO
{
    int    nWindow;
    int    nStride;
    int    nSx, nEx;
    int    nSy, nEy;
};

class KOpticalFlow : public KObject
{
    //초기화 정보
    KOPTICALFLOW_INFO   _oInfo;
    KArray<KPOINTF>     _arFlow;

    //결과 저장 배열
    std::vector<std::vector<KPOINTF>> _lFlow;

    //영상버퍼
    KImageGray          _igOld;
    KImageGray          _igCur;

    //계산용
    KMatrix             _mW;
    KMatrix             _mA;
    KVector             _vB;
    KVector             _vKernelG;
    KImageDouble        _idIx;
    KImageDouble        _idIy;
    KImageDouble        _idIt;

    int                 _nRow, _nCol;
    int                 _nSx, _nEx, _nSy, _nEy, _nHalf;

public:
    KOpticalFlow(){ }
    virtual ~KOpticalFlow(){ }

    virtual int     Init(void* pInfo);
    virtual void*   Run(void* pImg,void* p2=0,void* p3=0,void* p4=0,void* p5=0);
    virtual void*   Info(){ return (void*)&_oInfo; }

};

#endif // KOPTICALFLOWS_H
