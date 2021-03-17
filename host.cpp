#include "host.hpp"

bool operator< (const Port &p1, const Port &p2)
{
    QString p1_state=p1.get_port_state();
    QString p2_state=p2.get_port_state();
    if(p1_state==p2_state)
        return p1.get_port_num() < p2.get_port_num();
    if(p1_state=="open")
        return true;
    return false;
}

QString Port::port_to_str()
{
    QString port_format="%1/%2 - %3";
    QString q=QString::fromStdString(std::to_string(port_val));
    return port_format.arg(q, state, port_info );
}

void Host::add_port(Port port)
{
    ports.append(port);
}

void Host::sort()
{
    std::sort(ports.begin(), ports.end());
}

void Host::set_ip(const QString &_ip)
{
    ip=_ip;

}

void Host::set_hostname(const QString &_hostname)
{
    hostname=_hostname;
}

QString Host::host_to_str()
{
    QString host_header_format="%1 (%2)";
    host_header_format=host_header_format.arg(ip, hostname);
    for (Port port:ports)
    {
        host_header_format+="\n";
        host_header_format+=port.port_to_str();
    }
    return host_header_format;
}

void Host::clear()
{
    ports.clear();
}

void Scan::set_cmdline(const QString &_cmdline)
{
    cmdline=_cmdline;
}

void Scan::set_starttime(const QString &starttime)
{
    start_time=starttime;
}

void Scan::add_host(const Host& host)
{
    hosts.push_back(host);
}

QString Scan::scan_to_str()
{
    QString scan_str="";
    QString scan_header_format="Scan was started on %1\nCommand line of scan: %2\n";
    scan_header_format=scan_header_format.arg(start_time, cmdline);
    scan_str+=scan_header_format;
    QString host_str_format="\nHost %1 of %2: ";
    for(int i=0;i<hosts.length();i++)
    {
        scan_str+=host_str_format.arg(QString::number(i+1), QString::number(hosts.length()));
        scan_str+=hosts[i].host_to_str();
    }
    return scan_str;
}

void Scan::clear()
{
    for(Host h:hosts)
        h.clear();
    hosts.clear();
}

