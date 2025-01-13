//---------------------------------------------------------------------------
#include "pyramidGaussian.h"

using namespace std;

void KPyramidGaussian::Create(const KImageGray& igImg,double dSigma, int nOctave)
{
	//1차원 가우시안 커널
	KVector	  vKernel = KGaussianMulti::Kernel_1D((dSigma == 0.0 ? 1.4 : dSigma));

    //최대 옥타브는 커널 크기의 7배이상이 되도록 설정함
    double  dK   = 7.0;
    double  dDim = _MIN(igImg.Col(),igImg.Row());
    int     nMax = (int)(log(dDim/(double)(vKernel.Dim()*dK))/log(2.0) + 1);

    _nOctave = (nOctave == 0 ? nMax : _MIN(nMax, nOctave));

	//Level 0(Otarve 0)의 원본 이미지
    KImageDouble idBase(igImg,_NO_SCALING);

    //Level 0 이미지 설정
    vector<KImageDouble>::clear();
    vector<KImageDouble>::push_back(idBase);

	//피라미드 생성
    for(int i=1; i < _nOctave; i++)
	{
		//create an image for level i
        KImageDouble idTmp;

        //Gaussian Smoothing
        vector<KImageDouble>::back().Convolution(vKernel,_X_AXIS,false).Convolution(vKernel,_Y_AXIS,idTmp,false);

        //add the subsampled image to the Gaussian pyramid
        vector<KImageDouble>::push_back(idTmp.HalfSize());
    }
}
//---------------------------------------------------------------------------
