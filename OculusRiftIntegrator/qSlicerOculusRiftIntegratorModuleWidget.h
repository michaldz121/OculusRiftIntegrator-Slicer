/*==============================================================================

  Program: 3D Slicer

  Portions (c) Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==============================================================================*/

#ifndef __qSlicerOculusRiftIntegratorModuleWidget_h
#define __qSlicerOculusRiftIntegratorModuleWidget_h

// SlicerQt includes
#include "qSlicerAbstractModuleWidget.h"

#include "qSlicerOculusRiftIntegratorModuleExport.h"

#include <vtkMRMLCameraNode.h>
#include <vtkCamera.h>
#include <qMRMLOculusThreeDWidget.h>
#include <vtkPerspectiveTransform.h>

#include "OVR.h"


class qSlicerOculusRiftIntegratorModuleWidgetPrivate;
class vtkMRMLNode;

/// \ingroup Slicer_QtModules_ExtensionTemplate
class Q_SLICER_QTMODULES_OCULUSRIFTINTEGRATOR_EXPORT qSlicerOculusRiftIntegratorModuleWidget :
  public qSlicerAbstractModuleWidget
{
  Q_OBJECT

public:

  typedef qSlicerAbstractModuleWidget Superclass;
  qSlicerOculusRiftIntegratorModuleWidget(QWidget *parent=0);
  virtual ~qSlicerOculusRiftIntegratorModuleWidget();
  
  virtual void enter();

public slots:

  void onOpenWindow();
  void onReset();
  void onInitializeTracking();

  void onFrameUpdate();

  void onDepthChanged(double);
  
protected:
  void initializeOculus();

  OVR::Ptr<OVR::DeviceManager> pManager;
  OVR::Ptr<OVR::SensorDevice> pSensor;
  OVR::Ptr<OVR::HMDDevice> pHMD;
  OVR::SensorFusion* sFusion;
  OVR::HMDInfo HMDInfo;

  vtkCamera* riftCamera;

  QTimer* updateTimer;

  qMRMLOculusThreeDWidget* riftWindow;

  float pastRoll;
  float pastYaw;
  float pastPitch;
  float eyeYaw;
  float eyePitch;
  float eyeRoll;

  double neckPoint[3];
  double permaViewUp[3];
  double permaAxis[3];

  vtkPerspectiveTransform *HeadTransform;

  
protected:
  QScopedPointer<qSlicerOculusRiftIntegratorModuleWidgetPrivate> d_ptr;
  
  virtual void setup();


private:
  Q_DECLARE_PRIVATE(qSlicerOculusRiftIntegratorModuleWidget);
  Q_DISABLE_COPY(qSlicerOculusRiftIntegratorModuleWidget);
};

#endif
