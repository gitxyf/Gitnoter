#include "notelistcellwidget.h"
#include "ui_notelistcellwidget.h"
#include "tagorcategorylistcellwidget.h"

#include "tools.h"

NoteListCellWidget::NoteListCellWidget(NoteModel *noteModel, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NoteListCellWidget),
    noteModel(noteModel)
{
    ui->setupUi(this);
    ui->label_title->setText("asdf - 歌单 - 网易云音乐");
    ui->frame_body->setText("2016年8月24日 - asdf 吉子可-- 2016-08-24 创建 播放 收藏 分享 下载 评论 歌曲列表 90首歌 播放:4次 生成外链播放器 加载中... 生命是场马拉松 卡比巴拉的海"
                            "2016年8月24日 - asdf 吉子可-- 2016-08-24 创建 播放 收藏 分享 下载 评论 歌曲列表 90首歌 播放:4次 生成外链播放器 加载中... 生命是场马拉松 卡比巴拉的海");

    ui->label_datetime->setText(Tools::timestampToDateTime(noteModel->contentModel->getCreateDate()));

    if (!noteModel->categoriesModel->getName().isEmpty()) {
        ui->label_categoryList->setText(noteModel->categoriesModel->getName());
    }

    if (noteModel->tagsModelList->length()) {
        QString text = "";
        for (int i = 0; i < noteModel->tagsModelList->length(); ++i) {
            text += noteModel->tagsModelList->at(i)->getName() + ", ";
        }
        text.chop(2);
        ui->label_tagList->setText(text);
    }

    if (!noteModel->contentModel->getCreateDate()) {
        ui->label_datetime->setText(Tools::timestampToDateTime(noteModel->contentModel->getCreateDate()));
    }
}

NoteListCellWidget::~NoteListCellWidget()
{
    delete ui;
}