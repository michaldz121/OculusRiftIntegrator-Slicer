/*==============================================================================

  Program: 3D Slicer

  Copyright (c) Kitware Inc.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Jean-Christophe Fillion-Robin, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

// Qt includes
#include <QDebug>
#include <QEvent>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QToolButton>

// CTK includes
#include <ctkAxesWidget.h>
#include <ctkPopupWidget.h>

// qMRML includes
#include "qMRMLColors.h"
#include "qMRMLOculusThreeDView_p.h"

// MRMLDisplayableManager includes
#include <vtkMRMLAbstractDisplayableManager.h>
#include <vtkMRMLDisplayableManagerGroup.h>
#include <vtkMRMLThreeDViewDisplayableManagerFactory.h>
#include <vtkThreeDViewInteractorStyle.h>

// MRML includes
#include <vtkMRMLViewNode.h>
#include <vtkMRMLScene.h>

// VTK includes
#include <vtkCollection.h>
#include <vtkNew.h>
#include <vtkSmartPointer.h>

//--------------------------------------------------------------------------
// qMRMLOculusThreeDViewPrivate methods

//---------------------------------------------------------------------------
qMRMLOculusThreeDViewPrivate::qMRMLOculusThreeDViewPrivate(qMRMLOculusThreeDView& object)
  : q_ptr(&object)
{
  this->DisplayableManagerGroup = 0;
  this->MRMLScene = 0;
  this->MRMLViewNode = 0;
  this->PinButton = 0;
  this->PopupWidget = 0;
}

//---------------------------------------------------------------------------
qMRMLOculusThreeDViewPrivate::~qMRMLOculusThreeDViewPrivate()
{
  if (this->DisplayableManagerGroup)
    {
    this->DisplayableManagerGroup->Delete();
    }
}

//---------------------------------------------------------------------------
void qMRMLOculusThreeDViewPrivate::init()
{
  Q_Q(qMRMLOculusThreeDView);
  q->setRenderEnabled(this->MRMLScene != 0);

  this->PopupWidget = new ctkPopupWidget;
  QHBoxLayout* popupLayout = new QHBoxLayout;
  popupLayout->addWidget(new QToolButton);
  this->PopupWidget->setLayout(popupLayout);

  vtkNew<vtkThreeDViewInteractorStyle> interactorStyle;
  q->interactor()->SetInteractorStyle(interactorStyle.GetPointer());

  // Set default background color
  q->setBackgroundColor(QColor::fromRgbF(
    vtkMRMLViewNode::defaultBackgroundColor()[0],
    vtkMRMLViewNode::defaultBackgroundColor()[1],
    vtkMRMLViewNode::defaultBackgroundColor()[2]));

  q->setGradientBackground(true);

  // Hide orientation widget
  q->setOrientationWidgetVisible(false);

  q->setZoomFactor(0.05);

  q->setPitchDirection(ctkVTKRenderView::PitchUp);
  q->setRollDirection(ctkVTKRenderView::RollRight);
  q->setYawDirection(ctkVTKRenderView::YawLeft);

  this->initDisplayableManagers();
}

//---------------------------------------------------------------------------
void qMRMLOculusThreeDViewPrivate::initDisplayableManagers()
{
  Q_Q(qMRMLOculusThreeDView);
  vtkMRMLThreeDViewDisplayableManagerFactory* factory
    = vtkMRMLThreeDViewDisplayableManagerFactory::GetInstance();

  QStringList displayableManagers;
  displayableManagers << "vtkMRMLCameraDisplayableManager"
                      << "vtkMRMLViewDisplayableManager"
                      << "vtkMRMLModelDisplayableManager"
                      << "vtkMRMLThreeDReformatDisplayableManager";
  foreach(const QString& displayableManager, displayableManagers)
    {
    if(!factory->IsDisplayableManagerRegistered(displayableManager.toLatin1()))
      {
      factory->RegisterDisplayableManager(displayableManager.toLatin1());
      }
    }

  this->DisplayableManagerGroup
    = factory->InstantiateDisplayableManagers(q->renderer());
  // Observe displayable manager group to catch RequestRender events
  this->qvtkConnect(this->DisplayableManagerGroup, vtkCommand::UpdateEvent,
                    q, SLOT(scheduleRender()));
}

//---------------------------------------------------------------------------
void qMRMLOculusThreeDViewPrivate::setMRMLScene(vtkMRMLScene* newScene)
{
  Q_Q(qMRMLOculusThreeDView);
  if (newScene == this->MRMLScene)
    {
    return;
    }

  this->qvtkReconnect(
    this->MRMLScene, newScene,
    vtkMRMLScene::StartBatchProcessEvent, this, SLOT(onSceneStartProcessing()));

  this->qvtkReconnect(
    this->MRMLScene, newScene,
    vtkMRMLScene::EndBatchProcessEvent, this, SLOT(onSceneEndProcessing()));

  this->MRMLScene = newScene;
  q->setRenderEnabled(
    this->MRMLScene != 0 && !this->MRMLScene->IsBatchProcessing());
}

//---------------------------------------------------------------------------
//vtkMRMLCameraNode* qMRMLOculusThreeDViewPrivate::lookUpMRMLCameraNode(vtkMRMLViewNode* viewNode)
//{
//  Q_ASSERT(viewNode);
//
//  QString viewNodeID(viewNode->GetID());
//  Q_ASSERT(!viewNodeID.isEmpty());
//
//  std::vector<vtkMRMLNode*> cameraNodes;
//  int cameraNodeCount = this->MRMLScene->GetNodesByClass("vtkMRMLCameraNode", cameraNodes);
//  for (int n=0; n < cameraNodeCount; n++)
//    {
//    vtkMRMLCameraNode* cameraNode = vtkMRMLCameraNode::SafeDownCast(cameraNodes[n]);
//    Q_ASSERT(cameraNode);
//    QString activeTag(cameraNode->GetActiveTag());
//    if (activeTag == viewNodeID)
//      {
//      Q_ASSERT(this->MRMLScene->GetNodeByID(cameraNode->GetID()));
//      return cameraNode;
//      }
//    }
//  return 0;
//}

// --------------------------------------------------------------------------
void qMRMLOculusThreeDViewPrivate::onSceneStartProcessing()
{
  //qDebug() << "qMRMLOculusThreeDViewPrivate::onSceneStartProcessing";
  Q_Q(qMRMLOculusThreeDView);
  q->setRenderEnabled(false);
}

//
// --------------------------------------------------------------------------
void qMRMLOculusThreeDViewPrivate::onSceneEndProcessing()
{
  //qDebug() << "qMRMLOculusThreeDViewPrivate::onSceneImportedEvent";
  Q_Q(qMRMLOculusThreeDView);
  q->setRenderEnabled(true);
}

// --------------------------------------------------------------------------
void qMRMLOculusThreeDViewPrivate::updateWidgetFromMRML()
{
  Q_Q(qMRMLOculusThreeDView);
  if (!this->MRMLViewNode)
    {
    return;
    }
  q->setAnimationIntervalMs(this->MRMLViewNode->GetAnimationMs());
  q->setPitchRollYawIncrement(this->MRMLViewNode->GetRotateDegrees());
  q->setSpinIncrement(this->MRMLViewNode->GetSpinDegrees());
  q->setRockIncrement(this->MRMLViewNode->GetRockCount());
  q->setRockLength(this->MRMLViewNode->GetRockLength());

  q->setSpinEnabled(this->MRMLViewNode->GetAnimationMode() == vtkMRMLViewNode::Spin);
  q->setRockEnabled(this->MRMLViewNode->GetAnimationMode() == vtkMRMLViewNode::Rock);

  q->setUseDepthPeeling(this->MRMLViewNode->GetUseDepthPeeling() != 0);
  q->setFPSVisible(this->MRMLViewNode->GetFPSVisible() != 0);
}

// --------------------------------------------------------------------------
// qMRMLOculusThreeDView methods

// --------------------------------------------------------------------------
qMRMLOculusThreeDView::qMRMLOculusThreeDView(QWidget* _parent) : Superclass(_parent)
  , d_ptr(new qMRMLOculusThreeDViewPrivate(*this))
{
  Q_D(qMRMLOculusThreeDView);
  d->init();
}

// --------------------------------------------------------------------------
qMRMLOculusThreeDView::~qMRMLOculusThreeDView()
{
}

//------------------------------------------------------------------------------
void qMRMLOculusThreeDView::addDisplayableManager(const QString& displayableManagerName)
{
  Q_D(qMRMLOculusThreeDView);
  vtkSmartPointer<vtkMRMLAbstractDisplayableManager> displayableManager;
  displayableManager.TakeReference(
    vtkMRMLDisplayableManagerGroup::InstantiateDisplayableManager(
      displayableManagerName.toLatin1()));
  d->DisplayableManagerGroup->AddDisplayableManager(displayableManager);
}

//------------------------------------------------------------------------------
void qMRMLOculusThreeDView::setMRMLScene(vtkMRMLScene* newScene)
{
  Q_D(qMRMLOculusThreeDView);
  d->setMRMLScene(newScene);

  if (d->MRMLViewNode && newScene != d->MRMLViewNode->GetScene())
    {
    this->setMRMLViewNode(0);
    }
}

//---------------------------------------------------------------------------
void qMRMLOculusThreeDView::setMRMLViewNode(vtkMRMLViewNode* newViewNode)
{
  Q_D(qMRMLOculusThreeDView);
  
  //vtkMRMLViewNode* castViewNode = reinterpret_cast<vtkMRMLViewNode*> (newViewNode);
  
  if (d->MRMLViewNode == newViewNode)
    {
    return;
    }

  d->qvtkReconnect(
    d->MRMLViewNode, newViewNode,
    vtkCommand::ModifiedEvent, d, SLOT(updateWidgetFromMRML()));

  d->MRMLViewNode = newViewNode;
  d->DisplayableManagerGroup->SetMRMLDisplayableNode(newViewNode);

  d->updateWidgetFromMRML();
  // Enable/disable widget
  this->setEnabled(newViewNode != 0);
}

//---------------------------------------------------------------------------
vtkMRMLViewNode* qMRMLOculusThreeDView::mrmlViewNode()const
{
  Q_D(const qMRMLOculusThreeDView);
  return d->MRMLViewNode;
}

// --------------------------------------------------------------------------
void qMRMLOculusThreeDView::lookFromViewAxis(const ctkAxesWidget::Axis& axis)
{
  Q_D(qMRMLOculusThreeDView);
  if (!d->MRMLViewNode)
    {
    return;
    }
  double fov = d->MRMLViewNode->GetFieldOfView();
  Q_ASSERT(fov >= 0.0);
  this->lookFromAxis(axis, fov);
}

// --------------------------------------------------------------------------
void qMRMLOculusThreeDView::resetFocalPoint()
{
  Q_D(qMRMLOculusThreeDView);

  bool savedBoxVisibile = true;
  bool savedAxisLabelVisible = true;
  if (d->MRMLViewNode)
    {
    // Save current visiblity state of Box and AxisLabel
    savedBoxVisibile = d->MRMLViewNode->GetBoxVisible();
    savedAxisLabelVisible = d->MRMLViewNode->GetAxisLabelsVisible();

    int wasModifying = d->MRMLViewNode->StartModify();
    // Hide Box and AxisLabel so they don't get taken into account when computing
    // the view boundaries
    d->MRMLViewNode->SetBoxVisible(0);
    d->MRMLViewNode->SetAxisLabelsVisible(0);
    d->MRMLViewNode->EndModify(wasModifying);
    }
  // Superclass resets the camera.
  this->Superclass::resetFocalPoint();

  if (d->MRMLViewNode)
    {
    // Restore visibility state
    int wasModifying = d->MRMLViewNode->StartModify();
    d->MRMLViewNode->SetBoxVisible(savedBoxVisibile);
    d->MRMLViewNode->SetAxisLabelsVisible(savedAxisLabelVisible);
    d->MRMLViewNode->EndModify(wasModifying);
    // Inform the displayable manager that the view is reset, so it can
    // update the box/labels bounds.
    d->MRMLViewNode->InvokeEvent(vtkMRMLViewNode::ResetFocalPointRequestedEvent);
    }
}

//------------------------------------------------------------------------------
void qMRMLOculusThreeDView::getDisplayableManagers(vtkCollection *displayableManagers)
{
  Q_D(qMRMLOculusThreeDView);

  if (!displayableManagers)
    {
    return;
    }
  int num = d->DisplayableManagerGroup->GetDisplayableManagerCount();
  for (int n = 0; n < num; n++)
    {
    displayableManagers->AddItem(d->DisplayableManagerGroup->GetNthDisplayableManager(n));
    }
}
