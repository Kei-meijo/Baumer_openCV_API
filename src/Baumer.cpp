#include "Baumer.h"
#include <iostream>

namespace baumer {
	VideoCapture::VideoCapture() {
		openSystem();
	}

	VideoCapture::~VideoCapture() {
		try {
			for (auto cam : this->cameras) {
				cam.close();
			}
			this->deviceList.close();
			this->interfaceList.close();
			this->systemList.close();
		} catch (BGAPI2::Exceptions::IException& ex) {

		}
	}

	bool VideoCapture::openSystem() {
		systemList.initInstance();
		for (auto it_s = systemList.begin(); it_s != systemList.end(); it_s++) {
			if (!interfaceList.set(it_s))continue;
			for (auto it_i = interfaceList.begin(); it_i != interfaceList.end(); it_i++) {
				if (!deviceList.set(it_i))continue;
				for (auto it_d = deviceList.begin(); it_d != deviceList.end(); it_d++) {
					baumer_device dev;
					if(!dev.set(it_d))continue;
					this->cameras.push_back(dev);
				}
			}
		}
		if (this->cameras.size() == 0)return false;
		return true;
	}

	bool VideoCapture::start() {
		bool canStartCam = this->cameras.size() > 0;
		for (auto cam : this->cameras) {
			canStartCam &= cam.startCamera();
		}

		return canStartCam;
	}

	bool VideoCapture::stop() {
		bool canStopCam = true;
		for (auto cam : this->cameras) {
			canStopCam &= cam.stopCamera();
		}

		return canStopCam;
	}
}