//---------------------------------------------------------------------------
#ifndef circleHoughH
#define circleHoughH

#include "kfc.h"


using namespace std;

struct KCIRCLEHOUGH_INFO
{
	int		nLeft;
	int		nRight;
	int		nTop;
	int		nBottom;
	int		nMinR;
	int		nMaxR;
	int		nNumGridCx;
	int		nNumGridCy;
	int		nNumGridR;
    int		nDirEdge; 	    //defined in "kfc.h"
	int		nThreshVote;

};

class KCircleHough : public KObject
{
	KCIRCLEHOUGH_INFO	_oInfo;
    vector<vector<vector<double>>> _lllVotes;
    double				_dSx;
    double				_dSy;
    double				_dSr;

    //출력값
    KCircle             _oDetected;

public:
    KCircleHough(){}
    virtual ~KCircleHough(){}


    virtual int    Init(void* pInfo);
    virtual void*  Info(){ return &_oInfo; }
    virtual void*  Run(void* pEdge,void* pFrom=0,void* pTo=0,void* p4=0,void* p5=0);
};

//---------------------------------------------------------------------------
#endif
