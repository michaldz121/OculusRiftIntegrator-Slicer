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

#include "vtkSlicerOculusRiftIntegratorLogic.h"

#pragma comment(lib, "winmm.lib")

#include <cmath>

// Qt includes
#include <QDebug>
#include <QtCore>
#include <QDesktopWidget>
#include <QRect>
#include <QTimer>

#include <vtkMRMLScene.h>

// SlicerQt includes
#include "qSlicerOculusRiftIntegratorModuleWidget.h"
#include "ui_qSlicerOculusRiftIntegratorModule.h"
#include "qSlicerLayoutManager.h"
#include <qSlicerApplication.h>
#include <qSlicerIOManager.h>

#include <vtkMRMLViewNode.h>
#include <qMRMLThreeDView.h>
#include <qMRMLThreeDWidget.h>
#include <vtkMRMLCameraNode.h>

#include <vtkMRMLNode.h>

#include <vtkNew.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkFloatArray.h>
#include <vtkCamera.h>
#include <vtkMatrix4x4.h>
#include <vtkMath.h>
#include <vtkMRMLLinearTransformNode.h>
#include <vtkPerspectiveTransform.h>

// Lib OVR (Oculus) includes
#include <OVR.h>

using namespace OVR;

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerOculusRiftIntegratorModuleWidgetPrivate: public Ui_qSlicerOculusRiftIntegratorModule
{
  Q_DECLARE_PUBLIC( qSlicerOculusRiftIntegratorModuleWidget );
protected:
  qSlicerOculusRiftIntegratorModuleWidget* const q_ptr;
public:
  qSlicerOculusRiftIntegratorModuleWidgetPrivate( qSlicerOculusRiftIntegratorModuleWidget& object );
  vtkSlicerOculusRiftIntegratorLogic* logic() const;
};

//-----------------------------------------------------------------------------
// qSlicerOculusRiftIntegratorModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerOculusRiftIntegratorModuleWidgetPrivate::qSlicerOculusRiftIntegratorModuleWidgetPrivate( qSlicerOculusRiftIntegratorModuleWidget& object )
 : q_ptr( &object )
{
}


vtkSlicerOculusRiftIntegratorLogic* qSlicerOculusRiftIntegratorModuleWidgetPrivate::logic() const
{
  Q_Q( const qSlicerOculusRiftIntegratorModuleWidget );
  return vtkSlicerOculusRiftIntegratorLogic::SafeDownCast( q->logic() );
}


//-----------------------------------------------------------------------------
// qSlicerOculusRiftIntegratorModuleWidget methods
//-----------------------------------------------------------------------------
qSlicerOculusRiftIntegratorModuleWidget::qSlicerOculusRiftIntegratorModuleWidget(QWidget* _parent) : Superclass( _parent ) , d_ptr( new qSlicerOculusRiftIntegratorModuleWidgetPrivate(*this))
{
  this->updateTimer = new QTimer();
  updateTimer->setSingleShot(false);
  updateTimer->setInterval(70);

  pastRoll = 0;
  pastYaw = 0;
  pastPitch = 0;
  eyeRoll = 0;
  eyeYaw = 0;
  eyePitch = 0;

  neckPoint[0] = 0;
  neckPoint[1] = -750;
  neckPoint[2] = -100;

  permaAxis[0] = 0; permaAxis[1] = 1; permaAxis[2] = 0;

  this->HeadTransform = vtkPerspectiveTransform::New();
}

//-----------------------------------------------------------------------------
qSlicerOculusRiftIntegratorModuleWidget::~qSlicerOculusRiftIntegratorModuleWidget()
{
  OVR::System::Destroy();
  delete riftWindow;
}

//-----------------------------------------------------------------------------
void qSlicerOculusRiftIntegratorModuleWidget::onOpenWindow()
{
  Q_D(qSlicerOculusRiftIntegratorModuleWidget);

  //qSlicerApplication * app = qSlicerApplication::application();
  //vtkSmartPointer< vtkMRMLViewNode > viewNode = vtkSmartPointer< vtkMRMLViewNode >::New();
  //app->mrmlScene()->AddNode(viewNode);

  riftWindow = new qMRMLThreeDWidget();
  //vtkMRMLViewNode::SafeDownCast(d->viewNodeComboBox->currentNode())->SetStereoType(5);
  riftWindow->setMRMLViewNode(vtkMRMLViewNode::SafeDownCast(d->viewNodeComboBox->currentNode()));
  //riftWindow->setMRMLViewNode(viewNode);
  riftWindow->showNormal();


  riftWindow->setMRMLScene(this->mrmlScene());
}

//-----------------------------------------------------------------------------
void qSlicerOculusRiftIntegratorModuleWidget::onInitializeTracking()
{
  Q_D(qSlicerOculusRiftIntegratorModuleWidget);

  initializeOculus();

  riftCamera = vtkMRMLCameraNode::SafeDownCast(d->cameraNodeComboBox->currentNode())->GetCamera();

  riftCamera->ParallelProjectionOn();
  riftCamera->UseOffAxisProjectionOn();

  double p[3] = {0,-10,0};
  riftCamera->SetPosition(p);

  double f[3] = {0,0,0};
  riftCamera->SetFocalPoint(f);

  riftCamera->GetViewUp(permaViewUp);

  permaAxis[0] = riftCamera->GetViewTransformMatrix()->GetElement(0,0);
  permaAxis[1] = riftCamera->GetViewTransformMatrix()->GetElement(0,1);
  permaAxis[2] = riftCamera->GetViewTransformMatrix()->GetElement(0,2);

  riftCamera->SetClippingRange(0.01,10000);

  //riftCamera->SetEyeSeparation(0.01);
  riftCamera->SetEyeAngle(0.05);

  pastRoll=0;


  updateTimer->start();
}

//-----------------------------------------------------------------------------
void qSlicerOculusRiftIntegratorModuleWidget::onFrameUpdate()
{
  Q_D(qSlicerOculusRiftIntegratorModuleWidget);

  Quatf hmdOrient = sFusion->GetOrientation();
  float roll = 0.0f;
  float yaw = 0.0f;
  float pitch = 0.0f;
  hmdOrient.GetEulerAngles<OVR::Axis_Y, OVR::Axis_X, OVR::Axis_Z>(&yaw, &pitch, &roll);

  double newFocalPoint[3];
  double pos[3];
  double dirProj[3];
  double viewUp[3];
  double axis[3];

  eyeRoll = roll-pastRoll;
  eyeYaw = yaw-pastYaw;
  eyePitch = pitch-pastPitch;

  // Camera
  riftCamera->GetFocalPoint(newFocalPoint);
  riftCamera->GetPosition(pos);
  riftCamera->GetDirectionOfProjection(dirProj);
  riftCamera->Roll(OVR::RadToDegree(-eyeRoll));

  //Yaw
  riftCamera->GetFocalPoint(newFocalPoint);
  riftCamera->GetPosition(pos);
  riftCamera->GetViewUp(viewUp);
  this->HeadTransform->Identity();
  this->HeadTransform->Translate(+neckPoint[0], +neckPoint[1], +neckPoint[2]);
  this->HeadTransform->RotateWXYZ(OVR::RadToDegree(eyeYaw),permaViewUp);
  this->HeadTransform->Translate(-neckPoint[0], -neckPoint[1], -neckPoint[2]);

  //Pitch
  this->HeadTransform->Translate(+neckPoint[0], +neckPoint[1], +neckPoint[2]);
  this->HeadTransform->RotateWXYZ(OVR::RadToDegree(eyePitch),permaAxis);
  this->HeadTransform->Translate(-neckPoint[0], -neckPoint[1], -neckPoint[2]);

  //Roll
  this->HeadTransform->Translate(+neckPoint[0], +neckPoint[1], +neckPoint[2]);
  this->HeadTransform->RotateWXYZ(OVR::RadToDegree(eyeRoll),dirProj);
  this->HeadTransform->Translate(-neckPoint[0], -neckPoint[1], -neckPoint[2]);

  //Head position
  vtkMRMLLinearTransformNode* transform = vtkMRMLLinearTransformNode::SafeDownCast(d->transformNodeComboBox->currentNode());
#ifdef TRANSFORM_NODE_MATRIX_COPY_REQUIRED
  vtkSmartPointer<vtkMatrix4x4> outputMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  transform->GetMatrixTransformToParent(outputMatrix);
#else
  vtkMatrix4x4* outputMatrix = transform->GetMatrixTransformToParent();
#endif
  this->HeadTransform->Translate(outputMatrix->GetElement(0,3), outputMatrix->GetElement(1,3), outputMatrix->GetElement(2,3));

  this->HeadTransform->TransformPoint(newFocalPoint,newFocalPoint);
  riftCamera->SetFocalPoint(newFocalPoint);
  this->HeadTransform->TransformPoint(pos,pos);
  riftCamera->SetPosition(pos);

  //Save last values
  pastRoll = roll;
  pastPitch = pitch;
  pastYaw = yaw;

  d->RollLabel->setText(QString::number(roll*57));
  d->PitchLabel->setText(QString::number(pitch*57));
  d->YawLabel->setText(QString::number(yaw*57));
}

//-----------------------------------------------------------------------------
void qSlicerOculusRiftIntegratorModuleWidget::initializeOculus()
{
  System::Init(Log::ConfigureDefaultLog(LogMask_All));

  pManager = *DeviceManager::Create();

  int detectionResult = 1;
  const char* detectionMessage;

  // Release Sensor/HMD in case this is a retry.
  pSensor.Clear();
  pHMD.Clear();

  pHMD  = *pManager->EnumerateDevices<HMDDevice>().CreateDevice();
  if (pHMD)
  {
    pHMD->GetDeviceInfo(&HMDInfo);
    pSensor = *pHMD->GetSensor();
  }

  // If there was a problem detecting the Rift, display appropriate message.
  detectionResult  = 1;        

  if (!pHMD && !pSensor)
  {
    std::cerr << "Oculus Rift not detected." << std::endl;
    return;
  }
  else if (!pHMD)
  {
    std::cerr << "Oculus Sensor detected; HMD Display not detected." << std::endl;
  }
  else if (!pSensor)
  {
    std::cerr << "Oculus HMD Display detected; Sensor not detected." << std::endl;
  }
  else if (HMDInfo.DisplayDeviceName[0] == '\0')
  {
    std::cerr << "Oculus Sensor detected; HMD display EDID not detected." << std::endl;
  }
  
  sFusion = new SensorFusion();
  if (pSensor)
  {
    sFusion->AttachToSensor(pSensor);
  }
}

//-----------------------------------------------------------------------------
void qSlicerOculusRiftIntegratorModuleWidget::onDepthChanged(double separation)
{
  riftCamera->SetEyeAngle(separation);
  //vtkRenderer* dummy = NULL;
  //riftCamera->UpdateViewport(dummy);
  //std::cout<<"Seperation changed"<<std::endl;
}

//-----------------------------------------------------------------------------
void qSlicerOculusRiftIntegratorModuleWidget::onReset()
{
  double p[3] = {0,-700,0};
  riftCamera->SetPosition(p);

  double f[3] = {0,0,0};
  riftCamera->SetFocalPoint(f);

  riftCamera->GetViewUp(permaViewUp);

  permaAxis[0] = riftCamera->GetViewTransformMatrix()->GetElement(0,0);
  permaAxis[1] = riftCamera->GetViewTransformMatrix()->GetElement(0,1);
  permaAxis[2] = riftCamera->GetViewTransformMatrix()->GetElement(0,2);

  riftCamera->SetClippingRange(0.01,10000);

  Quatf hmdOrient = sFusion->GetOrientation();
  float roll = 0.0f;
  float yaw = 0.0f;
  float pitch = 0.0f;
  hmdOrient.GetEulerAngles<OVR::Axis_Y, OVR::Axis_X, OVR::Axis_Z>(&yaw, &pitch, &roll);

  pastRoll=roll;
}

//-----------------------------------------------------------------------------
void qSlicerOculusRiftIntegratorModuleWidget::enter()
{
  this->Superclass::enter();
}

//-----------------------------------------------------------------------------
void qSlicerOculusRiftIntegratorModuleWidget::setup()
{
  Q_D(qSlicerOculusRiftIntegratorModuleWidget);
  d->setupUi(this);
  
  this->Superclass::setup();
  
  connect(updateTimer, SIGNAL( timeout() ), this, SLOT( onFrameUpdate() ));

  connect( d->openWindowButton, SIGNAL( clicked() ), this, SLOT( onOpenWindow() ) );
  connect( d->startTrackingButton, SIGNAL( clicked() ), this, SLOT( onInitializeTracking() ) );
  connect( d->resetButton, SIGNAL( clicked() ), this, SLOT( onReset() ) );
  connect( d->depthSlider, SIGNAL( valueChanged(double) ), this, SLOT( onDepthChanged(double) ) );
}

