/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: $

  Author(s):  Anton Deguet
  Created on: 2011-12-15

  (C) Copyright 2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#ifndef _mtsComponentAddLatency_h
#define _mtsComponentAddLatency_h

#include <cisstMultiTask/mtsTaskPeriodic.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>

// Always include last
#include <cisstMultiTask/mtsExport.h>

/*!
  \ingroup cisstMultiTask

  Base class for component with latency.  This class can be used to
  create a component that delays all commands and events.  For
  example, one might want to simulate the latency introduced by the
  network between a master and a slave for a tele-operation task.  The
  delay component can then be inserted between the client and server.
  It must replicate the server provided interface(s) so that the
  client will connect its required interface(s) as if it was the
  server itself.  Let Cs the server component, Cc the client component
  and Cd the delay component.  Cd will need to be created with the
  same provided interfaces as Cs.  Then, instead of connecting Cc to
  Cs, we will connect Cc to Cd and Cd to Cs.

  The derived component needs to create it's required and provided
  interfaces using the methods AddInterfaceRequired and
  AddInterfaceProvided.  Once the interfaces are created, commands can
  be added using AddCommandVoidDelayed, AddCommandReadDelayed, ...

  At runtime, the delay component will continuously read from the
  server and buffer the results in a state table.  It is important to
  set the periodicity of the delay component so that enough data is
  collected but make sure the frequency is not too high either.  For
  void and write commands as well as events, the component buffers all
  requests and periodically check which commands/events should be
  forwarded.  Therefore the actual latency will be impacted by the
  period used be the delay component.
 */
class CISST_EXPORT mtsComponentAddLatency: public mtsTaskPeriodic
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

 protected:

    struct DelayedRead
    {
        bool Valid;
        std::string Name;
        mtsFunctionRead Function;
        mtsGenericObject * PlaceHolder;
    };

    struct DelayedVoid
    {
        std::string Name;
        mtsFunctionVoid Function;
        void Method(void);
    };

    struct DelayedWrite
    {
        std::string Name;
        mtsFunctionWrite Function;
        mtsCommandWriteBase * Command;
        void Method(const mtsGenericObject & data);
    };

    typedef std::list<DelayedRead *> DelayedReadList;
    DelayedReadList DelayedReads;

    typedef std::list<DelayedVoid *> DelayedVoidList;
    DelayedVoidList DelayedVoids;

    typedef std::list<DelayedWrite *> DelayedWriteList;
    DelayedWriteList DelayedWrites;

 public:
    // constructor
    mtsComponentAddLatency(const std::string & componentName, double periodInSeconds);
    virtual ~mtsComponentAddLatency();

    // methods defined as virtual in base class
    void Configure(const std::string & filename);
    void Startup(void);
    void Run(void);
    void Cleanup(void);

    double SetLatency(double latencyInSeconds);

 protected:

    /*! Add a read command with latency.  The provided and required
      interfaces must be created before using the methods
      AddInterfaceRequired and AddInterfaceProvided.  The command name
      should match the command name from the source component provided
      interface.  By default the provided interface with delay will
      use the same command name. */
    template <class _elementType>
    bool AddCommandReadDelayed(mtsInterfaceRequired * interfaceRequired,
                               const std::string & commandRequiredName,
                               mtsInterfaceProvided * interfaceProvided,
                               const std::string & commandProvidedName = "");

    bool AddCommandVoidDelayed(mtsInterfaceRequired * interfaceRequired,
                               const std::string & commandRequiredName,
                               mtsInterfaceProvided * interfaceProvided,
                               const std::string & commandProvidedName = "");

    template <class _elementType>
    bool AddCommandWriteDelayed(mtsInterfaceRequired * interfaceRequired,
                                const std::string & commandRequiredName,
                                mtsInterfaceProvided * interfaceProvided,
                                const std::string & commandProvidedName = "");

    bool AddEventVoidDelayed(mtsInterfaceRequired * interfaceRequired,
                             const std::string & eventRequiredName,
                             mtsInterfaceProvided * interfaceProvided,
                             const std::string & eventProvidedName = "");

    double Latency;
    mtsStateTable LatencyStateTable;

 private:
    bool AddCommandReadDelayedInternal(mtsGenericObject & data,
                                       mtsInterfaceRequired * interfaceRequired,
                                       const std::string & commandRequiredName);

    bool AddCommandWriteDelayedInternal(const mtsGenericObject & data,
                                        mtsInterfaceRequired * interfaceRequired,
                                        const std::string & commandRequiredName,
                                        mtsInterfaceProvided * interfaceProvided,
                                        const std::string & commandProvidedName);

};



template <class _elementType>
bool mtsComponentAddLatency::AddCommandReadDelayed(mtsInterfaceRequired * interfaceRequired,
                                                   const std::string & commandRequiredName,
                                                   mtsInterfaceProvided * interfaceProvided,
                                                   const std::string & commandProvidedName)
{
    // data object is used later to store the result of function
    // reads, state teble will keep a pointer on that object so it
    // must not be deleted until the task is killed.
    _elementType * data = new _elementType;
    this->LatencyStateTable.AddData(*data);
    interfaceProvided->AddCommandReadStateDelayed(this->LatencyStateTable,
                                                  *data,
                                                  commandProvidedName == ""
                                                  ? commandRequiredName
                                                  : commandProvidedName);
    return
        this->AddCommandReadDelayedInternal(*data, interfaceRequired, commandRequiredName);
}


template <class _elementType>
bool mtsComponentAddLatency::AddCommandWriteDelayed(mtsInterfaceRequired * interfaceRequired,
                                                    const std::string & commandRequiredName,
                                                    mtsInterfaceProvided * interfaceProvided,
                                                    const std::string & commandProvidedName)
{
    // data object is used to create an argument prototype but won't
    // be used afterwards
    _elementType data;
    return
        this->AddCommandWriteDelayedInternal(data,
                                             interfaceRequired,
                                             commandRequiredName,
                                             interfaceProvided,
                                             commandProvidedName == ""
                                             ? commandRequiredName
                                             : commandProvidedName);
}


CMN_DECLARE_SERVICES_INSTANTIATION(mtsComponentAddLatency)


#endif // _mtsComponentAddLatency_h

