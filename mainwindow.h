#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QApplication>
#include <QStandardItemModel>
#include <QTime>
#include <QPlainTextEdit>
#include "host.hpp"
QT_BEGIN_NAMESPACE
class QAbstractItemModel;
class QCheckBox;
class QComboBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QSortFilterProxyModel;
class QTreeView;
QT_END_NAMESPACE

class shitmap : public QMainWindow
{
    Q_OBJECT
private:
     QGroupBox* infoGroupBox;
     QLabel* filenameLabel;
     QLabel* cmdlineLabel;
     QLabel* startLabel;
     QSortFilterProxyModel *proxyModel;
     QPlainTextEdit *textEdit;
     QString current_filename;
     Scan cur_scan;
     QTreeView *proxyView;
     QGroupBox *searchGroupBox;
     QGroupBox *proxyGroupBox;
     QLineEdit *searchEdit;
     QComboBox *columnBox;
public:
    shitmap(QWidget *parent = nullptr);
    QString open_file(const QString& fileName);
    void create_menu();
    void parse_file_to_scan(const QString& scan_filename);
    void setSourceModel(QAbstractItemModel *model);
    Scan& get_scan() {return cur_scan;}
    void add_entries_to_model(Host& h);
    void update_scan_info();
private slots:
    void slot_open_scan();
    void slot_merge_scan();
    void searchChanged();
    void columnChanged();
    void about_shitmap();

};

QAbstractItemModel *createMailModel(QObject *parent);
void addEntries(QAbstractItemModel *model, const Host& h);
#endif // MAINWINDOW_H
