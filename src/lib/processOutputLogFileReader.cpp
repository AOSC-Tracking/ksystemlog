/***************************************************************************
 *   KSystemLog, a system log viewer tool                                  *
 *   Copyright (C) 2007 by Nicolas Ternisien                               *
 *   nicolas.ternisien@gmail.com                                           *
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

#include "processOutputLogFileReader.h"

#include <QTimer>
#include <QStringList>

#include <KLocalizedString>

#include "logFileReaderPrivate.h"

#include "logging.h"

class ProcessOutputLogFileReaderPrivate : public LogFileReaderPrivate
{
public:
    long previousLinesCount;

    QTimer processUpdater;

    QProcess *process = nullptr;

    QString buffer;
    QStringList availableStandardOutput;
};

ProcessOutputLogFileReader::ProcessOutputLogFileReader(const LogFile &logFile)
    : LogFileReader(*new ProcessOutputLogFileReaderPrivate(), logFile)
{
    init();
}

ProcessOutputLogFileReader::ProcessOutputLogFileReader(ProcessOutputLogFileReaderPrivate &dd, const LogFile &logFile)
    : LogFileReader(dd, logFile)
{
    init();
}

ProcessOutputLogFileReader::~ProcessOutputLogFileReader()
{
    // d pointer is deleted by the parent class
}

void ProcessOutputLogFileReader::init()
{
    Q_D(ProcessOutputLogFileReader);

    // Init current file position
    d->previousLinesCount = 0;
    d->availableStandardOutput.clear();
    d->process = nullptr;

    d->processUpdater.setInterval(PROCESS_OUTPUT_UPDATER_INTERVAL);
    connect(&(d->processUpdater), &QTimer::timeout, this, &ProcessOutputLogFileReader::startProcess);

    logDebug() << "Using process name " << d->logFile.url().toLocalFile();
}

void ProcessOutputLogFileReader::watchFile(bool enable)
{
    Q_D(ProcessOutputLogFileReader);

    if (enable == true) {
        logDebug() << "Monitoring process : " << d->logFile.url().toLocalFile();

        // Reinit current file position
        d->previousLinesCount = 0;

        // Launch the timer
        d->processUpdater.start();

        // Launch immediately the process updater
        startProcess();
    } else {
        // Stop regularly start process
        d->processUpdater.stop();
    }
}

void ProcessOutputLogFileReader::startProcess()
{
    logDebug() << "Starting process...";

    Q_D(ProcessOutputLogFileReader);

    if (d->logFile.url().isValid() == false) {
        const QString message(i18n("This file is not valid. Please adjust it in the settings of KSystemLog."));
        Q_EMIT errorOccured(i18n("File Does Not Exist"), message);
        Q_EMIT statusBarChanged(message);
    }

    logDebug() << "Starting process...";

    d->process = new QProcess();
    connect(d->process, &QProcess::readyReadStandardOutput, this, &ProcessOutputLogFileReader::logFileModified);
    connect(d->process, SIGNAL(finished(int,QProcess::ExitStatus)), this,
            SLOT(emitProcessOutput(int,QProcess::ExitStatus)));

    d->process->start(d->logFile.url().toLocalFile(), QStringList(), QIODevice::ReadOnly | QIODevice::Text);

    d->process->waitForStarted();

    logDebug() << "Process started";
}

void ProcessOutputLogFileReader::closeProcess()
{
    logDebug() << "Closing process...";

    Q_D(ProcessOutputLogFileReader);

    // Get the size file for the next calculation
    d->previousLinesCount = d->availableStandardOutput.count();
    logDebug() << "New lines count : " << d->previousLinesCount << " (" << d->logFile.url().toLocalFile() << ")";

    d->availableStandardOutput.clear();

    if (d->process) {
        d->process->close();
        delete d->process;
        d->process = nullptr;
    }

    logDebug() << "Process closed";
}

void ProcessOutputLogFileReader::emitProcessOutput(int /*exitCode*/, QProcess::ExitStatus exitStatus)
{
    Q_D(ProcessOutputLogFileReader);

    // First commit last lines of the buffer to the line list
    emptyBuffer();

    logDebug() << "Process terminated" << d->previousLinesCount << "previously /"
               << d->availableStandardOutput.count() << "currently";

    if (exitStatus == QProcess::CrashExit) {
        QString message(i18n("The process '%1' crashed.", d->logFile.url().toLocalFile()));
        Q_EMIT errorOccured(i18n("Process Crashed"), message);
        Q_EMIT statusBarChanged(message);
    }

    // If there is no new lines
    if (d->previousLinesCount == d->availableStandardOutput.count()) {
        /*
        //Q_EMIT an empty log lines list
        Q_EMIT contentChanged(this, false, QStringList());
        */
    }
    // If there are new lines in the file, insert only them or this is the first time we read this file
    else if (d->previousLinesCount != 0 && d->previousLinesCount <= d->availableStandardOutput.count()) {
        logDebug() << "Reading from line " << d->previousLinesCount << " (" << d->logFile.url().toLocalFile() << ")";

        QStringList newOutputs;

        int index = d->previousLinesCount - 1;
        while (index < d->availableStandardOutput.count()) {
            newOutputs.append(d->availableStandardOutput.at(index));

            ++index;
        }

        logDebug() << "Retrieving a part of the file...";

        Q_EMIT contentChanged(this, Analyzer::UpdatingRead, newOutputs);
    }
    // Else reread all lines, clear log list
    else {
        logDebug() << "New process or process already read. Reading entire content";

        Q_EMIT contentChanged(this, Analyzer::FullRead, d->availableStandardOutput);
    }

    closeProcess();
}

void ProcessOutputLogFileReader::logFileModified()
{
    Q_D(ProcessOutputLogFileReader);

    logDebug() << "Content available on process output...";

    // New added lines
    QByteArray bytesOutput = d->process->readAllStandardOutput();
    d->buffer.append(QLatin1String(bytesOutput));

    // Parse buffer
    int endLinePos = d->buffer.indexOf(QLatin1String("\n"));
    forever {
        if (endLinePos == -1) {
            break;
        }

        // Add the new found lines and
        d->availableStandardOutput.append(d->buffer.left(endLinePos));
        d->buffer.remove(0, endLinePos + 1);

        endLinePos = d->buffer.indexOf(QLatin1String("\n"));
    }

    logDebug() << "Received a total of" << d->availableStandardOutput.count() << "new lines";
}

/**
 * The buffer could contains some last characters that are added at last
 * (Normally useless)
 */
void ProcessOutputLogFileReader::emptyBuffer()
{
    Q_D(ProcessOutputLogFileReader);

    if (d->buffer.isEmpty() == false) {
        logWarning() << "Buffer was not empty !!";
        d->availableStandardOutput.append(d->buffer);
        d->buffer.clear();
    }
}
