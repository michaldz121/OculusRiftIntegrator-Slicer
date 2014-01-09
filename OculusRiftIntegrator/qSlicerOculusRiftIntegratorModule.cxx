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

// Qt includes
#include <QtPlugin>

// ExtensionTemplate Logic includes
#include <vtkSlicerOculusRiftIntegratorLogic.h>

// ExtensionTemplate includes
#include "qSlicerOculusRiftIntegratorModule.h"
#include "qSlicerOculusRiftIntegratorModuleWidget.h"

// Lib OVR (Oculus) includes
#include <OVR.h>

using namespace OVR;

//-----------------------------------------------------------------------------
Q_EXPORT_PLUGIN2(qSlicerOculusRiftIntegratorModule, qSlicerOculusRiftIntegratorModule);

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerOculusRiftIntegratorModulePrivate
{
public:
  qSlicerOculusRiftIntegratorModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerOculusRiftIntegratorModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerOculusRiftIntegratorModulePrivate::qSlicerOculusRiftIntegratorModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerOculusRiftIntegratorModule methods

//-----------------------------------------------------------------------------
qSlicerOculusRiftIntegratorModule::qSlicerOculusRiftIntegratorModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerOculusRiftIntegratorModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerOculusRiftIntegratorModule::~qSlicerOculusRiftIntegratorModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerOculusRiftIntegratorModule::helpText()const
{
  return "This module is still under construction";
}

//-----------------------------------------------------------------------------
QString qSlicerOculusRiftIntegratorModule::acknowledgementText()const
{
  return "";
}

//-----------------------------------------------------------------------------
QStringList qSlicerOculusRiftIntegratorModule::contributors()const
{
  QStringList moduleContributors;
  moduleContributors << QString( "Franklin King (Queen's University), " );
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerOculusRiftIntegratorModule::icon()const
{
  return QIcon(":/Icons/OculusRiftIntegrator.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerOculusRiftIntegratorModule::categories() const
{
  return QStringList() << "OculusRiftIntegrator";
}

//-----------------------------------------------------------------------------
QStringList qSlicerOculusRiftIntegratorModule::dependencies() const
{
  return QStringList();
}

//-----------------------------------------------------------------------------
void qSlicerOculusRiftIntegratorModule::setup()
{
  this->Superclass::setup();
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation * qSlicerOculusRiftIntegratorModule::createWidgetRepresentation()
{
  return new qSlicerOculusRiftIntegratorModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerOculusRiftIntegratorModule::createLogic()
{
  return vtkSlicerOculusRiftIntegratorLogic::New();
}
