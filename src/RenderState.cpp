#include "RenderState.h"
#include "FileConfig.h"
#include "Utilities.h"

void RenderState::LoadConfig(string configFileName){
    string path = configFileName;
    if(path.empty()){
        path = resourceFolder + "resource/imgui_gl_show.txt";
    }
    Config fileConfig;
    fileConfig.LoadConfig(path);

    enableWhiteBalance = fileConfig.Read("enableWhiteBalance", 0);
    enableEqualizeHist = fileConfig.Read("enableEqualizeHist", 0);
    enableLightEqualization = fileConfig.Read("enableLightEqualization", 0);
    obj_scale = fileConfig.Read("obj_scale", 0.01f);
    sur_model_scale_x = fileConfig.Read("sur_model_scale_x", obj_scale);
    sur_model_scale_y = fileConfig.Read("sur_model_scale_y", obj_scale);
    sur_model_scale_z = fileConfig.Read("sur_model_scale_z", obj_scale);
    sur_model_wall_scale_x = fileConfig.Read("sur_model_wall_scale_x", 1.0);
    sur_model_wall_scale_y = fileConfig.Read("sur_model_wall_scale_y", 1.0);
    sur_model_wall_scale_z = fileConfig.Read("sur_model_wall_scale_z", 1.0);
    sur_model_translate_x = fileConfig.Read("sur_model_translate_x", 0.0f);
    sur_model_translate_y = fileConfig.Read("sur_model_translate_y", -4.0f);
    sur_model_translate_z = fileConfig.Read("sur_model_translate_z", 0.0f);
    sur_model_rot_x = fileConfig.Read("sur_model_rot_x", 0.0f);
    sur_model_rot_y = fileConfig.Read("sur_model_rot_y", 0.0f);
    sur_model_rot_z = fileConfig.Read("sur_model_rot_z", 0.0f);
    car_model_scale = fileConfig.Read("car_model_scale", 1.4f);
    car_model_Alpha = fileConfig.Read("car_model_Alpha", 1.0f);
   
    //vector<float> scale_vec = fileConfig.ReadArray<float>("car_model_scale_vector", ",#_-+&%$ #@");
    car_model_scale_vector = fileConfig.Readvec3f("car_model_scale_vector");
    car_model_translate_x = fileConfig.Read("car_model_translate_x", 0.0f);
    car_model_translate_y = fileConfig.Read("car_model_translate_y", -4.0f);
    car_model_translate_z = fileConfig.Read("car_model_translate_z", 0.0f);
    car_model_rot_x = fileConfig.Read("car_model_rot_x", 0.0f);
    car_model_rot_y = fileConfig.Read("car_model_rot_y", 0.0f);
    car_model_rot_z = fileConfig.Read("car_model_rot_z", 0.0f);
    fovy = fileConfig.Read("fovy", PI / 2.0f);
    freqxz = fileConfig.Read("freqxz", 0.0f);
    amp_xz = fileConfig.Read("amp_xz", 1.0f);
    var_y = fileConfig.Read("var_y", 0.0f);
    lookat_x = fileConfig.Read("lookat_x", 0.0f);
    lookat_y = fileConfig.Read("lookat_y", 0.0f);
    lookat_z = fileConfig.Read("lookat_z", 0.0f);
    eye_x = fileConfig.Read("eye_x", 0.0f);
    eye_y = fileConfig.Read("eye_y", 0.0f);
    eye_z = fileConfig.Read("eye_z", 0.0f);
    drawCarModel = fileConfig.Read("drawCarModel", 1);
    phase = fileConfig.Read("view_angle_phase", 0.0f);// -PI / 2;
    sur_model_modify = fileConfig.Read("sur_model_modify", 1);
    sur_model_color_modify = fileConfig.Read("sur_model_color_modify", 0);
    sur_model_contrast   = fileConfig.Read("sur_model_contrast", 1.5);
    sur_model_ground_radius = fileConfig.Read("sur_model_ground_radius", 400.0);
    sur_model_wall_growth_rate_inv = fileConfig.Read("sur_model_wall_growth_rate_inv", 400.0);
    enableTransparent3D  = fileConfig.Read("enableTransparent3D", 1);
    transparent3DMethod = fileConfig.Read("transparent3DMethod", 2);
    car_frame_half_width = fileConfig.Read("car_frame_half_width", 13.0);
    car_frame_half_height = fileConfig.Read("car_frame_half_height", 28.0);
    cw_extra = fileConfig.Read("cw_extra", 0.0);
    ch_extra = fileConfig.Read("ch_extra", 0.0);
    ProcessFrame3DMethod = fileConfig.Read("ProcessFrame3DMethod", 0);
    di_3d = fileConfig.Read("di_3d", 40.0);
    da_3d = fileConfig.Read("da_3d", 50.0);
    half_checkboard_width = fileConfig.Read("half_checkboard_width", 28.0);
    half_checkboard_height = fileConfig.Read("half_checkboard_height", 42.0);
    radiusOfRoundCorner = fileConfig.Read("radiusOfRoundCorner", 1.0);
    carFrameBlurGap = fileConfig.Read("carFrameBlurGap", 1.0);
    HCameraScale_F = fileConfig.Read("HCameraScale_F", 0.0);
    HCameraScale_B = fileConfig.Read("HCameraScale_B", 0.0);
    HCameraScale_L = fileConfig.Read("HCameraScale_L", 0.0);
    HCameraScale_R = fileConfig.Read("HCameraScale_R", 0.0);

    scaleXCamera_F  = fileConfig.Read("scaleXCamera_F", 1.0);
    scaleYCamera_F = fileConfig.Read("scaleYCamera_F", 1.0);
    scaleZCamera_F = fileConfig.Read("scaleZCamera_F", 1.0);
    scaleXCamera_B = fileConfig.Read("scaleXCamera_B", 1.0);
    scaleYCamera_B = fileConfig.Read("scaleYCamera_B", 1.0);
    scaleZCamera_B = fileConfig.Read("scaleZCamera_B", 1.0);
    scaleXCamera_L = fileConfig.Read("scaleXCamera_L", 1.0);
    scaleYCamera_L = fileConfig.Read("scaleYCamera_L", 1.0);
    scaleZCamera_L = fileConfig.Read("scaleZCamera_L", 1.0);
    scaleXCamera_R = fileConfig.Read("scaleXCamera_R", 1.0);
    scaleYCamera_R = fileConfig.Read("scaleYCamera_R", 1.0);
    scaleZCamera_R = fileConfig.Read("scaleZCamera_R", 1.0);

    uyvyCorrection = fileConfig.Read("uyvyCorrection", 0);
    usingYCbCrConverter = fileConfig.Read("usingYCbCrConverter", 0);

    hideGround = fileConfig.Read("hideGround", 0);
    hideWall = fileConfig.Read("hideWall", 0);

    transParentEnable = fileConfig.Read("transParentEnable", 1);
    ratioWOverH = fileConfig.Read("ratioWOverHOfGLCamera", ratioWOverH);
    pitchAngle = fileConfig.Read("pitchAngle", pitchAngle);
    // birdeyeview renderstate
    radiusOfRoundCorner2D = fileConfig.Read("radiusOfRoundCorner2D", 20);
	carFrameBlurGap2D = fileConfig.Read("carFrameBlurGap2D", 10);
    cw_extra_2D = fileConfig.Read("cw_extra_2D", 0.0f);
	ch_extra_2D = fileConfig.Read("ch_extra_2D", 0.0f);


    //
    reloadAVMConfigs = fileConfig.Read("reloadAVMConfigs", 0);
    dumpAvmConfigs = fileConfig.Read("dumpAvmConfigs", 0);
    usingGPU = fileConfig.Read("usingGPU", usingGPU);

    default_1D_shader_local_size_y = fileConfig.Read("default_1D_shader_local_size_y", 64);
    default_2D_shader_local_size_x = fileConfig.Read("default_2D_shader_local_size_x", 16);
    default_2D_shader_local_size_y = fileConfig.Read("default_2D_shader_local_size_y", 16);

    drawGUI = fileConfig.Read("drawGUI", drawGUI);
    drawBG = fileConfig.Read("drawBG", drawBG);
    drawARSlot = fileConfig.Read("drawARSlot", drawARSlot);
    drawARGUI = fileConfig.Read("drawARGUI", drawARGUI);
    printGear = fileConfig.Read("printGear", printGear);

    std::vector<std::vector<float>> mat = fileConfig.ReadMatrix<float>("x_matrix", "|", ",");
    //matrix4f mat = fileConfig.Readmatrix4f("x_matrix", "|", ",");
    //car Info
    path = resourceFolder + "resource/carInfo.txt";
    fileConfig.LoadConfig(path);
    carInfo.cw = fileConfig.Read("carinfo.cw", carInfo.cw);
    carInfo.ch = fileConfig.Read("carinfo.ch", carInfo.ch);
    carInfo.cs = fileConfig.Read("carinfo.cs", carInfo.cs);
    carInfo.gap = fileConfig.Read("carinfo.gap", carInfo.gap);
    carInfo.rear_x = fileConfig.Read("carinfo.rear_x", carInfo.rear_x);
    carInfo.rear_y = fileConfig.Read("carinfo.rear_y", carInfo.rear_y);


    


}
