/***************************************************************************
 *   KSystemLog, a system log viewer tool                                  *
 *   Copyright (C) 2007 by Nicolas Ternisien                               *
 *   nicolas.ternisien@gmail.com                                           *
 *   Copyright (C) 2015 by Vyacheslav Matyushin                            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.          *
 ***************************************************************************/

#ifndef _JOURNALD_NETWORK_ANALYZER_H_
#define _JOURNALD_NETWORK_ANALYZER_H_

#include "analyzer.h"
#include "logFile.h"

#include <QtNetwork/QtNetwork>

class JournaldNetworkAnalyzer : public Analyzer
{
    Q_OBJECT

public:
    explicit JournaldNetworkAnalyzer(LogMode *logMode, QString address, quint16 port,
                                     QString filter = QString());

    virtual ~JournaldNetworkAnalyzer();

    virtual LogViewColumns initColumns();

    virtual void setLogFiles(const QList<LogFile> &logFiles);

    virtual void watchLogFiles(bool enabled);

private slots:
    void httpFinished();
    void httpReadyRead();
    void updateDataReadProgress(qint64 bytesRead, qint64 totalBytes);
    void error(QNetworkReply::NetworkError code);

private:
    QString m_url;
    QNetworkAccessManager m_networkManager;
    QNetworkReply *m_reply;
    QByteArray m_data;
};

#endif // _JOURNALD_NETWORK_ANALYZER_H_