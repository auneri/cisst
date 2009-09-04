/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: main.cpp 746 2009-08-27 23:20:34Z bvagvol1 $

  Author(s):	Balazs Vagvolgyi, Simon DiMaio, Anton Deguet
  Created on:	2008-05-23

  (C) Copyright 2008-2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

// temporary fix to configure input
// possible values:
#define UI3_NO_INPUT 0
#define UI3_OMNI1 1
#define UI3_OMNI1_OMNI2 2

// change this based on your configuration
#define UI3_INPUT UI3_OMNI1_OMNI2

#include <cisstOSAbstraction/osaThreadedLogFile.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsTaskManager.h>
#include <cisstDevices/devSensableHD.h>
#include <cisstCommon.h>
#include <cisstStereoVision.h>

#include <SimpleBehavior.h>
#include <BehaviorWithSlave.h>

int main()
{
    // log configuration
    cmnLogger::SetLoD(CMN_LOG_LOD_VERY_VERBOSE);
	cmnLogger::GetMultiplexer()->AddChannel(std::cout, CMN_LOG_LOD_VERY_VERBOSE);
    // add a log per thread
    osaThreadedLogFile threadedLog("example1-");
    cmnLogger::GetMultiplexer()->AddChannel(threadedLog, CMN_LOG_LOD_VERY_VERBOSE);
    // specify a higher, more verbose log level for these classes
    cmnClassRegister::SetLoD("ui3BehaviorBase", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("ui3Manager", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("mtsTaskInterface", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("mtsTaskManager", CMN_LOG_LOD_VERY_VERBOSE);

    mtsTaskManager * taskManager = mtsTaskManager::GetInstance();

    devSensableHD * sensable = new devSensableHD("Omni", "Omni1", "Omni2" /* name in driver, see Preferences in Sensable Driver */);
    taskManager->AddTask(sensable);

    ui3Manager guiManager;

    SimpleBehavior behavior("Example1");
    //BehaviorWithSlave behavior2("Example2");

    guiManager.AddBehavior(&behavior,       // behavior reference
                           0,               // position in the menu bar: default
                           "circle.png");   // icon file: no texture

   // guiManager.AddBehavior(&behavior2,       // behavior reference
    //                       2,             // position in the menu bar: default
    //                       "square.png");            // icon file: no texture

    guiManager.Configure("config.xml");


////////////////////////////////////////////////////////////////
// setup video stream
#if 0
#ifndef RENDER_ON_OVERLAY
    svlStreamManager vidStream(2);  // running on multiple threads

    svlFilterSourceVideoCapture vidBackgroundSource(true); // stereo source
    cout << "Setup LEFT camera:" << endl;
    vidBackgroundSource.DialogSetup(SVL_LEFT);
    cout << "Setup RIGHT camera:" << endl;
    vidBackgroundSource.DialogSetup(SVL_RIGHT);
    vidStream.Trunk().Append(&vidBackgroundSource);

#ifdef CAPTURE_SWAP_RGB
    svlFilterRGBSwapper vidRGBSwapper;
    vidStream.Trunk().Append(&vidRGBSwapper);
#endif //CAPTURE_SWAP_RGB

    // add guiManager as a filter to the pipeline, so it will receive video frames
    // "StereoVideo" is defined in the UI Manager as a possible video interface
    vidStream.Trunk().Append(guiManager.GetStreamSamplerFilter("StereoVideo"));

    vidStream.Initialize();
#endif //RENDER_ON_OVERLAY
#endif
////////////////////////////////////////////////////////////////
// setup renderers

    svlCameraGeometry camera_geometry;
    // Load Camera calibration results
//#if (UI3_INPUT == UI3_DAVINCI)
//    camera_geometry.LoadCalibration("/home/saw1/calibration/davinci_mock_or/calib_results.txt");
//#else
//    camera_geometry.LoadCalibration("D:/Development/calib_results.txt");
//#endif
    // Center world in between the two cameras (da Vinci specific)
//    camera_geometry.SetWorldToCenter();
    // Rotate world by 180 degrees (VTK specific)
//    camera_geometry.RotateWorldAboutY(180.0);
    // Display camera configuration
    std::cerr << camera_geometry;
#if 0
#ifdef RENDER_ON_OVERLAY

    // *** Left view ***
    guiManager.AddRenderer(svlRenderTargets::Get(0)->GetWidth(),  // render width
                           svlRenderTargets::Get(0)->GetHeight(), // render height
                           0, 0,                                  // window position
                           camera_geometry, SVL_LEFT,             // camera parameters
                           "LeftEyeView");                        // name of renderer

    // *** Right view ***
    guiManager.AddRenderer(svlRenderTargets::Get(1)->GetWidth(),  // render width
                           svlRenderTargets::Get(1)->GetHeight(), // render height
                           0, 0,                                  // window position
                           camera_geometry, SVL_RIGHT,             // camera parameters
                           "RightEyeView");                       // name of renderer

    // Sending renderer output to external render targets
    guiManager.SetRenderTargetToRenderer("LeftEyeView",  svlRenderTargets::Get(0));
    guiManager.SetRenderTargetToRenderer("RightEyeView", svlRenderTargets::Get(1));

#else //RENDER_ON_OVERLAY

    // *** Left view ***
    guiManager.AddRenderer(800,  // render width
                           600, // render height
                           0, 0,                                    // window position
                           camera_geometry, SVL_LEFT,               // camera parameters
                           "LeftEyeView");                          // name of renderer

    // *** Right view ***
    guiManager.AddRenderer(800,  // render width
                           600, // render height
                           20, 20,                                   // window position
                           camera_geometry, SVL_RIGHT,               // camera parameters
                           "RightEyeView");                          // name of renderer

    // Creating video background image planes
    guiManager.AddVideoBackgroundToRenderer("LeftEyeView",  "StereoVideo", SVL_LEFT);
    guiManager.AddVideoBackgroundToRenderer("RightEyeView", "StereoVideo", SVL_RIGHT);

#endif //RENDER_ON_OVERLAY

#ifdef DEBUG_WINDOW_WITH_OVERLAY
#ifdef DEBUG_WINDOW_HAS_VIDEO_BACKGROUND
    svlStreamManager vidStream(1);

    svlFilterSourceVideoCapture vidSource(false); // mono source
    cout << "Setup camera:" << endl;
    vidSource.DialogSetup();
    vidStream.Trunk().Append(&vidSource);

    svlFilterImageResizer vidResizer;
    vidResizer.SetOutputSize(384, 216);
    vidStream.Trunk().Append(&vidResizer);

#ifdef CAPTURE_SWAP_RGB
    svlFilterRGBSwapper vidRGBSwapper;
    vidStream.Trunk().Append(&vidRGBSwapper);
#endif //CAPTURE_SWAP_RGB

    // add guiManager as a filter to the pipeline, so it will receive video frames
    // "MonoVideo" is defined in the UI Manager as a possible video interface
    vidStream.Trunk().Append(guiManager.GetStreamSamplerFilter("MonoVideo"));

    vidStream.Initialize();
#endif //DEBUG_WINDOW_HAS_VIDEO_BACKGROUND
    // Add third camera: simple perspective camera placed in the world center
#endif
#endif

    camera_geometry.SetPerspective(400.0, 2);
    camera_geometry.RotateWorldAboutY(180.0);

    guiManager.AddRenderer(384,                // render width
                           216,                // render height
                           0, 0,               // window position
                           camera_geometry, 2, // camera parameters
                           "ThirdEyeView");    // name of renderer

#ifdef DEBUG_WINDOW_HAS_VIDEO_BACKGROUND
    guiManager.AddVideoBackgroundToRenderer("ThirdEyeView", "MonoVideo");
    vidStream.Start();
#endif //DEBUG_WINDOW_HAS_VIDEO_BACKGROUND
//#endif //DEBUG_WINDOW_WITH_OVERLAY

///////////////////////////////////////////////////////////////
// start streaming
#if 0
#ifndef RENDER_ON_OVERLAY
    vidStream.Start();
#endif //RENDER_ON_OVERLAY
#endif

#if (UI3_INPUT == UI3_OMNI1) || (UI3_INPUT == UI3_OMNI1_OMNI2)
    vctFrm3 transform;
    transform.Translation().Assign(+30.0, 0.0, -150.0); // recenter Omni's depth (right)
    ui3MasterArm * rightMaster = new ui3MasterArm("Omni1");
    guiManager.AddMasterArm(rightMaster);
    rightMaster->SetInput(sensable, "Omni1",
                          sensable, "Omni1Button1",
                          sensable, "Omni1Button2",
                          ui3MasterArm::PRIMARY);
    rightMaster->SetTransformation(transform, 0.5 /* scale factor */);
    ui3CursorBase * rightCursor = new ui3CursorSphere();
    rightCursor->SetAnchor(ui3CursorBase::CENTER_RIGHT);
    rightMaster->SetCursor(rightCursor);
#endif
#if (UI3_INPUT == UI3_OMNI1_OMNI2)
    transform.Translation().Assign(-30.0, 0.0, -150.0); // recenter Omni's depth (left)
    ui3MasterArm * leftMaster = new ui3MasterArm("Omni2");
    guiManager.AddMasterArm(leftMaster);
    leftMaster->SetInput(sensable, "Omni2",
                         sensable, "Omni2Button1",
                         sensable, "Omni2Button2",
                         ui3MasterArm::SECONDARY);
    leftMaster->SetTransformation(transform, 0.5 /* scale factor */);
    ui3CursorBase * leftCursor = new ui3CursorSphere();
    leftCursor->SetAnchor(ui3CursorBase::CENTER_LEFT);
    leftMaster->SetCursor(leftCursor);
#endif

#if (UI3_INPUT == UI3_DAVINCI)
    vctFrm3 transform;
    transform.Rotation().From(vctAxAnRot3(vctDouble3(0.0, 1.0, 0.0), cmnPI));

    // setup first arm
    ui3MasterArm * rightMaster = new ui3MasterArm("MTMR");
    guiManager.AddMasterArm(rightMaster);
    rightMaster->SetInput(daVinci, "MTMR",
                          daVinci, "MTMRButton",
                          daVinci, "MTMRClutch",
                          ui3MasterArm::PRIMARY);
    rightMaster->SetTransformation(transform, 0.5 /* scale factor */);
    ui3CursorBase * rightCursor = new ui3CursorSphere();
    rightCursor->SetAnchor(ui3CursorBase::CENTER_RIGHT);
    rightMaster->SetCursor(rightCursor);

    // setup second arm
    ui3MasterArm * leftMaster = new ui3MasterArm("MTML");
    guiManager.AddMasterArm(leftMaster);
    leftMaster->SetInput(daVinci, "MTML",
                         daVinci, "MTMLButton",
                         daVinci, "MTMLClutch",
                         ui3MasterArm::SECONDARY);
    leftMaster->SetTransformation(transform, 0.5 /* scale factor */);
    ui3CursorBase * leftCursor = new ui3CursorSphere();
    leftCursor->SetAnchor(ui3CursorBase::CENTER_LEFT);
    leftMaster->SetCursor(leftCursor);

    // first slave arm, i.e. PSM1
    ui3SlaveArm * slave1 = new ui3SlaveArm("Slave1");
    guiManager.AddSlaveArm(slave1);
    slave1->SetInput(daVinci, "PSM1");
    slave1->SetTransformation(transform, 1.0 /* scale factor */);

    // setup event for MaM transitions
    guiManager.SetupMaM(daVinci, "MastersAsMice");
#endif

    guiManager.ConnectAll();

    // following should be replaced by a utility function or method of ui3Manager 
    taskManager->CreateAll();
    taskManager->StartAll();

    osaSleep(1.0 * cmn_s);

    int ch;
    
    cerr << endl << "Keyboard commands:" << endl << endl;
    cerr << "  In command window:" << endl;
    cerr << "    'q'   - Quit" << endl << endl;
    do {
        ch = cmnGetChar();
        osaSleep(10.0 * cmn_ms);
    } while (ch != 'q');

    taskManager->KillAll();

    guiManager.SaveConfiguration("config.xml");
#if 0
#ifndef RENDER_ON_OVERLAY
    // It stops and disassembles the pipeline in proper
    // order even if it has several branches
    vidStream.EmptyFilterList();
#endif //RENDER_ON_OVERLAY
#endif
#ifdef DEBUG_WINDOW_WITH_OVERLAY
#ifdef DEBUG_WINDOW_HAS_VIDEO_BACKGROUND
    // It stops and disassembles the pipeline in proper
    // order even if it has several branches
    vidStream.EmptyFilterList();
#endif //DEBUG_WINDOW_HAS_VIDEO_BACKGROUND
#endif //DEBUG_WINDOW_WITH_OVERLAY

    return 0;
}

