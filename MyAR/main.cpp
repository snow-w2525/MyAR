#include "ArResource.hpp"
#include <opencv2/aruco.hpp>
#include <opencv2/objdetect/aruco_detector.hpp>
#include <algorithm>
#include <sstream>

#define RESIZE_WINDOW

/// <summary>
/// �e��ݒ���܂Ƃ߂��\����
/// </summary>
struct CameraConfig
{
    std::string BACKGROUND_TYPE;
    std::string CAMERA_TITLE;
    std::string BACKGROUND_IMAGE_NAME;
    int CAMERA_NO = 0;
    int BACKGROUND_WIDTH = 960;
    int BACKGROUND_HEIGHT = 720;
    std::vector<ArResource> resouces;
    cv::aruco::DetectorParameters detectorParams = cv::aruco::DetectorParameters();
    cv::aruco::Dictionary dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_50);
    cv::aruco::ArucoDetector detector = cv::aruco::ArucoDetector(dictionary, detectorParams);
    std::vector<int> markerIds;
    std::vector<std::vector<cv::Point2f>> markerCorners, rejectedCandidates;
};

/// <summary>
/// �A�v���̏���������
/// </summary>
static bool Initialize(CameraConfig& config) 
{
    const std::string CameraConfigFileName = "camera-config.json";
    std::ifstream ifs(CameraConfigFileName, std::ios::in);

    if (ifs.fail()) 
    {
        std::cout << CameraConfigFileName + "open fail" << std::endl; 
        return false;
    }
    nlohmann::json json_str;
    ifs >> json_str;
    config.BACKGROUND_TYPE = json_str["BACKGROUND_TYPE"];
    config.CAMERA_TITLE = json_str["CAMERA_TITLE"];
    config.BACKGROUND_IMAGE_NAME = json_str["BACKGROUND_IMAGE_NAME"];
    config.CAMERA_NO = json_str["CAMERA_NO"];
    config.BACKGROUND_WIDTH = json_str["BACKGROUND_WIDTH"];
    config.BACKGROUND_HEIGHT = json_str["BACKGROUND_HEIGHT"];

    int a = json_str["AR_RESOURCE_LIST"].size();
    for (int i = 0; i < json_str["AR_RESOURCE_LIST"].size(); i++)
    {
        ArResource ar_resouce;
        ar_resouce.image_type = json_str["AR_RESOURCE_LIST"][i]["TYPE"];
        ar_resouce.image_name = json_str["AR_RESOURCE_LIST"][i]["NAME"];
        ar_resouce.position_x = json_str["AR_RESOURCE_LIST"][i]["POSITON_X"];
        ar_resouce.position_y = json_str["AR_RESOURCE_LIST"][i]["POSITON_Y"];
        ar_resouce.width = json_str["AR_RESOURCE_LIST"][i]["WIDTH"];
        ar_resouce.height = json_str["AR_RESOURCE_LIST"][i]["HEIGHT"];
        ar_resouce.min_r = json_str["AR_RESOURCE_LIST"][i]["MIN_R"];
        ar_resouce.min_g = json_str["AR_RESOURCE_LIST"][i]["MIN_G"];
        ar_resouce.min_b = json_str["AR_RESOURCE_LIST"][i]["MIN_B"];
        ar_resouce.max_r = json_str["AR_RESOURCE_LIST"][i]["MAX_R"];
        ar_resouce.max_g = json_str["AR_RESOURCE_LIST"][i]["MAX_G"];
        ar_resouce.max_b = json_str["AR_RESOURCE_LIST"][i]["MAX_B"];
        ar_resouce.morph = json_str["AR_RESOURCE_LIST"][i]["MORPH"];
        ar_resouce.kernel_size = json_str["AR_RESOURCE_LIST"][i]["KERNEL_SIZE"];
        ar_resouce.top_left[0] = json_str["AR_RESOURCE_LIST"][i]["TOP_LEFT"][0];
        ar_resouce.top_left[1] = json_str["AR_RESOURCE_LIST"][i]["TOP_LEFT"][1];
        ar_resouce.top_right[0] = json_str["AR_RESOURCE_LIST"][i]["TOP_RIGHT"][0];
        ar_resouce.top_right[1] = json_str["AR_RESOURCE_LIST"][i]["TOP_RIGHT"][1];
        ar_resouce.bottom_left[0] = json_str["AR_RESOURCE_LIST"][i]["BOTTOM_LEFT"][0];
        ar_resouce.bottom_left[1] = json_str["AR_RESOURCE_LIST"][i]["BOTTOM_LEFT"][1];
        ar_resouce.bottom_right[0] = json_str["AR_RESOURCE_LIST"][i]["BOTTOM_RIGHT"][0];
        ar_resouce.bottom_right[1] = json_str["AR_RESOURCE_LIST"][i]["BOTTOM_RIGHT"][1];
        ar_resouce.combine = json_str["AR_RESOURCE_LIST"][i]["COMBINE"];
        ar_resouce.aruco = json_str["AR_RESOURCE_LIST"][i]["ARUCO"];
        ar_resouce.aruco_vertical = json_str["AR_RESOURCE_LIST"][i]["ARUCO_VERTICAL"];
        ar_resouce.aruco_offset = json_str["AR_RESOURCE_LIST"][i]["ARUCO_OFFSET"];
        ar_resouce.aruco_shift_x = json_str["AR_RESOURCE_LIST"][i]["ARUCO_SHIFT_X"];
        ar_resouce.aruco_shift_y = json_str["AR_RESOURCE_LIST"][i]["ARUCO_SHIFT_Y"];
        ar_resouce.aruco_shift_z = json_str["AR_RESOURCE_LIST"][i]["ARUCO_SHIFT_Z"];
        config.resouces.push_back(ar_resouce);
    }
    return true;

}

/// <summary>
/// AR���\�[�X�̎擾
/// </summary>
/// <remarks>
/// window�̏ꍇ�͉�ʂ��L���v�`������
/// image�̏ꍇ�͓��ɉ������Ȃ�
/// </remarks>
static bool GetResouces(CameraConfig& config)
{
    for (int i = 0; i < config.resouces.size(); i++)
    {
        if (!config.resouces[i].GetResource()) return false;
    }
    return true;
}

/// <summary>
/// �e�N�X�`���̎擾
/// </summary>
static bool GetTextures(CameraConfig& config)
{
    for (int i = 0; i < config.resouces.size(); i++)
    {
        if (!config.resouces[i].GetTexture()) return false;
    }
    return true;
}

static bool CombineImages(CameraConfig& config, cv::Mat& img_base)
{

    for (int i = 0; i < config.resouces.size(); i++)
    {
        if (!config.resouces[i].is_visble) continue;
        if(config.resouces[i].combine == "chromakey") config.resouces[i].Chromakey(img_base);
        else config.resouces[i].PersepectiveTransform(img_base);
    }
    return true;
}

static bool DetectArucoMarkers(CameraConfig& config, cv::Mat& img_base)
{
    if (config.BACKGROUND_TYPE != "camera") return true;

    std::vector<int> markerIds;
    std::vector<std::vector<cv::Point2f>> markerCorners, rejectedCandidates;

    // �}�[�J�̌��o
    // �}�[�J�[�̌��o���W�l�͍��ォ�玞�v���ɋl�߂���
    config.detector.detectMarkers(img_base, markerCorners, markerIds, rejectedCandidates);

#ifdef _DEBUG
    cv::aruco::drawDetectedMarkers(img_base, markerCorners, markerIds);
#endif

    for (int i = 0; i < config.resouces.size(); i++)
    {
        if (!config.resouces[i].aruco) continue;

        // std::find���g�p���ėv�f������
        auto it = std::find(markerIds.begin(), markerIds.end(), config.resouces[i].marker_id);
        if (it == markerIds.end()) continue;
        int index = std::distance(markerIds.begin(), it);

        if (config.resouces[i].aruco_vertical)
        {
            // �ŏ���bottom��������W�l�����߂邱��
            // XYZ�̌v�Z�͉E��n�Ōv�Z���邱��
            // X����aruco�}�[�J�[��0��1�����AZ����aruco�}�[�J�[��1��2�̕����AY���̓J������Y���Ƌt�̕���
            // ���L�v�Z�̓x�N�g�����g���čl����Ɖ�͂��₷��
            // ���̒i�K�ł͐��K�������l�ŎZ�o���邱��
            cv::Point2f btm_left = (markerCorners[index][0] + markerCorners[index][3] ) / 2;
            cv::Point2f btm_right = (markerCorners[index][1] + markerCorners[index][2]) / 2;
            cv::Point2f btm_back = (markerCorners[index][0] + markerCorners[index][1] ) / 2;
            cv::Point2f btm_front = (markerCorners[index][2] + markerCorners[index][3]) / 2;

            cv::Point2f btm_LtoR = btm_right - btm_left;
            cv::Point2f btm_BtoF = btm_front - btm_back;
            cv::Point2f btm_LRoffset = btm_LtoR * config.resouces[i].aruco_offset / 2;
            cv::Point2f btm_shiftX = btm_LtoR * config.resouces[i].aruco_shift_x;
            cv::Point2f btm_shiftZ = btm_BtoF * config.resouces[i].aruco_shift_z;
            //offset�͊g��k���Ȃ̂ŁA������LRoffset���}�C�i�X�ŉ��Z
            cv::Point2f btm_left_new = btm_left - btm_LRoffset + btm_shiftZ + btm_shiftX;
            cv::Point2f btm_right_new = btm_right + btm_LRoffset + btm_shiftZ + btm_shiftX;

            config.resouces[i].bottom_right[0] = btm_right_new.x / img_base.cols;
            config.resouces[i].bottom_right[1] = btm_right_new.y / img_base.rows - config.resouces[i].aruco_shift_y;
            config.resouces[i].bottom_left[0] = btm_left_new.x / img_base.cols;
            config.resouces[i].bottom_left[1] = btm_left_new.y / img_base.rows - config.resouces[i].aruco_shift_y;
            
            float act_width =
                std::sqrt(
                    std::pow(config.resouces[i].bottom_right[0] - config.resouces[i].bottom_left[0], 2) +
                    std::pow(config.resouces[i].bottom_right[1] - config.resouces[i].bottom_left[1], 2)
                );

            config.resouces[i].top_left[0] = config.resouces[i].bottom_left[0];
            config.resouces[i].top_left[1] = config.resouces[i].bottom_left[1] 
                - act_width / config.resouces[i].width * config.resouces[i].height;
            config.resouces[i].top_right[0] = config.resouces[i].bottom_right[0];
            config.resouces[i].top_right[1] = config.resouces[i].bottom_right[1] 
                - act_width / config.resouces[i].width * config.resouces[i].height;
        }
        else
        {
            //�ŏ���bottom��������W�l�����߂邱��
            config.resouces[i].bottom_right[0] = markerCorners[index][2].x / img_base.cols;
            config.resouces[i].bottom_right[1] = markerCorners[index][2].y / img_base.rows;
            config.resouces[i].bottom_left[0] = markerCorners[index][3].x / img_base.cols;
            config.resouces[i].bottom_left[1] = markerCorners[index][3].y / img_base.rows;

            config.resouces[i].top_left[0] = markerCorners[index][0].x / img_base.cols;
            config.resouces[i].top_left[1] = markerCorners[index][0].y / img_base.rows;
            config.resouces[i].top_right[0] = markerCorners[index][1].x / img_base.cols;
            config.resouces[i].top_right[1] = markerCorners[index][1].y / img_base.rows;
        }
    }

    return true;
}


static bool TryParseInt(const char* str, int& result) {
    char* endptr;
    result = std::strtol(str, &endptr, 10);
    // �ϊ����������A���ׂĂ̕������ϊ����ꂽ�����m�F
    return *endptr == '\0';
}



int main(int argc, char** argv)
{
    std::cout << "MyAR | ver: 20241102" << std::endl;

    try 
    {
        CameraConfig config;
        
        if (!Initialize(config)) return -1;

        // OpenCV�֘A�̕ϐ�
        cv::Mat img_base_pre;
        cv::Mat img_base;
        cv::Mat img_refresh;
        cv::VideoCapture capture;
        int camera_width = config.BACKGROUND_WIDTH;
        int camera_height = config.BACKGROUND_HEIGHT;

        // �w�i�摜���J��������擾����
        if (config.BACKGROUND_TYPE == "camera")
        {
            capture = cv::VideoCapture(config.CAMERA_NO);

            if (!capture.isOpened()) 
            { 
                std::cout << "camera open error" << std::endl; 
                std::cin.get(); throw;
            }
        }
        // �w�i�摜���摜����擾����
        else
        {
            img_base_pre = cv::imread(config.BACKGROUND_IMAGE_NAME);
            cv::resize(
                img_base_pre,
                img_base,
                cv::Size(camera_width, camera_height)
            );
            img_refresh = img_base.clone();
        }

        // AR���\�[�X���擾
        if (!GetResouces(config)) return -1;

        while (true)
        {
            // �w�i���J�����Ȃ�΁A�J��������摜�����炤�B
            if (config.BACKGROUND_TYPE == "camera")
            {
                capture >> img_base_pre;
                cv::resize(
                    img_base_pre,
                    img_base,
                    cv::Size(camera_width, camera_height)
                );
            }
            // �w�i���摜�Ȃ�΁A�w�i����x���t���b�V�����A�O�̕`��������B
            else
            {
                img_base = img_refresh.clone();
            }
            
            //Aruco�}�[�J�[�̌��o
            DetectArucoMarkers(config, img_base);

            // window���L���v�`��
            GetTextures(config);

            // �摜�̍���
            CombineImages(config, img_base);

            cv::imshow(config.CAMERA_TITLE, img_base);
            int input_number = 0;
            char ch = cv::waitKey(1);
            if (TryParseInt(&ch, input_number))
            {
                config.resouces[input_number].is_visble 
                    = !config.resouces[input_number].is_visble;
            }
            
            if (ch == 'q') { cv::destroyAllWindows(); break; }
            
        }
    
    }
    catch (...)
    {
        cv::destroyAllWindows();
        return -1;
    }
	
	return 0;
}