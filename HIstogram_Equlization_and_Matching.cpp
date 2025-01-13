#include "mainframe.h"
#include "ui_mainframe.h"
#include "imageform.h"
#include "edge.h"

#include <QFileDialog>
#include <QPainter>
#include <sstream>
#include <fstream>

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
        ui->pushHistoGramEQ->setEnabled( _q_pFormFocused && _q_pFormFocused->ID() == "OPEN" && _q_pFormFocused->ImageColor().Address());
    }
    else if(ui->tabWidget->currentIndex() == 1)
    {  
    }
    else if(ui->tabWidget->currentIndex() == 2)
    {        
    }
    else if(ui->tabWidget->currentIndex() == 3)
    {
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




void MainFrame::on_pushHistoGramEQ_clicked()
{
    //입력 영상
    KImageColor icMain = _q_pFormFocused->ImageColor();

    //Histogram Equaliztion
    icMain.HistoEqualization();

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
        q_pForm->Update(icMain);
    //새로운 창에 표시
    else
    {
        q_pForm = new ImageForm(icMain, "Histogram Equalized", this);
        q_pForm->show();
        _lImageForm.push_back(q_pForm);
    }

    //UI 활성화 갱신
    UpdateUI();
}

void MainFrame::on_pushHistoGramMatching_clicked()
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
    KImageColor icMatched = icMatched.HistogramMatching(icTarget, icSrc);

    //결과 표시
    ImageForm* q_pForm = new ImageForm(icMatched, "Histogram Matched", this);

    _lImageForm.push_back(q_pForm);
    q_pForm->show();
}

