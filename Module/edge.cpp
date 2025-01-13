#include "edge.h"

using namespace std;

void KEdge::Create(double dSigma,int nMaskLength,int nCapacity)
{
#ifdef _DEBUG
    assert(dSigma != 0.0);
#endif

    //set sigma
    _dSigma = dSigma;

    //init.
    vector<KEDGEPIXEL>::resize(nCapacity);
    _lDefinite.resize(nCapacity); //temporal stack


    if(nMaskLength == 0)
    {
        //set convolution mask length
        _nHalf  	 = ( 3.0*dSigma < 2.5 ? 2:(int)(3.0*dSigma+0.5) );
        _nMaskLength = _nHalf*2 +1;
    }
    else{
        //set convolution mask length
        _nMaskLength   =  (nMaskLength%2 == 0 ? nMaskLength+1 : nMaskLength);
        _nHalf         =  (_nMaskLength-1) / 2;
    }

    //create the conv. mask
    _mKernelX.Create(_nMaskLength,_nMaskLength);
    _mKernelY.Create(_nMaskLength,_nMaskLength);

    //compute the mask
    int    i,j,ii,jj;
    double dTmp,dScale=0.0,dSigma2 = 2.0*_SQR(dSigma);

    for(i=-_nHalf,ii=0; i<=_nHalf; i++,ii++)
    {
        dTmp = -i*exp(-(i*i)/2./_dSigma/_dSigma);

        for(j=-_nHalf,jj=0; j<=_nHalf; j++,jj++)
        {
            _mKernelY[ii][jj] = dTmp*exp(-(j*j)/dSigma2);
            _mKernelX[jj][ii] = _mKernelY[ii][jj];
            dScale += (i<0 ? _mKernelY[ii][jj] : 0.0);
        }
    }
    for(i=0; i<_nMaskLength; i++)
        for(j=0; j<_nMaskLength; j++)
        {
            _mKernelY[i][j] /= -dScale;
            _mKernelX[i][j] /= -dScale;
        }

    //for fast calculation
    for(int i=0; i<111; i++)
        _dwpSQR[i] = _SQR(i*10);
}

void KEdge::Gradient(const KImageGray& igIn, KImageGray& igOut)
{
    //set dimension of image
    _nWidth  = igIn.Col();
    _nHeight = igIn.Row();

    //init. memories
    igOut.Create(_nHeight,_nWidth);
    _idGradX.Create(_nHeight,_nWidth);
    _idGradY.Create(_nHeight,_nWidth);

    //compute magnitude and direction of edge
    int nSy = _nHalf, nEy = _nHeight - _nHalf;
    int nSx = _nHalf, nEx = _nWidth - _nHalf;
    double dGradX,dGradY,dMag;

    for(int i=nSy; i<nEy; i++){
        for(int j=nSx; j<nEx; j++){
            //convolution
            dGradX = dGradY = 0.0;
            for(int r=-_nHalf,rr=0; r<=_nHalf; r++,rr++){
                for(int c=-_nHalf,cc=0; c<=_nHalf; c++,cc++){
                    dGradX += igIn[i+r][j+c] * _mKernelX[rr][cc];
                    dGradY += igIn[i+r][j+c] * _mKernelY[rr][cc];
                }
            }
            //magnitude
            //dMag = _ABS(dGradX) + _ABS(dGradY);
            dMag = sqrt(_SQR(dGradX) + _SQR(dGradY));

            //orientation
            _idGradX[i][j] = dGradX;
            _idGradY[i][j] = dGradY;

            //set output
            igOut[i][j] = (unsigned char)(dMag);
        }
    }
}

void KEdge::Gradient(const KImageDouble& idIn)
{
    //init. memories
    _idGradX.Create(_nHeight,_nWidth);
    _idGradY.Create(_nHeight,_nWidth);

    //convolution
    int     nSy = _nHalf, nEy = _nHeight - _nHalf;
    int     nSx = _nHalf, nEx = _nWidth - _nHalf;

    for(int i=nSy; i<nEy; i++)
    {
        for(int j=nSx; j<nEx; j++)
            for(int r=-_nHalf,rr=0; r<=_nHalf; r++,rr++)
                for(int c=-_nHalf,cc=0; c<=_nHalf; c++,cc++)
                {
                    _idGradX[i][j] += idIn[i+r][j+c] * _mKernelX[rr][cc];
                    _idGradY[i][j] += idIn[i+r][j+c] * _mKernelY[rr][cc];
                }
    }
}

double KEdge::GradientEnergy(const KImageGray& igIn)
{
    int     nSy = _nHalf, nEy = igIn.Row() - _nHalf;
    int     nSx = _nHalf, nEx = igIn.Col() - _nHalf;
    double  dGradX, dGradY, dSum =0.0;

    for(int i=nSy; i<nEy; i++)
        for(int j=nSx; j<nEx; j++)
        {
            dGradX = dGradY = 0.0;

            for(int r=-_nHalf,rr=0; r<=_nHalf; r++,rr++)
                for(int c=-_nHalf,cc=0; c<=_nHalf; c++,cc++)
                {
                    dGradX += igIn[i+r][j+c] * _mKernelX[rr][cc];
                    dGradY += igIn[i+r][j+c] * _mKernelY[rr][cc];
                }
            dSum += dGradX*dGradX + dGradY*dGradY;
        }

    dSum /= (nEy-nSy)*(nEx-nSx);

    return dSum;
}

void KEdge::Gradient(const KRect& rcRegion,const KImageGray& igIn)
{
    //set dimension of image
    _nWidth  = rcRegion.Width();
    _nHeight = rcRegion.Height();

    //init. memories
    _idGradX.Create(_nHeight,_nWidth);
    _idGradY.Create(_nHeight,_nWidth);

    //compute magnitude and direction of edge
    for(int i=0,ii=rcRegion._nTop; i<_nHeight; i++,ii++){
        if(ii-_nHalf<0 || ii+_nHalf>igIn.Col()-1) continue;

        for(int j=0,jj=rcRegion._nLeft; j<_nWidth; j++,jj++){
            if(jj-_nHalf<0 || jj+_nHalf>igIn.Row()-1) continue;

            //convolution
            for(int r=-_nHalf,rr=0; r<=_nHalf; r++,rr++)
                for(int c=-_nHalf,cc=0; c<=_nHalf; c++,cc++){
                    _idGradX[i][j] += igIn[ii+r][jj+c]*_mKernelX[rr][cc];
                    _idGradY[i][j] += igIn[ii+r][jj+c]*_mKernelY[rr][cc];
                }}}
}

void KEdge::Gradient(const KRect& rcRegion,const KImageDouble& idIn)
{
    //set dimension of image
    _nWidth  = rcRegion.Width();
    _nHeight = rcRegion.Height();

    //init. memories
    _idGradX.Create(_nHeight,_nWidth);
    _idGradY.Create(_nHeight,_nWidth);

    //compute magnitude and direction of edge
    for(int i=0,ii=rcRegion._nTop; i<_nHeight; i++,ii++){
        for(int j=0,jj=rcRegion._nLeft; j<_nWidth; j++,jj++)
            //convolution
            for(int r=-_nHalf,rr=0; r<=_nHalf; r++,rr++)
                for(int c=-_nHalf,cc=0; c<=_nHalf; c++,cc++){
                    _idGradX[i][j] += idIn[ii+r][jj+c]*_mKernelX[rr][cc];
                    _idGradY[i][j] += idIn[ii+r][jj+c]*_mKernelY[rr][cc];
                }
    }
}

void KEdge::Canny(double dLow,double dHigh,const KRect& rcWin,const KImageGray& igIn)
{
    //set dimension of image
    _nWidth  = igIn.Col();  //입력 이미지 크기
    _nHeight = igIn.Row();


    //에지를 구할 영역 저장 확정
    _rcArea._nLeft 	= (rcWin._nLeft   < _nHalf ? _nHalf : rcWin._nLeft);
    _rcArea._nTop 	= (rcWin._nTop    < _nHalf ? _nHalf : rcWin._nTop);
    _rcArea._nRight	= (rcWin._nRight  < _nWidth -_nHalf ? rcWin._nRight : _nWidth-_nHalf-1);
    _rcArea._nBottom	= (rcWin._nBottom < _nHeight-_nHalf ? rcWin._nBottom : _nHeight-_nHalf-1);

    //init. memory
    int nLocalH = _rcArea.Height();
    int nLocalW = _rcArea.Width();

    _idMag.Create(nLocalH,nLocalW);
    _iwAng.Create(nLocalH,nLocalW);
    _igDir.Create(nLocalH,nLocalW);
    _idBuf.Create(nLocalH,nLocalW);

    //init.
    vector<KEDGEPIXEL>::clear();
    _lDefinite.clear();

    //compute magnitude and direction of edge
    int     i,j,r,c,ii,jj,rr,cc;
    double  fGradX,fGradY;
    double  dTmp;

    for(i=_rcArea._nTop,ii=0; i<=_rcArea._nBottom; i++,ii++){
        for(j=_rcArea._nLeft,jj=0; j<=_rcArea._nRight; j++,jj++){
            //convolution
            fGradX = fGradY = 0.0;
            for(r=-_nHalf,rr=0; r<=_nHalf; r++,rr++)
                for(c=-_nHalf,cc=0; c<=_nHalf; c++,cc++){
                    fGradX += igIn._ppA[i+r][j+c] * _mKernelX._ppA[rr][cc];
                    fGradY += igIn._ppA[i+r][j+c] * _mKernelY._ppA[rr][cc];
                }
            //magnitude
            _idMag._ppA[ii][jj] = _ABS(fGradX) + _ABS(fGradY);
            //direction
            if(_idMag._ppA[ii][jj] > dLow)
            {
                dTmp 		   		=  (atan2(fGradY, fGradX) + _PI) * 180.0 / _PI ;
                _iwAng._ppA[ii][jj] = (unsigned short)(dTmp+0.5);               //0~360
                _igDir._ppA[ii][jj] = (unsigned char)((((int)(dTmp/22.5)+1)>>1) & 0x00000003); //0~3
            }
            else
                _idMag._ppA[ii][jj] = 0.0;
        }}

    //non-maxima suppression
    int   		nShiftX[4] = {-1,1,0,-1};
    int   		nShiftY[4] = {0,1,1,1};
    KEDGEPIXEL  oEdgePxl;

    for(ii=1; ii<nLocalH-1; ii++)
    {
        for(jj=1; jj<nLocalW-1; jj++)
        {
            if(_idMag._ppA[ii][jj] > _idMag._ppA[ii+nShiftY[ _igDir._ppA[ii][jj] ]][jj+nShiftX[ _igDir._ppA[ii][jj] ]] &&
                _idMag._ppA[ii][jj] > _idMag._ppA[ii-nShiftY[_igDir._ppA[ii][jj] ]][jj-nShiftX[_igDir._ppA[ii][jj] ]])
            {
                if(_idMag._ppA[ii][jj] > dHigh)
                {
                    oEdgePxl.u    = (unsigned short)(jj + _rcArea._nLeft);
                    oEdgePxl.v 	  = (unsigned short)(ii + _rcArea._nTop);
                    oEdgePxl.wAng = _iwAng._ppA[ii][jj];
                    oEdgePxl.wDir = _igDir._ppA[ii][jj];
                    oEdgePxl.dMag  = _idMag._ppA[ii][jj];
                    _lDefinite.push_back(oEdgePxl);
                }
                _idBuf._ppA[ii][jj] = _idMag._ppA[ii][jj];
            }
        }}

    //hysteresis thresholding
    while(_lDefinite.empty() == false)
    {
        //pop out an edge pixel
        oEdgePxl = _lDefinite.back();
        _lDefinite.pop_back();

        //store the edge pixel
        vector<KEDGEPIXEL>::push_back(oEdgePxl);

        //get edge coordinate
        int jx  = oEdgePxl.u - _rcArea._nLeft;
        int iy  = oEdgePxl.v - _rcArea._nTop;

        //search neighbor edges
        for(i=-1; i<2; i++)
            for(j=-1; j<2; j++)
                if(_idBuf._ppA[iy+i][jx+j] && _idBuf._ppA[iy+i][jx+j]<=dHigh)
                {
                    oEdgePxl.u    = (unsigned short)(jx+j + _rcArea._nLeft);
                    oEdgePxl.v    = (unsigned short)(iy+i + _rcArea._nTop);
                    oEdgePxl.wAng = _iwAng._ppA[iy+i][jx+j];
                    oEdgePxl.wDir = _igDir._ppA[iy+i][jx+j];
                    oEdgePxl.dMag = _idBuf._ppA[iy+i][jx+j];

                    _lDefinite.push_back(oEdgePxl);
                    _idBuf._ppA[iy+i][jx+j] = 0.0;
                }
    }
}

void KEdge::Canny(double dLow,double dHigh,const KRect& rcFrom,const KRect& rcTo,const KImageGray& igIn)
{
    //init. stack
    vector<KEDGEPIXEL>::clear();
    _lDefinite.clear();

    //set dimension of image
    _nWidth  = igIn.Col();  //입력 이미지 크기
    _nHeight = igIn.Row();

    //에지를 구할 영역  확정
    _rcArea._nLeft 	 = (rcTo._nLeft   < _nHalf ? _nHalf : rcTo._nLeft);
    _rcArea._nTop 	 = (rcTo._nTop    < _nHalf ? _nHalf : rcTo._nTop);
    _rcArea._nRight	 = (rcTo._nRight  < _nWidth -_nHalf ? rcTo._nRight : _nWidth-_nHalf-1);
    _rcArea._nBottom = (rcTo._nBottom < _nHeight-_nHalf ? rcTo._nBottom : _nHeight-_nHalf-1);

    //init. memory
    int nLocalH = _rcArea.Height();
    int nLocalW = _rcArea.Width();

    _idMag.Create(nLocalH,nLocalW);
    _iwAng.Create(nLocalH,nLocalW);
    _igDir.Create(nLocalH,nLocalW);
    _idBuf.Create(nLocalH,nLocalW);


    //compute magnitude and direction of edge
    int     i,j,r,c,ii,jj,rr,cc;
    double  fGradX,fGradY;
    double  dTmp;

    for(i=_rcArea._nTop,ii=0; i<=_rcArea._nBottom; i++,ii++){
        for(j=_rcArea._nLeft,jj=0; j<=_rcArea._nRight; j++,jj++)
        {
            if(rcFrom.InRect((double)j,(double)i))
                continue;

            //convolution
            fGradX = fGradY = 0.0;
            for(r=-_nHalf,rr=0; r<=_nHalf; r++,rr++)
                for(c=-_nHalf,cc=0; c<=_nHalf; c++,cc++){
                    fGradX += igIn._ppA[i+r][j+c] * _mKernelX._ppA[rr][cc];
                    fGradY += igIn._ppA[i+r][j+c] * _mKernelY._ppA[rr][cc];
                }

            //magnitude
            _idMag._ppA[ii][jj] = _ABS(fGradX) + _ABS(fGradY);

            //direction
            if(_idMag._ppA[ii][jj] > dLow)
            {
                dTmp 		   		=  (atan2(fGradY, fGradX) + _PI) * 180.0 / _PI ;
                _iwAng._ppA[ii][jj] = (unsigned short)(dTmp+0.5);               //0~360
                _igDir._ppA[ii][jj] = (unsigned char)((((int)(dTmp/22.5)+1)>>1) & 0x00000003); //0~3
            }
            else
                _idMag._ppA[ii][jj] = 0.0;
        }}

    //non-maxima suppression
    int   		nShiftX[4] = {-1,1,0,-1};
    int   		nShiftY[4] = {0,1,1,1};
    KEDGEPIXEL  oEdgePxl;

    for(ii=1; ii<nLocalH-1; ii++)
    {
        for(jj=1; jj<nLocalW-1; jj++)
        {
            if(_idMag._ppA[ii][jj] > _idMag._ppA[ii+nShiftY[ _igDir._ppA[ii][jj] ]][jj+nShiftX[ _igDir._ppA[ii][jj] ]] &&
                _idMag._ppA[ii][jj] > _idMag._ppA[ii-nShiftY[_igDir._ppA[ii][jj] ]][jj-nShiftX[_igDir._ppA[ii][jj] ]])
            {
                if(_idMag._ppA[ii][jj] > dHigh)
                {
                    oEdgePxl.u    = (unsigned short)(jj + _rcArea._nLeft);
                    oEdgePxl.v 	  = (unsigned short)(ii + _rcArea._nTop);
                    oEdgePxl.wAng = _iwAng._ppA[ii][jj];
                    oEdgePxl.wDir = _igDir._ppA[ii][jj];
                    oEdgePxl.dMag  = _idMag._ppA[ii][jj];
                    _lDefinite.push_back(oEdgePxl);
                }
                _idBuf._ppA[ii][jj] = _idMag._ppA[ii][jj];
            }
        }}

    //hysteresis thresholding
    while(_lDefinite.empty() == false)
    {
        //pop out an edge pixel
        oEdgePxl = _lDefinite.back();
        _lDefinite.pop_back();

        //store the edge pixel
        vector<KEDGEPIXEL>::push_back(oEdgePxl);

        //get edge coordinate
        int jx  = oEdgePxl.u - _rcArea._nLeft;
        int iy  = oEdgePxl.v - _rcArea._nTop;

        //search neighbor edges
        for(i=-1; i<2; i++)
            for(j=-1; j<2; j++)
                if(_idBuf._ppA[iy+i][jx+j] && _idBuf._ppA[iy+i][jx+j]<=dHigh)
                {
                    oEdgePxl.u    = (unsigned short)(jx+j + _rcArea._nLeft);
                    oEdgePxl.v    = (unsigned short)(iy+i + _rcArea._nTop);
                    oEdgePxl.wAng = _iwAng._ppA[iy+i][jx+j];
                    oEdgePxl.wDir = _igDir._ppA[iy+i][jx+j];
                    oEdgePxl.dMag = _idBuf._ppA[iy+i][jx+j];

                    _lDefinite.push_back(oEdgePxl);
                    _idBuf._ppA[iy+i][jx+j] = 0.0;
                }
    }
}


void KEdge::Canny(double dLow,double dHigh,const KRect& rcWin,const KImageColor& icIn)
{
    //init. stack
    vector<KEDGEPIXEL>::clear();
    _lDefinite.clear();

    //set dimension of image
    _nWidth  = icIn.Col();  //입력 이미지 크기
    _nHeight = icIn.Row();

    //에지를 구할 영역 저장 확정
    _rcArea._nLeft 	= (rcWin._nLeft   < _nHalf ? _nHalf : rcWin._nLeft);
    _rcArea._nTop 	= (rcWin._nTop    < _nHalf ? _nHalf : rcWin._nTop);
    _rcArea._nRight	= (rcWin._nRight  < _nWidth -_nHalf ? rcWin._nRight : _nWidth-_nHalf-1);
    _rcArea._nBottom	= (rcWin._nBottom < _nHeight-_nHalf ? rcWin._nBottom : _nHeight-_nHalf-1);

    //init. mymory
    int nLocalH = _rcArea.Height();
    int nLocalW = _rcArea.Width();

    _idMag.Create(nLocalH,nLocalW);
    _iwAng.Create(nLocalH,nLocalW);
    _igDir.Create(nLocalH,nLocalW);
    _idBuf.Create(nLocalH,nLocalW);

    //대상 영역을 그레이 영상으로 변환
    KImageGray igIn;
    icIn.Crop(KRect(_rcArea._nLeft-_nHalf,_rcArea._nTop-_nHalf,_rcArea._nRight+_nHalf,_rcArea._nBottom+_nHalf),igIn);


    //compute magnitude and direction of edge
    int     i,j,r,c,ii,jj,rr,cc;
    double  fGradX,fGradY;
    double  dTmp;

    for(i=_nHalf,ii=0; ii<nLocalH; i++,ii++){
        for(j=_nHalf,jj=0; jj<nLocalW; j++,jj++){
            //convolution
            fGradX = fGradY = 0.0;
            for(r=-_nHalf,rr=0; r<=_nHalf; r++,rr++)
                for(c=-_nHalf,cc=0; c<=_nHalf; c++,cc++){
                    fGradX += igIn._ppA[i+r][j+c] * _mKernelX._ppA[rr][cc];
                    fGradY += igIn._ppA[i+r][j+c] * _mKernelY._ppA[rr][cc];
                }

            //magnitude
            _idMag._ppA[ii][jj] = _ABS(fGradX) + _ABS(fGradY);

            //direction
            if(_idMag._ppA[ii][jj] > dLow)
            {
                dTmp 		   		=  (atan2(fGradY, fGradX) + _PI) * 180.0 / _PI ;
                _iwAng._ppA[ii][jj] = (unsigned short)(dTmp+0.5);               //0~360
                _igDir._ppA[ii][jj] = (unsigned char)((((int)(dTmp/22.5)+1)>>1) & 0x00000003); //0~3
            }
            else
                _idMag._ppA[ii][jj] = 0.0;
        }}

    //non-maxima suppression
    int   		nShiftX[4] = {-1,1,0,-1};
    int   		nShiftY[4] = {0,1,1,1};
    KEDGEPIXEL  oEdgePxl;

    for(ii=1; ii<nLocalH-1; ii++)
    {
        for(jj=1; jj<nLocalW-1; jj++)
        {
            if(_idMag._ppA[ii][jj] > _idMag._ppA[ii+nShiftY[ _igDir._ppA[ii][jj] ]][jj+nShiftX[ _igDir._ppA[ii][jj] ]] &&
                _idMag._ppA[ii][jj] > _idMag._ppA[ii-nShiftY[_igDir._ppA[ii][jj] ]][jj-nShiftX[_igDir._ppA[ii][jj] ]])
            {
                if(_idMag._ppA[ii][jj] > dHigh)
                {
                    oEdgePxl.u    = (unsigned short)(jj + _rcArea._nLeft);
                    oEdgePxl.v 	  = (unsigned short)(ii + _rcArea._nTop);
                    oEdgePxl.wAng = _iwAng._ppA[ii][jj];
                    oEdgePxl.wDir = _igDir._ppA[ii][jj];
                    oEdgePxl.dMag = _idMag._ppA[ii][jj];
                    _lDefinite.push_back(oEdgePxl);
                }
                _idBuf._ppA[ii][jj] = _idMag._ppA[ii][jj];
            }
        }}

    //hysteresis thresholding
    while(_lDefinite.empty() == false)
    {
        //pop out an edge pixel
        oEdgePxl = _lDefinite.back();
        _lDefinite.pop_back();

        //store the edge pixel
        vector<KEDGEPIXEL>::push_back(oEdgePxl);

        //get edge coordinate
        int jx  = oEdgePxl.u - _rcArea._nLeft;
        int iy  = oEdgePxl.v - _rcArea._nTop;

        //search neighbor edges
        for(i=-1; i<2; i++)
            for(j=-1; j<2; j++)
                if(_idBuf._ppA[iy+i][jx+j] && _idBuf._ppA[iy+i][jx+j]<=dHigh)
                {
                    oEdgePxl.u    = (unsigned short)(jx+j + _rcArea._nLeft);
                    oEdgePxl.v    = (unsigned short)(iy+i + _rcArea._nTop);
                    oEdgePxl.wAng = _iwAng._ppA[iy+i][jx+j];
                    oEdgePxl.wDir = _igDir._ppA[iy+i][jx+j];
                    oEdgePxl.dMag = _idBuf._ppA[iy+i][jx+j];

                    _lDefinite.push_back(oEdgePxl);
                    _idBuf._ppA[iy+i][jx+j] = 0.0;
                }
    }
}

void KEdge::Canny(double dLow,double dHigh,const KRect& rcWin,const KImageColor24& icIn)
{
    //init. stack
    vector<KEDGEPIXEL>::clear();
    _lDefinite.clear();

    //set dimension of image
    _nWidth  = icIn.Col();  //입력 이미지 크기
    _nHeight = icIn.Row();

    //에지를 구할 영역 저장 확정
    _rcArea._nLeft 	= (rcWin._nLeft   < _nHalf ? _nHalf : rcWin._nLeft);
    _rcArea._nTop 	= (rcWin._nTop    < _nHalf ? _nHalf : rcWin._nTop);
    _rcArea._nRight	= (rcWin._nRight  < _nWidth -_nHalf ? rcWin._nRight : _nWidth-_nHalf-1);
    _rcArea._nBottom	= (rcWin._nBottom < _nHeight-_nHalf ? rcWin._nBottom : _nHeight-_nHalf-1);

    //init. mymory
    int nLocalH = _rcArea.Height();
    int nLocalW = _rcArea.Width();

    _idMag.Create(nLocalH,nLocalW);
    _iwAng.Create(nLocalH,nLocalW);
    _igDir.Create(nLocalH,nLocalW);
    _idBuf.Create(nLocalH,nLocalW);

    //대상 영역을 그레이 영상으로 변환
    KImageGray igIn;
    icIn.Crop(KRect(_rcArea._nLeft-_nHalf,_rcArea._nTop-_nHalf,_rcArea._nRight+_nHalf,_rcArea._nBottom+_nHalf),igIn);


    //compute magnitude and direction of edge
    int     i,j,r,c,ii,jj,rr,cc;
    double  fGradX,fGradY;
    double  dTmp;

    for(i=_nHalf,ii=0; ii<nLocalH; i++,ii++){
        for(j=_nHalf,jj=0; jj<nLocalW; j++,jj++)
        {
            //convolution
            fGradX = fGradY = 0.0;
            for(r=-_nHalf,rr=0; r<=_nHalf; r++,rr++)
                for(c=-_nHalf,cc=0; c<=_nHalf; c++,cc++){
                    fGradX += igIn._ppA[i+r][j+c] * _mKernelX._ppA[rr][cc];
                    fGradY += igIn._ppA[i+r][j+c] * _mKernelY._ppA[rr][cc];
                }

            //magnitude
            _idMag._ppA[ii][jj] = _ABS(fGradX) + _ABS(fGradY);

            //direction
            if(_idMag._ppA[ii][jj] > dLow)
            {
                dTmp 		   		=  (atan2(fGradY, fGradX) + _PI) * 180.0 / _PI ;
                _iwAng._ppA[ii][jj] = (unsigned short)(dTmp+0.5);               //0~360
                _igDir._ppA[ii][jj] = (unsigned char)((((int)(dTmp/22.5)+1)>>1) & 0x00000003); //0~3
            }
            else
                _idMag._ppA[ii][jj] = 0.0;
        }}

    //non-maxima suppression
    int   		nShiftX[4] = {-1,1,0,-1};
    int   		nShiftY[4] = {0,1,1,1};
    KEDGEPIXEL  oEdgePxl;

    for(ii=1; ii<nLocalH-1; ii++)
    {
        for(jj=1; jj<nLocalW-1; jj++)
        {
            if(_idMag._ppA[ii][jj] > _idMag._ppA[ii+nShiftY[ _igDir._ppA[ii][jj] ]][jj+nShiftX[ _igDir._ppA[ii][jj] ]] &&
                _idMag._ppA[ii][jj] > _idMag._ppA[ii-nShiftY[_igDir._ppA[ii][jj] ]][jj-nShiftX[_igDir._ppA[ii][jj] ]])
            {
                if(_idMag._ppA[ii][jj] > dHigh)
                {
                    oEdgePxl.u    = (unsigned short)(jj + _rcArea._nLeft);
                    oEdgePxl.v 	  = (unsigned short)(ii + _rcArea._nTop);
                    oEdgePxl.wAng = _iwAng._ppA[ii][jj];
                    oEdgePxl.wDir = _igDir._ppA[ii][jj];
                    oEdgePxl.dMag = _idMag._ppA[ii][jj];
                    _lDefinite.push_back(oEdgePxl);
                }
                _idBuf._ppA[ii][jj] = _idMag._ppA[ii][jj];
            }
        }}

    //hysteresis thresholding
    while(_lDefinite.empty() == false)
    {
        //pop out an edge pixel
        oEdgePxl = _lDefinite.back();
        _lDefinite.pop_back();

        //store the edge pixel
        vector<KEDGEPIXEL>::push_back(oEdgePxl);

        //get edge coordinate
        int jx  = oEdgePxl.u - _rcArea._nLeft;
        int iy  = oEdgePxl.v - _rcArea._nTop;

        //search neighbor edges
        for(i=-1; i<2; i++)
            for(j=-1; j<2; j++)
                if(_idBuf._ppA[iy+i][jx+j] && _idBuf._ppA[iy+i][jx+j]<=dHigh)
                {
                    oEdgePxl.u    = (unsigned short)(jx+j + _rcArea._nLeft);
                    oEdgePxl.v    = (unsigned short)(iy+i + _rcArea._nTop);
                    oEdgePxl.wAng = _iwAng._ppA[iy+i][jx+j];
                    oEdgePxl.wDir = _igDir._ppA[iy+i][jx+j];
                    oEdgePxl.dMag = _idBuf._ppA[iy+i][jx+j];

                    _lDefinite.push_back(oEdgePxl);
                    _idBuf._ppA[iy+i][jx+j] = 0.0;
                }
    }
}



void KEdge::Canny(double dLow,double dHigh,const KImageGray& igIn,KImageGray& igOut)
{
    //init. stack
    vector<KEDGEPIXEL>::clear();
    _lDefinite.clear();

    //set dimension of image
    _nWidth  = igIn.Col();
    _nHeight = igIn.Row();

    //init. mymories
    _idMag.Create(_nHeight,_nWidth);
    _iwAng.Create(_nHeight,_nWidth);
    _igDir.Create(_nHeight,_nWidth);
    _idBuf.Create(_nHeight,_nWidth);

    //compute magnitude and direction of edge
    int     i,j,r,c,rr,cc;
    double  dGradX,dGradY,dTmp;

    for(i=_nHalf; i<_nHeight-_nHalf; i++){
        for(j=_nHalf; j<_nWidth-_nHalf; j++)
        {
            //convolution
            dGradX = dGradY = 0.0;
            for(r=-_nHalf,rr=0; r<=_nHalf; r++,rr++)
                for(c=-_nHalf,cc=0; c<=_nHalf; c++,cc++){
                    dGradX += igIn._ppA[i+r][j+c] * _mKernelX._ppA[rr][cc];
                    dGradY += igIn._ppA[i+r][j+c] * _mKernelY._ppA[rr][cc];
                }

            //magnitude
            _idMag._ppA[i][j] = _ABS(dGradX) + _ABS(dGradY);

            //direction
            if(_idMag[i][j] > dLow)
            {
                dTmp 		   		=  (atan2(dGradY, dGradX) + _PI) * 180.0 / _PI ;
                _iwAng._ppA[i][j] = (unsigned short)(dTmp+0.5);               //0~360
                _igDir._ppA[i][j] = (unsigned char)((((int)(dTmp/22.5)+1)>>1) & 0x00000003); //0~3
            }
            else
                _idMag[i][j] = 0.0;
        }}

    //non-maxima suppression
    int         nShiftX[4] = {-1,1,0,-1};
    int         nShiftY[4] = {0,1,1,1};
    int         nH=_nHeight-_nHalf-1, nW=_nWidth-_nHalf-1;
    KEDGEPIXEL  oEdgePxl;

    for(i=_nHalf+1; i<nH; i++){
        for(j=_nHalf+1; j<nW; j++){
            if(_idMag[i][j] == 0.0) continue;

            if(_idMag._ppA[i][j] > _idMag._ppA[i+nShiftY[ _igDir._ppA[i][j] ]][j+nShiftX[ _igDir._ppA[i][j] ]] &&
                _idMag._ppA[i][j] > _idMag._ppA[i-nShiftY[_igDir._ppA[i][j] ]][j-nShiftX[_igDir._ppA[i][j] ]]){
                if(_idMag._ppA[i][j] > dHigh){
                    oEdgePxl.u    = (unsigned short)j;
                    oEdgePxl.v 	  = (unsigned short)i;
                    oEdgePxl.wAng = _iwAng._ppA[i][j];
                    oEdgePxl.wDir = _igDir._ppA[i][j];
                    oEdgePxl.dMag = _idMag._ppA[i][j];
                    _lDefinite.push_back(oEdgePxl);
                }
                _idBuf._ppA[i][j] = _idMag._ppA[i][j];
            }}}

    //hysteresis thresholding
    while(_lDefinite.empty() == false)
    {
        //pop out an edge pixel
        oEdgePxl = _lDefinite.back();
        _lDefinite.pop_back();

        //store the edge pixel
        vector<KEDGEPIXEL>::push_back(oEdgePxl);

        //get edge coordinate
        int jx  = oEdgePxl.u - _rcArea._nLeft;
        int iy  = oEdgePxl.v - _rcArea._nTop;

        //search neighbor edges
        for(i=-1; i<2; i++)
            for(j=-1; j<2; j++)
                if(_idBuf._ppA[iy+i][jx+j] && _idBuf._ppA[iy+i][jx+j]<=dHigh)
                {
                    oEdgePxl.u    = (unsigned short)(jx+j + _rcArea._nLeft);
                    oEdgePxl.v    = (unsigned short)(iy+i + _rcArea._nTop);
                    oEdgePxl.wAng = _iwAng._ppA[iy+i][jx+j];
                    oEdgePxl.wDir = _igDir._ppA[iy+i][jx+j];
                    oEdgePxl.dMag = _idBuf._ppA[iy+i][jx+j];

                    _lDefinite.push_back(oEdgePxl);
                    _idBuf._ppA[iy+i][jx+j] = 0.0;
                }
    }

    //output image
    igOut.Create(_nHeight,_nWidth);
    for(auto& item : *this)
    {
        oEdgePxl = item;
        igOut._ppA[oEdgePxl.v][oEdgePxl.u] = 255;
    }
}

void KEdge::Canny(double dLow,double dHigh,const KImageGray& igIn)
{
    //init. stack
    vector<KEDGEPIXEL>::clear();
    _lDefinite.clear();

    //set dimension of image
    _nWidth  = igIn.Col();
    _nHeight = igIn.Row();

    //init. mymories
    _idMag.Create(_nHeight,_nWidth);
    _iwAng.Create(_nHeight,_nWidth);
    _igDir.Create(_nHeight,_nWidth);
    _idBuf.Create(_nHeight,_nWidth);

    //compute magnitude and direction of edge
    int     i,j,r,c,rr,cc;
    double  dGradX,dGradY,dTmp;

    for(i=_nHalf; i<_nHeight-_nHalf; i++){
        for(j=_nHalf; j<_nWidth-_nHalf; j++)
        {
            //convolution
            dGradX = dGradY = 0.0;
            for(r=-_nHalf,rr=0; r<=_nHalf; r++,rr++)
                for(c=-_nHalf,cc=0; c<=_nHalf; c++,cc++){
                    dGradX += igIn._ppA[i+r][j+c] * _mKernelX._ppA[rr][cc];
                    dGradY += igIn._ppA[i+r][j+c] * _mKernelY._ppA[rr][cc];
                }

            //magnitude
            _idMag._ppA[i][j] = _ABS(dGradX) + _ABS(dGradY);

            //direction
            if(_idMag._ppA[i][j] > dLow)
            {
                dTmp              =  (atan2(dGradY, dGradX) + _PI) * 180.0 / _PI ;
                _iwAng._ppA[i][j] = (unsigned short)(dTmp+0.5);               //0~360
                _igDir._ppA[i][j] = (unsigned char)((((int)(dTmp/22.5)+1)>>1) & 0x00000003); //0~3
            }
            else
                _idMag._ppA[i][j] = 0.0;
        }}

    //non-maxima suppression
    int         nShiftX[4] = {1,1,0,-1};
    int         nShiftY[4] = {0,1,1,1};
    int         nH=_nHeight-_nHalf-1, nW=_nWidth-_nHalf-1;
    KEDGEPIXEL  oEdgePxl;

    for(i=_nHalf+1; i<nH; i++){
        for(j=_nHalf+1; j<nW; j++){
            if(_idMag._ppA[i][j] == 0.0) continue;

            if(_idMag._ppA[i][j] > _idMag._ppA[i+nShiftY[ _igDir._ppA[i][j] ]][j+nShiftX[ _igDir._ppA[i][j] ]] &&
                _idMag._ppA[i][j] > _idMag._ppA[i-nShiftY[_igDir._ppA[i][j] ]][j-nShiftX[_igDir._ppA[i][j] ]]){
                if(_idMag._ppA[i][j] > dHigh){
                    oEdgePxl.u    = (unsigned short)j;
                    oEdgePxl.v 	  = (unsigned short)i;
                    oEdgePxl.wAng = _iwAng._ppA[i][j];
                    oEdgePxl.wDir = _igDir._ppA[i][j];
                    oEdgePxl.dMag = _idMag._ppA[i][j];

                    _lDefinite.push_back(oEdgePxl);
                }
                _idBuf._ppA[i][j] = _idMag._ppA[i][j];
            }}}

    //hysteresis thresholding
    while(_lDefinite.empty() == false)
    {
        //pop out an edge pixel
        oEdgePxl = _lDefinite.back();
        _lDefinite.pop_back();

        //store the edge pixel
        vector<KEDGEPIXEL>::push_back(oEdgePxl);

        //get edge coordinate
        int jx  = oEdgePxl.u - _rcArea._nLeft;
        int iy  = oEdgePxl.v - _rcArea._nTop;

        //search neighbor edges
        for(i=-1; i<2; i++)
            for(j=-1; j<2; j++)
                if(_idBuf._ppA[iy+i][jx+j] && _idBuf._ppA[iy+i][jx+j]<=dHigh)
                {
                    oEdgePxl.u    = (unsigned short)(jx+j + _rcArea._nLeft);
                    oEdgePxl.v    = (unsigned short)(iy+i + _rcArea._nTop);
                    oEdgePxl.wAng = _iwAng._ppA[iy+i][jx+j];
                    oEdgePxl.wDir = _igDir._ppA[iy+i][jx+j];
                    oEdgePxl.dMag = _idBuf._ppA[iy+i][jx+j];

                    _lDefinite.push_back(oEdgePxl);
                    _idBuf._ppA[iy+i][jx+j] = 0.0;
                }
    }
}

void KEdge::Canny(double dLow,double dHigh,const KImageDouble& idIn,KImageGray& igOut)
{
    //init. stack
    vector<KEDGEPIXEL>::clear();
    _lDefinite.clear();

    //set dimension of image
    _nWidth  = idIn.Col();
    _nHeight = idIn.Row();

    //init. mymories
    _idMag.Create(_nHeight,_nWidth);
    _iwAng.Create(_nHeight,_nWidth);
    _igDir.Create(_nHeight,_nWidth);
    _idBuf.Create(_nHeight,_nWidth);

    //compute magnitude and direction of edge
    int     i,j,r,c,rr,cc;
    double  dGradX,dGradY,dTmp;

    for(i=_nHalf; i<_nHeight-_nHalf; i++)
    {
        for(j=_nHalf; j<_nWidth-_nHalf; j++)
        {
            //convolution
            dGradX = dGradY = 0.0;
            for(r=-_nHalf,rr=0; r<=_nHalf; r++,rr++)
                for(c=-_nHalf,cc=0; c<=_nHalf; c++,cc++){
                    dGradX += idIn._ppA[i+r][j+c] * _mKernelX._ppA[rr][cc];
                    dGradY += idIn._ppA[i+r][j+c] * _mKernelY._ppA[rr][cc];
                }

            //magnitude
            _idMag._ppA[i][j] = _ABS(dGradX) + _ABS(dGradY);

            //direction
            if(_idMag._ppA[i][j] > dLow)
            {
                dTmp 		   	  =  (atan2(dGradY, dGradX) + _PI) * 180.0 / _PI ;
                _iwAng._ppA[i][j] = (unsigned short)(dTmp+0.5);               //0~360
                _igDir._ppA[i][j] = (unsigned char)((((int)(dTmp/22.5)+1)>>1) & 0x00000003); //0~3
            }
            else
                _idMag._ppA[i][j] = 0.0;
        }
    }

    //non-maxima suppression
    int         nShiftX[4] = {1,1,0,-1};
    int         nShiftY[4] = {0,1,1,1};
    int         nH=_nHeight-_nHalf-1, nW=_nWidth-_nHalf-1;
    KEDGEPIXEL  oEdgePxl;

    for(i=_nHalf+1; i<nH; i++){
        for(j=_nHalf+1; j<nW; j++){
            if(_idMag[i][j] == 0.0) continue;

            if(_idMag._ppA[i][j] > _idMag._ppA[i+nShiftY[ _igDir._ppA[i][j] ]][j+nShiftX[ _igDir._ppA[i][j] ]] &&
                _idMag._ppA[i][j] > _idMag._ppA[i-nShiftY[_igDir._ppA[i][j] ]][j-nShiftX[_igDir._ppA[i][j] ]]){
                if(_idMag._ppA[i][j] > dHigh){
                    oEdgePxl.u    = (unsigned short)j;
                    oEdgePxl.v 	  = (unsigned short)i;
                    oEdgePxl.wAng = _iwAng._ppA[i][j];
                    oEdgePxl.wDir = _igDir._ppA[i][j];
                    oEdgePxl.dMag = _idMag._ppA[i][j];
                    _lDefinite.push_back(oEdgePxl);
                }
                _idBuf._ppA[i][j] = _idMag._ppA[i][j];
            }}}

    //hysteresis thresholding
    while(_lDefinite.empty() == false)
    {
        //pop out an edge pixel
        oEdgePxl = _lDefinite.back();
        _lDefinite.pop_back();

        //store the edge pixel
        vector<KEDGEPIXEL>::push_back(oEdgePxl);

        //get edge coordinate
        int jx  = oEdgePxl.u - _rcArea._nLeft;
        int iy  = oEdgePxl.v - _rcArea._nTop;

        //search neighbor edges
        for(i=-1; i<2; i++)
            for(j=-1; j<2; j++)
                if(_idBuf._ppA[iy+i][jx+j] && _idBuf._ppA[iy+i][jx+j]<=dHigh)
                {
                    oEdgePxl.u    = (unsigned short)(jx+j + _rcArea._nLeft);
                    oEdgePxl.v    = (unsigned short)(iy+i + _rcArea._nTop);
                    oEdgePxl.wAng = _iwAng._ppA[iy+i][jx+j];
                    oEdgePxl.wDir = _igDir._ppA[iy+i][jx+j];
                    oEdgePxl.dMag = _idBuf._ppA[iy+i][jx+j];

                    _lDefinite.push_back(oEdgePxl);
                    _idBuf._ppA[iy+i][jx+j] = 0.0;
                }
    }

    //output image
    igOut.Create(_nHeight,_nWidth);

    for(auto& item : *this)
    {
        oEdgePxl = item;
        igOut._ppA[oEdgePxl.v][oEdgePxl.u] = 255;
    }

}

void KEdge::DT(const double& dBW,KImageWord& iwDist,KImageWord& iwDir)
{
    //create a output object
    iwDist.Create(_nHeight,_nWidth);
    iwDir.Create(_nHeight,_nWidth);
    iwDist = (unsigned short)(0xFFFF);

    //
    KEDGEPIXEL 		oEdgePxl;
    int 			nSx,nSy,nEx,nEy;
    unsigned short 	wDist;
    double			dDist,dBW2 = _SQR(dBW);

    for(auto& item : *this)
    {
        oEdgePxl = item;

        nSy = (int)_MAX(oEdgePxl.v-dBW,0);
        nEy = (int)_MIN(oEdgePxl.v+dBW,_nHeight-1);
        nSx = (int)_MAX(oEdgePxl.u-dBW,0);
        nEx = (int)_MIN(oEdgePxl.u+dBW,_nWidth-1);

        for(int ic=nEy-nSy+1,i=nSy; ic; ic--,i++)
            for(int jc=nEx-nSx+1,j=nSx; jc; jc--,j++)
            {
                if((dDist=_SQR((i-oEdgePxl.v)) + _SQR((j-oEdgePxl.u))) > dBW2)
                    continue;
                if((wDist=(unsigned short)(dDist+0.5)) > iwDist._ppA[i][j])
                    continue;

                iwDist._ppA[i][j] =  wDist;
                iwDir._ppA[i][j]  =  (unsigned short)(oEdgePxl.wAng | (oEdgePxl.wDir<<14));
            }
    }
}
