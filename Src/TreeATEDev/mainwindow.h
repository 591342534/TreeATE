///
/// @project       TreeATE
/// @brief         MainWindow of the TreeATEDev
/// @author        David Yin  2018-12 willage.yin@163.com
///
/// @license       GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVariant>

namespace Ui {
class MainWindow;
}

class TaScriptEdit;
class QTableView;
class QListView;
class QTreeView;
class TestUnitsModel;
class QStandardItemModel;
class QFileSystemModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    bool openProjectFile(const QString& strPrjFile);

private slots:
    void on_action_About_triggered();

    void on_action_Open_triggered();

    void on_testitems_clicked(const QModelIndex &index);

private:
    Ui::MainWindow *ui;
    TaScriptEdit*   m_scriptEdit;
    QTreeView*      m_tvModelsView;
    QFileSystemModel* m_fileSysModel;
    QListView*      m_lvFunction;
    QTreeView*      m_tvTestItems;
    TestUnitsModel* m_pUnitModel;
    QStandardItemModel*    m_tmPublicPara;
    QVariant        m_vaPrj;
};

#endif // MAINWINDOW_H