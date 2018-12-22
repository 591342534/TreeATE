///
/// @project       TreeATE
/// @brief         Test Unit's model for QTreeView
/// @author        David Yin  2018-12 willage.yin@163.com
///
/// @license       GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "testunitsmodel.h"
#include "testunititem.h"

TestUnitsModel::TestUnitsModel(const QStringList &headers, QObject *parent)
        : QAbstractItemModel(parent)
{
    QVector<QVariant> rootData;
    foreach (QString header, headers)
        rootData << header;

    m_rootUnit = new TestUnitItem(rootData);
}

TestUnitsModel::~TestUnitsModel()
{
    if(m_rootUnit)
        delete m_rootUnit;
}


int TestUnitsModel::columnCount(const QModelIndex &) const
{
    if(m_rootUnit)
        return m_rootUnit->columnCount();
    return 0;
}

QVariant TestUnitsModel::getDataForColumn(const QModelIndex &index, int column)
{
    TestUnitItem* item = getItem(index);
    if(item)
        return item->data(column);
    return QVariant();
}

QVariant TestUnitsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();

    TestUnitItem *item = getItem(index);

    return item->data(index.column());
}


Qt::ItemFlags TestUnitsModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
}

TestUnitItem *TestUnitsModel::getItem(const QModelIndex &index) const
{
    if (index.isValid()) {
        TestUnitItem *item = static_cast<TestUnitItem*>(index.internalPointer());
        if (item)
            return item;
    }
    return m_rootUnit;
}

QVariant TestUnitsModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return m_rootUnit->data(section);

    return QVariant();
}


QModelIndex TestUnitsModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid() && parent.column() != 0)
        return QModelIndex();

    TestUnitItem *parentItem = getItem(parent);

    TestUnitItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

bool TestUnitsModel::insertColumns(int position, int columns, const QModelIndex &parent)
{
    if(NULL == m_rootUnit)
        return false;

    bool success;

    beginInsertColumns(parent, position, position + columns - 1);
    success = m_rootUnit->insertColumns(position, columns);
    endInsertColumns();

    return success;
}

bool TestUnitsModel::insertRows(int position, int rows, const QModelIndex &parent)
{
    TestUnitItem *parentItem = getItem(parent);
    if(NULL == parentItem || NULL == m_rootUnit)
        return false;

    bool success;

    beginInsertRows(parent, position, position + rows - 1);
    success = parentItem->insertChildren(position, rows, m_rootUnit->columnCount());
    endInsertRows();

    return success;
}

QModelIndex TestUnitsModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TestUnitItem *childItem = getItem(index);
    TestUnitItem *parentItem = childItem->parent();

    if (parentItem == m_rootUnit)
        return QModelIndex();

    return createIndex(parentItem->childNumber(), 0, parentItem);
}

bool TestUnitsModel::removeColumns(int position, int columns, const QModelIndex &parent)
{
    bool success;

    beginRemoveColumns(parent, position, position + columns - 1);
    success = m_rootUnit->removeColumns(position, columns);
    endRemoveColumns();

    if (m_rootUnit->columnCount() == 0)
        removeRows(0, rowCount());

    return success;
}

bool TestUnitsModel::removeRows(int position, int rows, const QModelIndex &parent)
{
    TestUnitItem *parentItem = getItem(parent);
    if(NULL == parentItem)
        return false;

    bool success = true;

    beginRemoveRows(parent, position, position + rows - 1);
    success = parentItem->removeChildren(position, rows);
    endRemoveRows();

    return success;
}

int TestUnitsModel::rowCount(const QModelIndex &parent) const
{
    TestUnitItem *parentItem = getItem(parent);
    if(NULL == parentItem)
        return 0;

    return parentItem->childCount();
}

bool TestUnitsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != Qt::EditRole)
        return false;

    TestUnitItem *item = getItem(index);
    bool result = item->setData(index.column(), value);

    if (result)
        emit dataChanged(index, index);

    return result;
}

bool TestUnitsModel::setHeaderData(int section, Qt::Orientation orientation,
                              const QVariant &value, int role)
{
    if (role != Qt::EditRole || orientation != Qt::Horizontal)
        return false;

    bool result = m_rootUnit->setData(section, value);

    if (result)
        emit headerDataChanged(orientation, section, section);

    return result;
}

QVector<QVariant> TestUnitsModel::getDataFromVM(const QVariantMap& vmData, bool isKey)
{
    QVector<QVariant> columnData;
    if(!isKey) {
        columnData << vmData["Name"].toString()
                   << vmData["Desc"].toString();
    }

    if(vmData["Parameter"].isValid()) {
         QVariantMap vmPara = vmData["Parameter"].toMap();
         for(auto itor = vmPara.begin(); itor != vmPara.end(); ++itor)
         {
             if(isKey)
                 columnData << itor.key();
             else
                 columnData << itor.value();
         }
    }

    return columnData;
}

bool TestUnitsModel::SetPrjData(const QVariant& data)
{
    if(NULL == m_rootUnit)
        return false;

    QVariantMap vmPrj = data.toMap();
    QVector<QVariant> headerData = getDataFromVM(vmPrj, true);
    int oldCnt = m_rootUnit->columnCount();
    m_rootUnit->insertColumns(m_rootUnit->columnCount(), headerData.count());
    for (int column = 0; column < headerData.size(); ++column) {
        setHeaderData(oldCnt + column, Qt::Horizontal, headerData[column]);
    }

    // Test Project data
    QVector<QVariant> prjData = getDataFromVM(vmPrj, false);
    int nCnt = m_rootUnit->childCount();
    m_rootUnit->insertChildren(nCnt, 1, m_rootUnit->columnCount());
    TestUnitItem* item = m_rootUnit->child(nCnt);
    for(int i = 0; i < prjData.size(); ++i) {
       item->setData(i, prjData[i]);
    }

    // Test Suite data
    QVariantList vlSuite = vmPrj["TestSuite"].toList();
    for(int x = 0; x < vlSuite.count(); ++x) {
        QVariantMap vmSuite = vlSuite.at(x).toMap();

        QVector<QVariant> suiteData = getDataFromVM(vmSuite, false);
        nCnt = item->childCount();
        item->insertChildren(nCnt, 1, m_rootUnit->columnCount());
        TestUnitItem* itemSuite = item->child(nCnt);
        for(int i = 0; i < suiteData.size(); ++i) {
            itemSuite->setData(i, suiteData[i]);
        }

        // Test Case data
        QVariantList vlCase = vmSuite["TestCase"].toList();
        for(int y = 0; y < vlCase.count(); ++y) {
            QVariantMap vmCase = vlCase.at(y).toMap();

            QVector<QVariant> caseData = getDataFromVM(vmCase, false);
            nCnt = itemSuite->childCount();
            itemSuite->insertChildren(nCnt, 1, m_rootUnit->columnCount());
            TestUnitItem* itemCase = itemSuite->child(nCnt);
            for(int j = 0; j < caseData.size(); ++j) {
                itemCase->setData(j, caseData[j]);
            }
        }
    }

    return true;
}

QVariant TestUnitsModel::GetPrjData()
{
    QVariant vPrj;
    return vPrj;
}