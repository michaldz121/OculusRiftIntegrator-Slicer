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
#include <QDebug>
#include <QFileInfo>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QResizeEvent>
#include <QToolButton>

// CTK includes
#include <ctkPopupWidget.h>

// qMRML includes
#include "qMRMLOculusThreeDViewControllerWidget.h"
#include "qMRMLOculusThreeDView.h"
#include "qMRMLOculusThreeDWidget.h"

// VTK includes
#include <vtkCollection.h>

//--------------------------------------------------------------------------
// qMRMLSliceViewPrivate
class qMRMLOculusThreeDWidgetPrivate
  : public QObject
{
  Q_DECLARE_PUBLIC(qMRMLOculusThreeDWidget);
protected:
  qMRMLOculusThreeDWidget* const q_ptr;
public:
  qMRMLOculusThreeDWidgetPrivate(qMRMLOculusThreeDWidget& object);
  ~qMRMLOculusThreeDWidgetPrivate();
  
  void init();

  qMRMLOculusThreeDView*       ThreeDView;
  qMRMLOculusThreeDViewControllerWidget* ThreeDController;
};


//---------------------------------------------------------------------------
qMRMLOculusThreeDWidgetPrivate::qMRMLOculusThreeDWidgetPrivate(qMRMLOculusThreeDWidget& object)
  : q_ptr(&object)
{
  this->ThreeDView = 0;
  this->ThreeDController = 0;
}

//---------------------------------------------------------------------------
qMRMLOculusThreeDWidgetPrivate::~qMRMLOculusThreeDWidgetPrivate()
{
}

//---------------------------------------------------------------------------
void qMRMLOculusThreeDWidgetPrivate::init()
{
  Q_Q(qMRMLOculusThreeDWidget);
  
  QVBoxLayout* layout = new QVBoxLayout(q);
  layout->setSpacing(0);
  layout->setContentsMargins(0, 0, 0, 0);

  this->ThreeDController = new qMRMLOculusThreeDViewControllerWidget;
  layout->addWidget(this->ThreeDController);

  this->ThreeDView = new qMRMLOculusThreeDView;
  layout->addWidget(this->ThreeDView);
  
  this->ThreeDController->setThreeDView(this->ThreeDView);

  QObject::connect(q, SIGNAL(mrmlSceneChanged(vtkMRMLScene*)),
                   this->ThreeDView, SLOT(setMRMLScene(vtkMRMLScene*)));
}

// --------------------------------------------------------------------------
// qMRMLOculusThreeDWidget methods

// --------------------------------------------------------------------------
qMRMLOculusThreeDWidget::qMRMLOculusThreeDWidget(QWidget* parentWidget)
  : Superclass(parentWidget)
  , d_ptr(new qMRMLOculusThreeDWidgetPrivate(*this))
{
  Q_D(qMRMLOculusThreeDWidget);
  d->init();
}

// --------------------------------------------------------------------------
qMRMLOculusThreeDWidget::~qMRMLOculusThreeDWidget()
{
}

// --------------------------------------------------------------------------
void qMRMLOculusThreeDWidget::addDisplayableManager(const QString& dManager)
{
  Q_D(qMRMLOculusThreeDWidget);
  d->ThreeDView->addDisplayableManager(dManager);
}

// --------------------------------------------------------------------------
void qMRMLOculusThreeDWidget::setMRMLViewNode(vtkMRMLViewNode* newViewNode)
{
  Q_D(qMRMLOculusThreeDWidget);
  //vtkMRMLViewNode* upcastViewNode = newViewNode;
  d->ThreeDView->setMRMLViewNode(newViewNode);
  d->ThreeDController->setMRMLViewNode(newViewNode);
}

// --------------------------------------------------------------------------
vtkMRMLViewNode* qMRMLOculusThreeDWidget::mrmlViewNode()const
{
  Q_D(const qMRMLOculusThreeDWidget);
  return d->ThreeDView->mrmlViewNode();
}

// --------------------------------------------------------------------------
qMRMLOculusThreeDView* qMRMLOculusThreeDWidget::threeDView()const
{
  Q_D(const qMRMLOculusThreeDWidget);
  return d->ThreeDView;
}

//---------------------------------------------------------------------------
void qMRMLOculusThreeDWidget::setViewLabel(const QString& newViewLabel)
{
  Q_D(qMRMLOculusThreeDWidget);
  d->ThreeDController->setViewLabel(newViewLabel);
}

//---------------------------------------------------------------------------
void qMRMLOculusThreeDWidget::setQuadBufferStereoSupportEnabled(bool value)
{
  Q_D(qMRMLOculusThreeDWidget);
  d->ThreeDController->setQuadBufferStereoSupportEnabled(value);
}

//---------------------------------------------------------------------------
QString qMRMLOculusThreeDWidget::viewLabel()const
{
  Q_D(const qMRMLOculusThreeDWidget);
  return d->ThreeDController->viewLabel();
}

//------------------------------------------------------------------------------
void qMRMLOculusThreeDWidget::getDisplayableManagers(vtkCollection *displayableManagers)
{
  Q_D(qMRMLOculusThreeDWidget);
  d->ThreeDView->getDisplayableManagers(displayableManagers);
}