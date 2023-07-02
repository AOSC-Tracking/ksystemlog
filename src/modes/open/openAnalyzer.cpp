/*
    SPDX-FileCopyrightText: 2007 Nicolas Ternisien <nicolas.ternisien@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "openAnalyzer.h"

OpenAnalyzer::OpenAnalyzer(LogMode *logMode)
    : SyslogAnalyzer(logMode)
{
}

#include "moc_openAnalyzer.cpp"
