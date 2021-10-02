/*
    SPDX-FileCopyrightText: 2007 Nicolas Ternisien <nicolas.ternisien@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "logModeConfigurationWidget.h"

#include <QVBoxLayout>

#include <KLocalizedString>

#include "globals.h"
#include "ksystemlog_debug.h"

#include "logLevelFileList.h"

#include "logLevel.h"

#include "systemConfiguration.h"

#include "systemLogMode.h"

class SystemConfigurationWidget : public LogModeConfigurationWidget
{
    Q_OBJECT

public:
    SystemConfigurationWidget();

    ~SystemConfigurationWidget() override
    {
    }

    bool isValid() const override;

    void saveConfig() override;

    void readConfig() override;

    void defaultConfig() override;

private:
    LogLevelFileList *mFileList = nullptr;
};

