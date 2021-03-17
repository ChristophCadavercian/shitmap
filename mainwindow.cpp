#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "host.hpp"
#include "util.h"
#include <QtWidgets>
#include <QGridLayout>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QFile>
#include <QTextEdit>
#include <thread>         // std::this_thread::sleep_for
#include <chrono>

shitmap::shitmap(QWidget *parent) : QMainWindow(parent)
{

    //Setup scan info
    QString filStr="Scan filename: ";
    QString cmdline="Scan cmdline: ";
    QString start="Scan start time: ";
    QVBoxLayout *infoLayout = new QVBoxLayout;
    infoGroupBox = new QGroupBox(tr("&Current scan info:"));
    infoGroupBox->setStyleSheet("QGroupBox { font-weight: bold; background-color: yellow }");
    filenameLabel=new QLabel(filStr);
    cmdlineLabel = new QLabel(cmdline);
    startLabel = new QLabel(start);
    infoLayout->addWidget(filenameLabel);
    infoLayout->addWidget(cmdlineLabel);
    infoLayout->addWidget(startLabel);
    infoGroupBox->setLayout(infoLayout);

    //setup search stuff
    searchGroupBox = new QGroupBox(tr("Search:"));
    searchGroupBox->setAlignment(Qt::AlignHCenter);
    columnBox = new QComboBox;
    columnBox->addItem(tr("Ip"));
    columnBox->addItem(tr("Port"));
    columnBox->addItem(tr("Service"));
    columnBox->addItem(tr("Service info"));
    searchEdit=new QLineEdit;
    auto *searchByLabel = new QLabel("Search by");
    searchByLabel->setBuddy(columnBox);
    connect(searchEdit, &QLineEdit::textChanged, this, &shitmap::searchChanged);
    connect(columnBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &shitmap::columnChanged);
    QHBoxLayout* searchLayout = new QHBoxLayout;
    searchLayout->addWidget(searchByLabel);
    searchLayout->addWidget(columnBox);
    searchLayout->addWidget(searchEdit);
    searchGroupBox->setLayout(searchLayout);

    //setup main model
    auto central=new QWidget;
    proxyModel = new QSortFilterProxyModel;
    proxyView = new QTreeView;
    proxyView->setRootIsDecorated(false);
    proxyView->setAlternatingRowColors(true);
    proxyView->setSortingEnabled(true);
    QGridLayout *proxyLayout = new QGridLayout;
    proxyLayout->addWidget(proxyView, 0, 0, 1, 3);
    proxyGroupBox = new QGroupBox(tr("Current Scan Results"));
    proxyGroupBox->setLayout(proxyLayout);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(infoGroupBox);
    mainLayout->addWidget(proxyGroupBox);
    mainLayout->addWidget(searchGroupBox);
    create_menu();
    mainLayout->setMenuBar(menuBar());
    central->setLayout(mainLayout);
    setCentralWidget(central);
    setWindowTitle(tr("Shitmap"));
}

void shitmap::create_menu()
{
    //File option
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    //Open File
    auto* file_action=new QAction("Open new scan", this);
    connect(file_action, &QAction::triggered, this, &shitmap::slot_open_scan);
    fileMenu->addAction(file_action);

    auto* merge_action=new QAction("Merge scans", this);
    connect(merge_action, &QAction::triggered, this, &shitmap::slot_merge_scan);
    fileMenu->addAction(merge_action);

    //About option
    QMenu *aboutMenu = menuBar()->addMenu(tr("&About"));
    auto* about_action=new QAction("About Shitmap", this);
    connect(about_action, &QAction::triggered, this, &shitmap::about_shitmap);
    aboutMenu->addAction(about_action);

}

void shitmap::about_shitmap()
{
    QMessageBox::about(this, tr("About Shitmap"),
                tr("<h2>Shitmap 1.337</h2>"
                   "<p>Copyright 2021 by <b>noname</b>"
                   "<p>Shitmap is a shit application which "
                   "parses nmap scan result. "
     ));
}

void shitmap::slot_open_scan()
{
    proxyModel->removeRows(0, proxyModel->rowCount());
    cur_scan.clear();
    QString file_content;
    QString fileName = QFileDialog::getOpenFileName(this, "Select scan file");
    current_filename=fileName;
    if (!fileName.isEmpty())
        parse_file_to_scan(fileName);
    for(Host h:cur_scan.get_hosts())
    {
        add_entries_to_model(h);
    }
    update_scan_info();
    //textEdit->setPlainText(cur_scan.scan_to_str());
}

void shitmap::slot_merge_scan()
{
    cur_scan.clear();
    QString file_content;
    QString fileName = QFileDialog::getOpenFileName(this);
    current_filename=fileName;
    if (!fileName.isEmpty())
        parse_file_to_scan(fileName);
    for(Host h:cur_scan.get_hosts())
    {
        add_entries_to_model(h);
    }
    //textEdit->setPlainText(cur_scan.scan_to_str());
}

void shitmap::searchChanged()
{
    QRegExp regExp(searchEdit->text(), Qt::CaseInsensitive);
    proxyModel->setFilterRegExp(regExp);
}

void shitmap::columnChanged()
{
    proxyModel->setFilterKeyColumn(columnBox->currentIndex());
}

void shitmap::parse_file_to_scan(const QString &scan_filename)
{
    QFile scan_file(scan_filename);
    Host h;
    Port p;
    scan_file.open(QFile::ReadOnly | QFile::Text);
    QXmlStreamReader xmlReader;
    xmlReader.setDevice(&scan_file);
    while(!xmlReader.atEnd())
    {
        xmlReader.readNext();
        //parse header
        //find nmaprun node
        if(xmlReader.name()=="nmaprun" && xmlReader.tokenType()!=QXmlStreamReader::EndElement)
        {
            cur_scan.set_cmdline(get_attr(xmlReader, "args"));
            cur_scan.set_starttime(get_attr(xmlReader, "startstr"));
            continue;

        }
        //start parsing hosts
        if(xmlReader.name()=="address" && xmlReader.tokenType()!=QXmlStreamReader::EndElement)
        {
            //qDebug()<<"Address found\n";
            if(get_attr(xmlReader, "addrtype")=="ipv4")
                h.set_ip(get_attr(xmlReader, "addr"));
            continue;
        }
        if(xmlReader.name()=="hostname" && xmlReader.tokenType()!=QXmlStreamReader::EndElement)
        {
            h.set_hostname(get_attr(xmlReader, "name"));
            continue;
        }
        if(xmlReader.name()=="port" && xmlReader.tokenType()!=QXmlStreamReader::EndElement)
        {
           p.set_port_val(get_attr(xmlReader, "portid").toInt());
           xmlReader.readNext();
           p.set_port_state(get_attr(xmlReader, "state"));
           h.add_port(p);
           continue;

        }
        if(xmlReader.name()=="ports" && xmlReader.tokenType()==QXmlStreamReader::EndElement)
        {
            if(h.get_ports().length()>0)
                cur_scan.add_host(h);
            h.clear();
            continue;
        }

    }
    //qDebug()<<cur_scan.scan_to_str();

}

void shitmap::setSourceModel(QAbstractItemModel *model)
{
    proxyModel->setSourceModel(model);
    proxyView->setModel(proxyModel);
}

QAbstractItemModel *createMailModel(QObject *parent)
{
    QStandardItemModel *model = new QStandardItemModel(0, 3, parent);
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("Ip"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Port"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("State"));
    return model;
}

void shitmap::add_entries_to_model(Host &h)
{
    if(h.get_ip()=="10.238.156.252")
    {
        qDebug()<<"Got it\n";
    }
    if(h.get_ports().length()==0)
        return;
    QVector<Port> ports=h.get_ports();
    QVectorIterator<Port> p(ports);
    int i=0;
    while(p.hasNext())
    {

        Port pp=p.next();
        qDebug()<<"Trying to add host " << h.get_ip() << pp.get_port_num() << "/" << pp.get_port_state()<<'\n';
        if(!proxyModel->insertRow(0))
        {
            qDebug()<<"FAIL1\n";
            exit(0);
        }
        i++;
        if(!(proxyModel->setData(proxyModel->index(0,0), h.get_ip())))
        {
            qDebug()<<"FAIL2\n";
            exit(0);
        }
        if(!(proxyModel->setData(proxyModel->index(0,1), QString::number(pp.get_port_num()))))
        {
            qDebug()<<"FAIL3\n";
            exit(0);
        }
        qDebug()<<proxyModel->data(proxyModel->index(0,1));
        if(!(proxyModel->setData(proxyModel->index(0,2), pp.get_port_state())))
        {
            qDebug()<<"FAIL4\n";
            exit(0);
        }
    }
}

void shitmap::update_scan_info()
{
    QString filStr="Scan filename: "+current_filename;
    QString cmdline="Scan cmdline: "+cur_scan.get_cmdline();
    QString start="Scan start time: "+cur_scan.get_starttime();
    filenameLabel->setText(filStr);
    cmdlineLabel->setText(cmdline);
    startLabel->setText(start);
}
