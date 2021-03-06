#include "ui_loginwidget.h"

#include "loginwidget.h"
#include "mainwindow.h"
#include "tools.h"
#include "globals.h"

LoginWidget::LoginWidget(MenuBar *menuBar, QWidget *parent) :
        QWidget(parent),
        mMenuBar(menuBar),
        ui(new Ui::LoginWidget),
        mGitManager(new GitManager())
{
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint);    //去掉窗口边框
    setAttribute(Qt::WA_TranslucentBackground, true);   // 背景透明
    ui->lineEdit_username->setAttribute(Qt::WA_MacShowFocusRect, false);
    ui->lineEdit_password->setAttribute(Qt::WA_MacShowFocusRect, false);
    ui->lineEdit_repoUrl->setAttribute(Qt::WA_MacShowFocusRect, false);
}

LoginWidget::~LoginWidget()
{
    delete ui;
}

void LoginWidget::on_pushButton_close_clicked()
{
    close();
}

void LoginWidget::on_lineEdit_repoUrl_textChanged(const QString &)
{
    changeEnterButtonStatus();
}

void LoginWidget::on_lineEdit_username_textChanged(const QString &)
{
    changeEnterButtonStatus();
}

void LoginWidget::on_lineEdit_password_textChanged(const QString &)
{
    changeEnterButtonStatus();
}

void LoginWidget::on_pushButton_enter_clicked()
{
    if (changeEnterButtonStatus()) {
        gConfigModel->setRepoUrl(ui->lineEdit_repoUrl->displayText());
        gConfigModel->setRepoUsername(ui->lineEdit_username->displayText());
        gConfigModel->setRepoPassword(ui->lineEdit_password->text());

        int result = initGitManager();
        if (!result) {
            gConfigModel->setLocalRepoStatus(Gitnoter::RemoteRepo);
            openMainWindow();
        }
        else {
            qDebug() << "void LoginWidget::on_pushButton_enter_clicked(): " << result;
        }
    }
}

bool LoginWidget::changeEnterButtonStatus()
{
    if (!ui->lineEdit_password->displayText().isEmpty()
        && !ui->lineEdit_repoUrl->displayText().isEmpty()
        && !ui->lineEdit_username->displayText().isEmpty()) {
        ui->pushButton_enter->setStyleSheet(
                "QPushButton#pushButton_enter{border-image: url(:/images/icon-enter-up.png);}");
        return true;
    }
    else {
        ui->pushButton_enter->setStyleSheet(
                "QPushButton#pushButton_enter{border-image: url(:/images/icon-enter-down.png);}");
        return false;
    }
}

void LoginWidget::openMainWindow()
{
    Tools::createMkDir(__RepoNoteTextPath__);
    Tools::createMkDir(__RepoNoteDataPath__);

    close();
    (new MainWindow(mMenuBar))->show();
}

void LoginWidget::on_pushButton_initLocal_clicked()
{
    int result = initGitManager();
    if (!result) {
        gConfigModel->setLocalRepoStatus(Gitnoter::LocalRepo);
        openMainWindow();
    }
    else {
        qDebug() << "void LoginWidget::on_pushButton_initLocal_clicked(): " << result;
    }
}

int LoginWidget::initGitManager()
{
    if (QDir(__RepoPath__).exists()) {
        return mGitManager->open(Tools::qstringToConstData(__RepoPath__));
    }

    if (!gConfigModel->getRepoUsername().isEmpty()
        && !gConfigModel->getRepoPassword().isEmpty()
        && !gConfigModel->getRepoUrl().isEmpty()) {
        mGitManager->setUserPass(Tools::qstringToConstData(gConfigModel->getRepoUsername()),
                                        Tools::qstringToConstData(gConfigModel->getRepoPassword()));
        return mGitManager->clone(Tools::qstringToConstData(gConfigModel->getRepoUrl()), Tools::qstringToConstData(__RepoPath__));
    }

    return mGitManager->initLocalRepo(Tools::qstringToConstData(__RepoPath__), true);
}
