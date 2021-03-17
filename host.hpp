#ifndef HOST_HPP
#define HOST_HPP
#include <QXmlStreamReader>
#include <QApplication>
#include <QFile>
#include <vector>
#include <iostream>
#include <QtDebug>
#include <algorithm>
#include <QVector>
#include <string>
#include <QGroupBox>
#include <QLabel>
class Port
{
private:
    int port_val;
    QString state;
    QString port_info="";
public:
    Port(){};
    Port(int port_num, QString c_state)
    {
        port_val=port_num;
        state=c_state;
    }

    int get_port_num() const { return port_val;}
    QString get_port_state() const { return state;}
    QString port_to_str();
    void set_port_val(int _port_val) { port_val=_port_val;}
    void set_port_state(QString _state) {state=_state;}
    void set_port_info(QString _info) {port_info=_info;}
};

bool operator< (const Port &p1, const Port &p2);
class Host
{
private:
    QString ip;
    QString hostname;
    QVector<Port> ports;


public:
    Host() {};

    Host(QString c_ip, QString c_hostname)
    {
        ip=c_ip;
        hostname=c_hostname;
    }

    Host(QString c_ip, QString c_hostname, QVector<Port> c_ports)
    {
        ip=c_ip;
        hostname=c_hostname;
        ports=c_ports;
    }

    QString get_ip() const {return ip;}
    QString get_hostname() const {return hostname;}
    QVector<Port> const & get_ports() {return ports;}
    void add_port(Port port);
    void sort();
    void set_ip(const QString &_ip);
    void set_hostname(const QString &_hostname);
    QString host_to_str();
    void clear();

};

class Scan
{
private:
    QString cmdline;
    QString start_time;
    QVector<Host> hosts;
public:
    Scan() {};
    void set_cmdline(const QString& _cmdline);
    void set_starttime(const QString& starttime);
    void add_host(const Host& host);
    QString get_cmdline() {return cmdline;}
    QString get_starttime() {return start_time;}
    QVector<Host>& get_hosts() {return hosts;}
    QString scan_to_str();
    void clear();
};

#endif // HOST_HPP
