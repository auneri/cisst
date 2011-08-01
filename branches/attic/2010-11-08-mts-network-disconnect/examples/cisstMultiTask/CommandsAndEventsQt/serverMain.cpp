/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet, Min Yang Jung
  Created on: 2004-04-30

  (C) Copyright 2004-2010 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstOSAbstraction/osaThreadedLogFile.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask.h>

#include <QApplication>

#include "serverQtComponent.h"


int main(int argc, char * argv[])
{
    // set global component manager IP
    std::string globalComponentManagerIP;
    if (argc == 1) {
        std::cerr << "Using default, i.e. 127.0.0.1 to find global component manager" << std::endl;
        globalComponentManagerIP = "127.0.0.1";
    } else if (argc == 2) {
        globalComponentManagerIP = argv[1];
    } else {
        std::cerr << "Usage: " << argv[0] << " (global component manager IP)" << std::endl;
        return -1;
    }

    // log configuration
    cmnLogger::SetMask(CMN_LOG_ALLOW_ALL);
    cmnLogger::AddChannel(std::cout, CMN_LOG_ALLOW_ALL);

    // add a log per thread
    osaThreadedLogFile threadedLog("QtCommandsAndEventsServer-");
    cmnLogger::AddChannel(threadedLog, CMN_LOG_ALLOW_ALL);

    // set the log level of detail on select tasks
    cmnLogger::SetMaskClassMatching("mts", CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskClass("serverQtComponent", CMN_LOG_ALLOW_ALL);

    // create a Qt user interface
    QApplication application(argc, argv);

    // create our server task
    serverQtComponent * server = new serverQtComponent("Server");

    // Get the TaskManager instance and set operation mode
    // Get the local component manager
    mtsManagerLocal * localManager;
    try {
        localManager = mtsManagerLocal::GetInstance(globalComponentManagerIP, "cisstMultiTaskCommandsAndEventsQtServer");
    } catch (...) {
        CMN_LOG_INIT_ERROR << "Failed to initialize local component manager" << std::endl;
        return 1;
    }
    localManager->AddComponent(server);

    //
    // TODO: Hide this waiting routine inside mtsTaskManager using events or other things.
    //
    osaSleep(0.5 * cmn_s);

    // create and start all tasks
    localManager->CreateAll();
    localManager->StartAll();

    // run Qt user interface
    application.exec();

    // kill all tasks and perform cleanup
    localManager->KillAll();
    localManager->Cleanup();

    return 0;
}