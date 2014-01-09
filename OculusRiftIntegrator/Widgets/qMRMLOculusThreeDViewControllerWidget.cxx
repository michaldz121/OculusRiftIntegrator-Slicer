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

  This file was originally developed by Julien Finet, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

// Qt includes
#include <QActionGroup>
#include <QDebug>
#include <QInputDialog>
#include <QLabel>
#include <QMenu>

// CTK includes
#include <ctkButtonGroup.h>
#include <ctkPopupWidget.h>
#include <ctkSignalMapper.h>

// qMRML includes
#include "qMRMLColors.h"
#include "qMRMLNodeFactory.h"
#include "qMRMLSceneViewMenu.h"
#include "qMRMLOculusThreeDView.h"
#include "qMRMLOculusThreeDViewControllerWidget_p.h"

// MRML includes
#include <vtkMRMLScene.h>
#include <vtkMRMLViewNode.h>
#include <vtkMRMLSceneViewNode.h>

// VTK includes
#include <vtkRenderWindow.h>

//--------------------------------------------------------------------------
// qMRMLOculusThreeDViewControllerWidgetPrivate methods

//---------------------------------------------------------------------------
qMRMLOculusThreeDViewControllerWidgetPrivate::qMRMLOculusThreeDViewControllerWidgetPrivate(
  qMRMLOculusThreeDViewControllerWidget& object)
  : Superclass(object)
{
  this->ViewNode = 0;
  this->ThreeDView = 0;
  this->CenterToolButton = 0;
}

//---------------------------------------------------------------------------
qMRMLOculusThreeDViewControllerWidgetPrivate::~qMRMLOculusThreeDViewControllerWidgetPrivate()
{
}

//---------------------------------------------------------------------------
void qMRMLOculusThreeDViewControllerWidgetPrivate::setupPopupUi()
{
  Q_Q(qMRMLOculusThreeDViewControllerWidget);

  this->Superclass::setupPopupUi();
  this->PopupWidget->setAlignment(Qt::AlignBottom | Qt::AlignLeft);
  this->Ui_qMRMLOculusThreeDViewControllerWidget::setupUi(this->PopupWidget);

  // Look from axes
  QObject::connect(this->AxesWidget,
                   SIGNAL(currentAxisChanged(ctkAxesWidget::Axis)),
                   q, SLOT(lookFromAxis(ctkAxesWidget::Axis)));

  // Pitch, Roll, Yaw buttons
  QObject::connect(this->PitchButton, SIGNAL(clicked()),
                   q, SLOT(pitchView()));
  QObject::connect(this->RollButton, SIGNAL(clicked()),
                   q, SLOT(rollView()));
  QObject::connect(this->YawButton, SIGNAL(clicked()),
                   q, SLOT(yawView()));

  // Orthographic/perspective button
  QObject::connect(this->OrthoButton, SIGNAL(toggled(bool)),
                   q, SLOT(setOrthographicModeEnabled(bool)));

  // ZoomIn, ZoomOut button
  QObject::connect(this->ZoomInButton, SIGNAL(clicked()),
                   q, SLOT(zoomIn()));
  QObject::connect(this->ZoomOutButton, SIGNAL(clicked()),
                   q, SLOT(zoomOut()));

  // ResetFocalPoint button
  this->CenterButton->setDefaultAction(this->actionCenter);
  QObject::connect(this->actionCenter, SIGNAL(triggered()),
                   q, SLOT(resetFocalPoint()));

  // StereoType actions
  this->StereoTypesMapper = new ctkSignalMapper(this->PopupWidget);
  this->StereoTypesMapper->setMapping(this->actionNoStereo,
                                      vtkMRMLViewNode::NoStereo);
  this->StereoTypesMapper->setMapping(this->actionSwitchToAnaglyphStereo,
                                      vtkMRMLViewNode::Anaglyph);
  this->StereoTypesMapper->setMapping(this->actionSwitchToQuadBufferStereo,
                                      vtkMRMLViewNode::QuadBuffer);
  this->StereoTypesMapper->setMapping(this->actionSwitchToInterlacedStereo,
                                      vtkMRMLViewNode::Interlaced);
  this->StereoTypesMapper->setMapping(this->actionSwitchToRedBlueStereo,
                                      vtkMRMLViewNode::RedBlue);
  this->StereoTypesMapper->setMapping(this->actionSwitchToOculusRift,
                                      vtkMRMLViewNode::OculusRift);									  
  QActionGroup* stereoTypesActions = new QActionGroup(this->PopupWidget);
  stereoTypesActions->setExclusive(true);
  stereoTypesActions->addAction(this->actionNoStereo);
  stereoTypesActions->addAction(this->actionSwitchToRedBlueStereo);
  stereoTypesActions->addAction(this->actionSwitchToAnaglyphStereo);
  stereoTypesActions->addAction(this->actionSwitchToInterlacedStereo);
  stereoTypesActions->addAction(this->actionSwitchToQuadBufferStereo);
  stereoTypesActions->addAction(this->actionSwitchToOculusRift);
  QMenu* stereoTypesMenu = new QMenu("Stereo Modes", this->PopupWidget);
  stereoTypesMenu->setObjectName("stereoTypesMenu");
  stereoTypesMenu->addActions(stereoTypesActions->actions());
  this->StereoButton->setMenu(stereoTypesMenu);
  QObject::connect(this->StereoTypesMapper, SIGNAL(mapped(int)),
                   q, SLOT(setStereoType(int)));
  QObject::connect(stereoTypesActions, SIGNAL(triggered(QAction*)),
                   this->StereoTypesMapper, SLOT(map(QAction*)));
  this->actionSwitchToQuadBufferStereo->setEnabled(false); // Disabled by default

  QMenu* visibilityMenu = new QMenu("Visibility", this->PopupWidget);
  visibilityMenu->setObjectName("visibilityMenu");
  this->VisibilityButton->setMenu(visibilityMenu);

  // Show 3D Axis, 3D Axis label
  visibilityMenu->addAction(this->actionSet3DAxisVisible);
  visibilityMenu->addAction(this->actionSet3DAxisLabelVisible);
  QObject::connect(this->actionSet3DAxisVisible, SIGNAL(triggered(bool)),
                   q, SLOT(set3DAxisVisible(bool)));
  QObject::connect(this->actionSet3DAxisLabelVisible, SIGNAL(triggered(bool)),
                   q, SLOT(set3DAxisLabelVisible(bool)));

  // More controls
  QMenu* moreMenu = new QMenu("More", this->PopupWidget);
  moreMenu->addAction(this->actionUseDepthPeeling);
  moreMenu->addAction(this->actionSetFPSVisible);
  this->MoreToolButton->setMenu(moreMenu);

  // Depth peeling
  QObject::connect(this->actionUseDepthPeeling, SIGNAL(toggled(bool)),
                   q, SLOT(setUseDepthPeeling(bool)));

  // FPS
  QObject::connect(this->actionSetFPSVisible, SIGNAL(toggled(bool)),
                   q, SLOT(setFPSVisible(bool)));

  // Background color
  QActionGroup* backgroundColorActions = new QActionGroup(this->PopupWidget);
  backgroundColorActions->setExclusive(true);
  visibilityMenu->addAction(this->actionSetLightBlueBackground);
  visibilityMenu->addAction(this->actionSetBlackBackground);
  visibilityMenu->addAction(this->actionSetWhiteBackground);
  backgroundColorActions->addAction(this->actionSetLightBlueBackground);
  backgroundColorActions->addAction(this->actionSetBlackBackground);
  backgroundColorActions->addAction(this->actionSetWhiteBackground);
  QObject::connect(this->actionSetLightBlueBackground, SIGNAL(triggered()),
                   q, SLOT(setLightBlueBackground()));
  QObject::connect(this->actionSetWhiteBackground, SIGNAL(triggered()),
                   q, SLOT(setWhiteBackground()));
  QObject::connect(this->actionSetBlackBackground, SIGNAL(triggered()),
                   q, SLOT(setBlackBackground()));

  // SpinView, RockView buttons
  this->AnimateViewButtonGroup = new ctkButtonGroup(this->PopupWidget);
  this->AnimateViewButtonGroup->addButton(this->SpinButton, vtkMRMLViewNode::Spin);
  this->AnimateViewButtonGroup->addButton(this->RockButton, vtkMRMLViewNode::Rock);
  this->AnimateViewButtonGroup = new ctkButtonGroup(this->PopupWidget);
  this->AnimateViewButtonGroup->addButton(this->SpinButton);
  this->AnimateViewButtonGroup->addButton(this->RockButton);
  QObject::connect(this->SpinButton, SIGNAL(toggled(bool)),
                   q, SLOT(spinView(bool)));
  QObject::connect(this->RockButton, SIGNAL(toggled(bool)),
                   q, SLOT(rockView(bool)));
}

//---------------------------------------------------------------------------
void qMRMLOculusThreeDViewControllerWidgetPrivate::init()
{
  Q_Q(qMRMLOculusThreeDViewControllerWidget);
  this->Superclass::init();

  this->CenterToolButton = new QToolButton(q);
  this->CenterToolButton->setAutoRaise(true);
  this->CenterToolButton->setDefaultAction(this->actionCenter);
  this->CenterToolButton->setFixedSize(15, 15);
  this->BarLayout->insertWidget(2, this->CenterToolButton);

  this->ViewLabel->setText(qMRMLOculusThreeDViewControllerWidget::tr("1"));
  this->BarLayout->addStretch(1);
  this->setColor(qMRMLColors::threeDViewBlue());
}

// --------------------------------------------------------------------------
// qMRMLOculusThreeDViewControllerWidget methods

// --------------------------------------------------------------------------
qMRMLOculusThreeDViewControllerWidget::qMRMLOculusThreeDViewControllerWidget(QWidget* parentWidget)
  : Superclass(new qMRMLOculusThreeDViewControllerWidgetPrivate(*this), parentWidget)
{
  Q_D(qMRMLOculusThreeDViewControllerWidget);
  d->init();
}

// --------------------------------------------------------------------------
qMRMLOculusThreeDViewControllerWidget::~qMRMLOculusThreeDViewControllerWidget()
{
}

// --------------------------------------------------------------------------
void qMRMLOculusThreeDViewControllerWidget::setThreeDView(qMRMLOculusThreeDView* view)
{
  Q_D(qMRMLOculusThreeDViewControllerWidget);
  d->ThreeDView = view;
  if(d->ThreeDView != 0)
    {
    d->actionSwitchToQuadBufferStereo->setEnabled(
          d->ThreeDView->renderWindow()->GetStereoCapableWindow());
    }
}

//---------------------------------------------------------------------------
void qMRMLOculusThreeDViewControllerWidget::setViewLabel(const QString& newViewLabel)
{
  Q_D(qMRMLOculusThreeDViewControllerWidget);

  if (d->ViewNode)
    {
    qCritical() << "qMRMLOculusThreeDViewControllerWidget::setViewLabel should be called before setViewNode !";
    return;
    }

  d->ThreeDViewLabel = newViewLabel;
  d->ViewLabel->setText(d->ThreeDViewLabel);
}

//---------------------------------------------------------------------------
CTK_GET_CPP(qMRMLOculusThreeDViewControllerWidget, QString, viewLabel, ThreeDViewLabel);


// --------------------------------------------------------------------------
void qMRMLOculusThreeDViewControllerWidget::setMRMLViewNode(
    vtkMRMLViewNode * viewNode)
{
  Q_D(qMRMLOculusThreeDViewControllerWidget);
  this->qvtkReconnect(d->ViewNode, viewNode, vtkCommand::ModifiedEvent,
                      this, SLOT(updateWidgetFromMRML()));
  d->ViewNode = viewNode;
  this->updateWidgetFromMRML();
}

// --------------------------------------------------------------------------
void qMRMLOculusThreeDViewControllerWidget::updateWidgetFromMRML()
{
  Q_D(qMRMLOculusThreeDViewControllerWidget);
  // Enable buttons
  QList<QWidget*> widgets;
  widgets << d->AxesWidget
    << d->PitchButton << d->RollButton << d->YawButton
    << d->CenterButton << d->OrthoButton << d->VisibilityButton
    << d->ZoomInButton << d->ZoomOutButton << d->StereoButton
    << d->RockButton << d->SpinButton << d->MoreToolButton;
  foreach(QWidget* w, widgets)
    {
    w->setEnabled(d->ViewNode != 0);
    }

  if (!d->ViewNode)
    {
    return;
    }

  d->actionSet3DAxisVisible->setChecked(d->ViewNode->GetBoxVisible());
  d->actionSet3DAxisLabelVisible->setChecked(
    d->ViewNode->GetAxisLabelsVisible());

  d->actionUseDepthPeeling->setChecked(d->ViewNode->GetUseDepthPeeling());
  d->actionSetFPSVisible->setChecked(d->ViewNode->GetFPSVisible());

  double* color = d->ViewNode->GetBackgroundColor();
  QColor backgroundColor = QColor::fromRgbF(color[0], color[1], color[2]);
  d->actionSetBlackBackground->setChecked(backgroundColor == Qt::black);
  d->actionSetWhiteBackground->setChecked(backgroundColor == Qt::white);
  d->actionSetLightBlueBackground->setChecked(
    !d->actionSetBlackBackground->isChecked() &&
    !d->actionSetWhiteBackground->isChecked());

  d->OrthoButton->setChecked(
    d->ViewNode->GetRenderMode() == vtkMRMLViewNode::Orthographic);

  QAction* action = qobject_cast<QAction*>(d->StereoTypesMapper->mapping(
    d->ViewNode->GetStereoType()));
  action->setChecked(true);

  d->SpinButton->setChecked(d->ViewNode->GetAnimationMode() == vtkMRMLViewNode::Spin);
  d->RockButton->setChecked(d->ViewNode->GetAnimationMode() == vtkMRMLViewNode::Rock);
}

// --------------------------------------------------------------------------
void qMRMLOculusThreeDViewControllerWidget::setQuadBufferStereoSupportEnabled(bool value)
{
  Q_D(qMRMLOculusThreeDViewControllerWidget);
  d->actionSwitchToQuadBufferStereo->setEnabled(value);
}

// --------------------------------------------------------------------------
void qMRMLOculusThreeDViewControllerWidget::setOrthographicModeEnabled(bool enabled)
{
  Q_D(qMRMLOculusThreeDViewControllerWidget);
  d->ViewNode->SetRenderMode(
    enabled ? vtkMRMLViewNode::Orthographic : vtkMRMLViewNode::Perspective);
}

// --------------------------------------------------------------------------
void qMRMLOculusThreeDViewControllerWidget::lookFromAxis(const ctkAxesWidget::Axis& axis)
{
  Q_D(qMRMLOculusThreeDViewControllerWidget);
  if (!d->ThreeDView)
    {
    return;
    }
  d->ThreeDView->lookFromViewAxis(axis);
}

// --------------------------------------------------------------------------
void qMRMLOculusThreeDViewControllerWidget::pitchView()
{
  Q_D(qMRMLOculusThreeDViewControllerWidget);
  if (!d->ThreeDView)
    {
    return;
    }
  d->ThreeDView->pitch();
}

// --------------------------------------------------------------------------
void qMRMLOculusThreeDViewControllerWidget::rollView()
{
  Q_D(qMRMLOculusThreeDViewControllerWidget);
  if (!d->ThreeDView)
    {
    return;
    }
  d->ThreeDView->roll();
}

// --------------------------------------------------------------------------
void qMRMLOculusThreeDViewControllerWidget::yawView()
{
  Q_D(qMRMLOculusThreeDViewControllerWidget);
  if (!d->ThreeDView)
    {
    return;
    }
  d->ThreeDView->yaw();
}

// --------------------------------------------------------------------------
void qMRMLOculusThreeDViewControllerWidget::zoomIn()
{
  Q_D(qMRMLOculusThreeDViewControllerWidget);
  if (!d->ViewNode)
    {
    return;
    }
  d->ThreeDView->zoomIn();
}

// --------------------------------------------------------------------------
void qMRMLOculusThreeDViewControllerWidget::zoomOut()
{
  Q_D(qMRMLOculusThreeDViewControllerWidget);
  if (!d->ViewNode)
    {
    return;
    }
  d->ThreeDView->zoomOut();
}

// --------------------------------------------------------------------------
void qMRMLOculusThreeDViewControllerWidget::spinView(bool enabled)
{
  this->setAnimationMode(enabled ? vtkMRMLViewNode::Spin : vtkMRMLViewNode::Off);
}

// --------------------------------------------------------------------------
void qMRMLOculusThreeDViewControllerWidget::rockView(bool enabled)
{
  this->setAnimationMode(enabled ? vtkMRMLViewNode::Rock : vtkMRMLViewNode::Off);
}

// --------------------------------------------------------------------------
void qMRMLOculusThreeDViewControllerWidget::setAnimationMode(int newAnimationMode)
{
  Q_D(qMRMLOculusThreeDViewControllerWidget);
  if (!d->ViewNode)
    {
    return;
    }
  d->ViewNode->SetAnimationMode(newAnimationMode);
}

// --------------------------------------------------------------------------
void qMRMLOculusThreeDViewControllerWidget::resetFocalPoint()
{
  Q_D(qMRMLOculusThreeDViewControllerWidget);
  if (!d->ThreeDView)
    {
    return;
    }
  d->ThreeDView->resetFocalPoint();
}

// --------------------------------------------------------------------------
void qMRMLOculusThreeDViewControllerWidget::set3DAxisVisible(bool visible)
{
  Q_D(qMRMLOculusThreeDViewControllerWidget);
  if (!d->ViewNode)
    {
    return;
    }
  d->ViewNode->SetBoxVisible(visible);
}

// --------------------------------------------------------------------------
void qMRMLOculusThreeDViewControllerWidget::set3DAxisLabelVisible(bool visible)
{
  Q_D(qMRMLOculusThreeDViewControllerWidget);
  if (!d->ViewNode)
    {
    return;
    }
  d->ViewNode->SetAxisLabelsVisible(visible);
}

// --------------------------------------------------------------------------
void qMRMLOculusThreeDViewControllerWidget::setUseDepthPeeling(bool use)
{
  Q_D(qMRMLOculusThreeDViewControllerWidget);
  if (!d->ViewNode)
    {
    return;
    }
  d->ViewNode->SetUseDepthPeeling(use ? 1 : 0);
}

// --------------------------------------------------------------------------
void qMRMLOculusThreeDViewControllerWidget::setFPSVisible(bool visible)
{
  Q_D(qMRMLOculusThreeDViewControllerWidget);
  if (!d->ViewNode)
    {
    return;
    }
  d->ViewNode->SetFPSVisible(visible ? 1 : 0);
}

// --------------------------------------------------------------------------
void qMRMLOculusThreeDViewControllerWidget::setLightBlueBackground()
{
  this->setBackgroundColor(QColor::fromRgbF(
    vtkMRMLViewNode::defaultBackgroundColor()[0],
    vtkMRMLViewNode::defaultBackgroundColor()[1],
    vtkMRMLViewNode::defaultBackgroundColor()[2]),
    QColor::fromRgbF(
    vtkMRMLViewNode::defaultBackgroundColor2()[0],
    vtkMRMLViewNode::defaultBackgroundColor2()[1],
    vtkMRMLViewNode::defaultBackgroundColor2()[2]));
}

// --------------------------------------------------------------------------
void qMRMLOculusThreeDViewControllerWidget::setBlackBackground()
{
  this->setBackgroundColor(Qt::black);
}

// --------------------------------------------------------------------------
void qMRMLOculusThreeDViewControllerWidget::setWhiteBackground()
{
  this->setBackgroundColor(Qt::white);
}

// --------------------------------------------------------------------------
void qMRMLOculusThreeDViewControllerWidget::setBackgroundColor(
  const QColor& newColor, QColor newColor2)
{
  Q_D(qMRMLOculusThreeDViewControllerWidget);
  if (!d->ViewNode)
    {
    return;
    }
  int wasModifying = d->ViewNode->StartModify();
  // The ThreeDView displayable manager will change the background color of
  // the renderer.
  d->ViewNode->SetBackgroundColor(newColor.redF(), newColor.greenF(), newColor.blueF());
  if (!newColor2.isValid())
    {
    newColor2 = newColor;
    }
  d->ViewNode->SetBackgroundColor2(newColor2.redF(), newColor2.greenF(), newColor2.blueF());
  d->ViewNode->EndModify(wasModifying);
}

// --------------------------------------------------------------------------
void qMRMLOculusThreeDViewControllerWidget::setStereoType(int newStereoType)
{
  Q_D(qMRMLOculusThreeDViewControllerWidget);
  if (!d->ViewNode)
    {
    return;
    }
  d->ViewNode->SetStereoType(newStereoType);
}
