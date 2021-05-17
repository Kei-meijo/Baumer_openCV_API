#include <opencv2/opencv.hpp>
#include "Baumer.h"

int main() {
	baumer::VideoCapture cap;
	//カメラスタート(必須)
	if (!cap.start()) {
		//cap.start()がfalseを返す時はカメラが接続されていないなどのエラー
		std::cout << "error" << std::endl;
		//エラーなので終了
		exit(EXIT_FAILURE);
	}


	//接続されているカメラの数を取得
	int camera_number = cap.size();
	for (int i = 0; i < camera_number; i++) {
		//ディスプレイ表示名,モデル名,シリアルナンバーを取得可能
		std::cout << cap[i].getDisplayName() << " " << cap[i].getModel() << "[" << cap[i].getSerialNumber() << "]" << std::endl;
		//カメラの画面サイズ
		std::cout << cap[i].getSize() << std::endl;
	}


	//メインループ
	while (true) {

		bool frameNotEmpty = true;
		//カメラごとに処理
		for (int i = 0; i < camera_number; i++) {
			cv::Mat frame;
			//カメラから画像取得
			if (!cap[i].read(frame)) {
				//画像取得できなかったため終了
				frameNotEmpty = false;
				break;
			}

			//画像を縮小して表示
			cv::resize(frame, frame, cv::Size(), 0.5, 0.5);
			cv::imshow("frame" + std::to_string(i), frame);
		}

		//1つ以上のカメラから画像取得できなかったため終了
		if (!frameNotEmpty)break;

		//何かキー入力があれば終了
		int key = cv::waitKey(10);
		if (key != -1)break;
	}

	std::cout << "finish" << std::endl;
	//カメラ終了(必須)
	cap.stop();

	return EXIT_SUCCESS;
}