#include "thumbnailstrip.h"

// TODO: move item highlight logic to base class

ThumbnailStrip::ThumbnailStrip(QWidget *parent)
    : ThumbnailView(THUMBNAILVIEW_HORIZONTAL, parent),
      panelSize(100),
      thumbnailSpacing(0)
{
    this->setAttribute(Qt::WA_NoMousePropagation, true);
    this->setFocusPolicy(Qt::NoFocus);
    setupLayout();
    mWrapper.reset(new DirectoryViewWrapper(this));
}

void ThumbnailStrip::updateScrollbarIndicator() {
    qreal itemCenter = (qreal)(selectedIndex() + 0.5) / itemCount();
    if(scrollBar->orientation() == Qt::Horizontal)
        indicator = QRect(scrollBar->width() * itemCenter - indicatorSize, 0, indicatorSize, scrollBar->height());
    else
        indicator = QRect(0, scrollBar->height() * itemCenter - indicatorSize, scrollBar->width(), indicatorSize);
    scrollBar->update(indicator);
}

std::shared_ptr<DirectoryViewWrapper> ThumbnailStrip::wrapper() {
    return mWrapper;
}

//  no layout; manual item positioning
//  graphical issues otherwise
void ThumbnailStrip::setupLayout() {
    this->setAlignment(Qt::AlignLeft | Qt::AlignTop);
}

ThumbnailWidget* ThumbnailStrip::createThumbnailWidget() {
    ThumbnailWidget *widget = new ThumbnailWidget();
    widget->setThumbnailSize(this->mThumbnailSize);
    widget->setDrawLabel(true);
    return widget;
}

void ThumbnailStrip::ensureSelectedItemVisible() {

}

void ThumbnailStrip::addItemToLayout(ThumbnailWidget* widget, int pos) {
    scene.addItem(widget);
    updateThumbnailPositions(pos, thumbnails.count() - 1);
}

void ThumbnailStrip::removeItemFromLayout(int pos) {
    if(checkRange(pos)) {
        ThumbnailWidget *thumb = thumbnails.at(pos);
        scene.removeItem(thumb);
        // move items left
        ThumbnailWidget *tmp;
        for(int i = pos; i < thumbnails.count(); i++) {
            tmp = thumbnails.at(i);
            tmp->moveBy(-tmp->boundingRect().width(), 0);
        }
    }
}

void ThumbnailStrip::removeAll() {
    scene.clear(); // also calls delete on all items
    thumbnails.clear();
}

void ThumbnailStrip::updateThumbnailPositions() {
    updateThumbnailPositions(0, thumbnails.count() - 1);
}

void ThumbnailStrip::updateThumbnailPositions(int start, int end) {
    if(start > end || !checkRange(start) || !checkRange(end)) {
        return;
    }
    // assume all thumbnails are the same size
    int thumbWidth = static_cast<int>(thumbnails.at(start)->boundingRect().width()) + thumbnailSpacing;
    ThumbnailWidget *tmp;
    for(int i = start; i <= end; i++) {
        tmp = thumbnails.at(i);
        tmp->setPos(i * thumbWidth, 0);
    }
}

void ThumbnailStrip::focusOn(int index) {
    if(!checkRange(index))
        return;
    ThumbnailWidget *thumb = thumbnails.at(index);
    ensureVisible(thumb, 0, 0);
    loadVisibleThumbnails();
}

void ThumbnailStrip::ensureThumbnailVisible(int pos) {
    if(checkRange(pos))
        ensureVisible(thumbnails.at(pos)->sceneBoundingRect(),
                      mThumbnailSize / 2, 0);
}

// scene stuff??
void ThumbnailStrip::setThumbnailSize(int newSize) {
    if(newSize >= 20) {
        mThumbnailSize = newSize;
        for(int i=0; i<thumbnails.count(); i++) {
            thumbnails.at(i)->setThumbnailSize(newSize);
        }
        //scene.invalidate(scene.sceneRect());
        updateThumbnailPositions(0, thumbnails.count() - 1);
        fitSceneToContents();
        ensureThumbnailVisible(selectedIndex());
    }
}

// resizes thumbnailSize to fit new widget size
// TODO: find some way to make this trigger while hidden
void ThumbnailStrip::resizeEvent(QResizeEvent *event) {
    Q_UNUSED(event)
    ThumbnailView::resizeEvent(event);
    if(event->oldSize().height() != height())
        updateThumbnailSize();
    if(event->oldSize().width() < width())
        loadVisibleThumbnailsDelayed();
}

// update size based on widget's size
// reposition thumbnails within scene if needed
void ThumbnailStrip::updateThumbnailSize() {
    int newSize = height() - 25;
    if( newSize % 2 )
        --newSize;
    if(newSize != mThumbnailSize) {
        setThumbnailSize(newSize);
    }
}
