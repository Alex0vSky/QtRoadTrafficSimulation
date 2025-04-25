// src\Via\QuickItem\BaseQQuickItem.h - something based
namespace syscross::TraffModel::Via::QuickItem {
/**
 * Base QQuickItem with common initialization
 */
class BaseQQuickItem : public QQuickItem, public Common {
public: 
	/**
	 * @brief Constructs the base QQuickItem
	 * @param parent Optional parent QQuickItem
	 */
    explicit BaseQQuickItem(QQuickItem *parent = nullptr) : 
		QQuickItem( parent )
	{
		setFlag( QQuickItem::ItemHasContents, true );
	}
	// Disable copying and moving
    BaseQQuickItem(const BaseQQuickItem&) = delete;
    BaseQQuickItem& operator=(const BaseQQuickItem&) = delete;
};
} // namespace syscross::TraffModel::Via::QuickItem
