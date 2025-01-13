#include "mainframe.h"
#include "ui_mainframe.h"
#include "imageform.h"
#include "edge.h"
#include "corner.h"
#include "circleHough.h"
#include "opticalFlow.h"
#include "pyramidGaussian.h"

#include <QFileDialog>
#include <QPainter>
#include <sstream>
#include <fstream>
#include <random>
#include <QtDebug>

using namespace std;

MainFrame::MainFrame(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MainFrame)
{
    ui->setupUi(this);

    _q_pFormFocused     = 0;

    //객체 맴버의 초기화


    //리스트 출력창을 안보이게    
    ui->listWidget->setVisible(false);
    this->adjustSize();

    //UI 활성화 갱신
    UpdateUI();
}

MainFrame::~MainFrame()
{ 
    delete ui;         

    for(auto& item : _lImageForm)
        delete item;

}

void MainFrame::CloseImageForm(ImageForm *pForm)
{
    //ImageForm 포인터 삭제
    unsigned int idx = std::find(_lImageForm.begin(), _lImageForm.end(), pForm) - _lImageForm.begin();
    if(idx != _lImageForm.size())
    {
        delete _lImageForm[idx];
        _lImageForm.erase(_lImageForm.begin() + idx);
    }

    //활성화 ImageForm 초기화
    _q_pFormFocused     = 0;

    //관련 객체 삭제

    //UI 활성화 갱신
    UpdateUI();
}

void MainFrame::UpdateUI()
{    
    if(ui->tabWidget->currentIndex() == 0)
    {
        ui->pushRGB2Gray->setEnabled( _q_pFormFocused && _q_pFormFocused->ID() == "OPEN");
    }
    else if(ui->tabWidget->currentIndex() == 1)
    {  
        ui->pushSepiaTone->setEnabled( _q_pFormFocused && _q_pFormFocused->ID() == "OPEN");
        ui->pushContrast->setEnabled( _q_pFormFocused && _q_pFormFocused->ID() == "OPEN");
        ui->pushHistogram->setEnabled( _q_pFormFocused && (_q_pFormFocused->ID() == "OPEN" || _q_pFormFocused->ID() == "Gray Image") );
    }
    else if(ui->tabWidget->currentIndex() == 2)
    {        
        //조건에 맞으면 버튼 활성화
        ui->pushOstu->setEnabled( _q_pFormFocused && _q_pFormFocused->ID() == "OPEN" && _q_pFormFocused->ImageGray().Address());
        ui->pushThresholding->setEnabled( _q_pFormFocused && _q_pFormFocused->ID() == "OPEN" && _q_pFormFocused->ImageGray().Address());
        ui->pushDilation->setEnabled( _q_pFormFocused && _q_pFormFocused->ID() == "Binary Image" && _q_pFormFocused->ImageGray().Address());
        ui->pushErosion->setEnabled( _q_pFormFocused && _q_pFormFocused->ID() == "Binary Image" && _q_pFormFocused->ImageGray().Address());
        ui->pushBoundary->setEnabled( _q_pFormFocused && _q_pFormFocused->ID() == "Binary Image" && _q_pFormFocused->ImageGray().Address());
        ui->pushBoundary_FG->setEnabled( _q_pFormFocused && _q_pFormFocused->ID() == "Binary Image" && _q_pFormFocused->ImageGray().Address());
    }
    else if(ui->tabWidget->currentIndex() == 3)
    {
        ui->pushGaussianNoise->setEnabled( _q_pFormFocused && _q_pFormFocused->ID() == "OPEN" && _q_pFormFocused->ImageGray().Address());
        ui->pushSaltPepper->setEnabled( _q_pFormFocused && _q_pFormFocused->ID() == "OPEN" && _q_pFormFocused->ImageGray().Address());
        ui->pushGaussianSmoothing->setEnabled( _q_pFormFocused && _q_pFormFocused->ID() == "Noised Image" && _q_pFormFocused->ImageGray().Address());
        ui->pushMedianFiltering->setEnabled( _q_pFormFocused && _q_pFormFocused->ID() == "Noised Image" && _q_pFormFocused->ImageGray().Address());
    }
    else if(ui->tabWidget->currentIndex() == 4)
    {
        ui->pushHistogramEQ->setEnabled( _q_pFormFocused && _q_pFormFocused->ID() == "OPEN" && _q_pFormFocused->ImageGray().Address());
    }
    else if(ui->tabWidget->currentIndex() == 5)
    {
        ui->pushHarrisCorner->setEnabled( _q_pFormFocused && _q_pFormFocused->ID() == "OPEN" && _q_pFormFocused->ImageGray().Address());
        ui->pushCircleLocalization->setEnabled( _q_pFormFocused && _q_pFormFocused->ID() == "OPEN" && _q_pFormFocused->ImageGray().Address());
    }
    else if(ui->tabWidget->currentIndex() == 6)
    {
        ui->pushPyramid->setEnabled( _q_pFormFocused && _q_pFormFocused->ID() == "OPEN" && _q_pFormFocused->ImageGray().Address());
        ui->pushOpticalFlow->setEnabled(_opOpticalFlow == 0);
        ui->pushOpticalFlowRun->setEnabled(_opOpticalFlow != 0);
        ui->editWindowFlow->setEnabled(_opOpticalFlow == 0);
    }
}

void MainFrame::OnMousePos(const int &nX, const int &nY, ImageForm* q_pForm)
{
}

void MainFrame::focusInEvent(QFocusEvent * event)
{
    Q_UNUSED(event) ;

    UpdateUI();
}

void MainFrame::closeEvent(QCloseEvent* event)
{
    //생성된 ImageForm을 닫는다.
    for(int i=0; i< _lImageForm.size(); i++)
        delete _lImageForm[i];

    //리스트에서 삭제한다.
    _lImageForm.clear();
}


void MainFrame::on_buttonOpen_clicked()
{
    //이미지 파일 선택
    QFileDialog::Options    q_Options   =  QFileDialog::DontResolveSymlinks  | QFileDialog::DontUseNativeDialog; // | QFileDialog::ShowDirsOnly
    QString                 q_stFile    =  QFileDialog::getOpenFileName(this, tr("Select a Image File"),  "./data", "Image Files(*.bmp *.ppm *.pgm *.png)",0, q_Options);

    if(q_stFile.length() == 0)
        return;

    //이미지 출력을 위한 ImageForm 생성    
    ImageForm*              q_pForm   = new ImageForm(q_stFile, "OPEN", this);

    _lImageForm.push_back(q_pForm);
    q_pForm->show();

    //UI 활성화 갱신
    UpdateUI();
}

void MainFrame::on_buttonDeleteContents_clicked()
{
    //생성된 ImageForm을 닫는다.
    for(int i=_lImageForm.size()-1; i>=0; i--)
        delete _lImageForm[i];

    //리스트에서 삭제한다.
    _lImageForm.clear();

    //객체 삭제


    ui->listWidget->clear();
}

void MainFrame::on_tabWidget_currentChanged(int index)
{
    static int nOld = -1;

    if(nOld == 0)
    {

    }
    else if(nOld == 1)
    {

    }
    nOld = index;

    //UI 활성화 갱신
    UpdateUI();
}

void MainFrame::on_buttonShowList_clicked()
{
    static int nWidthOld = ui->tabWidget->width();

    if(ui->listWidget->isVisible())
    {
        nWidthOld = ui->listWidget->width();
        ui->listWidget->hide();
        this->adjustSize();
    }
    else
    {        
        ui->listWidget->show();
        QRect q_rcWin = this->geometry();

        this->setGeometry(q_rcWin.left(), q_rcWin.top(), q_rcWin.width()+nWidthOld, q_rcWin.height());
    }
}

void MainFrame::on_pushRGB2Gray_clicked()
{
    //포커스 된 ImageForm으로부터 영상을 가져옴
    KImageColor icMain;
    if(_q_pFormFocused != 0 && _q_pFormFocused->ImageColor().Address() && _q_pFormFocused->ID() == "OPEN")
        icMain = _q_pFormFocused->ImageColor();
    else
        return;

    //RGB to Gray
    KImageGray igImg = icMain.ColorToGray();

    //출력을 위한 ImageForm 생성
    ImageForm* q_pForm = new ImageForm(igImg, "Gray Image", this);

    //출력할 포인터 저장
    _lImageForm.push_back(q_pForm);
    q_pForm->show();
}


void MainFrame::on_pushOstu_clicked()
{
    //Ostu thresholding
    KImageGray igMain = _q_pFormFocused->ImageGray();
    KImageGray igBin;
    KBINARIZATION_OUTPUT* opBinOutput = KHisto().Ostu(&igMain,&igBin);

    //출력창 결정
    ImageForm* q_pForm = 0;
    for(auto item : _lImageForm)
        if(item->ID() == "Binary Image" && item->size() == _q_pFormFocused->size())
        {
            q_pForm = item;
            break;
        }

    //기존 창에 표시
    if(q_pForm)
        q_pForm->Update(igBin);
    //새로운 창에 표시
    else
    {
        q_pForm = new ImageForm(igBin, "Binary Image", this);
        q_pForm->show();
        _lImageForm.push_back(q_pForm);
    }

    //Threshold 값 표시
    ui->spinOstu->setValue(opBinOutput->nThresh);
    ui->listWidget->addItem(QString(">> Ostu Threshold : %1").arg(opBinOutput->nThresh));
    ui->listWidget->show();

    //UI 활성화 갱신
    UpdateUI();
}

void MainFrame::on_pushThresholding_clicked()
{
    //Thresholding 적용
    KImageGray igMain = _q_pFormFocused->ImageGray();
    igMain.Thresholded(ui->spinOstu->value());

    // 출력창 결정
    ImageForm* q_pForm = 0;
    for (auto item : _lImageForm)
    {
        if (item->ID() == "Binary Image" && item->size() == _q_pFormFocused->size())
        {
            q_pForm = item;
            break;
        }
    }

    // 기존 창에 표시
    if (q_pForm)
        q_pForm->Update(igMain);
    // 새로운 창에 표시
    else
    {
        q_pForm = new ImageForm(igMain, "Binary Image", this);
        q_pForm->show();
        _lImageForm.push_back(q_pForm);
    }

    //Threshold 값 표시
    ui->listWidget->addItem(QString(">> Manual Threshold : %1").arg(ui->spinOstu->value()));
    ui->listWidget->show();

    //UI 활성화 갱신
    UpdateUI();

}

void MainFrame::on_pushSepiaTone_clicked()
{
    //포커스 된 ImageForm으로부터 영상을 가져옴
    KImageColor icMain;
    if(_q_pFormFocused != 0 && _q_pFormFocused->ImageColor().Address() && _q_pFormFocused->ID() == "OPEN")
        icMain = _q_pFormFocused->ImageColor();
    else
        return;

    //RGB -> HSV 변환
    KArray<KHSV> arHSV;
    icMain.RGBtoHSV(arHSV);

    //설정된 Hue, Saturation으로 변경
    double dHue = ui->spinHue->value();
    double dSat = ui->spinSat->value();

    for(unsigned int i=0; i<arHSV.Row();i++)
        for(unsigned int j=0;j<arHSV.Col();j++)
        {
            arHSV[i][j].h = dHue;
            arHSV[i][j].s = dSat;
        }

    //HSV -> RGB 변환
    icMain.FromHSV(arHSV);

    //출력창 결정
    ImageForm* q_pForm = 0;
    for(auto form : _lImageForm)
        if(form->ID() == "Sepia Tone" && form->size() == _q_pFormFocused->size())
        {
            q_pForm = form;
            break;
        }

    //기존 창에 표시
    if(q_pForm)
        q_pForm->Update(icMain);

    //새로운 창에 표시
    else
    {
        q_pForm = new ImageForm(icMain, "Sepia Tone", this);
        q_pForm->show();
        _lImageForm.push_back(q_pForm);
    }

    //UI 활성화 갱신
    UpdateUI();
}

void MainFrame::on_pushContrast_clicked()
{
    //포커스 된 ImageForm으로부터 영상을 가져옴
    if(_q_pFormFocused == 0 || _q_pFormFocused->ImageGray().Address() == 0 || _q_pFormFocused->ID() != "OPEN")
        return;

    //포커스 된 ImageForm의 영상을 Contrast Transform
    KImageGray igImg = _q_pFormFocused->ImageGray().ContrastTransform(ui->spinContrastA->value(),
                                                                      ui->spinContrastB->value(),0,255);

    //출력창 결정
    ImageForm* q_pForm = 0;
    for(auto item : _lImageForm)
        if(item->ID() == "Contrast Transform" && item->size() == _q_pFormFocused->size())
        {
            q_pForm = item;
            break;
        }

    //기존 창에 표시
    if(q_pForm)
        q_pForm->Update(igImg);

    //새로운 창에 표시
    else
    {
        q_pForm = new ImageForm(igImg, "Contrast Transform", this);
        q_pForm->show();
        _lImageForm.push_back(q_pForm);
    }

    //UI 활성화 갱신
    UpdateUI();
}

void MainFrame::on_pushHistogram_clicked()
{
    //포커스 된 ImageForm으로부터 영상을 가져옴
    KImageGray igMain;
    if(_q_pFormFocused != 0 && _q_pFormFocused->ImageGray().Address() && _q_pFormFocused->ID() == "OPEN")
        igMain = _q_pFormFocused->ImageGray();
    else
        return;

    //Histogram을 구함
    vector<int> lHisto(256);
    int         nRow = igMain.Row();
    int         nCol = igMain.Col();

    for(int i=0; i<nRow; i++)
        for(int j=0; j<nCol; j++)
            lHisto[ igMain[i][j] ] ++;

    //파일에 저장
    ofstream fout("./output/histo.csv");
    for(auto item : lHisto)
        fout << item << ',';

    //결과 표시
    if(ui->listWidget->isVisible() == false)
        on_buttonShowList_clicked();
    ui->listWidget->addItem(QString(">> finish histogramming --> histo.csv"));
}



void MainFrame::on_pushDilation_clicked()
{
    //Dilation

    KImageGray igBin = _q_pFormFocused->ImageGray().BinaryDilate(_WHITE);

    //출력창 결정 - 기존에 있는 창 활용하도록 설정
    ImageForm* q_pForm = 0;
    for(auto item : _lImageForm)
        if(item->ID() == "Binary Image" && item->size() == _q_pFormFocused->size())
        {
            q_pForm = item;
            break;
        }

    //기존 창에 출력
    if(q_pForm)
        q_pForm->Update(igBin);

    //새로운 창에 표시
    else
    {
        q_pForm = new ImageForm(igBin, "Binary Image", this);
        q_pForm->show();
        _lImageForm.push_back(q_pForm);
    }

    //UI 활성화 갱신
    UpdateUI();
}

void MainFrame::on_pushErosion_clicked()
{
    //Erosion
    KImageGray igBin = _q_pFormFocused->ImageGray().BinaryErode(_WHITE);

    //출력창 결정 - 기존에 있는 창 활용하도록 설정
    ImageForm* q_pForm = 0;
    for(auto item : _lImageForm)
        if(item->ID() == "Binary Image" && item->size() == _q_pFormFocused->size())
        {
            q_pForm = item;
            break;
        }

    //기존 창에 출력
    if(q_pForm)
        q_pForm->Update(igBin);

    //새로운 창에 표시
    else
    {
        q_pForm = new ImageForm(igBin, "Binary Image", this);
        q_pForm->show();
        _lImageForm.push_back(q_pForm);
    }

    //UI 활성화 갱신
    UpdateUI();
}

void MainFrame::on_pushBoundary_clicked()
{
    //Dilation
    KImageGray igBin = _q_pFormFocused->ImageGray();
    KImageGray igDilate = igBin.BinaryDilate(_WHITE);

    //Boundary Extraction
    for(unsigned int i=0; i<igDilate.Row(); i++)
        for(unsigned int j=0; j<igDilate.Col(); j++)
            if(igDilate[i][j] == _BLACK)
                continue;
            else if(igBin[i][j])
                igDilate[i][j] = _BLACK;

    //출력창 결정
    ImageForm* q_pForm = 0;
    for(auto item : _lImageForm)
        if(item->ID() == "Binary Image" && item->size() == _q_pFormFocused->size())
        {
            q_pForm = item;
            break;
        }

    //기존 창에 표시
    if(q_pForm)
        q_pForm->Update(igDilate);

    //새로운 창에 표시
    else
    {
        q_pForm = new ImageForm(igDilate, "Binary Image", this);
        q_pForm->show();
        _lImageForm.push_back(q_pForm);
    }

    //UI 활성화 갱신
    UpdateUI();
}

void MainFrame::on_pushBoundary_FG_clicked()
{
    //Dilation
    KImageGray igBin = _q_pFormFocused->ImageGray();
    KImageGray igErode = igBin.BinaryErode(_WHITE);

    //Boundary Extraction
    for(unsigned int i=0; i<igErode.Row(); i++)
        for(unsigned int j=0; j<igErode.Col(); j++)
            if(igBin[i][j] == _BLACK)
                continue;
            else if(igErode[i][j] == _BLACK)
                igErode[i][j] = igBin[i][j];
            else
            {
                igErode[i][j] = _BLACK;
            }

    //출력창 결정
    ImageForm* q_pForm = 0;
    for(auto item : _lImageForm)
        if(item->ID() == "Binary Image" && item->size() == _q_pFormFocused->size())
        {
            q_pForm = item;
            break;
        }

    //기존 창에 표시
    if(q_pForm)
        q_pForm->Update(igErode);

    //새로운 창에 표시
    else
    {
        q_pForm = new ImageForm(igErode, "Binary Image", this);
        q_pForm->show();
        _lImageForm.push_back(q_pForm);
    }

    //UI 활성화 갱신
    UpdateUI();
}

void MainFrame::on_pushGaussianNoise_clicked()
{
    //입력 영상
    KImageGray igMain = _q_pFormFocused->ImageGray();

    //가우시안 노이즈
    double dSigma = ui->editSigmaNoise->text().toDouble();
    double dIntensity;
    random_device rd;
    mt19937 gen(rd());
    normal_distribution<double> dist(0.0,dSigma);

    for(unsigned int i=0; i<igMain.Row(); i++)
        for(unsigned int j=0; j<igMain.Col(); j++)
        {
            dIntensity = igMain[i][j] + dist(gen);
            igMain[i][j] = (unsigned char)(dIntensity < 0.0 ? 0 :
                                                              (dIntensity > 255.0 ? 255 : dIntensity));
        }

    //출력창 결정
    ImageForm* q_pForm = 0;
    for(auto item : _lImageForm)
        if(item->ID() == "Noised Image" && item->size() == _q_pFormFocused->size())
        {
            q_pForm = item;
            break;
        }

    //기존 창에 표시
    if(q_pForm)
        q_pForm->Update(igMain);

    //새로운 창에 표시
    else
    {
        q_pForm = new ImageForm(igMain, "Noised Image", this);
        q_pForm->show();
        _lImageForm.push_back(q_pForm);
    }

    //UI 활성화 갱신
    UpdateUI();
}

void MainFrame::on_pushSaltPepper_clicked()
{
    //입력 영상
    KImageGray igMain = _q_pFormFocused->ImageGray();

    //Salt & Pepper 노이즈
    double dTau = ui->editTau->text().toDouble();
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<double> dist(0.0,1.0);
    double dRandom;

    for(unsigned int i=0; i<igMain.Row(); i++)
        for(unsigned int j=0; j<igMain.Col(); j++)
        {
            dRandom = dist(gen);
            if(dRandom < dTau)
                igMain[i][j] = 0;
            else if(dRandom > 1.0-dTau)
                igMain[i][j] = 255;
        }

    //출력창 결정
    ImageForm* q_pForm = 0;
    for(auto item : _lImageForm)
        if(item->ID() == "Noised Image" && item->size() == _q_pFormFocused->size())
        {
            q_pForm = item;
            break;
        }

    //기존 창에 표시
    if(q_pForm)
        q_pForm->Update(igMain);

    //새로운 창에 표시
    else
    {
        q_pForm = new ImageForm(igMain, "Noised Image", this);
        q_pForm->show();
        _lImageForm.push_back(q_pForm);
    }

    //UI 활성화 갱신
    UpdateUI();
}

void MainFrame::on_pushGaussianSmoothing_clicked()
{
    //입력 영상
    KImageGray igMain = _q_pFormFocused->ImageGray();

    //Gaussian Smoothing
    double dSigma = ui->editSigmaFiltering->text().toDouble();
    igMain.GaussianSmoothed(dSigma);

    //출력창 결정
    ImageForm* q_pForm = 0;
    for(auto item : _lImageForm)
        if(item->ID() == "Gaussian Smoothed Image" && item->size() == _q_pFormFocused->size())
        {
            q_pForm = item;
            break;
        }

    //기존 창에 표시
    if(q_pForm)
        q_pForm->Update(igMain);

    //새로운 창에 표시
    else
    {
        q_pForm = new ImageForm(igMain, "Gaussian Smoothed Image", this);
        q_pForm->show();
        _lImageForm.push_back(q_pForm);
    }

    //UI 활성화 갱신
    UpdateUI();
}

void MainFrame::on_pushMedianFiltering_clicked()
{
    //입력 영상
    KImageGray igMain = _q_pFormFocused->ImageGray();

    //Median Filtering
    int nWindow = ui->editWindowSize->text().toInt();
    igMain.MedianFiltered(nWindow);

    //출력창 결정
    ImageForm* q_pForm = 0;
    for(auto item : _lImageForm)
        if(item->ID() == "Median Filtered Image" && item->size() == _q_pFormFocused->size())
        {
            q_pForm = item;
            break;
        }

    //기존 창에 표시
    if(q_pForm)
        q_pForm->Update(igMain);

    //새로운 창에 표시
    else
    {
        q_pForm = new ImageForm(igMain, "Median Filtered Image", this);
        q_pForm->show();
        _lImageForm.push_back(q_pForm);
    }

    //UI 활성화 갱신
    UpdateUI();
}

void MainFrame::on_pushHistogramEQ_clicked()
{
    //입력 영상
    KImageGray igMain = _q_pFormFocused->ImageGray();

    //Histograming
    KHisto oHisto;
    oHisto.Histogram(igMain);

    //파일에 저장
    ofstream fout("./output/hist_before.csv");
    for(int i=0; i<oHisto.Dim(); i++)
        fout << oHisto[i] << ',';
    fout.close();

    //Histogram Equaliztion
    oHisto.HEQ(igMain, true);

    //파일에 저장
    fout.open("./output/hist_after.csv");
    for(int i=0; i<oHisto.Dim(); i++)
        fout << oHisto[i] << ',';
    fout .close();

    //출력창 결정
    ImageForm* q_pForm = 0;
    for(auto item : _lImageForm)
        if(item->ID() == "Histogram Equalized" && item->size() == _q_pFormFocused->size())
        {
            q_pForm = item;
            break;
        }

    //기존 창에 표시
    if(q_pForm)
        q_pForm->Update(igMain);
    //새로운 창에 표시
    else
    {
        q_pForm = new ImageForm(igMain, "Histogram Equalized", this);
        q_pForm->show();
        _lImageForm.push_back(q_pForm);
    }

    //UI 활성화 갱신
    UpdateUI();
}

void MainFrame::on_pushHistogramMatching_clicked()
{
    //목표 이미지 파일 선택
    QFileDialog::Options q_Options = QFileDialog::DontResolveSymlinks   |
                                     QFileDialog::DontUseNativeDialog;
    QString              q_stFile  = QFileDialog::getOpenFileName(this,
                                       tr("Select a Target Image"), "./data",
                                       "Image Files(*.ppm *.pgm *.tif)",0, q_Options);
    if(q_stFile.length() == 0)
        return;


    ImageForm*  q_pFormT = new ImageForm(q_stFile, "Target Image", this);
    KImageColor  icTarget = q_pFormT->ImageColor();

    _lImageForm.push_back(q_pFormT);
    q_pFormT->show();

    //소스 이미지 파일 선택
    q_stFile = QFileDialog::getOpenFileName(this, tr("Select a Source Image"),
                                            "./data", "Image Files(*.bmp *.ppm *.pgm *.tif)",
                                            0, q_Options);
    if(q_stFile.length() == 0)
        return;

    ImageForm*  q_pFormS = new ImageForm(q_stFile, "Source Image", this);
    KImageColor icSrc    = q_pFormS->ImageColor();

    _lImageForm.push_back(q_pFormS);
    q_pFormS->show();

    //Histogram Matching
    KHisto      hMatch;
    KImageColor icMatched = hMatch.HistogramMatching(icTarget, icSrc);

    //결과 표시
    ImageForm* q_pForm = new ImageForm(icMatched, "Histogram Matched", this);

    _lImageForm.push_back(q_pForm);
    q_pForm->show();
}

void MainFrame::on_pushEdgeFDG_clicked()
{
    //입력 영상
    KImageGray igMain = _q_pFormFocused->ImageGray();

    //FDG
    KImageGray igMag;
    double     dSigma = ui->editSigmaFDG->text().toDouble();
    KEdge      oEdge(dSigma);

    oEdge.Gradient(igMain, igMag);

    //출력창 결정
    ImageForm* q_pForm = 0;
    for(auto item : _lImageForm)
        if(item->ID() == "Egde Magnitude" && item->size() == _q_pFormFocused->size())
        {
            q_pForm = item;
            break;
        }
    //기존 창에 표시
    if(q_pForm)
        q_pForm->Update(igMag);
    //새로운 창에 표시
    else
    {
        q_pForm = new ImageForm(igMag, "Egde Magnitude", this);
        q_pForm->show();
        _lImageForm.push_back(q_pForm);
    }
    //UI 활성화 갱신
    UpdateUI();
}

void MainFrame::on_pushCanny_clicked()
{
    //입력 영상
    KImageGray igMain = _q_pFormFocused->ImageGray();

    //Canny Operator
    KImageGray igEdge;
    double dSigma    = ui->editSigmaCanny->text().toDouble();
    double dLow      = ui->editLow->text().toDouble();
    double dHigh     = ui->editHigh->text().toDouble();
    KEdge oEdge(dSigma);

    oEdge.Canny(dLow, dHigh, igMain, igEdge);

    //출력창 결정
    ImageForm* q_pForm = 0;
    for(auto item : _lImageForm)
        if(item->ID() == "Egde Image" && item->size() == _q_pFormFocused->size())
        {
            q_pForm = item;
            break;
        }
    //기존 창에 표시
    if(q_pForm)
        q_pForm->Update(igEdge);
    //새로운 창에 표시
    else
    {
        q_pForm = new ImageForm(igEdge, "Egde Image", this);
        q_pForm->show();
        _lImageForm.push_back(q_pForm);
    }
    //UI 활성화 갱신
    UpdateUI();
}


void MainFrame::on_pushHarrisCorner_clicked()
{
    //파라미터 읽어 들임
    double  dThresh = ui->editThreshCorner->text().toDouble();
    double  dSigma  = ui->editSigmaCorner->text().toDouble();
    int     nSize   = ui->editSizeBlock->text().toInt();

    //포커스된 이미지 폼으로부터 영상 가져오기
    KImageGray& igMain = _q_pFormFocused->ImageGray();

    //해리스 코너 검출
    KCorner oCorner(dSigma, nSize);
    oCorner.HarrisCorner(dThresh, igMain);

    //출력창 결정
    ImageForm*  q_pForm = 0;
    for(auto item : _lImageForm)
        if(item->ID() == "Corner Detection" && item->size() == _q_pFormFocused->size())
        {
            q_pForm = item;
            q_pForm->Update(igMain.GrayToRGB());
            break;
        }
    if(q_pForm == 0){
        q_pForm = new ImageForm(igMain.GrayToRGB(), "Corner Detection", this);
        _lImageForm.push_back(q_pForm);
    }

    //출력창에 코너 표시
    for(auto& oPixel : oCorner)
        q_pForm->DrawEllipse(QPoint(oPixel.u, oPixel.v),1,1,QColor(255,0,0),2);
    q_pForm->update();
    q_pForm->show();

    //UI 활성화 갱신
    UpdateUI();
}

void MainFrame::on_pushCircleLocalization_clicked()
{
    //초기화 정보
    KImageGray          igMain = _q_pFormFocused->ImageGray();
    KCIRCLEHOUGH_INFO   oInfo;

    oInfo.nLeft         = ui->editMinR->text().toInt();
    oInfo.nTop          = ui->editMinR->text().toInt();
    oInfo.nRight        = igMain.Col() - ui->editMinR->text().toInt();
    oInfo.nBottom       = igMain.Row() - ui->editMinR->text().toInt();
    oInfo.nMinR         = ui->editMinR->text().toInt();
    oInfo.nMaxR         = ui->editMaxR->text().toInt();
    oInfo.nNumGridCx    = (int)((oInfo.nRight - oInfo.nLeft + 1)/ui->editResolution->text().toDouble());
    oInfo.nNumGridCy    = (int)((oInfo.nBottom - oInfo.nTop + 1)/ui->editResolution->text().toDouble());
    oInfo.nNumGridR     = (int)((oInfo.nMaxR - oInfo.nMinR + 1)/ui->editResolution->text().toDouble());
    oInfo.nDirEdge      = _OUTWARD_EDGE;
    oInfo.nThreshVote   = ui->editThreshVote->text().toInt();

    //에지 검출
    KEdge       oEdge(ui->editSigmaCanny->text().toDouble());
    KImageGray  igEdge;
    oEdge.Canny(ui->editLow->text().toInt(), ui->editHigh->text().toInt(), igMain, igEdge);

    KPGM(igEdge).Save("./output/edge4Hough.pgm"); //확인용

    //Hough Transform
    KCircleHough oCircleHough;
    KCircle*     opCircle;
    oCircleHough.Init((void*)&oInfo);
    opCircle = (KCircle*)oCircleHough.Run((void*)&oEdge);
    if(opCircle == 0)
    {
        //결과 표시
        if(ui->listWidget->isVisible() == false)
            on_buttonShowList_clicked();
        ui->listWidget->insertItem(0, QString("There is no circle detected !"));

        return;
    }

    //출력창 결정
    ImageForm*  q_pForm = 0;
    for(auto item : _lImageForm)
        if(item->ID() == "Circle Localization" && item->size() == _q_pFormFocused->size())
        {
            q_pForm = item;
            q_pForm->Update(igMain.GrayToRGB());
            break;
        }
    if(q_pForm == 0){
        q_pForm = new ImageForm(igMain.GrayToRGB(), "Circle Localization", this);
        _lImageForm.push_back(q_pForm);
    }

    //출력창에 원 표시
    q_pForm->DrawEllipse(QPoint((int)(opCircle->_dCx),(int)(opCircle->_dCy)),(int)(opCircle->_dRadius),
                         (int)(opCircle->_dRadius), QColor(255,0,0), 2);
    q_pForm->update();
    q_pForm->show();

    //결과 표시
    if(ui->listWidget->isVisible() == false)
        on_buttonShowList_clicked();
    ui->listWidget->insertItem(0, QString(">> detected circle --> radius = %1").arg(opCircle->_dRadius));

    //UI 활성화 갱신
    UpdateUI();
}

void MainFrame::on_pushPyramid_clicked()
{
    //영상 가져오기
    KImageGray igMain = _q_pFormFocused->ImageGray();

    //이미지 피라미드 생성
    int                 nOctave = ui->editOctave->text().toInt();
    double              dSigma  = ui->editSigmaPyramid->text().toDouble();
    KPyramidGaussian    oPyramid(igMain, dSigma, nOctave);

    //출력창 결정
    ImageForm*  q_pForm;
    QString     stOctave;

    for(int i = 0; i < oPyramid.Octave(); i++)
    {
        stOctave = QString("%1").arg(i);
        q_pForm = 0;

        for(auto item : _lImageForm)
            if(item->ID() == "Image Pyramid" && item->Atrb() == stOctave && item->size() == _q_pFormFocused->size())
            {
                q_pForm = item;
                q_pForm->Update(oPyramid[i].ToGray());
                break;
            }
        if(q_pForm == 0){
            q_pForm = new ImageForm(oPyramid[i].ToGray(), "Image Pyramid", this, stOctave);
            _lImageForm.push_back(q_pForm);
        }
        q_pForm->show();
    }
}

void MainFrame::on_pushOpticalFlow_clicked()
{
    //이미지 파일 선택
    QFileDialog::Options    q_Options = QFileDialog::DontResolveSymlinks | QFileDialog::DontUseNativeDialog;

    _stlFile = QFileDialog::getOpenFileNames(this, tr("Select Images"), "./data",
                                            "Image Files(*.pgm)",0, q_Options);

    if(_stlFile.length() == 0)
        return;

    //첫 영상을 읽어들임
    QImage      q_oImage;
    KImageGray  igImg;

    if(q_oImage.load(_stlFile.value(0)) == false)
        return;
    igImg.Create(q_oImage.height(), q_oImage.width(), q_oImage.bits(), _LOCK);

    //출력창 결정
    ImageForm*  q_pForm = 0;
    for(auto item : _lImageForm)
        if(item->ID() == "Optical Flow" && item->size() == _q_pFormFocused->size())
        {
            q_pForm = item;
            q_pForm->Update(igImg.GrayToRGB());
            break;
        }
    if(q_pForm == 0){
        q_pForm = new ImageForm(igImg.GrayToRGB(), "Optical Flow", this);
        _lImageForm.push_back(q_pForm);
    }

    //첫 영상 표시 및 optical flow 출력창 저장
    (_q_pFormFlow = q_pForm)->show();
//    if(q_pForm != 0)
//        (_q_pFormFlow = q_pForm)->show();

    //KOpticalFLows 객체 생성
    int                 nWindow     = ui->editWindowFlow->text().toInt();
    int                 nStride     = ui->editStrideFlow->text().toInt();
    KOPTICALFLOW_INFO   oInfo       = { nWindow, nStride, 0, 0, 0, 0 };

    if (_opOpticalFlow)
        delete _opOpticalFlow;  // 메모리 해제

    _opOpticalFlow = new KOpticalFlow();
    _opOpticalFlow->Init((void*)&oInfo);
    _opOpticalFlow->Run((void*)&igImg); //첫 영상

    UpdateUI();
}



void MainFrame::on_pushOpticalFlowRun_clicked()
{
    static int nNo;
    QImage     q_oImage;

    //영상 읽어들이기
    if(++nNo >= _stlFile.length() || q_oImage.load(_stlFile.value(nNo)) == false)
    {
        nNo = 0;
        delete _opOpticalFlow;
        _opOpticalFlow = 0;

        UpdateUI();
        return;
    }
    //영상처리용 객체 설정
    KImageGray igImg;
    if(q_oImage.width() % 4 == 0)
        igImg.Create(q_oImage.height(), q_oImage.width(), q_oImage.bits(), _LOCK);
    else
    {
        igImg.Create(q_oImage.height(), q_oImage.width());
        for(unsigned int i =0; i < igImg.Row(); i++)
            memcpy(igImg[i], q_oImage.scanLine(i), igImg.Col());
    }

    //optical flow 게산
//    KArray<KPOINTF>& arFlow = *(KArray<KPOINTF>*)_opOpticalFlow->Run((void*)&igImg);
    vector<vector<KPOINTF>>* vector_arFlow =
        (vector<vector<KPOINTF>>*)_opOpticalFlow->Run((void*)&igImg);

    // arFlow에 제대로 된 값이 들어갔는지 확인
    if (!vector_arFlow || vector_arFlow->empty()) {
        qDebug() << "arFlow is empty or invalid.";
        return;
    }

    // vector를 KArray로 변환
    KArray<KPOINTF> arFlow;
    arFlow.Create(vector_arFlow->size(), (*vector_arFlow)[0].size());

    for (size_t row = 0; row < vector_arFlow->size(); ++row) {
        for (size_t col = 0; col < (*vector_arFlow)[row].size(); ++col) {
            arFlow[row][col] = (*vector_arFlow)[row][col];
        }
    }

    //출력창에 영상 표시
    _q_pFormFlow->Update(igImg.GrayToRGB());

    //출력창에 optical flow 표시
    KOPTICALFLOW_INFO oInfo = *(KOPTICALFLOW_INFO*)_opOpticalFlow->Info();
    double            dScale = 3.0;
    int               nStep = ui->editPeriodFlow->text().toInt() * oInfo.nStride;

    for(int nVo = oInfo.nSy; nVo < oInfo.nEy; nVo += nStep)
        for(int nUo = oInfo.nSx; nUo < oInfo.nEx; nUo += nStep)
        {

            //flow의 크기 및 방향
            double dMag = sqrt(_SQR(arFlow[nVo][nUo].fX) + _SQR(arFlow[nVo][nUo].fY));
            double dX   = (dMag < 0.5 ? 0.0 : arFlow[nVo][nUo].fX / dMag);
            double dY   = (dMag < 0.5 ? 0.0 : arFlow[nVo][nUo].fY / dMag);


            //flow의 끝점 계산
            int nUf = (int)(nUo + dX*dMag*dScale);
            int nVf = (int)(nVo + dY*dMag*dScale);

//            qDebug() << "nVo:" << nVo << ", nUo:" << nUo
//                     << ", dMag:" << dMag
//                     << ", dX:" << dX << ", dY:" << dY;

            //화살표 그리기
            _q_pFormFlow->DrawEllipse(QPoint(nUo,nVo),1,1,QColor(0,0,255));
            _q_pFormFlow->DrawLine(nUo,nVo,nUf,nVf,QColor(255,0,0));
        }
    _q_pFormFlow->update();

//    delete _opOpticalFlow; // 객체를 해제
//    _opOpticalFlow = nullptr; // 포인터를 null로 설정

    //UI 업데이트
    UpdateUI();
}
