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

// .NAME vtkSlicerOculusRiftIntegratorLogic
// .SECTION Description
#ifndef __vtkSlicerOculusRiftIntegratorLogic_h
#define __vtkSlicerOculusRiftIntegratorLogic_h

// Slicer includes
#include "vtkSlicerModuleLogic.h"

// MRML includes
#include "vtkMRMLSliceNode.h"
#include "qMRMLSliceWidget.h"
#include <qSlicerApplication.h>
#include <vtkRenderer.h>

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>

// STD includes
#include <cstdlib>

#include "vtkSlicerOculusRiftIntegratorModuleLogicExport.h"

/// \ingroup Slicer_QtModules_ExtensionTemplate
class VTK_SLICER_OCULUSRIFTINTEGRATOR_MODULE_LOGIC_EXPORT vtkSlicerOculusRiftIntegratorLogic :
  public vtkSlicerModuleLogic
{
public:

  static vtkSlicerOculusRiftIntegratorLogic *New();
  vtkTypeMacro(vtkSlicerOculusRiftIntegratorLogic, vtkSlicerModuleLogic);
  void PrintSelf(ostream& os, vtkIndent indent);
  

protected:
  vtkSlicerOculusRiftIntegratorLogic();
  virtual ~vtkSlicerOculusRiftIntegratorLogic();

  virtual void SetMRMLSceneInternal(vtkMRMLScene* newScene);
  /// Register MRML Node classes to Scene. Gets called automatically when the MRMLScene is attached to this logic class.
  
  
private:

  vtkSlicerOculusRiftIntegratorLogic(const vtkSlicerOculusRiftIntegratorLogic&); // Not implemented
  void operator=(const vtkSlicerOculusRiftIntegratorLogic&);               // Not implemented
};

#endif
