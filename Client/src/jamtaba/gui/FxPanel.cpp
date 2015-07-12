#include "FxPanel.h"
//#include "ui_fxpanel.h"
#include "FxPanelItem.h"
#include "plugins/guis.h"
#include <QPainter>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QStyleOption>
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
FxPanel::FxPanel(QWidget *parent) :
    QWidget(parent),
    fxMenu(nullptr)
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(QMargins(2, 2, 2, 2));

    QWidget* contentPane = new QWidget(this);
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QVBoxLayout* contentLayout = new QVBoxLayout( contentPane);
    contentLayout->setContentsMargins(QMargins(1, 0, 2, 0));
    contentLayout->setSpacing(2);

    scrollArea->setWidget(contentPane);
    scrollArea->setWidgetResizable(true);
    mainLayout->addWidget(scrollArea);

    for(int i=0; i < 4; i++){
        FxPanelItem* item = new FxPanelItem(this);
        items.append(item);
        contentLayout->addWidget(item);
        QObject::connect(item, SIGNAL(editingPlugin(Audio::Plugin*)), this, SIGNAL(editingPlugin(Audio::Plugin*)));
        QObject::connect(item, SIGNAL(pluginRemoved(Audio::Plugin*)), this, SIGNAL(pluginRemoved(Audio::Plugin*)));
    }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void FxPanel::setFxMenu(QMenu *fxMenu){
    this->fxMenu = fxMenu;
    foreach (FxPanelItem* item, items) {
        item->setFxMenu(fxMenu);
    }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void FxPanel::addPlugin(Audio::Plugin * plugin){
    QList<FxPanelItem*> items = findChildren<FxPanelItem*>();
    for(FxPanelItem* item : items){//find the first free slot to put the new plugin
        if(!item->containPlugin()){
            item->setPlugin(plugin);
            return;
        }
    }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
FxPanel::~FxPanel()
{
    //delete ui;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void FxPanel::paintEvent(QPaintEvent* ){
    //default code to use stylesheets
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++