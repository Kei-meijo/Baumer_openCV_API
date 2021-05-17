#ifndef RSDLAB_BAUMER_OPENCV_API
#define RSDLAB_BAUMER_OPENCV_API


#if _MSC_VER > 1000
#pragma once
#endif

#include <vector>
#include <exception>
#include <opencv2/opencv.hpp>
#include "bgapi2_genicam/bgapi2_genicam.hpp"

namespace baumer {
	class VideoCapture {
	private:

		/**
		* baumer��System�̈ꗗ�Ɋւ��鏈��
		* (private)
		*/
		struct baumer_system_list {
			BGAPI2::SystemList *systemList = NULL;

			/**
			* ������
			*/
			void initInstance() {
				systemList = BGAPI2::SystemList::GetInstance();
				systemList->Refresh();
			}


			/**
			* �C�e���[�^
			*/
			BGAPI2::SystemList::iterator begin() {
				return systemList->begin();
			}
			
			/**
			* �C�e���[�^
			*/
			BGAPI2::SystemList::iterator end() {
				return systemList->end();
			}

			/**
			* �I������
			* @return bool �������I���ł�����
			*/
			bool close() {
				BGAPI2::SystemList::ReleaseInstance();
				return true;
			}
		};
		baumer_system_list systemList;

		/**
		* baumer��interface�̈ꗗ�Ɋւ��鏈��
		* (private)
		*/
		struct baumer_interface_list {
			BGAPI2::System * pSystem = NULL;
			BGAPI2::String sSystemID;
			BGAPI2::InterfaceList *interfaceList = NULL;

			/**
			* �����ݒ�
			* @param[in] it �V�X�e�����X�g�C�e���[�^
			* @return bool �ݒ肪�������s������
			*/
			bool set(BGAPI2::SystemList::iterator it) {
				try {
					pSystem = it->second;
					sSystemID = it->first;

					pSystem->Open();

					interfaceList = pSystem->GetInterfaces();
					interfaceList->Refresh(100);
				} catch (BGAPI2::Exceptions::IException& ex) { return false; }

				if (sSystemID == "") {
					pSystem->Close();
					BGAPI2::SystemList::ReleaseInstance();
					return false;
				}

				return true;
			}

			/**
			* �C�e���[�^
			*/
			BGAPI2::InterfaceList::iterator begin() {
				return interfaceList->begin();
			}

			/**
			* �C�e���[�^
			*/
			BGAPI2::InterfaceList::iterator end() {
				return interfaceList->end();
			}

			/**
			* �I������
			* @return bool �������I���ł�����
			*/
			bool close() {
				pSystem->Close();
				return true;
			}
		};
		baumer_interface_list interfaceList;

		/**
		* Baumer��Device�̈ꗗ�Ɋւ��鏈��
		* (private)
		*/
		struct baumer_device_list {
			BGAPI2::Interface * pInterface = NULL;
			BGAPI2::String sInterfaceID;
			BGAPI2::DeviceList *deviceList = NULL;

			/**
			* �����ݒ�
			* @oaram[int] it �C���^�[�t�F�[�X���X�g�C�e���[�^
			* @return �����������ݒ肪�s��ꂽ��
			*/
			bool set(BGAPI2::InterfaceList::iterator it) {
				try {
					pInterface = it->second;
					sInterfaceID = it->first;
					pInterface->Open();

					deviceList = pInterface->GetDevices();
					deviceList->Refresh(100);

					if (deviceList->size() > 0) {
						sInterfaceID = it->first;
					}
				} catch (BGAPI2::Exceptions::ResourceInUseException& ex) { return false; }

				if (sInterfaceID == "") {
					pInterface->Close();
					return false;
				}

				return true;
			}

			/**
			* �C�e���[�^
			*/
			BGAPI2::DeviceList::iterator begin() {
				return deviceList->begin();
			}

			/**
			* �C�e���[�^
			*/
			BGAPI2::DeviceList::iterator end() {
				return deviceList->end();
			}

			/**
			* �I������
			* @return bool �������I���ł�����
			*/
			bool close() {
				pInterface->Close();
				return true;
			}
		};
		baumer_device_list deviceList;


	public:
		/**
		* Baumer�̃J�����f�o�C�X�̏������܂Ƃ߂��N���X
		*/
		struct baumer_device {
		private:
			BGAPI2::Device * pDevice = NULL;
			BGAPI2::String sDeviceID;

			BGAPI2::Node* pExposureTime = NULL;
			BGAPI2::Node* pGain = NULL;

			bo_double fExposureTimeMin = 0;
			bo_double fExposureTimeMax = 0;
			BGAPI2::String sExposureNodeName = "";

			bo_double fGainMin = 0;
			bo_double fGainMax = 0;
			bool capturing = false;

			/**
			* �f�[�^�X�g���[���̏������܂Ƃ߂��N���X
			*/
			struct beumer_data_stream {
				BGAPI2::DataStreamList *datastreamList = NULL;
				BGAPI2::DataStream * pDataStream = NULL;
				BGAPI2::String sDataStreamID;

				BGAPI2::BufferList *bufferList = NULL;
				BGAPI2::Buffer * pBuffer = NULL;
				BGAPI2::String sBufferID;
				BGAPI2::Buffer * pBufferFilled = NULL;
				bool streaming = false;

				/**
				* �����ݒ�
				* @param[in] dev �f�o�C�X
				* @return bool �����ݒ肪�������s��ꂽ��
				*/
				bool set(BGAPI2::Device* dev) {
					datastreamList = dev->GetDataStreams();
					datastreamList->Refresh();

					for (auto dstIterator = datastreamList->begin(); dstIterator != datastreamList->end(); dstIterator++) {
						try {
							dstIterator->second->Open();
							sDataStreamID = dstIterator->first;
							break;
						} catch (BGAPI2::Exceptions::IException& ex) {}
					}

					if (sDataStreamID == "") {
						return false;
					} else {
						pDataStream = (*datastreamList)[sDataStreamID];
					}

					return true;
				}

				/**
				* �f�[�^�]���J�n
				* @return bool
				* �f�[�^�]���������J�n�ł�����
				*/
				bool startStream() {
					if (streaming)return true;
					bufferList = pDataStream->GetBufferList();

					try {
						for (int i = 0; i<4; i++) {
							pBuffer = new BGAPI2::Buffer();
							bufferList->Add(pBuffer);
						}
						for (BGAPI2::BufferList::iterator bufIterator = bufferList->begin(); bufIterator != bufferList->end(); bufIterator++) {
							bufIterator->second->QueueBuffer();
						} 
					} catch (BGAPI2::Exceptions::IException& ex) { return false; }

					try {
						pDataStream->StartAcquisitionContinuous();
					} catch (BGAPI2::Exceptions::IException& ex) { return false; }

					streaming = true;
					return true;
				}

				/**
				* �f�[�^�]���I��
				* @return bool �f�[�^�]�����I���ł�����
				*/
				bool stopStream() {
					if (!streaming)return false;
					try {
						pDataStream->StopAcquisition();
						bufferList->DiscardAllBuffers();
					} catch (BGAPI2::Exceptions::IException& ex) { return false; }

					streaming = false;
					return true;
				}

				/**
				* �摜�̓ǂݍ���
				* @param[out] mat �摜�o��
				* @param bool �摜���ǂݍ��߂���
				*/
				bool read(cv::Mat& mat) {
					try {
						pBufferFilled = pDataStream->GetFilledBuffer(1000); //timeout 1000 msec
						if (pBufferFilled == NULL) {
							std::cerr << "Error: Buffer Timeout after 1000 msec" << std::endl;
							return false;
						} else if (pBufferFilled->GetIsIncomplete()) {
							std::cerr << "Error: Image is incomplete" << std::endl;
							// queue buffer again
							pBufferFilled->QueueBuffer();
						} else {
							if ((pBufferFilled->GetPixelFormat() == "BGR8") || (pBufferFilled->GetPixelFormat() == "BGR8Packed")) {//BGR8 is openCV default format
								cv::Mat *tmp = new cv::Mat((int)pBufferFilled->GetHeight(), (int)pBufferFilled->GetWidth(), CV_8UC3, (char *)((bo_uint64)(pBufferFilled->GetMemPtr()) + pBufferFilled->GetImageOffset()));
								mat = *tmp;

							} else if ((pBufferFilled->GetPixelFormat() == "BayerRG8") || (pBufferFilled->GetPixelFormat() == "BayerGB8")) {//need conversion to BGR8 is openCV default format
								cv::Mat* imOriginal = new cv::Mat((int)pBufferFilled->GetHeight(), (int)pBufferFilled->GetWidth(), CV_8UC1, (char *)((bo_uint64)(pBufferFilled->GetMemPtr()) + pBufferFilled->GetImageOffset()));
								cv::Mat* imTransformBGR8 = new cv::Mat((int)pBufferFilled->GetHeight(), (int)pBufferFilled->GetWidth(), CV_8UC3); //memory allocation

								if (pBufferFilled->GetPixelFormat() == "BayerRG8") {
									cv::cvtColor(*imOriginal, *imTransformBGR8, cv::COLOR_BayerBG2BGR); //to BGR
								} else if (pBufferFilled->GetPixelFormat() == "BayerGB8") {
									cv::cvtColor(*imOriginal, *imTransformBGR8, cv::COLOR_BayerGR2BGR); //to BGR
								}

								//release opencv images
								delete imOriginal;                     //release opencv image

								mat = *imTransformBGR8;


							} else if ((pBufferFilled->GetPixelFormat() == "Mono8") || (pBufferFilled->GetPixelFormat() == "BayerRG8") || (pBufferFilled->GetPixelFormat() == "BayerGB8")) {//openCV format (CV_8UC1)
								cv::Mat* tmp = new cv::Mat((int)pBufferFilled->GetHeight(), (int)pBufferFilled->GetWidth(), CV_8UC1, (char *)((bo_uint64)(pBufferFilled->GetMemPtr()) + pBufferFilled->GetImageOffset()));
								mat = *tmp;
							} else if (pBufferFilled->GetPixelFormat() == "Mono16") {
								cv::Mat* imOriginal = new cv::Mat((int)pBufferFilled->GetHeight(), (int)pBufferFilled->GetWidth(), CV_16UC1, (char *)((bo_uint64)(pBufferFilled->GetMemPtr()) + pBufferFilled->GetImageOffset()));
								mat = *imOriginal;
							} else if ((pBufferFilled->GetPixelFormat() == "Mono12") || (pBufferFilled->GetPixelFormat() == "BayerRG12") || (pBufferFilled->GetPixelFormat() == "BayerGB12")) {
								cv::Mat* imOriginal = new cv::Mat((int)pBufferFilled->GetHeight(), (int)pBufferFilled->GetWidth(), CV_16UC1, (char *)((bo_uint64)(pBufferFilled->GetMemPtr()) + pBufferFilled->GetImageOffset()));
								*imOriginal *= 16; //shift 4 bits
								mat = *imOriginal;
							} else if ((pBufferFilled->GetPixelFormat() == "Mono10") || (pBufferFilled->GetPixelFormat() == "BayerRG10") || (pBufferFilled->GetPixelFormat() == "BayerGB10")) { //openCV format (CV_16UC1)
								cv::Mat* imOriginal = new cv::Mat((int)pBufferFilled->GetHeight(), (int)pBufferFilled->GetWidth(), CV_16UC1, (char *)((bo_uint64)(pBufferFilled->GetMemPtr()) + pBufferFilled->GetImageOffset()));
								*imOriginal *= 64; //shift 6 bits
								mat = *imOriginal;
							} else if (pBufferFilled->GetPixelFormat() == "BGR16") {
								cv::Mat* imOriginal = new cv::Mat((int)pBufferFilled->GetHeight(), (int)pBufferFilled->GetWidth(), CV_16UC3, (char *)((bo_uint64)(pBufferFilled->GetMemPtr()) + pBufferFilled->GetImageOffset()));
								cv::Mat* imConvert = new cv::Mat((int)pBufferFilled->GetHeight(), (int)pBufferFilled->GetWidth(), CV_16UC3); //memory allocation
								imOriginal->convertTo(*imConvert, CV_16UC3, 1.0); //full copy with previous memory allocation AND scaling of 1.0
								mat = *imConvert;
							} else if (pBufferFilled->GetPixelFormat() == "BGR12") {
								cv::Mat* imOriginal = new cv::Mat((int)pBufferFilled->GetHeight(), (int)pBufferFilled->GetWidth(), CV_16UC3, (char *)((bo_uint64)(pBufferFilled->GetMemPtr()) + pBufferFilled->GetImageOffset()));
								cv::Mat* imConvert = new cv::Mat((int)pBufferFilled->GetHeight(), (int)pBufferFilled->GetWidth(), CV_16UC3); //memory allocation
								imOriginal->convertTo(*imConvert, CV_16UC3, 16.0); //full copy with previous memory allocation AND scaling of 16.0 to convert 12-Bit to 16-Bit 
								mat = *imConvert;
							} else if (pBufferFilled->GetPixelFormat() == "BGR10") {
								cv::Mat* imOriginal = new cv::Mat((int)pBufferFilled->GetHeight(), (int)pBufferFilled->GetWidth(), CV_16UC3, (char *)((bo_uint64)(pBufferFilled->GetMemPtr()) + pBufferFilled->GetImageOffset()));
								cv::Mat* imConvert = new cv::Mat((int)pBufferFilled->GetHeight(), (int)pBufferFilled->GetWidth(), CV_16UC3); //memory allocation
								imOriginal->convertTo(*imConvert, CV_16UC3, 64.0); //full copy with previous memory allocation AND scaling of 64.0 to convert 10-Bit to 16-Bit 
								mat = *imConvert;
							}
							// queue buffer again
							pBufferFilled->QueueBuffer();
						}
					} catch (BGAPI2::Exceptions::IException& ex) { return false; }
					catch (std::exception& e) { return false; }

					if (mat.empty())return false;
					pBufferFilled = NULL;
					return true;
				}

				/**
				* �I������
				* @return bool �I���ł�����
				*/
				bool close() {
					return true;
				}

				/**
				* �������J��
				* @return bool �J���ł�����
				*/
				bool release() {
					try {
						while (bufferList && bufferList->size() > 0) {
							pBuffer = bufferList->begin()->second;
							bufferList->RevokeBuffer(pBuffer);
							delete pBuffer;
						}

						pDataStream->Close();
					} catch (BGAPI2::Exceptions::IException& ex) {
						return false;
					}
					return true;
				}

			};
			beumer_data_stream stream;

		public:
			/**
			* �����ݒ�
			* @param[in] it �f�o�C�X���X�g�C�e���[�^
			* @return �����������ݒ�ł�����
			*/
			bool set(BGAPI2::DeviceList::iterator it) {
				try{
					it->second->Open();
					pDevice = it->second;
					sDeviceID = it->first;
				} catch (BGAPI2::Exceptions::ResourceInUseException& ex) {
					return false;
				} catch (BGAPI2::Exceptions::AccessDeniedException& ex) {
					return false;
				}

				if (sDeviceID == "") {
					return false;
				}

				try {
					//Stop Acquisition
					pDevice->GetRemoteNode("AcquisitionStop")->Execute();

					//SET TRIGGER MODE OFF (FreeRun)
					pDevice->GetRemoteNode("TriggerMode")->SetString("Off");

					const char* nodeName[] = { "BGR8Packed" ,"BGR8" ,"BayerRG8" ,"BayerGB8" ,"Mono16" ,"Mono12" ,"BayerRG12","BayerGB12","Mono10","BayerRG10","BayerGB10",
						"BGR16" ,"BGR12" ,"BGR10" };

					for (int i = 0; i < 14; i++) {
						if (!pDevice->GetRemoteNode("PixelFormat")->GetEnumNodeList()->GetNodePresent(nodeName[i]))continue;
						if (!pDevice->GetRemoteNode("PixelFormat")->GetEnumNodeList()->GetNode(nodeName[i])->IsReadable())continue;
						pDevice->GetRemoteNode("PixelFormat")->SetString(nodeName[i]);
						break;
					}

					//�I�����Ԋ֘A
					if (pDevice->GetRemoteNodeList()->GetNodePresent("ExposureTime")) {
						sExposureNodeName = "ExposureTime";
					} else if (pDevice->GetRemoteNodeList()->GetNodePresent("ExposureTimeAbs")) {
						sExposureNodeName = "ExposureTimeAbs";
					}
					pExposureTime = pDevice->GetRemoteNode(sExposureNodeName);

					fExposureTimeMin = pExposureTime->GetDoubleMin();
					fExposureTimeMax = pExposureTime->GetDoubleMax();

					//�Q�C���֘A
					if (pDevice->GetRemoteNodeList()->GetNodePresent("Gain")) {
						BGAPI2::Node* pGainSelector = pDevice->GetRemoteNodeList()->GetNode("GainSelector");
						if ((pGainSelector->GetEnumNodeList()->GetNodePresent("All")) &&
							(pGainSelector->GetEnumNodeList()->GetNode("All")->GetAvailable())) {
							pGainSelector->SetValue("All");
							pGain = pDevice->GetRemoteNodeList()->GetNode("Gain");

							fGainMin = pGain->GetDoubleMin();
							fGainMax = pGain->GetDoubleMax();
						}
					}
				} catch (BGAPI2::Exceptions::IException& ex) { 
					pDevice->Close();
					return false; 
				}

				if (!this->stream.set(pDevice)) {
					pDevice->Close();
					return false;
				}
				return true;
			}

			/**
			* �J�����J�n
			* @return bool �J�������J�n�ł�����
			*/
			bool startCamera() {
				if (capturing)return true;
				try {
					if (!stream.startStream())return false;
					pDevice->GetRemoteNode("AcquisitionStart")->Execute();
				} catch (BGAPI2::Exceptions::IException& ex) { return false; }

				capturing = true;
				return true;
			}

			/**
			* �J�����I��
			* �J�������I���ł�����
			*/
			bool stopCamera() {
				if (!capturing)return true;
				try {
					if (pDevice->GetRemoteNodeList()->GetNodePresent("AcquisitionAbort")) {
						pDevice->GetRemoteNode("AcquisitionAbort")->Execute();
					}

					pDevice->GetRemoteNode("AcquisitionStop")->Execute();

					if (!stream.stopStream())return false;
				} catch (BGAPI2::Exceptions::IException& ex) { return false; }
				capturing = false;
				return true;
			}

			/**
			* �J�����̃N���[�Y
			* @param bool �N���[�Y�ł�����
			*/
			bool close() {
				pDevice->Close();
				return true;
			}

			/**
			* �������J��
			* @return bool �J���ł�����
			*/
			bool relese() {
				bool canRelese = true;
				try {
					canRelese &= stream.release();
					pDevice->Close();
				} catch (BGAPI2::Exceptions::IException& ex) {
					return false;
				}

				return canRelese;
			}

			/**
			* �摜�ǂݍ���
			* @param[out] mat �摜�o��
			* @return bool �摜���ǂݍ��߂���
			*/
			bool read(cv::Mat& mat) {
				return stream.read(mat);
			}

			/**
			* �ŏ��I�����Ԃ̎擾
			* @return double �ŏ��I������
			*/
			inline double getExposureTimeMin() {
				return fExposureTimeMin;
			}

			/**
			* �ő�I�����Ԃ̎擾
			* @return double �ő�I������
			*/
			inline double getExposureTimeMax() {
				return fExposureTimeMax;
			}

			/**
			* �I�����Ԃ̎擾
			* @return double �I������
			*/
			inline double getExposureTime() {
				return pExposureTime->GetDouble();
			}

			/**
			* �I�����Ԃ̐ݒ�
			* @param[in] time �ŏ��I������
			*/
			inline void setExposureTime(double time) {
				double dTime = time;
				if (dTime < fExposureTimeMin) {
					dTime = fExposureTimeMin;
				} else if (dTime > fExposureTimeMax) {
					dTime = fExposureTimeMax;
				}

				pExposureTime->SetDouble(dTime);
			}

			/**
			* �ŏ��Q�C���̎擾
			* @return double �ŏ��Q�C��
			*/
			inline double getGainMin() {
				return fGainMin;
			}

			/**
			* �ő�Q�C���̎擾
			* @return double �ő�Q�C��
			*/
			inline double getGainMax() {
				return fGainMax;
			}

			/**
			* �Q�C���̎擾
			* @return double �Q�C��
			*/
			inline double getGainVal() {
				return pGain->GetDouble();
			}

			/**
			* �Q�C���̐ݒ�
			* @param[in] gain �Q�C��
			*/
			inline void setGain(double gain) {
				double dGain = gain;
				if (dGain < fGainMin) {
					dGain = fGainMin;
				} else if (dGain > fGainMax) {
					dGain = fGainMax;
				}
				
				pGain->SetDouble(dGain);
			}

			/**
			* ��ʃT�C�Y�̎擾
			* @return cv::Size ��ʃT�C�Y
			*/
			inline cv::Size getSize() {
				int width = pDevice->GetRemoteNode("Width")->GetInt();
				int height = pDevice->GetRemoteNode("Height")->GetInt();

				return cv::Size(width, height);
			}

			/**
			* ��ʃT�C�Y�̐ݒ�(�T�|�[�g����Ȃ��J��������)
			* @param[in] size ��ʃT�C�Y
			* @return bool �ݒ�ύX�ł�����
			*/
			bool setSize(cv::Size size) {
				if (!setWidth(size.width))return false;
				if (!setHeight(size.height))return false;
				return true;
			}

			/**
			* ��ʃT�C�Y(��)�̐ݒ�(�T�|�[�g����Ȃ��J��������)
			* @param[in] size ��ʃT�C�Y(��)
			* @return bool �ݒ�ύX�ł�����
			*/
			bool setWidth(int width) {
				if (!pDevice->GetRemoteNode("Width")->IsWriteable())return false;
				bo_int64 iImageWidthMin = pDevice->GetRemoteNode("Width")->GetIntMin();
				bo_int64 iImageWidthMax = pDevice->GetRemoteNode("Width")->GetIntMax();
				bo_int64 iImageWidthInc = pDevice->GetRemoteNode("Width")->GetIntInc();

				bo_int64 widthvalue = (width / iImageWidthInc) * iImageWidthInc;

				if (widthvalue < iImageWidthMin) { widthvalue = iImageWidthMin; }
				if (widthvalue > iImageWidthMax) { widthvalue = iImageWidthMax; }

				pDevice->GetRemoteNode("Width")->SetInt(widthvalue);
				return true;
			}

			/**
			* ��ʃT�C�Y(�c)�̐ݒ�(�T�|�[�g����Ȃ��J��������)
			* @param[in] size ��ʃT�C�Y(�c)
			* @return bool �ݒ�ύX�ł�����
			*/
			bool setHeight(int height) {
				if (!pDevice->GetRemoteNode("Height")->IsWriteable())return false;
				bo_int64 iImageHeightMin = pDevice->GetRemoteNode("Height")->GetIntMin();
				bo_int64 iImageHeightMax = pDevice->GetRemoteNode("Height")->GetIntMax();
				bo_int64 iImageHeightInc = pDevice->GetRemoteNode("Height")->GetIntInc();

				bo_int64 heightvalue = (height / iImageHeightInc) * iImageHeightInc;

				if (heightvalue < iImageHeightMin) { heightvalue = iImageHeightMin; }
				if (heightvalue > iImageHeightMax) { heightvalue = iImageHeightMax; }

				pDevice->GetRemoteNode("Height")->SetInt(heightvalue);
				return true;
			}

			/**
			* �J�����̃��f�����擾
			* @return BGAPI2::String �J�����̃��f����
			*/
			inline BGAPI2::String getModel() {
				return pDevice->GetModel();
			}

			/**
			* �J�����̃V���A���i���o�[�擾
			* @return BGAPI2::String �J�����̃V���A���i���o�[
			*/
			inline BGAPI2::String getSerialNumber() {
				return pDevice->GetSerialNumber();
			}

			/**
			* �J�����̃f�B�X�v���C�\�����擾
			* @return BGAPI2::String �J�����̃f�B�X�v���C�\�����擾
			*/
			inline BGAPI2::String getDisplayName() {
				return pDevice->GetDisplayName();
			}

			/**
			* Baumer�f�t�H���g�֐��̂��߂̃m�[�h�擾
			* @return BGAPI2::Node* �m�[�h
			*/
			inline BGAPI2::Node* getRemoteNode(BGAPI2::String str) {
				return pDevice->GetRemoteNode(str);
			}

			/**
			* Baumer�f�t�H���g�֐��̂��߂̃m�[�h�ꗗ�擾
			* @return BGAPI2::NodeMap* �m�[�h�ꗗ
			*/
			inline BGAPI2::NodeMap* getRemoteNodeList() {
				return pDevice->GetRemoteNodeList();
			}


		};
		/**
		* �J�����ꗗ
		*/
		std::vector<baumer_device> cameras;
	public:
		VideoCapture();
		~VideoCapture();
		/**
		* �J�����摜�擾�J�n
		* @return bool ����ɃJ�������J�n�ł�����
		*/
		bool start();

		/**
		* �J�����摜�擾�I��
		* @return bool ����ɃJ�������I���ł�����
		*/
		bool stop();

		/**
		* �J�������擾
		* @return int �ڑ����ꂽ�J�����̐�
		*/
		inline int size() {
			return cameras.size();
		}

		baumer_device &operator[](int n) {
			if (n < 0)return cameras[0];
			if (n >= cameras.size())return cameras[cameras.size() - 1];

			return cameras[n];
		}

		baumer_device const &operator[](int n) const{
			if (n < 0)return cameras[0];
			if (n >= cameras.size())return cameras[cameras.size() - 1];

			return cameras[n];
		}

		/**
		* �J�������ڑ����ꂽ��
		* @return bool �J�������ڑ����ꂽ��
		*/
		inline bool isOpened() {
			return this->cameras.size() > 0;
		}

	private:
		
		bool openSystem();
	};
}

#endif