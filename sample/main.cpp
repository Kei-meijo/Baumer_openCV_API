#include <opencv2/opencv.hpp>
#include "Baumer.h"

int main() {
	baumer::VideoCapture cap;
	//�J�����X�^�[�g(�K�{)
	if (!cap.start()) {
		//cap.start()��false��Ԃ����̓J�������ڑ�����Ă��Ȃ��Ȃǂ̃G���[
		std::cout << "error" << std::endl;
		//�G���[�Ȃ̂ŏI��
		exit(EXIT_FAILURE);
	}


	//�ڑ�����Ă���J�����̐����擾
	int camera_number = cap.size();
	for (int i = 0; i < camera_number; i++) {
		//�f�B�X�v���C�\����,���f����,�V���A���i���o�[���擾�\
		std::cout << cap[i].getDisplayName() << " " << cap[i].getModel() << "[" << cap[i].getSerialNumber() << "]" << std::endl;
		//�J�����̉�ʃT�C�Y
		std::cout << cap[i].getSize() << std::endl;
	}


	//���C�����[�v
	while (true) {

		bool frameNotEmpty = true;
		//�J�������Ƃɏ���
		for (int i = 0; i < camera_number; i++) {
			cv::Mat frame;
			//�J��������摜�擾
			if (!cap[i].read(frame)) {
				//�摜�擾�ł��Ȃ��������ߏI��
				frameNotEmpty = false;
				break;
			}

			//�摜���k�����ĕ\��
			cv::resize(frame, frame, cv::Size(), 0.5, 0.5);
			cv::imshow("frame" + std::to_string(i), frame);
		}

		//1�ȏ�̃J��������摜�擾�ł��Ȃ��������ߏI��
		if (!frameNotEmpty)break;

		//�����L�[���͂�����ΏI��
		int key = cv::waitKey(10);
		if (key != -1)break;
	}

	std::cout << "finish" << std::endl;
	//�J�����I��(�K�{)
	cap.stop();

	return EXIT_SUCCESS;
}