/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMRMLOculusViewNode.h,v $
  Date:      $Date: 2006/03/19 17:12:28 $
  Version:   $Revision: 1.6 $

=========================================================================auto=*/

#ifndef __vtkMRMLOculusViewNode_h
#define __vtkMRMLOculusViewNode_h

// VTK includes
#include "vtkMRMLViewNode.h"
#include "vtkMRMLAbstractViewNode.h"

#include "vtkSlicerOculusRiftIntegratorModuleLogicExport.h"

class VTK_SLICER_OCULUSRIFTINTEGRATOR_MODULE_LOGIC_EXPORT vtkMRMLOculusViewNode
 : public vtkMRMLViewNode
{
public:
  static vtkMRMLOculusViewNode *New();
  vtkTypeMacro(vtkMRMLOculusViewNode,vtkMRMLAbstractViewNode);
  
  //--------------------------------------------------------------------------
  /// MRMLNode methods
  //--------------------------------------------------------------------------

  virtual vtkMRMLNode* CreateNodeInstance();

  /// 
  /// Read node attributes from XML file
  virtual void ReadXMLAttributes( const char** atts);

  /// 
  /// Write this node's information to a MRML file in XML format.
  virtual void WriteXML(ostream& of, int indent);

  /// Stereo modes
  enum
    {
      NoStereo = 0,
      RedBlue,
      Anaglyph,
      QuadBuffer,
      Interlaced,
	    OculusRift
    };

protected:
  vtkMRMLOculusViewNode();
  ~vtkMRMLOculusViewNode();
  vtkMRMLOculusViewNode(const vtkMRMLOculusViewNode&);
  void operator=(const vtkMRMLOculusViewNode&);

};

#endif
