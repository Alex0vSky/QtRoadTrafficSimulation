// src\Via\QuickPaintedItem\BaseQQuickPaintedItem.h - something based
namespace syscross::TraffModel::Via::QuickPaintedItem {
class BaseQQuickPaintedItem : public QQuickPaintedItem, public Common {
	W_OBJECT( BaseQQuickPaintedItem ) //Q_OBJECT
	using QQuickPaintedItem::QQuickPaintedItem;

	BaseQQuickPaintedItem(const BaseQQuickPaintedItem&) = delete;
	BaseQQuickPaintedItem& operator=(const BaseQQuickPaintedItem&) = delete;
};
W_OBJECT_IMPL( BaseQQuickPaintedItem ) //Q_OBJECT
} // namespace syscross::TraffModel::Via::QuickPaintedItem
