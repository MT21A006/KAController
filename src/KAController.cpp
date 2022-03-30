// -*- C++ -*-
/*!
 * @file  KAController.cpp
 * @brief ${rtcParam.description}
 * @date $Date$
 *
 * $Id$
 */

#define _USE_MATH_DEFINES
#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include "KAController.h"

using namespace std;

const int NUM_ACTUATOR = 5;
const int cylinder_size = 7;
const int ave_set = 10;

const double X_distance = 0.15; // Kobukiとアームのズレ:x軸の距離
const double Y_distance = 0.1; // Kobukiとアームのズレ:y軸の距離

// 構造体「Cylinder」のオブジェクト(インスタンス) (x,y,z,radius,hue)
Cylinder cylinder;
// 比較関数
bool cmp(const Cylinder& a, const Cylinder& b);

// Module specification
// <rtc-template block="module_spec">
static const char* kacontroller_spec[] =
  {
    "implementation_id", "KAController",
    "type_name",         "KAController",
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
KAController::KAController(RTC::Manager* manager)
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
KAController::~KAController()
{
}



RTC::ReturnCode_t KAController::onInitialize()
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  addInPort("cylinder", m_cylinderIn);
  addInPort("armStatus", m_armStatusIn);
  addInPort("inStatus", m_inStatusIn);
  addInPort("currentPose", m_currentPoseIn);
  
  // Set OutPort buffer
  addOutPort("armTipTarget", m_armTipTargetOut);
  addOutPort("outData", m_outDataOut);
  addOutPort("outStatus", m_outStatusOut);
  addOutPort("camera_xy", m_camera_xyOut);
  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

  // <rtc-template block="bind_config">
  // </rtc-template>
  
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t KAController::onFinalize()
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t KAController::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t KAController::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/


RTC::ReturnCode_t KAController::onActivated(RTC::UniqueId ec_id)
{
    cout << "KAController::onActivated()" << endl;
    m_armTipTarget.data.length(NUM_ACTUATOR + 1);
    m_outData.data.length(NUM_ACTUATOR + 1);
    return RTC::RTC_OK;
}


RTC::ReturnCode_t KAController::onDeactivated(RTC::UniqueId ec_id)
{
    cylinderData.clear();
    return RTC::RTC_OK;
}


RTC::ReturnCode_t KAController::onExecute(RTC::UniqueId ec_id)
{
	double armform[3];//物体の距離[x,y,z]
	double armpoint;//アームから物体までの直線距離　
	ostringstream oss;
	m_inStatusIn.read();
	string aiueo = m_inStatus.data;
	//string aiueo = "赤色";
	if (aiueo == "aka" || aiueo == "ao" || aiueo == "midori" || aiueo == "ki") {
		if (m_cylinderIn.isNew()) {
			cout << "aiueo= " << aiueo << endl;
			cout << "m_cylinderisNew" << endl;////確認用
			m_cylinderIn.read();
			cout << "m_cylinder.data.length()" << m_cylinder.data.length() << endl;
			if (m_cylinder.data.length() >= 5) {
				cylinderData.clear();
				// 円柱の情報(x,y,z,radius,hue)を取得,Cylinder構造体のcylinderDataに格納する
				for (int i = 0; i < m_cylinder.data.length() / 5; i++) {
					Cylinder a;
					a.x = (float)m_cylinder.data[0 + i * 5];
					a.y = (float)m_cylinder.data[1 + i * 5];
					a.z = (float)m_cylinder.data[2 + i * 5];
					a.radius = (float)m_cylinder.data[3 + i * 5];
					a.hue = (float)m_cylinder.data[4 + i * 5];

					cylinderData.push_back(a);
				}
				for (int i = 0; i < cylinderData.size(); i++)
					cout << "距離参考 昇順 cylinderData[" << i << "] = " << cylinderData[i].hue << endl;
				if (cylinderData.size() > 0) {
					// 円柱が複数ある場合，各円柱の距離を近い順に並び替える．
					sort(cylinderData.begin(), cylinderData.end(), cmp);


					m_outStatus.data = "Watch";
					m_outStatusOut.write();
					cout << "cylinder_length = " << m_cylinder.data.length() << endl;
					cout << setw(10) << "x= " << m_cylinder.data[0] << ", y= " << m_cylinder.data[1] << ", z= " << m_cylinder.data[2] << ", r= " << m_cylinder.data[3] << endl;
					//cout << setw(11) << "x= " << -m_cylinder.data[2] << ",  y= " << -m_cylinder.data[0] << ", z= " << m_cylinder.data[1] << endl;

					cout << "円柱を認識しました 2秒停止" << endl;
					Sleep(2000);

					// Passedならxyz座標を計算して出力

					double r = m_cylinder.data[3];
					bool select = false;
					for (int i = 0; i < cylinderData.size(); i++) {
						select = false;
						r = cylinderData[i].radius;
						cout << "距離参考 昇順 cylinderData[" << i << "] = " << cylinderData[i].hue << endl;
						// 最も近い円柱色の取得し，colorに対応する色を格納
						if ((285 < cylinderData[i].hue && cylinderData[i].hue <= 360 && aiueo == "aka") || (0 <= cylinderData[i].hue && cylinderData[i].hue <= 45 && aiueo == "aka")) {
							cout << "円柱の色は 赤色" << endl;

							// 円柱距離等の平均をとる
							if (r <= 0.03) {
								cout << "認識できません もしくは 細すぎます" << endl;
								select = false;
							}
							else if (0.03 < r && r <= 0.045) {
								cout << "円柱を見つけました" << endl;
								select = true;
							}
							else if (0.045 < r) {
								cout << "認識できません もしくは 太すぎます" << endl;
								select = false;
							}
							else {
								cout << "認識できませんでした" << endl;
								select = false;
							}

						}
						else if (45 < cylinderData[i].hue && cylinderData[i].hue <= 75 && aiueo == "ki") {
							cout << "円柱の色は 黄色" << endl;

							// 円柱距離等の平均をとる
							if (r <= 0.03) {
								cout << "認識できません もしくは 細すぎます" << endl;
								select = false;
							}
							else if (0.03 < r && r <= 0.045) {
								cout << "円柱を見つけました" << endl;
								select = true;
							}
							else if (0.045 < r) {
								cout << "認識できません もしくは 太すぎます" << endl;
								select = false;
							}
							else {
								cout << "認識できませんでした" << endl;
								select = false;
							}

						}
						else if (75 < cylinderData[i].hue && cylinderData[i].hue <= 165 && aiueo == "midori") {
							cout << "円柱の色は 緑色" << endl;

							// 円柱距離等の平均をとる
							if (r <= 0.025) {
								cout << "認識できません もしくは 細すぎます" << endl;
								select = false;
							}
							else if (0.025 < r && r <= 0.045) {
								cout << "円柱を見つけました" << endl;
								select = true;
							}
							else if (0.045 < r) {
								cout << "認識できません もしくは 太すぎます" << endl;
								select = false;
							}
							else {
								cout << "認識できませんでした" << endl;
								select = false;
							}

						}
						else if (165 < cylinderData[i].hue && cylinderData[i].hue <= 285 && aiueo == "ao") {
							cout << "円柱の色は 青色" << endl;

							// 円柱距離等の平均をとる
							if (r <= 0.03) {
								cout << "認識できません もしくは 細すぎます" << endl;
								select = false;
							}
							else if (0.03 < r && r <= 0.045) {
								cout << "円柱を見つけました" << endl;
								select = true;
							}
							else if (0.045 < r) {
								cout << "認識できません もしくは 太すぎます" << endl;
								select = false;
							}
							else {
								cout << "認識できませんでした" << endl;
								select = false;
							}

						}
						else {
							cout << "色情報の取得に失敗" << endl;
							cout << cylinder.hue << endl;
						}
						if (select == true) {

							armform[0] = cylinderData[i].x;//Arm:x
							armform[1] = cylinderData[i].y;//Arm:y
							armform[2] = cylinderData[i].z + 0.1;//Arm:z

							cout << "x=" << armform[0] << ",  y=" << armform[1] << ", z=" << armform[2];

							armpoint = sqrt(pow(armform[0], 2) + pow(armform[1], 2));
							cout << "物体までの距離" << armpoint << "[m]" << endl;
							break;
						}
					}

					if (select == true && 0.18 <= armpoint && armpoint <= 0.25) {
						select = true;
					}
					else if (select == true && (0.18 >= armpoint || armpoint >= 0.25)) {
						m_outStatus.data = "Path";
						m_outStatusOut.write();
						NotFound = 0;
						select = false;
						cout << "armform[0]" << "= " << armform[0] << ", armform[1]= " << armform[1];
						double xpath = armform[0] - 0.15;
						double ypath = armform[1] - 0.1;
						oss << "goal" << " " << xpath << " " << ypath;
						m_camera_xy.data = oss.str().c_str();
						cout << m_camera_xy.data << endl;
						m_camera_xyOut.write();
					}
					if (select == true) {
						//cout << "把持動作を行います" << endl;
						m_outStatus.data = "Move";
						m_outStatusOut.write();
						//把持準備動作
						m_armTipTarget.data[0] = (armpoint - 0.05) * (armform[0]) / armpoint; //x
						m_armTipTarget.data[1] = (armpoint - 0.05) * (armform[1]) / armpoint; //y
						m_armTipTarget.data[2] = armform[2]; //z
						m_armTipTarget.data[3] = 0.0;   // pitch
						m_armTipTarget.data[4] = 1.0;   // hand
						m_armTipTarget.data[5] = 4;     // time
						m_armTipTargetOut.write();
						Sleep(5000);

						//
						m_armTipTarget.data[0] = armform[0]; //x
						m_armTipTarget.data[1] = armform[1]; //y
						m_armTipTarget.data[2] = armform[2]; //z
						m_armTipTarget.data[3] = 0.0;   // pitch
						m_armTipTarget.data[4] = 0.8;   // hand
						m_armTipTarget.data[5] = 3;     // time
						m_armTipTargetOut.write();
						Sleep(5000);

						//掴む
						m_armTipTarget.data[0] = armform[0];
						m_armTipTarget.data[1] = armform[1];
						m_armTipTarget.data[2] = armform[2];
						m_armTipTarget.data[3] = 0.0;   // pitch
						m_armTipTarget.data[4] = 0.3;   // hand
						m_armTipTarget.data[5] = 3;     // time
						m_armTipTargetOut.write();
						Sleep(5000);

						//掴んだまま引き寄せる
						m_armTipTarget.data[0] = (armpoint - 0.08) * (armform[0]) / armpoint;
						m_armTipTarget.data[1] = (armpoint - 0.08) * (armform[1]) / armpoint;
						m_armTipTarget.data[2] = armform[2] + 0.03;
						m_armTipTarget.data[3] = 0.0;   // pitch
						m_armTipTarget.data[4] = 0.3;   // hand
						m_armTipTarget.data[5] = 3;     // time
						m_armTipTargetOut.write();
						Sleep(5000);

						m_outData.data[0] = 1.8;
						m_outData.data[1] = 2.7;
						m_outData.data[2] = -2.3;
						m_outData.data[3] = -0.5;   // pitch
						m_outData.data[4] = 0.1;   // hand
						m_outData.data[5] = 4;     // time
						m_outDataOut.write();
						Sleep(5000);
						m_outStatus.data = "Moved";
						m_outStatusOut.write();

					}

					cout << endl;

					//Sleep(3000);


		  }
	  }
	}

  }
	else if (aiueo == "Put") {
		cout << "物体を置く動作" << endl;
		m_outData.data[0] = 1.8;   // Kinect2ToPC:z -> Arm:x
		m_outData.data[1] = 2.7;   // Kinect2ToPC:x -> Arm:y
		m_outData.data[2] = -2.3;
		m_outData.data[3] = -0.3;   // pitch
		m_outData.data[4] = 0.3;   // hand
		m_outData.data[5] = 3;     // time
		m_outDataOut.write();
		Sleep(5000);

		m_armTipTarget.data[0] = 0.23;   // Kinect2ToPC:z -> Arm:x
		m_armTipTarget.data[1] = 0.0;   // Kinect2ToPC:x -> Arm:y
		m_armTipTarget.data[2] = 0.18;
		m_armTipTarget.data[3] = 0.0;   // pitch
		m_armTipTarget.data[4] = 0.3;   // hand
		m_armTipTarget.data[5] = 5;     // time
		m_armTipTargetOut.write();
		Sleep(5000);

		m_armTipTarget.data[0] = 0.23;   // Kinect2ToPC:z -> Arm:x
		m_armTipTarget.data[1] = 0.0;   // Kinect2ToPC:x -> Arm:y
		m_armTipTarget.data[2] = 0.13;
		m_armTipTarget.data[3] = 0.0;   // pitch
		m_armTipTarget.data[4] = 0.3;   // hand
		m_armTipTarget.data[5] = 5;     // time
		m_armTipTargetOut.write();
		Sleep(5000);

		m_armTipTarget.data[0] = 0.23;   // Kinect2ToPC:z -> Arm:x
		m_armTipTarget.data[1] = 0.0;   // Kinect2ToPC:x -> Arm:y
		m_armTipTarget.data[2] = 0.0;
		m_armTipTarget.data[3] = 0.0;   // pitch
		m_armTipTarget.data[4] = 0.3;   // hand
		m_armTipTarget.data[5] = 5;     // time
		m_armTipTargetOut.write();
		Sleep(5000);

		m_armTipTarget.data[0] = 0.18;   // Kinect2ToPC:z -> Arm:x
		m_armTipTarget.data[1] = 0.0;   // Kinect2ToPC:x -> Arm:y
		m_armTipTarget.data[2] = 0.0;
		m_armTipTarget.data[3] = 0.0;   // pitch
		m_armTipTarget.data[4] = 0.8;   // hand
		m_armTipTarget.data[5] = 5;     // time
		m_armTipTargetOut.write();
		Sleep(5000);

		m_armTipTarget.data[0] = 0.16;   // Kinect2ToPC:z -> Arm:x
		m_armTipTarget.data[1] = 0.0;   // Kinect2ToPC:x -> Arm:y
		m_armTipTarget.data[2] = 0.13;
		m_armTipTarget.data[3] = 0.0;   // pitch
		m_armTipTarget.data[4] = 1;   // hand
		m_armTipTarget.data[5] = 3;     // time
		m_armTipTargetOut.write();
		Sleep(5000);

		m_outData.data[0] = 0;   // Kinect2ToPC:z -> Arm:x
		m_outData.data[1] = 2.7;   // Kinect2ToPC:x -> Arm:y
		m_outData.data[2] = -2.3;
		m_outData.data[3] = -0.5;   // pitch
		m_outData.data[4] = 0;   // hand
		m_outData.data[5] = 5;     // time
		m_outDataOut.write();
		Sleep(5000);

		m_outStatus.data = "Moved";
		m_outStatusOut.write();
	}

  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t KAController::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t KAController::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t KAController::onReset(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t KAController::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t KAController::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

bool cmp(const Cylinder& a, const Cylinder& b)
{
	float aDistance = sqrt(pow(a.x, 2) + pow(a.y, 2));
	float bDistance = sqrt(pow(b.x, 2) + pow(b.y, 2));
	return aDistance < bDistance;
}

extern "C"
{
 
  void KAControllerInit(RTC::Manager* manager)
  {
    coil::Properties profile(kacontroller_spec);
    manager->registerFactory(profile,
                             RTC::Create<KAController>,
                             RTC::Delete<KAController>);
  }
  
};


