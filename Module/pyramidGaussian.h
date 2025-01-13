//---------------------------------------------------------------------------
#ifndef pyramidGaussianH
#define pyramidGaussianH

#include "kfc.h"

class KPyramidGaussian : public std::vector<KImageDouble>
{
    int _nOctave;

public:
        KPyramidGaussian(){ };
        KPyramidGaussian(const KImageGray& igImg,double dSigma=0.0, int nOctave = 0){
            Create(igImg,dSigma, nOctave);
		};
		virtual ~KPyramidGaussian(){ };

        void    Create(const KImageGray& igImg,double dSigma=0.0, int nOctave = 0);

        int     Octave(){ return _nOctave; }
};

//---------------------------------------------------------------------------
#endif
