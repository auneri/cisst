
#include <cisstDevices/controlloops/devControlLoop.h>
#include <cisstDevices/devExport.h>

#ifndef _devJointsPD_h
#define _devJointsPD_h

class CISST_EXPORT devJointsPD : public devControlLoop{

private:

  vctDynamicMatrix<double> Kp, Kd;
  double period;

  vctDynamicVector<double> olde;
  vctDynamicVector<double> oldq;

public:

  devJointsPD( const vctDynamicMatrix<double>& Kp,
	       const vctDynamicMatrix<double>& Kd,
	       const std::string& taskname, 
	       double period,
	       const std::string& robfile,
	       const vctDynamicVector<double>& qinit,
	       const vctFrame4x4<double>& Rtwb = vctFrame4x4<double>(),
	       const std::vector<devGeometry*> geoms=std::vector<devGeometry*>() );

  vctDynamicVector<double> Control( const vctDynamicVector<double>& q );

};

#endif
