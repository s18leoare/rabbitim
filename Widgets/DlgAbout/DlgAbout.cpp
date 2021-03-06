#include "DlgAbout.h"
#include "ui_DlgAbout.h"
#include "Tool.h"
#include "Version.h"
#include <QFile>
#include "Global/Global.h"
#include <QDir>
#include <QDateTime>

CDlgAbout::CDlgAbout(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgAbout)
{
    ui->setupUi(this);
    CTool::SetWindowsGeometry(this);
 
    QString szVersion(tr("Version:%1.%2.%3.%4").arg(QString::number(REVISION_VERSION_NUMBER),
                                                 QString::number(MINOR_VERSION_NUMBER),
                                                 QString::number(REVISION_VERSION_NUMBER),
                                                 BUILD_VERSION
                                                    ));
    ui->lbVersion->setText(szVersion);
    ui->lbDate->setText(tr("Build date:%1 %2").arg(__DATE__, __TIME__));
    ui->lbAuthor->setText(tr("Author: KangLin\nEmail or MSN:kl222@126.com"));
    ui->lbHome->setOpenExternalLinks(true);
    ui->lbHome->setText(tr("Home page:") + "<a href=\"https://github.com/KangLin/rabbitim\">https://github.com/KangLin/rabbitim</a>");
    ui->lbCopyright->setText(tr(" Copyright (C) 2014 - %1 KangLin Studio").arg(QString::number(QDate::currentDate().year())));

    QString szFile;
#ifdef MOBILE
    szFile = ":/file/ChangeLog";
    AppendFile(ui->txtChange, szFile);
    szFile = ":/file/License";
    AppendFile(ui->txtLicense, szFile);
    szFile = ":/file/Authors";
    AppendFile(ui->txtThinks, szFile);
#else
    szFile = CGlobal::Instance()->GetDirApplication() + QDir::separator() + "ChangeLog.md";
    AppendFile(ui->txtChange, szFile);
    szFile = CGlobal::Instance()->GetDirApplication() + QDir::separator() + "License.md";
    AppendFile(ui->txtLicense, szFile);
    szFile = CGlobal::Instance()->GetDirApplication() + QDir::separator() + "Authors.txt";
    AppendFile(ui->txtThinks, szFile);
#endif
}

CDlgAbout::~CDlgAbout()
{
    delete ui;
}

int CDlgAbout::AppendFile(QTextEdit* pEdit, const QString &szFile)
{
    QFile readme(szFile);
    if(readme.open(QFile::ReadOnly))
    {
        pEdit->append(readme.readAll());
        //把光标移动文档开始处  
        QTextCursor cursor =   pEdit->textCursor();
        cursor.movePosition(QTextCursor::Start);
        pEdit->setTextCursor(cursor);
        readme.close();
    }
    return 0;
}
