#include "mainframe.h"
#include "ui_mainframe.h"
#include "imageform.h"
#include "edge.h"
#include "corner.h"
#include "circleHough.h"

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
        ui->pushCircleLocalization->setEnabled( _q_pFormFocused && _q_pFormFocused->ID() == "OPEN" && _q_pFormFocused->ImageGray().Address());
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
    oCircleHough.Init((void*)&oInfo);

    void* Result = oCircleHough.Run((void*)&oEdge);
    if (Result == nullptr) // nullptr 반환 시 처리
    {
        // 결과 표시
        if (ui->listWidget->isVisible() == false)
            on_buttonShowList_clicked();
        ui->listWidget->insertItem(0, QString("There is no circle detected !"));
        return;
    }

    vector<KCircle*>* opCircles = reinterpret_cast<vector<KCircle*>*>(Result);
    if (opCircles->empty()) // 검출된 원이 없을 때 처리
    {
        // 결과 표시
        if (ui->listWidget->isVisible() == false)
            on_buttonShowList_clicked();
        ui->listWidget->insertItem(0, QString("There is no circle detected !"));
        delete opCircles; // 메모리 해제
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
    for(auto* circle : *opCircles)
    {
    q_pForm->DrawEllipse(QPoint((int)(circle->_dCx),(int)(circle->_dCy)),(int)(circle->_dRadius),
                         (int)(circle->_dRadius), QColor(255,0,0), 2);
    }
    q_pForm->update();
    q_pForm->show();

    //결과 표시
    if(ui->listWidget->isVisible() == false)
        on_buttonShowList_clicked();
    for(auto* circle : *opCircles)
        ui->listWidget->insertItem(0, QString(">> detected circle --> radius = %1").arg(circle->_dRadius));

    //UI 활성화 갱신
    UpdateUI();

    // 메모리 해제
    for (auto* circle : *opCircles) {
        delete circle;
    }
}
