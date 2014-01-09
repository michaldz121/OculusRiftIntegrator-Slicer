/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

See COPYRIGHT.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkMRMLOculusViewNode.cxx,v $
Date:      $Date: 2006/03/03 22:26:39 $
Version:   $Revision: 1.3 $

=========================================================================auto=*/

// MRML includes
#include "vtkMRMLScene.h"
#include "vtkMRMLViewNode.h"
#include "vtkMRMLOculusViewNode.h"

// VTK includes
#include <vtkObjectFactory.h>

// STD includes
#include <sstream>

//----------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLOculusViewNode);


//----------------------------------------------------------------------------
vtkMRMLOculusViewNode::vtkMRMLOculusViewNode()
{
  this->BoxVisible = 1;
  this->AxisLabelsVisible = 1;
  this->AxisLabelsCameraDependent = 1;
  this->FiducialsVisible = 1;
  this->FiducialLabelsVisible = 1;
  this->FieldOfView = 200;
  this->LetterSize = 0.05;
  this->AnimationMode = vtkMRMLOculusViewNode::Off;
  this->ViewAxisMode = vtkMRMLOculusViewNode::LookFrom;
  this->SpinDegrees = 2.0;
  this->RotateDegrees = 5.0;
  this->SpinDirection = vtkMRMLOculusViewNode::YawLeft;
  this->AnimationMs = 5;
  this->RockLength = 200;
  this->RockCount = 0;
  this->StereoType = vtkMRMLOculusViewNode::NoStereo;
  this->RenderMode = vtkMRMLOculusViewNode::Perspective;
  this->BackgroundColor[0] = this->defaultBackgroundColor()[0];
  this->BackgroundColor[1] = this->defaultBackgroundColor()[1];
  this->BackgroundColor[2] = this->defaultBackgroundColor()[2];
  this->BackgroundColor2[0] = this->defaultBackgroundColor2()[0];
  this->BackgroundColor2[1] = this->defaultBackgroundColor2()[1];
  this->BackgroundColor2[2] = this->defaultBackgroundColor2()[2];
  this->UseDepthPeeling = 0;
  this->FPSVisible = 0;
 }
 
//----------------------------------------------------------------------------
vtkMRMLOculusViewNode::~vtkMRMLOculusViewNode()
{
}

//----------------------------------------------------------------------------
void vtkMRMLOculusViewNode::WriteXML(ostream& of, int nIndent)
{
  // Write all attributes not equal to their defaults

  this->Superclass::WriteXML(of, nIndent);

  vtkIndent indent(nIndent);

  of << indent << " fieldOfView=\"" << this->GetFieldOfView() << "\"";
  of << indent << " letterSize=\"" << this->GetLetterSize() << "\"";
  of << indent << " boxVisible=\"" << (this->BoxVisible ? "true" : "false") << "\"";
  of << indent << " fiducialsVisible=\"" << (this->FiducialsVisible ? "true" : "false") << "\"";
  of << indent << " fiducialLabelsVisible=\"" << (this->FiducialLabelsVisible ? "true" : "false") << "\"";
  of << indent << " axisLabelsVisible=\"" << (this->AxisLabelsVisible ? "true" : "false") << "\"";
  of << indent << " axisLabelsCameraDependent=\"" << (this->AxisLabelsCameraDependent ? "true" : "false") << "\"";

  // spin or rock?
  if ( this->GetAnimationMode() == vtkMRMLOculusViewNode::Off )
    {
    of << indent << " animationMode=\"" << "Off" << "\"";
    }
  else if ( this->GetAnimationMode() == vtkMRMLOculusViewNode::Spin )
    {
    of << indent << " animationMode=\"" << "Spin" << "\"";
    }
  else if ( this->GetAnimationMode() == vtkMRMLOculusViewNode::Rock )
    {
    of << indent << " animationMode=\"" << "Rock" << "\"";
    }
  
  if ( this->GetViewAxisMode() == vtkMRMLOculusViewNode::LookFrom )
    {
    of << indent << " viewAxisMode=\"" << "LookFrom" << "\"";
    }
  else if ( this->GetViewAxisMode() == vtkMRMLOculusViewNode::RotateAround )
    {
    of << indent << " viewAxisMode=\"" << "RotateAround" << "\"";
    }
  
  // configure spin
  of << indent << " spinDegrees=\"" << this->GetSpinDegrees() << "\"";
  of << indent << " spinMs=\"" << this->GetAnimationMs() << "\"";
  if ( this->GetSpinDirection() == vtkMRMLOculusViewNode::PitchUp )
    {
    of << indent << " spinDirection=\"" << "PitchUp" << "\"";
    }
  else if ( this->GetSpinDirection() == vtkMRMLOculusViewNode::PitchDown )
    {
    of << indent << " spinDirection=\"" << "PitchDown" << "\"";
    }
  else if ( this->GetSpinDirection() == vtkMRMLOculusViewNode::RollLeft )
    {
    of << indent << " spinDirection=\"" << "RollLeft" << "\"";
    }
  else if ( this->GetSpinDirection() == vtkMRMLOculusViewNode::RollRight )
    {
    of << indent << " spinDirection=\"" << "RollRight" << "\"";
    }
  else if ( this->GetSpinDirection() == vtkMRMLOculusViewNode::YawLeft )
    {
    of << indent << " spinDirection=\"" << "YawLeft" << "\"";
    }
  else if ( this->GetSpinDirection() == vtkMRMLOculusViewNode::YawRight )
    {
    of << indent << " spinDirection=\"" << "YawRight" << "\"";
    }

  of << indent << " rotateDegrees=\"" << this->GetRotateDegrees() << "\"";
  
  // configure rock
  of << indent << " rockLength=\"" << this->GetRockLength() << "\"";
  of << indent << " rockCount=\"" << this->GetRockCount() << "\"";
  
  // configure stereo
  if ( this->GetStereoType() == vtkMRMLOculusViewNode::NoStereo )
    {
    of << indent << " stereoType=\"" << "NoStereo" << "\"";    
    }
  else if ( this->GetStereoType() == vtkMRMLOculusViewNode::RedBlue )
    {
    of << indent << " stereoType=\"" << "RedBlue" << "\"";    
    }
  else if ( this->GetStereoType() == vtkMRMLOculusViewNode::Anaglyph )
    {
    of << indent << " stereoType=\"" << "Anaglyph" << "\"";    
    }
  else if ( this->GetStereoType() == vtkMRMLOculusViewNode::QuadBuffer )
    {
    of << indent << " stereoType=\"" << "QuadBuffer" << "\"";
    }
  else if ( this->GetStereoType() == vtkMRMLOculusViewNode::Interlaced )
    {
    of << indent << " stereoType=\"" << "Interlaced" << "\"";    
    }
  else if ( this->GetStereoType() == vtkMRMLOculusViewNode::OculusRift )
    {
    of << indent << " stereoType=\"" << "OculusRift" << "\"";    
    }	

  // configure render mode
  if (this->GetRenderMode() == vtkMRMLOculusViewNode::Perspective )
    {
    of << indent << " renderMode=\"" << "Perspective" << "\"";
    }
  else if ( this->GetRenderMode() == vtkMRMLOculusViewNode::Orthographic )
    {
    of << indent << " renderMode=\"" << "Orthographic" << "\"";
    }

  of << indent << " useDepthPeeling=\"" << this->GetUseDepthPeeling() << "\"";

}

//----------------------------------------------------------------------------
void vtkMRMLOculusViewNode::ReadXMLAttributes(const char** atts)
{
  int disabledModify = this->StartModify();

  this->Superclass::ReadXMLAttributes(atts);

  const char* attName;
  const char* attValue;
  while (*atts != NULL) 
    {
    attName = *(atts++);
    attValue = *(atts++);
    if (!strcmp(attName, "fieldOfView")) 
      {
      std::stringstream ss;
      ss << attValue;
      double fov;
      ss >> fov;
      this->FieldOfView = fov;
      }
    else if (!strcmp(attName, "letterSize")) 
      {
      std::stringstream ss;
      ss << attValue;
      double fov;
      ss >> fov;
      this->LetterSize = fov;
      }
    else if (!strcmp(attName, "boxVisible")) 
      {
      if (!strcmp(attValue,"true")) 
        {
        this->BoxVisible = 1;
        }
      else
        {
        this->BoxVisible = 0;
        }
      }
    else if (!strcmp(attName, "fiducialsVisible")) 
      {
      if (!strcmp(attValue,"true")) 
        {
        this->FiducialsVisible = 1;
        }
      else
        {
        this->FiducialsVisible = 0;
        }
      }
    else if (!strcmp(attName, "fiducialLabelsVisible")) 
      {
      if (!strcmp(attValue,"true")) 
        {
        this->FiducialLabelsVisible = 1;
        }
      else
        {
        this->FiducialLabelsVisible = 0;
        }
      }
    else if (!strcmp(attName, "axisLabelsVisible")) 
      {
      if (!strcmp(attValue,"true")) 
        {
        this->AxisLabelsVisible = 1;
        }
      else
        {
        this->AxisLabelsVisible = 0;
        }
      }
    else if (!strcmp(attName, "axisLabelsCameraDependent")) 
      {
      if (!strcmp(attValue,"true")) 
        {
        this->AxisLabelsCameraDependent = 1;
        }
      else
        {
        this->AxisLabelsCameraDependent = 0;
        }
      }
    else if (!strcmp(attName, "stereoType")) 
      {
      if (!strcmp(attValue,"NoStereo")) 
        {
        this->StereoType = vtkMRMLOculusViewNode::NoStereo;
        }
      else if ( !strcmp (attValue, "RedBlue" ))
        {
        this->StereoType = vtkMRMLOculusViewNode::RedBlue;
        }
      else if ( !strcmp (attValue, "Anaglyph" ))
        {
        this->StereoType = vtkMRMLOculusViewNode::Anaglyph;
        }
      else if ( !strcmp (attValue, "QuadBuffer" ))
        {
        this->StereoType = vtkMRMLOculusViewNode::QuadBuffer;
        }
      else if ( !strcmp (attValue, "Interlaced" ))
        {
        this->StereoType = vtkMRMLOculusViewNode::Interlaced;
        }
      else if ( !strcmp (attValue, "OculusRift" ))
        {
        this->StereoType = vtkMRMLOculusViewNode::OculusRift;
        }
      }
    else if (!strcmp(attName, "rockLength" ))
      {
      std::stringstream ss;
      ss << attValue;
      int len;
      ss >> len;
      this->RockLength = len;
      }
    else if (!strcmp(attName, "rockCount" ))
      {
      std::stringstream ss;
      ss << attValue;
      int count;
      ss >> count;
      this->RockCount = count;
      }
    else if (!strcmp(attName, "animationMode")) 
      {
      if (!strcmp(attValue,"Off")) 
        {
        this->AnimationMode = vtkMRMLOculusViewNode::Off;
        }
      else if (!strcmp(attValue,"Spin")) 
        {
        this->AnimationMode = vtkMRMLOculusViewNode::Spin;
        }
      else if (!strcmp(attValue,"Rock")) 
        {
        this->AnimationMode = vtkMRMLOculusViewNode::Rock;
        }
      }
    else if (!strcmp (attName, "viewAxisMode"))
      {
      if (!strcmp (attValue, "RotateAround"))
        {
        this->ViewAxisMode = vtkMRMLOculusViewNode::RotateAround;
        }
      else if (!strcmp (attValue, "LookFrom"))
        {
        this->ViewAxisMode = vtkMRMLOculusViewNode::LookFrom;
        }
      }
    else if (!strcmp(attName, "spinDegrees" ))
      {
      std::stringstream ss;
      ss << attValue;
      double deg;
      ss >> deg;
      this->SpinDegrees = deg;
      }
    else if ( !strcmp ( attName, "rotateDegrees"))
      {
      std::stringstream ss;
      ss << attValue;
      double deg;
      ss >> deg;
      this->RotateDegrees = deg;
      }
    else if (!strcmp(attName, "spinMs" ))
      {
      std::stringstream ss;
      ss << attValue;
      int ms;
      ss >> ms;
      this->AnimationMs = ms;
      }
    else if (!strcmp(attName, "spinDirection")) 
      {
      if (!strcmp(attValue,"RollLeft")) 
        {
        this->SpinDirection = vtkMRMLOculusViewNode::RollLeft;
        }
      else if ( !strcmp (attValue, "RollRight" ))
        {
        this->SpinDirection = vtkMRMLOculusViewNode::RollRight;
        }
      else if (!strcmp(attValue,"YawLeft")) 
        {
        this->SpinDirection = vtkMRMLOculusViewNode::YawLeft;
        }
      else if ( !strcmp (attValue, "YawRight" ))
        {
        this->SpinDirection = vtkMRMLOculusViewNode::YawRight;
        }
      else if ( !strcmp (attValue, "PitchUp" ))
        {
        this->SpinDirection = vtkMRMLOculusViewNode::PitchUp;
        }
      else if ( !strcmp (attValue, "PitchDown" ))
        {
        this->SpinDirection = vtkMRMLOculusViewNode::PitchDown;
        }
      }
    else if (!strcmp(attName, "renderMode")) 
      {
      if (!strcmp(attValue,"Perspective")) 
        {
        this->RenderMode = vtkMRMLOculusViewNode::Perspective;
        }
      else if ( !strcmp (attValue, "Orthographic" ))
        {
        this->RenderMode = vtkMRMLOculusViewNode::Orthographic;
        }
      }
    else if (!strcmp(attName, "useDepthPeeling" ))
      {
      std::stringstream ss;
      ss << attValue;
      int use;
      ss >> use;
      this->SetUseDepthPeeling(use);
      }
    }
  this->EndModify(disabledModify);
}
