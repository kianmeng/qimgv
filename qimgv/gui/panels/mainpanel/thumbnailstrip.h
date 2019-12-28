#pragma once

#include <QApplication>
#include <QLabel>
#include <QBoxLayout>
#include <QScrollArea>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QScrollBar>
#include <QPainter>
#include <QResizeEvent>
#include <cmath>

#include "gui/customwidgets/thumbnailview.h"
#include "gui/directoryviewwrapper.h"
#include "sourcecontainers/thumbnail.h"

class ThumbnailStrip : public ThumbnailView
{
    Q_OBJECT
public:
    explicit ThumbnailStrip(QWidget *parent = nullptr);

    std::shared_ptr<DirectoryViewWrapper> wrapper();

private:
    int panelSize;

    int thumbnailSpacing;

    void updateThumbnailPositions(int start, int end);
    void updateThumbnailPositions();
    void setThumbnailSize(int);
    void updateThumbnailSize();
    void setupLayout();

    std::shared_ptr<DirectoryViewWrapper> mWrapper;

public slots:
    virtual void focusOn(int index);

protected:
    virtual void resizeEvent(QResizeEvent *event);
    virtual void updateScrollbarIndicator();
    void ensureThumbnailVisible(int pos);
    void addItemToLayout(ThumbnailWidget *widget, int pos);
    void removeItemFromLayout(int pos);
    void removeAll();
    ThumbnailWidget *createThumbnailWidget();
    void ensureSelectedItemVisible();
};
