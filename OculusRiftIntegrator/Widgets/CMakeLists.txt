project(qSlicer${MODULE_NAME}ModuleWidgets)

set(KIT ${PROJECT_NAME})

set(${KIT}_EXPORT_DIRECTIVE "Q_SLICER_MODULE_${MODULE_NAME_UPPER}_WIDGETS_EXPORT")

set(${KIT}_INCLUDE_DIRECTORIES
  ${vtkSlicerOculusRiftIntegratorModuleLogic_INCLUDE_DIRS}
  )

set(${KIT}_SRCS
  qMRMLOculusThreeDViewControllerWidget.cxx
  qMRMLOculusThreeDViewControllerWidget.h
  qMRMLOculusThreeDViewControllerWidget_p.h
  qMRMLOculusThreeDView.cxx
  qMRMLOculusThreeDView.h
  qMRMLOculusThreeDView_p.h
  qMRMLOculusThreeDWidget.cxx
  qMRMLOculusThreeDWidget.h
  )

set(${KIT}_MOC_SRCS
  qMRMLOculusThreeDViewControllerWidget.h
  qMRMLOculusThreeDWidget.h
  qMRMLOculusThreeDView.h
  qMRMLOculusThreeDView_p.h
  )

set(${KIT}_UI_SRCS
  Resources/UI/qMRMLOculusThreeDViewControllerWidget.ui
  )

set(${KIT}_RESOURCES
  Resources/${KIT}.qrc
  )

set(${KIT}_TARGET_LIBRARIES
  vtkSlicer${MODULE_NAME}ModuleLogic
  )

#-----------------------------------------------------------------------------
SlicerMacroBuildModuleWidgets(
  NAME ${KIT}
  EXPORT_DIRECTIVE ${${KIT}_EXPORT_DIRECTIVE}
  INCLUDE_DIRECTORIES ${${KIT}_INCLUDE_DIRECTORIES}
  SRCS ${${KIT}_SRCS}
  MOC_SRCS ${${KIT}_MOC_SRCS}
  UI_SRCS ${${KIT}_UI_SRCS}
  TARGET_LIBRARIES ${${KIT}_TARGET_LIBRARIES}
  RESOURCES ${${KIT}_RESOURCES}
  WRAP_PYTHONQT
  )

#-----------------------------------------------------------------------------
if(BUILD_TESTING)
  #add_subdirectory(Testing)
endif()