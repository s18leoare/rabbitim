#include "DlgUservCard.h"
#include "ui_DlgUservCard.h"
#include "../../Global/Global.h"
#include <QImageWriter>
#include <QDesktopWidget>
#include "Tool.h"

CDlgUservCard::CDlgUservCard(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgUservCard)
{
    Init();
    m_bModify = false;
}

CDlgUservCard::CDlgUservCard(const QString &jid, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgUservCard)
{
    Init();

    m_bModify = false;
    m_szJid = jid;

    bool check = connect(GET_CLIENT.data(),
                         SIGNAL(sigUpdateRosterUserInfo(QString,QSharedPointer<CUser>)),
                         SLOT(slotUpdateRoster(QString, QSharedPointer<CUser>)));
    Q_ASSERT(check);

    GET_CLIENT->RequestUserInfoRoster(jid);
}

CDlgUservCard::CDlgUservCard(QSharedPointer<CUserInfo> user, bool bModify, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgUservCard),
    m_UserInfo(user),
    m_bModify(bModify)
{
    Init();
}

CDlgUservCard::~CDlgUservCard()
{
    LOG_MODEL_DEBUG("CDlgUservCard", "CDlgUservCard::~CDlgUservCard");
    GET_CLIENT.data()->disconnect(this);
    delete ui;
}

int CDlgUservCard::Init()
{
    ui->setupUi(this);
    CTool::SetWindowsGeometry(this);
    return 0;
}

void CDlgUservCard::showEvent(QShowEvent *)
{
    LOG_MODEL_DEBUG("CDlgUservCard", "CDlgUservCard::showEvent");

    if(m_UserInfo.isNull())
        return;

    ui->txtJID->setText(m_UserInfo->GetId());
    ui->txtName->setText(m_UserInfo->GetName());
    ui->txtNick->setText(m_UserInfo->GetNick());
    ui->dateBirthday->setDate(m_UserInfo->GetBirthday());
    ui->txtEmail->setText(m_UserInfo->GetEmail());
    ui->txtDescription->setText(m_UserInfo->GetDescription());

    QImageWriter imageWriter(&m_PhotoBuffer, "png");
    m_PhotoBuffer.open(QIODevice::WriteOnly);
    if(!imageWriter.write(m_UserInfo->GetPhoto()))
        LOG_MODEL_ERROR("CDlgUservCard", "error:%s", imageWriter.errorString().toStdString().c_str());
    m_PhotoBuffer.close();

    QPixmap pixmap;
    pixmap.convertFromImage(m_UserInfo->GetPhoto());
    ui->lbPhoto->setPixmap(pixmap);
    ui->lbPhoto->setScaledContents(true);

    ui->pbBrower->setVisible(m_bModify);
    ui->pbClear->setVisible(m_bModify);
    ui->pbOK->setVisible(m_bModify);
}

void CDlgUservCard::closeEvent(QCloseEvent *)
{
}

void CDlgUservCard::changeEvent(QEvent *e)
{
    switch(e->type())
    {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    }
}

void CDlgUservCard::on_pbBrower_clicked()
{
    QString szFile, szFilter("*.png *.jpg *.bmp *.gif *.jpeg");
    szFile = CTool::FileDialog(this, QString(), szFilter, tr("Open File"));
    if(szFile.isEmpty())
       return; 

    //TODO:现在只上传小图片,以后增加上传原图  
    //原因是openfire把vcard存在数据库中,导制数据库存性能,网络性能降低  
    QPixmap pixmap(szFile), map;
    int nWidth = pixmap.width();
    int nHeight = pixmap.height();
    if(nWidth > RABBITIM_AVATAR_SIZE)
        nWidth = RABBITIM_AVATAR_SIZE;
    if(nHeight > RABBITIM_AVATAR_SIZE)
        nHeight = RABBITIM_AVATAR_SIZE;
    map = pixmap.scaled(nWidth, nHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    QImageWriter imageWriter(&m_PhotoBuffer, "png");
    m_PhotoBuffer.open(QIODevice::WriteOnly);
    if(!imageWriter.write(map.toImage()))
        LOG_MODEL_ERROR("CDlgUservCard", "error:%s", imageWriter.errorString().toStdString().c_str());
    m_PhotoBuffer.close();

    ui->lbPhoto->setPixmap(map);
}

void CDlgUservCard::on_pbClear_clicked()
{
    ui->lbPhoto->clear();
    m_PhotoBuffer.setData(NULL, 0);
}

void CDlgUservCard::on_pbOK_clicked()
{
    QSharedPointer<CUserInfo> userInfo = USER_INFO_LOCALE->GetInfo();
    userInfo->SetName(ui->txtName->text());
    userInfo->SetNick(ui->txtNick->text());
    userInfo->SetBirthday(ui->dateBirthday->date());
    userInfo->SetEmail(ui->txtEmail->text());
    userInfo->SetDescription(ui->txtDescription->text());
    userInfo->SetPhoto(m_PhotoBuffer.buffer());
    GET_CLIENT->setlocaleUserInfo(userInfo);
    close();
}

void CDlgUservCard::on_pbCancel_clicked()
{
    close();
}

void CDlgUservCard::slotUpdateRoster(const QString& szId, QSharedPointer<CUser> userInfo)
{
    if(szId == m_szJid)
    {
        if(m_UserInfo.isNull())
        {
            m_UserInfo = userInfo->GetInfo();
        }
        showEvent(NULL);
    }
}
