#ifndef __RENDER_STATE_H__
#define __RENDER_STATE_H__
#include <string>
#include <vector>
#include "Vector.h"
using std::vector;
using MB::vec3f;

using std::string;
struct CarInfo {
    int cw = 240;     // Car Frame Width 
    int ch = 650;     // Car Frame Height 
    int rear_x = 608;
    int rear_y = 959;
    int gap = 20;
    int fusion_gap = 80;
    float cs = 1.0;    // Car size scale off 
};
class RenderState
{
    private:
        string resourceFolder;

    
public:
    void setResourceFolder(string resFolder){
        resourceFolder = resFolder;
    }
    void LoadConfig(string configFileName);

    float amp_xz = 40.0001;
    float HCameraScale_B = 0;
    float HCameraScale_F = 0;
    float HCameraScale_L = 0;
    float HCameraScale_R = 0;
    int ProcessFrame3DMethod = 2;
    int texture3d_surround_sample_step = 10;
    int bw_3d = 3000;
    int bh_3d = 3000;

    int camera_size_x = 1280;
    int camera_size_y = 960;

    float radiusOfRoundCorner2D = 15;
    float carFrameBlurGap2D = 7;
    float carFrameBlurGap = 1.0;
    float car_model_diffusePower = 11.0;
    int car_model_lightNum = 3;
    float car_model_rot_x = 0.;
    float car_model_rot_y = 0;
    float car_model_rot_z = 0;
    float car_model_scale = 1.4;
    vec3f car_model_scale_vector;
    float car_model_specularPower = 150.0;
    float car_model_Alpha = 0.6;
    float car_model_translate_x = 0;
    float car_model_translate_y = 0;
    float car_model_translate_z = 0;
    float car_frame_half_width = 11.5;
    float car_frame_half_height = 28;
    float cw_extra = 0;
    float ch_extra = 0;
    float cw_extra_2D = 0;
    float ch_extra_2D = 0;
    float da_3d = 50;
    float di_3d = 30;
    int drawCarModel = 1;
    int enableEqualizeHist = 0;
    int enableLightEqualization = 0;
    int enableWhiteBalance = 0;
    int enableTransparent3D = 1;
    int transparent3DMethod = 2;
    float eye_x = 0;
    float eye_y = 40;
    float eye_z = 0;
    float fovy = 1.4;
    float freqxz = 0.00002;
    float gamma_factor = 1.0;
    int hideGround = 0;
    int hideWall = 0;
    int heightAdjustThreshold = 100;
    float half_checkboard_height = 38;
    float half_checkboard_width = 24.5;
    float lightEquaReverseChannel = 0;
    float load_sur_3d_world_from_bin_File = 1;
    float load_sur_3d_world_resampled = 1;
    float lookat_x = 0;
    float lookat_y = 0.0;
    float lookat_z = 0;
    float obj_scale = 0.8;
    float phase = 0.0;
    float ratioWOverH = 1.0 ;
    float pitchAngle = 0;

    float radiusOfRoundCorner = 2.0;
    float reInitSurroundCfg = 0;
    float scaleXCamera_B = 1.0;
    float scaleXCamera_F = 1.0;
    float scaleXCamera_L = 1.0;
    float scaleXCamera_R = 1.0;
    float scaleYCamera_B = 1.0;
    float scaleYCamera_F = 1;
    float scaleYCamera_L = 1.0;
    float scaleYCamera_R = 1.0;
    float scaleZCamera_B = 1.0;
    float scaleZCamera_F = 1.0;
    float scaleZCamera_L = 1.0;
    float scaleZCamera_R = 1.0;
    int sur_model_color_modify = 0;
    float sur_model_contrast = 1.6;
    float sur_model_ground_radius = 300.0;
    int sur_model_modify = 0;
    float sur_model_rot_x = 0.;
    float sur_model_rot_y = 0;
    float sur_model_rot_z = 0;
    float sur_model_scale_x = 2.3;
    float sur_model_scale_y = 2.3;
    float sur_model_scale_z = 2.3;
    float sur_model_translate_x = 0;
    float sur_model_translate_y = 0;
    float sur_model_translate_z = 0;
    float sur_model_wall_growth_rate_inv = 730.0;
    float sur_model_wall_scale_x = 1;
    float sur_model_wall_scale_y = 1;
    float sur_model_wall_scale_z = 500;
    int transParentEnable = 0;

    float uv_3d_mirror_u = 1;
    float uv_3d_mirror_v = 0;
    int uyvyCorrection = 1;
    int usingYCbCrConverter = 0;
    float var_y;

    //debug flags
    int reloadAVMConfigs = 0;
    int dumpAvmConfigs = 0;

    CarInfo carInfo;

    bool usingGPU = 1;

                //default shader configuration
    int default_2D_shader_local_size_x = 16;
    int default_2D_shader_local_size_y = 16;

    int default_1D_shader_local_size_y = 64;

    int drawGUI = 0;
    int drawBG = 0;
    
    //apa test
    int drawARSlot = 0;
    int printGear = 0;
    int drawARGUI = 1;
};

#endif
