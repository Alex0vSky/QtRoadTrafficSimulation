// src\Via\QuickPaintedItem\BaseQQuickPaintedItem.h - something based
namespace syscross::TraffModel::Via::QuickPaintedItem {
class BaseQQuickPaintedItem : public QQuickPaintedItem, public Common {
	using QQuickPaintedItem::QQuickPaintedItem;

	BaseQQuickPaintedItem(const BaseQQuickPaintedItem&) = delete;
	BaseQQuickPaintedItem& operator=(const BaseQQuickPaintedItem&) = delete;
};
} // namespace syscross::TraffModel::Via::QuickPaintedItem
