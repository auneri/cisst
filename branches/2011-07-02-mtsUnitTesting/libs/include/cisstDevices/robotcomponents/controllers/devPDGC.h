#ifndef _devPDGC_h
#define _devPDGC_h

#include <cisstVector/vctDynamicMatrix.h>
#include <cisstRobot/robManipulator.h>

#include <cisstDevices/robotcomponents/controllers/devController.h>
#include <cisstDevices/devExport.h>

class CISST_EXPORT devPDGC : 
  public devController,
  public robManipulator {

 private:

  RnIO* input;
  RnIO* output;
  RnIO* feedback;

  vctDynamicMatrix<double> Kp;
  vctDynamicMatrix<double> Kd;

  vctDynamicVector<double> qold;
  vctDynamicVector<double> eold;

  double told;

 protected:

  void Evaluate();

 public:

  devPDGC( const std::string& name, 
	   double period,
	   devController::State state,
	   osaCPUMask mask,
	   const std::string& robfile,
	   const vctFrame4x4<double>& Rtw0, 
	   const vctDynamicMatrix<double>& Kp,
	   const vctDynamicMatrix<double>& Kd );
  

};

#endif