/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/* $Id$ */

#ifndef _UITask_h
#define _UITask_h

#include <cisstMultiTask.h>
#include "displayUI.h"

class UITask: public mtsTaskPeriodic {
    // set log level to 5
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);
    
protected:
    // local copy of data used in commands
    mtsDouble Data;
    mtsDouble AmplitudeData;

    // user interface generated by FTLK/fluid
    displayUI UI;
    
 public:
    // see sineTask.h documentation
    UITask(const std::string & taskName, double period);
    ~UITask() {};
    void Configure(const std::string & CMN_UNUSED(filename) = "");
    void Startup(void);
    void Run(void);
    void Cleanup(void) {};    
};

CMN_DECLARE_SERVICES_INSTANTIATION(UITask);

#endif // _UITask_h

/*
  Author(s):  Min Yang Jung
  Created on: 2009-04-25

  (C) Copyright 2004-2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/
