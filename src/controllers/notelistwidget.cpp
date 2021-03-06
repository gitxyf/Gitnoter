#include "notelistwidget.h"
#include "globals.h"
#include "notelistcellwidget.h"

#include <QDate>
#include <QDir>

NoteListWidget::NoteListWidget(QWidget *parent) : QListWidget(parent)
{

}

NoteListWidget::~NoteListWidget()
{

}

void NoteListWidget::init(MainWindow *mainWindow)
{
    mMainWindow = mainWindow;
    QDir dir(__RepoNoteTextPath__);
    for (auto &&mfi : dir.entryInfoList()) {
        if (mfi.fileName() == "." || mfi.fileName() == "..") {
            continue;
        }
        if (mfi.isDir()) {
            QDir dir2(mfi.absoluteFilePath());
            NoteModel *noteModel = new NoteModel(dir2.filePath(__NoteTextFileName__),
                                                 dir2.filePath(__NoteDataFileName__));
            mListWidgetItemList.append(append(noteModel));
        }
    }

    setListWidget();
}

QListWidgetItem *NoteListWidget::append(NoteModel *noteModel)
{
    QListWidgetItem *listWidgetItem = new QListWidgetItem(this);
    listWidgetItem->setText(QString(noteModel->getUpdateDate()));
    listWidgetItem->setData(Qt::UserRole, QVariant::fromValue(noteModel));
    addItem(listWidgetItem);
    NoteListCellWidget *noteListCellWidget = new NoteListCellWidget(noteModel, this);
    listWidgetItem->setSizeHint(noteListCellWidget->minimumSize());
    setItemWidget(listWidgetItem, noteListCellWidget);

    return listWidgetItem;
}

NoteModel *NoteListWidget::append(const QString &category)
{
    NoteModel *noteModel = new NoteModel;
    noteModel->setCategory(category);
    append(noteModel);

    if (category.isEmpty()) {
        gConfigModel->setSideSelected(
                Gitnoter::Unclassified,
                mMainWindow->groupTreeWidget()->getGroupModel(Gitnoter::Unclassified)->getName());
    }
    else {
        gConfigModel->setSideSelected(Gitnoter::Category, category);
    }

    mMainWindow->groupTreeWidget()->add(Gitnoter::All);
    mMainWindow->groupTreeWidget()->add(Gitnoter::Unclassified);
    mMainWindow->groupTreeWidget()->add(Gitnoter::Recent);

    return noteModel;
}

QListWidgetItem *NoteListWidget::getItem(const QString &uuid)
{
    for (int i = 0; i < count(); ++i) {
        QListWidgetItem *listWidgetItem = item(i);
        if (listWidgetItem->data(Qt::UserRole).value<NoteModel *>()->getUuid() == uuid) {
            return listWidgetItem;
        }
    }

    return nullptr;
}

NoteModel *NoteListWidget::getNoteModel(const QString &uuid)
{
    QListWidgetItem *listWidgetItem = getItem(uuid);
    if (listWidgetItem) {
        return listWidgetItem->data(Qt::UserRole).value<NoteModel *>();
    }
    return nullptr;
}

void NoteListWidget::remove(const QString &uuid)
{
    NoteModel *noteModel = getNoteModel(uuid);
    QListWidgetItem *listWidgetItem = getItem(noteModel->getUuid());
    QDir(noteModel->getNoteDir()).removeRecursively();
    delete listWidgetItem;
    mMainWindow->groupTreeWidget()->subtract(Gitnoter::Trash);
}

void NoteListWidget::trash(const QString &uuid)
{
    NoteModel *noteModel = getNoteModel(uuid);
    noteModel->setIsDelete(1);
    noteModel->saveNoteDataToLocal();

    mMainWindow->groupTreeWidget()->subtract(noteModel);
}

void NoteListWidget::trash(NoteModel *noteModel)
{
    noteModel->setIsDelete(1);
    noteModel->saveNoteDataToLocal();

    mMainWindow->groupTreeWidget()->subtract(noteModel);
}

void NoteListWidget::restore(const QString &uuid)
{
    NoteModel *noteModel = getNoteModel(uuid);
    noteModel->setIsDelete(0);
    noteModel->saveNoteDataToLocal();

    mMainWindow->groupTreeWidget()->add(noteModel);
}

void NoteListWidget::setListWidget()
{
    Gitnoter::GroupType type = gConfigModel->getSideSelectedType();
    const QString name = gConfigModel->getSideSelectedName();
    mListWidgetItemList = getListWidgetItemList(type, name);

    for (int i = 0; i < count(); ++i) {
        item(i)->setHidden(true);
    }

    for (auto &&listWidgetItem : mListWidgetItemList) {
        listWidgetItem->setHidden(false);
    }

    // TODO: fix layout bug
    mMainWindow->splitter()->setSizes(gConfigModel->getMainWindowSplitterSizes());

    setItemSelected();
}

void NoteListWidget::setItemSelected()
{
    clearSelection();
    const QString uuid = gConfigModel->openMainWindowNoteUuid();
    QListWidgetItem *selectedItem = nullptr;
    if (has(uuid)) {
        selectedItem = getItem(uuid);
    }
    else if (mListWidgetItemList.length() > 0) {
        selectedItem = mListWidgetItemList[0];
        gConfigModel->setOpenMainWindowNoteUuid(getNoteModel(selectedItem)->getUuid());
    }
    else {
        gConfigModel->setOpenMainWindowNoteUuid("");
    }

    if (selectedItem) {
        selectedItem->setSelected(true);
        scrollToItem(selectedItem, QAbstractItemView::PositionAtTop);
    }
}

void NoteListWidget::noteModelChanged(NoteModel *noteModel)
{
    QListWidgetItem *listWidgetItem = getItem(noteModel->getUuid());
    if (listWidgetItem) {
        ((NoteListCellWidget *) itemWidget(listWidgetItem))->reloadData();
    }
}

QList<NoteModel *> NoteListWidget::getNoteModelList()
{
    QList<NoteModel *> noteModelList = {};

    for (int i = 0; i < count(); ++i) {
        noteModelList.append(getNoteModel(QListWidget::item(i)));
    }

    return noteModelList;
}

QList<QListWidgetItem *> NoteListWidget::getListWidgetItemList(Gitnoter::GroupType type, const QString &name)
{
    QList<QListWidgetItem *> listWidgetItemList = {};

    for (int i = 0; i < count(); ++i) {
        QListWidgetItem *listWidgetItem = QListWidget::item(i);
        NoteModel *noteModel = getNoteModel(listWidgetItem);

        if (noteModel->getIsDelete()) {
            if (type == Gitnoter::Trash) {
                listWidgetItemList.append(listWidgetItem);
            }
        }
        else if ((type == Gitnoter::All) ||
                 (type == Gitnoter::Recent && noteModel->getUpdateDate() > (QDateTime::currentSecsSinceEpoch() - __RecentlyUseDay__)) ||
                 (type == Gitnoter::Unclassified && noteModel->getCategory().isEmpty()) ||
                 (type == Gitnoter::Category && name == noteModel->getCategory()) ||
                 (type == Gitnoter::Tag && noteModel->getTagList().indexOf(name) != -1)) {
            listWidgetItemList.append(listWidgetItem);
        }
    }

    return listWidgetItemList;
}

QList<NoteModel *> NoteListWidget::getNoteModelList(GroupModel *groupModel)
{
    return getNoteModelList(groupModel->getType(), groupModel->getName());
}

QList<NoteModel *> NoteListWidget::getNoteModelList(Gitnoter::GroupType type, const QString &name)
{
    QList<NoteModel *> noteModelList = {};
    QList<QListWidgetItem *> listWidgetItemList = getListWidgetItemList(type, name);

    for (auto &&listWidgetItem : listWidgetItemList) {
        noteModelList.append(getNoteModel(listWidgetItem));
    }

    return noteModelList;
}

void NoteListWidget::modify()
{

}

void NoteListWidget::search(const QString &text)
{
    QList<QListWidgetItem *> listWidgetItemList = {};
    for (auto &&listWidgetItem : mListWidgetItemList) {
        NoteModel *noteModel = getNoteModel(listWidgetItem);
        bool find = false;
        if (noteModel->getNoteText().indexOf(text, 0, Qt::CaseInsensitive) != -1) {
            find = true;
        }
        else if (noteModel->getCategory().indexOf(text, 0, Qt::CaseInsensitive) != -1) {
            find = true;
        }
        else {
            for (auto &&tag : noteModel->getTagList()) {
                if (tag.indexOf(text, 0, Qt::CaseInsensitive) != -1) {
                    find = true;
                    break;
                }
            }
        }

        if (find) {
            listWidgetItemList.append(listWidgetItem);
            listWidgetItem->setHidden(false);
        }
        else {
            listWidgetItem->setHidden(true);
        }
    }
}

void NoteListWidget::sort()
{
    Qt::SortOrder type = gConfigModel->getNoteSortType();
    Gitnoter::SortBasis basis = gConfigModel->getNoteSortBasis();

    for (int i = 0; i < count(); ++i) {
        QListWidgetItem *listWidgetItem = item(i);
        switch (basis) {
            case Gitnoter::Title:
                listWidgetItem->setText(getNoteModel(listWidgetItem)->getTitle());
                break;
            case Gitnoter::CreateDate:
                listWidgetItem->setText(QString(getNoteModel(listWidgetItem)->getCreateDate()));
                break;
            case Gitnoter::UpdateDate:
                listWidgetItem->setText(QString(getNoteModel(listWidgetItem)->getUpdateDate()));
                break;
            default:
                break;
        }
    }

    sortItems(type);
}

NoteModel *NoteListWidget::getNoteModel(QListWidgetItem *listWidgetItem)
{
    return listWidgetItem->data(Qt::UserRole).value<NoteModel *>();
}

bool NoteListWidget::has(const QString &uuid)
{
    for (auto &&listWidgetItem : mListWidgetItemList) {
        if (getNoteModel(listWidgetItem)->getUuid() == uuid) {
            return  true;
        }
    }

    return false;
}

void NoteListWidget::onActionTriggered()
{
    sort();
}

void NoteListWidget::clear()
{
    QListWidget::clear();
    mListWidgetItemList.clear();
}
