#include "corner.h"

void KCorner::Create(const double& dSigma_a, const int& nBlockSize)
{
    //커널 크기
    int nHalf  	 = ( 3.0*dSigma_a < 1.0 ? 1 : (int)(3.0*dSigma_a+0.5) );
    _nKernelSize = nHalf*2 +1;

    _mKernelX.Create(_nKernelSize,_nKernelSize);
    _mKernelY.Create(_nKernelSize,_nKernelSize);

    //그래디언트 커널 생성
    int    i,j,ii,jj;
    double dTmp,dScale=0.0,dSigma2 = 2.0*_SQR(dSigma_a);

    for(i=-nHalf,ii=0; i<=nHalf; i++,ii++)
    {
        dTmp = -i*exp(-(i*i)/2./dSigma_a/dSigma_a);

        for(j=-nHalf,jj=0; j<=nHalf; j++,jj++)
        {
            _mKernelY[ii][jj] = dTmp*exp(-(j*j)/dSigma2);
            _mKernelX[jj][ii] = _mKernelY[ii][jj];
            dScale += (i<0 ? _mKernelY[ii][jj] : 0.0);
        }
    }
    for(i=0; i<_nKernelSize; i++)
        for(j=0; j<_nKernelSize; j++)
        {
            _mKernelY[i][j] /= -dScale;
            _mKernelX[i][j] /= -dScale;
        }

    //블럭 가중치 생성
    int     nHalfB   = nBlockSize/2;
    double  dSigma_b = nHalfB / 3.0;

    _nBlockSize = nHalfB*2 + 1;
    _mBlockWeight.Create(_nBlockSize,_nBlockSize);

    dSigma2 = 2.0*_SQR(dSigma_b);
    dScale  = 0.0;

    for(int i=0, ii=-nHalfB; ii<=nHalfB; i++,ii++)
    {
        for(int j=0, jj=-nHalfB; jj<=nHalfB; j++,jj++)
        {
            _mBlockWeight[i][j] = exp(-(ii*ii+jj*jj)/dSigma2);
            dScale     += _mBlockWeight[i][j];
        }
    }
    _mBlockWeight /= dScale;
}


void KCorner::HarrisCorner(const double& dThresh, const KImageGray& igIn)
{
    //그래디언트 계산
    KImageDouble idGradX, idGradY;
    KImageGray   igImg(igIn);

    igImg.Convolution(_mKernelX,idGradX);
    igImg.Convolution(_mKernelY,idGradY);

    int nHalf = _nKernelSize/2;
    int nRow  = igIn.Row(), nCol = igIn.Col();
    int nSx = nHalf, nSy = nHalf;
    int nEx = igIn.Col() - nHalf, nEy = igIn.Row() - nHalf;

    _idGradX2.Create(nRow, nCol);
    _idGradY2.Create(nRow, nCol);
    _idGradXY.Create(nRow, nCol);

    for(int i=nSy; i<nEy; i++ )
        for(int j=nSx; j<nEx; j++)
        {
            _idGradX2[i][j] = idGradX[i][j] * idGradX[i][j];
            _idGradY2[i][j] = idGradY[i][j] * idGradY[i][j];
            _idGradXY[i][j] = idGradX[i][j] * idGradY[i][j];
        }

    //Corner Response 계산
    KMatrix mM(2,2), mU,mV;
    KVector vW;
    int     nHalfB = _nBlockSize / 2;

    _idR.Create(nRow, nCol);

    nSx = nSy =  nHalf + nHalfB;
    nEx = igIn.Col() - nHalf - nHalfB;
    nEy = igIn.Row() - nHalf - nHalfB;

    for(int i=nSy; i<nEy; i++)
        for(int j=nSx; j<nEx; j++)
        {
            //autocorrelation matrix
            mM = 0.0;
            for(int r=-nHalfB, rr=0; r<=nHalfB; r++,rr++)
                for(int c=-nHalfB, cc=0; c<=nHalfB; c++,cc++)
                {
                    mM[0][0] += _idGradX2[i+r][j+c] * _mBlockWeight[rr][cc];
                    mM[1][1] += _idGradY2[i+r][j+c] * _mBlockWeight[rr][cc];
                    mM[0][1] += _idGradXY[i+r][j+c] * _mBlockWeight[rr][cc];
                }
            mM[1][0] = mM[0][1];

            //corner response
            mM.SVD(mU,vW,mV);
            _idR[i][j] = vW[0]*vW[1] - 0.04*_SQR(vW[0] + vW[1]);
        }

    //nonmaxima suppression
    KCORNERPIXEL oPixel;
    nSx++; nSy++;
    nEx--; nEy--;

    vector<KCORNERPIXEL>::clear();

    for(int i=nSy; i<nEy; i++)
        for(int j=nSx; j<nEx; j++)
        {
            if(_idR[i][j] < dThresh || _idR[i][j] < _idR[i-1][j-1] || _idR[i][j] < _idR[i][j-1] || _idR[i][j] < _idR[i+1][j-1] ||
                _idR[i][j] < _idR[i-1][j] || _idR[i][j] < _idR[i+1][j] ||
                _idR[i][j] < _idR[i-1][j+1] || _idR[i][j] < _idR[i][j+1] || _idR[i][j] < _idR[i+1][j+1])
                continue;

            oPixel.u   = j;
            oPixel.v   = i;
            oPixel.dR = _idR[i][j];
            vector<KCORNERPIXEL>::push_back(oPixel);
        }
}

//void KCorner::HarrisCorner2(const double& dThresh, const KImageGray& igIn, const KMatrix& mPca, const KVector& vMean)
//{
//    //gradients
//    KImageDouble idGradX, idGradY;
//    KImageGray   igImg(igIn);

//    igImg.Convolution(_mKernelX,idGradX);
//    igImg.Convolution(_mKernelY,idGradY);

//    int nHalf = _nKernelSize / 2;
//    int nRow  = igIn.Row(), nCol = igIn.Col();
//    int nSx = nHalf, nSy = nHalf;
//    int nEx = igIn.Col() - nHalf, nEy = igIn.Row() - nHalf;

//    _idGradX2.Create(nRow, nCol);
//    _idGradXY.Create(nRow, nCol);
//    _idGradY2.Create(nRow, nCol);

//    for(int i=nSy; i<nEy; i++ )
//        for(int j=nSx; j<nEx; j++)
//        {
//            _idGradX2[i][j] = idGradX[i][j] * idGradX[i][j];
//            _idGradXY[i][j] = idGradX[i][j] * idGradY[i][j];
//            _idGradY2[i][j] = idGradY[i][j] * idGradY[i][j];
//        }

//    //compute Response
//    KMatrix mM(2,2), mU,mV;
//    KVector vW;
//    int     nHalfB = _nBlockSize / 2;

//    _idR.Create(nRow, nCol);

//    nSx = nSy =  nHalf + nHalfB;
//    nEx = nCol - nHalf - nHalfB;
//    nEy = nRow - nHalf - nHalfB;

//    for(int i=nSy; i<nEy; i++)
//        for(int j=nSx; j<nEx; j++)
//        {
//            //autocorrelation matrix
//            mM = 0.0;

//            for(int r=-nHalfB, rr=0; r<=nHalfB; r++,rr++)
//                for(int c=-nHalfB, cc=0; c<=nHalfB; c++,cc++)
//                {
//                    mM[0][0] += _idGradX2[i+r][j+c] * _mBlockWeight[rr][cc];
//                    mM[1][1] += _idGradY2[i+r][j+c] * _mBlockWeight[rr][cc];
//                    mM[0][1] += _idGradXY[i+r][j+c] * _mBlockWeight[rr][cc];
//                }
//            mM[1][0] = mM[0][1];

//            //response
//            mM.SVD(mU,vW,mV);
//            _idR[i][j] = vW[0]*vW[1] - 0.04*_SQR(vW[0] + vW[1]);
//        }

//    //nonmaxima suppression
//    KCORNERPIXEL oCornerPixel;
//    KVector      vPatch(39*39*2);

//    nSx++; nSy++;
//    nEx--; nEy--;

//    vector<KCORNERPIXEL>::clear();

//    for(int i=nSy; i<nEy; i++)
//        for(int j=nSx; j<nEx; j++)
//        {
//            if(_idR[i][j] < dThresh ||  _idR[i][j] < _idR[i-1][j-1] || _idR[i][j] < _idR[i][j-1] || _idR[i][j] < _idR[i+1][j-1] ||
//                _idR[i][j] < _idR[i-1][j] || _idR[i][j] < _idR[i+1][j] ||
//                _idR[i][j] < _idR[i-1][j+1] || _idR[i][j] < _idR[i][j+1] || _idR[i][j] < _idR[i+1][j+1])
//                continue;

//            if(i < 20 || j < 20 || i+21 > igIn.Row() || j+21 > igIn.Col())
//                continue;

//            int nCnt = 0;

//            for(int iy = i-19; iy <= i+19; iy++)
//            {
//                for(int jx = j-19; jx <= j+19; jx++)
//                {
//                    vPatch[nCnt]     = igImg[iy][jx+1] - igImg[iy][jx-1];
//                    vPatch[nCnt + 1] = igImg[iy+1][jx] - igImg[iy-1][jx];
//                    nCnt            += 2;
//                }
//            }

//            //normalize vPatch
//            double  dMean = 0.0;
//            for(int iy=0; iy<vPatch.Dim(); iy++)
//                dMean += _ABS(vPatch[iy]);
//            dMean /= (double)(vPatch.Dim());

//            for(int iy=0; iy<vPatch.Dim(); iy++)
//                vPatch[iy] *= (100.0/dMean);


//            oCornerPixel.u  = j;
//            oCornerPixel.v  = i;
//            oCornerPixel.dR = _idR[i][j];
//            oCornerPixel.vD = mPca * (vPatch - vMean);

//            vector<KCORNERPIXEL>::push_back(oCornerPixel);
//        }
//}
