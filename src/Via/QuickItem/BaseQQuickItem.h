// src\Via\QuickItem\BaseQQuickItem.h - something based
namespace syscross::TraffModel::Via::QuickItem {
class BaseQQuickItem : public QQuickItem {
	W_OBJECT( BaseQQuickItem ) //Q_OBJECT

public: 
    explicit BaseQQuickItem(QQuickItem *parent) : 
		QQuickItem( parent )
	{}
    BaseQQuickItem(const BaseQQuickItem&) = delete;
    BaseQQuickItem& operator=(const BaseQQuickItem&) = delete;
};
W_OBJECT_IMPL( BaseQQuickItem ) //Q_OBJECT
} // namespace syscross::TraffModel::Via::QuickItem
