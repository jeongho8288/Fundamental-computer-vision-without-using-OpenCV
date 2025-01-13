#include "mainframe.h"
#include "ui_mainframe.h"
#include "imageform.h"

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

//void MainFrame::on_pushRGB2Gray_clicked()
//{
//    // 포커스 된 ImageForm으로부터 영상을 가져옴
//    KImageColor icMain;
//    if(_q_pFormFocused != 0 && _q_pFormFocused->ImageColor().Address() && _q_pFormFocused->ID() == "OPEN")
//        icMain = _q_pFormFocused->ImageColor();
//    else
//        return;

//    // RGB to Gray
//    KImageGray igImg = icMain.ColorToGray();

//    // 출력을 위한 ImageForm 생성
//    ImageForm* q_pForm = new ImageForm(igImg, "Gray Image", this);

//    // 출력창 포인터 저장
//    _lImageForm.push_back(q_pForm);
//    q_pForm->show();
//}

void MainFrame::on_pushRGB2HSI_clicked()
{
        // 포커스 된 ImageForm으로부터 영상을 가져옴
        KImageColor icMain;

        if(_q_pFormFocused != 0 && _q_pFormFocused->ImageColor().Address() && _q_pFormFocused->ID() == "OPEN")
            icMain = _q_pFormFocused->ImageColor();
        else
            return;

        // RGB to HSI
        KArray<KHSI> arHSI;
        icMain.RGBtoHSI(arHSI);

        // 출력을 위한 ImageForm 생성
        ImageForm* q_pForm_H = new ImageForm(arHSI, "H Image", this, 'H');
        ImageForm* q_pForm_S = new ImageForm(arHSI, "S Image", this, 'S');
        ImageForm* q_pForm_I = new ImageForm(arHSI, "I Image", this, 'I');

        // 출력창 포인터 저장
        _lImageForm.push_back(q_pForm_H);
        q_pForm_H->show();

        _lImageForm.push_back(q_pForm_S);
        q_pForm_S->show();

        _lImageForm.push_back(q_pForm_I);
        q_pForm_I->show();
}

void MainFrame::on_pushRGB2NRG_clicked()
{
    // 포커스 된 ImageForm으로부터 영상을 가져옴
    KImageColor icMain;

    if(_q_pFormFocused != 0 && _q_pFormFocused->ImageColor().Address() && _q_pFormFocused->ID() == "OPEN")
        icMain = _q_pFormFocused->ImageColor();
    else
        return;

    // RGB to NRG
    KArray<KNRG> arNRG;
    icMain.RGBtoNRG(arNRG);

    // 출력을 위한 ImageForm 생성
    ImageForm* q_pForm_NR = new ImageForm(arNRG, "NR Image", this, 'R');
    ImageForm* q_pForm_NG = new ImageForm(arNRG, "NG Image", this, 'G');

    // 출력창 포인터 저장
    _lImageForm.push_back(q_pForm_NR);
    q_pForm_NR->show();

    _lImageForm.push_back(q_pForm_NG);
    q_pForm_NG->show();
}
