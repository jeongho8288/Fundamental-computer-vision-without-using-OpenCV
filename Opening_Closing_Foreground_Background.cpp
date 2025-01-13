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
        ui->pushOstu->setEnabled(_q_pFormFocused &&
                                 _q_pFormFocused->ID() == "OPEN" &&
                                 _q_pFormFocused->ImageGray().Address());
        ui->pushOpening->setEnabled(_q_pFormFocused &&
                                     _q_pFormFocused->ID() == "Binary Image" &&
                                     _q_pFormFocused->ImageGray().Address());
        ui->pushClosing->setEnabled(_q_pFormFocused &&
                                    _q_pFormFocused->ID() == "Binary Image" &&
                                    _q_pFormFocused->ImageGray().Address());
        ui->pushBoundary->setEnabled(_q_pFormFocused &&
                                    (_q_pFormFocused->ID() == "Binary Image" || _q_pFormFocused->ID() == "Opening Image" || _q_pFormFocused->ID() == "Closing Image") &&
                                    _q_pFormFocused->ImageGray().Address());


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




void MainFrame::on_pushOstu_clicked()
{
    KImageGray igMain = _q_pFormFocused->ImageGray();
    KImageGray igBin;
    KBINARIZATION_OUTPUT* opBinOutput = KHisto().Ostu(&igMain, &igBin);

    ImageForm* q_pForm = 0;
    for(auto item: _lImageForm)
        if(item->ID() == "Binary Image" && item->size() == _q_pFormFocused->size())
        {
            q_pForm = item;
            break;
        }

    if(q_pForm)
        q_pForm->Update(igBin);
    else
    {
        q_pForm = new ImageForm(igBin, "Binary Image", this);
        q_pForm->show();
        _lImageForm.push_back(q_pForm);
    }

    ui->listWidget->addItem(QString(">> Ostu Threshold : %1").arg(opBinOutput->nThresh));
    ui->listWidget->show();

    UpdateUI();
}

void MainFrame::on_pushOpening_clicked()
{
    int nSize = (ui->checkbox3x3->isChecked()?3:5); //체크박스는 둘 중 하나는 true 하나는 false
    KImageGray igBin = _q_pFormFocused->ImageGray().BinaryErode(_WHITE,nSize).BinaryDilate(_WHITE,nSize);

    ImageForm* q_pForm = 0;
    for(auto item: _lImageForm)
        if(item->ID() == "Opening Image" && item->size() == _q_pFormFocused->size())
        {
            q_pForm = item;
            break;
        }

    if(q_pForm)
        q_pForm->Update(igBin);
    else
    {
        q_pForm = new ImageForm(igBin, "Opening Image", this);
        q_pForm->show();
        _lImageForm.push_back(q_pForm);
    }

    UpdateUI();
}

void MainFrame::on_pushClosing_clicked()
{
    int nSize = (ui->checkbox3x3->isChecked()?3:5);
    KImageGray igBin = _q_pFormFocused->ImageGray().BinaryDilate(_WHITE,nSize).BinaryErode(_WHITE,nSize);

    ImageForm* q_pForm = 0;
    for(auto item: _lImageForm)
        if(item->ID() == "Closing Image" && item->size() == _q_pFormFocused->size())
        {
            q_pForm = item;
            break;
        }

    if(q_pForm)
        q_pForm->Update(igBin);
    else
    {
        q_pForm = new ImageForm(igBin, "Closing Image", this);
        q_pForm->show();
        _lImageForm.push_back(q_pForm);
    }

    UpdateUI();
}

void MainFrame::on_pushBoundary_clicked()
{
        int nbSize = (ui->checkbox4N->isChecked()?4:8);

        KImageGray igMain = _q_pFormFocused->ImageGray();

        KMASK _mask;
        if(nbSize == 4)
            _mask = MASK::set(3, 3, 0., 1., 0., 1., 1., 1., 0., 1., 0.);
        else
            _mask = MASK::fill(3, 3, 1.);

        KImageGray igBoundary;
        const char* ID;
        if(ui->checkboxFG->isChecked()){
            igBoundary = igMain.BoundaryFG(3, _mask); //3x3 mask
            ID = "FG_Boundary";
        }
        else{
            igBoundary = igMain.BoundaryBG(3, _mask); //3x3 mask
            ID = "BG_Boundary";
        }

        ImageForm* q_pForm = 0;
        for(auto item: _lImageForm)
            if(item->ID() == ID && item->size() == _q_pFormFocused->size())
            {
                q_pForm = item;
                break;
            }

        if(q_pForm)
            q_pForm->Update(igBoundary);
        else
        {
            q_pForm = new ImageForm(igBoundary, ID, this);
            q_pForm->show();
            _lImageForm.push_back(q_pForm);
        }


        UpdateUI();
}



void MainFrame::on_checkbox3x3_clicked()
{
    ui->checkbox3x3->setChecked(true);
    ui->checkbox5x5->setChecked(false);
}

void MainFrame::on_checkbox5x5_clicked()
{
    ui->checkbox3x3->setChecked(false);
    ui->checkbox5x5->setChecked(true);
}

void MainFrame::on_checkbox4N_clicked()
{
    ui->checkbox4N->setChecked(true);
    ui->checkbox8N->setChecked(false);
}

void MainFrame::on_checkbox8N_clicked()
{
    ui->checkbox4N->setChecked(false);
    ui->checkbox8N->setChecked(true);
}

void MainFrame::on_checkboxBG_clicked()
{
    ui->checkboxBG->setChecked(true);
    ui->checkboxFG->setChecked(false);
}

void MainFrame::on_checkboxFG_clicked()
{
    ui->checkboxBG->setChecked(false);
    ui->checkboxFG->setChecked(true);
}
