// src\Via\QuickItem\BaseQQuickItem.h - something based
namespace syscross::TraffModel::Via::QuickItem {
class BaseQQuickItem : public QQuickItem, public Common {

public: 
    explicit BaseQQuickItem(QQuickItem *parent = nullptr) : 
		QQuickItem( parent )
	{
		setFlag( QQuickItem::ItemHasContents, true );
	}
    BaseQQuickItem(const BaseQQuickItem&) = delete;
    BaseQQuickItem& operator=(const BaseQQuickItem&) = delete;
};
} // namespace syscross::TraffModel::Via::QuickItem
