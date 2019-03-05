#include "netmanager.h"

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <asm/types.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <string.h>

#include <QProcess>

using namespace std;

#define BUFLEN 20480
#define ERROR "NLMSG_ERROR"

NetManager::NetManager(QObject *parent) : QObject(parent)
{

}

void NetManager::doWork()
{
    emit resultReady("Checking network ....");
    QProcess* terminal = new QProcess;
    //connect(terminal,SIGNAL(readyRead()),this,SLOT(readoutput()));
    //connect(terminal,SIGNAL(readyReadStandardOutput()),this, SLOT(readdstandoutput()));
    //connect(terminal,SIGNAL(readyReadStandardError()),this, SLOT(readerrooutput()));

    int fd, retval;
    char buf[BUFLEN] = {0};
    int len = BUFLEN;
    struct sockaddr_nl addr;
    struct nlmsghdr *nh;
    struct ifinfomsg *ifinfo;
    struct rtattr *attr;

    fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &len, sizeof(len));
    memset(&addr, 0, sizeof(addr));
    addr.nl_family = AF_NETLINK;
    addr.nl_groups = RTNLGRP_LINK;
    bind(fd, (struct sockaddr*)&addr, sizeof(addr));

    emit resultReady("Start while ....");
    while ((retval = read(fd, buf, BUFLEN)) > 0)
    {
        for (nh = (struct nlmsghdr *)buf; NLMSG_OK(nh, retval); nh = NLMSG_NEXT(nh, retval))
        {
            if (nh->nlmsg_type == NLMSG_DONE)
                break;
            else if (nh->nlmsg_type == NLMSG_ERROR)
                emit resultReady(ERROR);
            else if (nh->nlmsg_type != RTM_NEWLINK)
                continue;
            ifinfo = (ifinfomsg*)(NLMSG_DATA(nh));

            QString ni = "";
            attr = (struct rtattr*)(((char*)nh) + NLMSG_SPACE(sizeof(*ifinfo)));
            len = nh->nlmsg_len - NLMSG_SPACE(sizeof(*ifinfo));
            for (; RTA_OK(attr, len); attr = RTA_NEXT(attr, len))
            {
                if (attr->rta_type == IFLA_IFNAME)
                {
                    ni = QString((char*)RTA_DATA(attr));
                    break;
                }
            }

            if(!ni.isEmpty())
            {
                if(ifinfo->ifi_flags & IFF_LOWER_UP)
                {
                    emit resultReady("network state changed");
                    terminal->start("ifconfig " + ni + " 192.168.1.91 netmask 255.255.255.0");
                    terminal->waitForFinished();
                    terminal->start("route add -net 192.168.1.0 netmask 255.255.255.0 gw 0.0.0.0 dev " + ni);
                    terminal->waitForFinished();
                    terminal->start("route add -net 0.0.0.0 netmask 0.0.0.0 gw 192.168.1.1 dev " + ni);
                    terminal->waitForFinished();
                    terminal->start("ifconfig eth1 up");
                    terminal->waitForFinished();

                    if(ni == "eth1")
                    {
                        terminal->start("ping -c 4 192.168.1.91");
                    }
                    else if(ni == "eth0")
                    {
                        terminal->start("ping -c 4 192.168.0.90");
                    }
                    terminal->waitForFinished();

                    QByteArray qbaOutput = terminal->readAllStandardError();
                    QString qstrYouWant = qbaOutput;
                    if(!qstrYouWant.isEmpty())
                    {
                        emit resultReady(ni + " is unavaliable! Please check network connection.");
                    }
                    else
                    {
                        qbaOutput = terminal->readAllStandardOutput();
                        qstrYouWant = qbaOutput;

                        emit resultReady(ni + " is up and ready!");
                        emit resultReady(qstrYouWant);
                    }
                }
                else
                {
                    emit resultReady(ni + " is down!");
                }
            }

        }
    }

    //emit resultReady(result);
}
