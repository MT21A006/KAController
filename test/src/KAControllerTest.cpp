// -*- C++ -*-
/*!
 * @file  KAControllerTest.cpp
 * @brief ${rtcParam.description}
 * @date $Date$
 *
 * $Id$
 */

#include "KAControllerTest.h"

// Module specification
// <rtc-template block="module_spec">
static const char* kacontroller_spec[] =
  {
    "implementation_id", "KAControllerTest",
    "type_name",         "KAControllerTest",
    "description",       "${rtcParam.description}",
    "version",           "1.0.0",
    "vendor",            "VenderName",
    "category",          "Category",
    "activity_type",     "EVENTDRIVEN",
    "kind",              "DataFlowComponent",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "compile",
    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
KAControllerTest::KAControllerTest(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_cylinderIn("cylinder", m_cylinder),
    m_armStatusIn("armStatus", m_armStatus),
    m_inStatusIn("inStatus", m_inStatus),
    m_currentPoseIn("currentPose", m_currentPose),
    m_armTipTargetOut("armTipTarget", m_armTipTarget),
    m_outDataOut("outData", m_outData),
    m_outStatusOut("outStatus", m_outStatus),
    m_camera_xyOut("camera_xy", m_camera_xy)

    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
KAControllerTest::~KAControllerTest()
{
}



RTC::ReturnCode_t KAControllerTest::onInitialize()
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  addInPort("armTipTarget", m_armTipTargetIn);
  addInPort("outData", m_outDataIn);
  addInPort("outStatus", m_outStatusIn);
  addInPort("camera_xy", m_camera_xyIn);
  
  // Set OutPort buffer
  addOutPort("cylinder", m_cylinderOut);
  addOutPort("armStatus", m_armStatusOut);
  addOutPort("inStatus", m_inStatusOut);
  addOutPort("currentPose", m_currentPoseOut);
  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

  // <rtc-template block="bind_config">
  // </rtc-template>
  
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t KAControllerTest::onFinalize()
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t KAControllerTest::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t KAControllerTest::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/


RTC::ReturnCode_t KAControllerTest::onActivated(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}


RTC::ReturnCode_t KAControllerTest::onDeactivated(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}


RTC::ReturnCode_t KAControllerTest::onExecute(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t KAControllerTest::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t KAControllerTest::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t KAControllerTest::onReset(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t KAControllerTest::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t KAControllerTest::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/



extern "C"
{
 
  void KAControllerTestInit(RTC::Manager* manager)
  {
    coil::Properties profile(kacontroller_spec);
    manager->registerFactory(profile,
                             RTC::Create<KAControllerTest>,
                             RTC::Delete<KAControllerTest>);
  }
  
};


