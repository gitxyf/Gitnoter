#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "categorylistwidget.h"
#include "configmodel.h"
#include "settingdialog.h"
#include "aboutdialog.h"
#include "grouptreewidget.h"
#include "notelistwidget.h"
#include "markdowneditorwidget.h"
#include "menubar.h"
#include "notemodel.h"
#include "enterlicensedialog.h"
#include "gitmanager.h"
#include "lockdialog.h"

#include <UGlobalHotkey/uglobalhotkeys.h>


#include <QMainWindow>
#include <QtCore>
#include <QtGui>
#include <QPushButton>
#include <QTableWidgetItem>
#include <QListWidgetItem>
#include <QTimer>
#include <QTreeWidgetItem>
#include <QStackedWidget>
#include <QSplitter>

namespace Ui
{
    class MainWindow;
}

class LockDialog;
class GroupTreeWidget;
class MarkdownEditorWidget;
class NoteListWidget;
class MenuBar;
class UGlobalHotkey;

class MainWindow : public QMainWindow
{
Q_OBJECT

public:
    explicit MainWindow(MenuBar *menubar, QWidget *parent = 0);

    ~MainWindow();

    void updateView(Gitnoter::UpdateViewFlags flags = Gitnoter::GroupTreeWidget);
    void setNoteListTitle();

private:
    void setupUi();

    void initTempDir();

protected:
    void changeEvent(QEvent *event) override;
    void moveEvent(QMoveEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

    void closeEvent(QCloseEvent *event) override;

public slots:
    void init();
    void reload();

    void appendGroup();
    void removeGroup();

    void appendNote();
    void trashNote();         // set isDelete: 1
    void removeNote();    // remove from disk
    void restoreNote();

    void sortNote();
    void searchNote(const QString &text);
    void openSettingWidget();
    void setSearchFocus();

    void syncRepo();
    void importEvernote();

    void showSidebar(bool clicked);
    void showListBar(bool clicked);

    void enterFullScreen();
    void fullScreenEditMode();

    void newWindow(QListWidgetItem *listWidgetItem = nullptr);
    void showLastWindow();
    void showNextWindow();
    void closeWindow();
    void closeAllWindow();
    void lockWindow();
    void preposeWindow();

    void guide();
    void historyChange();
    void markdownLanguage();
    void feedback();
    void issue();
    void about();

    void aboutGitnoter();
    void setting();

    void setRemoteToRepo();

private slots:
    void on_splitter_splitterMoved(int pos, int index);

    // QTreeWidget

    void on_pushButton_add_clicked();

    void on_pushButton_subtract_clicked();

    void on_groupTreeWidget_itemClicked(QTreeWidgetItem *item, int column);

    // QListWidget

    void on_pushButton_noteAdd_clicked();

    void on_pushButton_noteSubtract_clicked();

    void on_noteListWidget_itemClicked(QListWidgetItem *item);

    void on_lineEdit_noteSearch_textChanged(const QString &arg1);

    void fullScreenShot(size_t id = 1);
    void windowScreenShot(size_t id = 1);
    void partScreenShot(size_t id = 1);

    void updateAutoSyncRepoTimer();
    void updateAutoLockTimer();

    void setRepoApplyClicked();
    void setRepoCloseClicked();

    void openPurchasePanel();

public:
    GroupTreeWidget *groupTreeWidget();
    NoteListWidget *noteListWidget();
    MarkdownEditorWidget *markdownEditorWidget();
    QStackedWidget *stackedWidget();
    QSplitter *splitter();
    MenuBar *menuBar();

    SettingDialog *settingDialog() { return mSettingDialog; }
    EnterLicenseDialog *enterLicenseDialog() { return mEnterLicenseDialog; }
    LockDialog *lockDialog() { return mLockDialog; }

private:
    Ui::MainWindow *ui;
    MenuBar *mMenuBar;
    AboutDialog *mAboutDialog;
    SettingDialog *mSettingDialog;
    EnterLicenseDialog *mEnterLicenseDialog;
    LockDialog *mLockDialog;

    QTimer *mAutoSyncRepoTimer;
    QTimer *mAutoLockTimer;
    int mOpenPurchasePanelTimestamp;

    GitManager *mGitManager;

};

#endif // MAINWINDOW_H