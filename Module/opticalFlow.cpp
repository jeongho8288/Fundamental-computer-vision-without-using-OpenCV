#include "opticalFlow.h"
#include <QtDebug>

int KOpticalFlow::Init(void* pInfo)
{
    //KOpticalFlow Info.
    _oInfo  = *(KOPTICALFLOW_INFO*)pInfo;
    _oInfo.nWindow += (1 - _oInfo.nWindow % 2); //nWindow shoulde be odd

    //Gaussian weights
    double  dSigma = _oInfo.nWindow / 5.0; //window_size ~ 5 * sigma
    _mW = KGaussianMulti::Kernel_2D(dSigma, _oInfo.nWindow);
    _mW = KMatrix::Diag(_mW.Size(), _mW.Address());

    //Gaussian Smoothing Mask
    _vKernelG = KGaussianMulti::Kernel_1D(0.7);

    //Init. mA and vB : mA * (u,v) = vB
    _mA.Create(_SQR(_oInfo.nWindow), 2);
    _vB.Create(_SQR(_oInfo.nWindow));

    //t-1에서의 영상 초기화
    _igOld.Release();

    return 1;
}

void* KOpticalFlow::Run(void* pImg,void* p2,void* p3,void* p4,void* p5)
{
    //첫 이미지인 경우
    if(_igOld.Address() == 0)
    {
        _igOld = *(KImageGray*)pImg;
        _igOld.Convoluted(_vKernelG,_X_AXIS).Convoluted(_vKernelG,_Y_AXIS);

        //For flow computation
        _lFlow.resize(_igOld.Row());
        for(auto& row : _lFlow)
            row.resize(_igOld.Col());

        //For flow computation
//        _arFlow.Create(_igOld.Row(), _igOld.Col());

        _oInfo.nSx  = _oInfo.nWindow/2 + _vKernelG.Dim()/2 + 1;
        _oInfo.nEx  = _igOld.Col() - _oInfo.nSx;
        _oInfo.nSy  = _oInfo.nWindow/2 + _vKernelG.Dim()/2 + 1;
        _oInfo.nEy  = _igOld.Row() - _oInfo.nSy;

        _idIx.Create(_igOld.Row(), _igOld.Col());
        _idIy.Create(_igOld.Row(), _igOld.Col());
        _idIt.Create(_igOld.Row(), _igOld.Col());

        return 0;
    }

    //가우시안 필터링
    _igCur  = *(KImageGray*)pImg;
    _igCur.Convoluted(_vKernelG,_X_AXIS).Convoluted(_vKernelG,_Y_AXIS);

    //Ix, Iy, It 계산
    int nHg = _vKernelG.Dim()/2 + 1;

    for(unsigned int i = nHg; i < _igCur.Row() - nHg; i++)
        for(unsigned int j = nHg; j < _igCur.Col() - nHg; j++)
        {
            _idIx[i][j] = _igCur[i][j+1] - _igCur[i][j-1];
            _idIy[i][j] = _igCur[i+1][j] - _igCur[i-1][j];
            _idIt[i][j] = _igCur[i][j]   - _igOld[i][j];
        }

    //optical flow 구함
    static KVector vFlow(2);
    static KMatrix mAt, mTmp;
    int            nHf = _oInfo.nWindow/2;
    double         dDet;

    for(int i = _oInfo.nSy; i < _oInfo.nEy; i += _oInfo.nStride)
        for(int j = _oInfo.nSx; j < _oInfo.nEx; j += _oInfo.nStride)
        {
            //mA, vB 계산
            for(int ii = -nHf, idx = 0; ii <= nHf; ii++)
                for(int jj = -nHf; jj <= nHf; jj++){
                    _mA[idx][0] = _idIx[i+ii][j+jj]; _mA[idx][1] = _idIy[i+ii][j+jj];
                    _vB[idx++]  = -_idIt[i+ii][j+jj];
                }

            //(u,v) 계산
            mAt   = _mA.Tr();
            mTmp  = mAt*_mW*_mA;
            dDet  = _ABS(mTmp[0][0]*mTmp[1][1] - mTmp[0][1]*mTmp[1][0]);

            if(dDet < 0.00001)
                _lFlow[i][j].fX = _lFlow[i][j].fY = 0.0;
            else
            {
                double dTmp = mTmp[0][0];
                mTmp[0][0] = mTmp[1][1] / dDet;     mTmp[0][1] = mTmp[0][1] / (-dDet);
                mTmp[1][0] = mTmp[1][0] / (-dDet);  mTmp[1][1] = dTmp / dDet;

                vFlow = mTmp * mAt * _mW * _vB;
                _lFlow[i][j].fX = (float)vFlow[0];
                _lFlow[i][j].fY = (float)vFlow[1];
            }
        }

    //현재 이미지 저장
    _igOld = _igCur;

    qDebug() << "Returning _lFlow with size:" << _lFlow.size();
    return (void*)&_lFlow;
}

