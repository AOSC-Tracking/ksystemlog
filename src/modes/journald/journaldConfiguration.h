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

#ifndef _JOURNALD_CONFIGURATION_H_
#define _JOURNALD_CONFIGURATION_H_

#include "logModeConfiguration.h"

#include "defaults.h"

#include "journaldLogMode.h"

#include "ksystemlogConfig.h"

class JournaldConfiguration : public LogModeConfiguration
{
    Q_OBJECT

public:
    JournaldConfiguration();

    virtual ~JournaldConfiguration() {}

    bool displayCurrentBootOnly() const;
    void setDisplayCurrentBootOnly(bool displayCurrentBootOnly);

    bool displayCurrentUserProcesses() const;
    void setDisplayCurrentUserProcesses(bool displayCurrentUserProcesses);

    bool displaySystemServices() const;
    void setDisplaySystemServices(bool displaySystemServices);

    struct RemoteJournalAddress {
        QString address;
        QString port;
    };

    QList<RemoteJournalAddress> remoteJournals() const;
    void setRemoteJournals(const QList<RemoteJournalAddress> &remoteJournals);

private:
    bool m_displayCurrentBootOnly;
    bool m_displayCurrentUserProcesses;
    bool m_displaySystemServices;
    QStringList m_remoteJournals;
};

#endif // _JOURNALD_CONFIGURATION_H_