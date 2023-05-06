#include <iostream>
#include <ctime>
#include <random>
#include <Eigen/Geometry>
#include <opencv2/highgui/highgui.hpp>
#include <iomanip>
#include "optimizer.h"
#include "transform_util.h"
#include "texture_extractor.h"
#include<opencv2/opencv.hpp>
#include<opencv2/features2d/features2d.hpp>
#include<vector>
#include<chrono>
#include<Eigen/Core>
#include<Eigen/Dense>
#include<opencv2/core/eigen.hpp>
#include<thread>
#include<time.h> 
#include<math.h>
#include<fstream>

using namespace cv;
using namespace std;

double during_bev;
double during_compute_error;
double during_wrap;


void CameraOptimization(Optimizer &opt,string cameraType){
    
    //cameraType:left--1;behind--2;right--3
    cout<<"**************************************1st****************************************"<<endl;
    int thread_num=7;
    vector<thread>threads(thread_num);
    auto start_calib = chrono::steady_clock::now();

    if(cameraType=="right"){
        int iter_nums=50000;
        threads[0]=thread(&Optimizer::Calibrate_right,&opt,iter_nums,-3,3,-3,3,-3,3,
                                        -0.1,0.1,-0.1,0.1,-0.1,0.1);  
        threads[1]=thread(&Optimizer::Calibrate_right,&opt,iter_nums,-3,0,-3,3,-3,3,
                                        -0.1,0.1,-0.1,0.1,-0.1,0.1);
        threads[2]=thread(&Optimizer::Calibrate_right,&opt,iter_nums,0,3,-3,3,-3,3,
                                        -0.1,0.1,-0.1,0.1,-0.1,0.1);
        threads[3]=thread(&Optimizer::Calibrate_right,&opt,iter_nums,-3,3,-3,0,-3,3,
                                        -0.1,0.1,-0.1,0.1,-0.1,0.1);
        threads[4]=thread(&Optimizer::Calibrate_right,&opt,iter_nums,-3,3,0,3,-3,3,
                                        -0.1,0.1,-0.1,0.1,-0.1,0.1);
        threads[5]=thread(&Optimizer::Calibrate_right,&opt,iter_nums,-3,3,-3,3,-3,0,
                                        -0.1,0.1,-0.1,0.1,-0.1,0.1);
        threads[6]=thread(&Optimizer::Calibrate_right,&opt,iter_nums,-3,3,-3,3,0,3,
                                        -0.1,0.1,-0.1,0.1,-0.1,0.1);
    }
    else if(cameraType=="left"){
        int iter_nums=50000;
        threads[0]=thread(&Optimizer::Calibrate_left,&opt,iter_nums,-3,3,-3,3,-3,3,
                                        -0.1,0.1,-0.1,0.1,-0.1,0.1);  
        threads[1]=thread(&Optimizer::Calibrate_left,&opt,iter_nums,-3,0,-3,3,-3,3,
                                        -0.1,0.1,-0.1,0.1,-0.1,0.1);
        threads[2]=thread(&Optimizer::Calibrate_left,&opt,iter_nums,0,3,-3,3,-3,3,
                                        -0.1,0.1,-0.1,0.1,-0.1,0.1);
        threads[3]=thread(&Optimizer::Calibrate_left,&opt,iter_nums,-3,3,-3,0,-3,3,
                                        -0.1,0.1,-0.1,0.1,-0.1,0.1);
        threads[4]=thread(&Optimizer::Calibrate_left,&opt,iter_nums,-3,3,0,3,-3,3,
                                        -0.1,0.1,-0.1,0.1,-0.1,0.1);
        threads[5]=thread(&Optimizer::Calibrate_left,&opt,iter_nums,-3,3,-3,3,-3,0,
                                        -0.1,0.1,-0.1,0.1,-0.1,0.1);
        threads[6]=thread(&Optimizer::Calibrate_left,&opt,iter_nums,-3,3,-3,3,0,3,
                                        -0.1,0.1,-0.1,0.1,-0.1,0.1);
    }
    else if(cameraType=="behind"){
        int iter_nums=50000;
        threads[0]=thread(&Optimizer::Calibrate_behind,&opt,iter_nums,-3,3,-3,3,-3,3,
                                        -0.1,0.1,-0.1,0.1,-0.1,0.1);  
        threads[1]=thread(&Optimizer::Calibrate_behind,&opt,iter_nums,-3,0,-3,3,-3,3,
                                        -0.1,0.1,-0.1,0.1,-0.1,0.1);
        threads[2]=thread(&Optimizer::Calibrate_behind,&opt,iter_nums,0,3,-3,3,-3,3,
                                        -0.1,0.1,-0.1,0.1,-0.1,0.1);
        threads[3]=thread(&Optimizer::Calibrate_behind,&opt,iter_nums,-3,3,-3,0,-3,3,
                                        -0.1,0.1,-0.1,0.1,-0.1,0.1);
        threads[4]=thread(&Optimizer::Calibrate_behind,&opt,iter_nums,-3,3,0,3,-3,3,
                                        -0.1,0.1,-0.1,0.1,-0.1,0.1);
        threads[5]=thread(&Optimizer::Calibrate_behind,&opt,iter_nums,-3,3,-3,3,-3,0,
                                        -0.1,0.1,-0.1,0.1,-0.1,0.1);
        threads[6]=thread(&Optimizer::Calibrate_behind,&opt,iter_nums,-3,3,-3,3,0,3,
                                        -0.1,0.1,-0.1,0.1,-0.1,0.1);
    }
    
    for(int i=0;i<thread_num;i++){
        threads[i].join();
    }
    auto end_calib_ = chrono::steady_clock::now();
	double during_calib_=std::chrono::duration<double>(end_calib_-start_calib).count();
    cout<<"time:"<<during_calib_<<endl;
    cout<<"best search parameters:"<<endl;
    if(cameraType=="left"){
        cout<<"luminorsity loss before pre opt:"<<opt.max_left_loss<<endl;
        cout<<"luminorsity loss after pre opt:"<<opt.cur_left_loss<<endl;
        cout<<"extrinsic after pre opt:"<<endl<<opt.extrinsic_left_opt<<endl;
        cout<<"best search parameters:"<<endl;
        for(auto e:opt.bestVal_[0])
            cout<<fixed<<setprecision(3)<<e<<" ";
    }
    else if(cameraType=="behind"){
        cout<<"luminorsity loss before pre opt:"<<opt.max_behind_loss<<endl;
        cout<<"luminorsity loss after pre opt:"<<opt.cur_behind_loss<<endl;
        cout<<"extrinsic after pre opt:"<<endl<<opt.extrinsic_behind_opt<<endl;
        cout<<"best search parameters:"<<endl;
        for(auto e:opt.bestVal_[2])
            cout<<fixed<<setprecision(3)<<e<<" ";
    }
    else if(cameraType=="right"){
        cout<<"luminorsity loss before pre opt:"<<opt.max_right_loss<<endl;
        cout<<"luminorsity loss after pre opt:"<<opt.cur_right_loss<<endl;
        cout<<"extrinsic after pre opt:"<<endl<<opt.extrinsic_right_opt<<endl;
        cout<<"best search parameters:"<<endl;
        for(auto e:opt.bestVal_[1])
            cout<<fixed<<setprecision(3)<<e<<" ";
    }
    cout<<endl;


    cout<<"**************************************2nd****************************************"<<endl;
    int thread_num_=5;
    vector<thread>threads_(thread_num_);
    for(int i=0;i<thread_num_;i++){
        if(cameraType=="right"){
            threads_[i]=thread(&Optimizer::fine_Calibrate_right,&opt,5000,-1,1,-1,1,-1,1,
                                        -0.05,0.05,-0.05,0.05,-0.05,0.05);
        }
        else if(cameraType=="left"){
            threads_[i]=thread(&Optimizer::fine_Calibrate_left,&opt,5000,-1,1,-1,1,-1,1,
                                        -0.05,0.05,-0.05,0.05,-0.05,0.05);
        }
        else if(cameraType=="behind"){
            threads_[i]=thread(&Optimizer::fine_Calibrate_behind,&opt,5000,-1,1,-1,1,-1,1,
                                        -0.05,0.05,-0.05,0.05,-0.05,0.05);
        }
        
    }
    for(int i=0;i<thread_num_;i++){
        threads_[i].join();
    }                                    
    auto end_calib__ = chrono::steady_clock::now();
	double during_calib__=std::chrono::duration<double>(end_calib__-end_calib_).count();
    cout<<"time:"<<during_calib__<<endl;
    if(cameraType=="left"){
        cout<<"luminorsity loss after opt:"<<opt.cur_left_loss<<endl;
        cout<<"extrinsic after opt:"<<endl<<opt.extrinsic_left_opt<<endl;
        cout<<"best search parameters:"<<endl;
        for(auto e:opt.bestVal_[0])
            cout<<fixed<<setprecision(3)<<e<<" ";
    }
    else if(cameraType=="behind"){
        cout<<"luminorsity loss after opt:"<<opt.cur_behind_loss<<endl;
        cout<<"extrinsic after opt:"<<endl<<opt.extrinsic_behind_opt<<endl;
        for(auto e:opt.bestVal_[2])
            cout<<fixed<<setprecision(3)<<e<<" ";
    }
    else if(cameraType=="right"){
        cout<<"luminorsity loss after opt:"<<opt.cur_right_loss<<endl;
        cout<<"extrinsic after opt:"<<endl<<opt.extrinsic_right_opt<<endl;
        for(auto e:opt.bestVal_[1])
            cout<<fixed<<setprecision(3)<<e<<" ";
    }
    cout<<endl;

    cout<<"**************************************3rd****************************************"<<endl;
    int thread_num__=3;
    vector<thread>threads__(thread_num__);
    for(int i=0;i<thread_num__;i++){
        if(cameraType=="right"){
            threads__[i]=thread(&Optimizer::fine_Calibrate_right,&opt,4000,-0.1,0.1,-0.1,0.1,-0.1,0.1,
                                        -0.02,0.02,-0.02,0.02,-0.02,0.02);
            
        }
        else if(cameraType=="left"){
            threads__[i]=thread(&Optimizer::fine_Calibrate_left,&opt,4000,-0.1,0.1,-0.1,0.1,-0.1,0.1,
                                        -0.02,0.02,-0.02,0.02,-0.02,0.02);
        }
        else if(cameraType=="behind"){
            threads__[i]=thread(&Optimizer::fine_Calibrate_behind,&opt,4000,-0.1,0.1,-0.1,0.1,-0.1,0.1,
                                        -0.02,0.02,-0.02,0.02,-0.02,0.02);
        }
    
    }
    for(int i=0;i<thread_num__;i++){
        threads__[i].join();
    }   
    auto end_calib___ = chrono::steady_clock::now();
	double during_calib___=std::chrono::duration<double>(end_calib___-end_calib__).count();
    cout<<"time:"<<during_calib___<<endl;
    if(cameraType=="left"){
        cout<<"luminorsity loss after opt:"<<opt.cur_left_loss<<endl;
        cout<<"extrinsic after opt:"<<endl<<opt.extrinsic_left_opt<<endl;
        cout<<"best search parameters:"<<endl;
        for(auto e:opt.bestVal_[0])
            cout<<fixed<<setprecision(3)<<e<<" ";
    }
    else if(cameraType=="behind"){
        cout<<"luminorsity loss after opt:"<<opt.cur_behind_loss<<endl;
        cout<<"extrinsic after opt:"<<endl<<opt.extrinsic_behind_opt<<endl;
        cout<<"best search parameters:"<<endl;
        for(auto e:opt.bestVal_[2])
            cout<<fixed<<setprecision(3)<<e<<" ";
    }
    else if(cameraType=="right"){
        cout<<"luminorsity loss after opt:"<<opt.cur_right_loss<<endl;
        cout<<"extrinsic after opt:"<<endl<<opt.extrinsic_right_opt<<endl;
        cout<<"best search parameters:"<<endl;
        for(auto e:opt.bestVal_[1])
            cout<<fixed<<setprecision(3)<<e<<" ";
    }
    cout<<endl<<"calibration time: "<<during_calib_+during_calib__+during_calib___<<"s"<<endl;
}  

int main(){
    string file="imgs/luminorsity_example/24";

    //camera_model:0-fisheye;1-Ocam;2-pinhole
    int camera_model=2;

	//read frames
    Mat imgf=cv::imread(file+"/samples/Front.png");
    Mat imgl=cv::imread(file+"/samples/Left.png");
    Mat imgb=cv::imread(file+"/samples/Back.png");
    Mat imgr=cv::imread(file+"/samples/Right.png");

    //initilize the optimizer
	Optimizer opt(&imgf,&imgl,&imgb,&imgr,camera_model,1500,1500);

    
	//bev images before optimization
	Mat GF=opt.project_on_ground(imgf,opt.extrinsic_front,opt.intrinsic_front,opt.distortion_params_front,opt.KG,opt.brows,opt.bcols,opt.hf);
	Mat GB=opt.project_on_ground(imgb,opt.extrinsic_behind,opt.intrinsic_behind,opt.distortion_params_behind,opt.KG,opt.brows,opt.bcols,opt.hb);
	Mat GL=opt.project_on_ground(imgl,opt.extrinsic_left,opt.intrinsic_left,opt.distortion_params_left,opt.KG,opt.brows,opt.bcols,opt.hl);
	Mat GR=opt.project_on_ground(imgr,opt.extrinsic_right,opt.intrinsic_right,opt.distortion_params_right,opt.KG,opt.brows,opt.bcols,opt.hr);

    GF=opt.tail(GF,"f");
    GB=opt.tail(GB,"b");
    GL=opt.tail(GL,"l");
    GR=opt.tail(GR,"r");

	Mat bev_before=opt.generate_surround_view(GF,GL,GB,GR);
	imshow("opt_before",bev_before);
	//waitKey(0);
	imwrite("imgs/luminorsity_example/before_all_cameras_calib.png",bev_before);


	//texture extraction of front view
	extractor ext1(GF,GL);
	ext1.Binarization();
	ext1.findcontours();
	Mat texture_fl=ext1.extrac_textures_and_save(file+"/fl_textures.csv");
    imwrite(file+"/texture_fl.png",texture_fl);
    opt.fl_pixels_texture_less=opt.readfromcsv(file+"/fl_textures.csv");
    Mat pG_fl = Mat::ones(3,opt.fl_pixels_texture_less.size(),CV_64FC1);
    for(int i=0;i<opt.fl_pixels_texture_less.size();i++){
        pG_fl.at<double>(0,i) = opt.fl_pixels_texture_less[i].x;
        pG_fl.at<double>(1,i) = opt.fl_pixels_texture_less[i].y;
    }  
    opt.pG_fl=pG_fl; 
    Mat PG_fl = Mat::ones(4,opt.fl_pixels_texture_less.size(),CV_64FC1);
    PG_fl(cv::Rect(0,0,opt.fl_pixels_texture_less.size(),3))=opt.eigen2mat(opt.KG.inverse())*pG_fl*opt.hl; 
    opt.PG_fl=PG_fl;

	extractor ext2(GF,GR);
	ext2.Binarization();
	ext2.findcontours();
	Mat texture_fr=ext2.extrac_textures_and_save(file+"/fr_textures.csv");
    imwrite(file+"/texture_fr.png",texture_fr);
    opt.fr_pixels_texture_less=opt.readfromcsv(file+"/fr_textures.csv");
    Mat pG_fr = Mat::ones(3,opt.fr_pixels_texture_less.size(),CV_64FC1);
    for(int i=0;i<opt.fr_pixels_texture_less.size();i++){
        pG_fr.at<double>(0,i) = opt.fr_pixels_texture_less[i].x;
        pG_fr.at<double>(1,i) = opt.fr_pixels_texture_less[i].y;
    }   
    opt.pG_fr=pG_fr;
    Mat PG_fr = Mat::ones(4,opt.fr_pixels_texture_less.size(),CV_64FC1);
    PG_fr(cv::Rect(0,0,opt.fr_pixels_texture_less.size(),3))=opt.eigen2mat(opt.KG.inverse())*pG_fr*opt.hr; 
    opt.PG_fr=PG_fr; 
    
    cout<<"*********************************start right**************************************"<<endl;

    CameraOptimization(opt,"right");
    Mat GR_opt=opt.project_on_ground(imgr,opt.extrinsic_right_opt,opt.intrinsic_right,opt.distortion_params_right,opt.KG,opt.brows,opt.bcols,opt.hr);
    GR_opt=opt.tail(GR_opt,"r");
    imwrite(file+"/GR_opt.png",GR_opt);
    

    cout<<"*********************************start left**************************************"<<endl;

    CameraOptimization(opt,"left");
    Mat GL_opt=opt.project_on_ground(imgl,opt.extrinsic_left_opt,opt.intrinsic_left,opt.distortion_params_left,opt.KG,opt.brows,opt.bcols,opt.hl);
    GL_opt=opt.tail(GL_opt,"l");
    imwrite(file+"/GL_opt.png",GL_opt);    


    // texture extraction of behind view
    Mat GL_opt_=imread(file+"/GL_opt.png");
    opt.imgl_bev_rgb=GL_opt_;
    opt.imgl_bev=opt.gray_gamma(opt.imgl_bev_rgb);
	extractor ext3(GL_opt_,GB);
	ext3.Binarization();
	ext3.findcontours();
	Mat texture_bl=ext3.extrac_textures_and_save(file+"/bl_textures.csv");
    imwrite(file+"/texture_bl.png",texture_bl);
	opt.bl_pixels_texture_less=opt.readfromcsv(file+"/bl_textures.csv");
    Mat pG_bl = Mat::ones(3,opt.bl_pixels_texture_less.size(),CV_64FC1);
    for(int i=0;i<opt.bl_pixels_texture_less.size();i++){
        pG_bl.at<double>(0,i) = opt.bl_pixels_texture_less[i].x;
        pG_bl.at<double>(1,i) = opt.bl_pixels_texture_less[i].y;
    }  
    opt.pG_bl=pG_bl; 
    Mat PG_bl = Mat::ones(4,opt.bl_pixels_texture_less.size(),CV_64FC1);
    PG_bl(cv::Rect(0,0,opt.bl_pixels_texture_less.size(),3))=opt.eigen2mat(opt.KG.inverse())*pG_bl*opt.hb; 
    opt.PG_bl=PG_bl;

	Mat GR_opt_=imread(file+"/GR_opt.png");
    opt.imgr_bev_rgb=GR_opt_;
    opt.imgr_bev=opt.gray_gamma(opt.imgr_bev_rgb);
    extractor ext4(GR_opt_,GB);
	ext4.Binarization();
	ext4.findcontours();
	Mat texture_br=ext4.extrac_textures_and_save(file+"/br_textures.csv");
    imwrite(file+"/texture_br.png",texture_br);
	opt.br_pixels_texture_less=opt.readfromcsv(file+"/br_textures.csv");
    Mat pG_br = Mat::ones(3,opt.br_pixels_texture_less.size(),CV_64FC1);
    for(int i=0;i<opt.br_pixels_texture_less.size();i++){
        pG_br.at<double>(0,i) = opt.br_pixels_texture_less[i].x;
        pG_br.at<double>(1,i) = opt.br_pixels_texture_less[i].y;
    }  
    opt.pG_br=pG_br; 
    Mat PG_br = Mat::ones(4,opt.br_pixels_texture_less.size(),CV_64FC1);
    PG_br(cv::Rect(0,0,opt.br_pixels_texture_less.size(),3))=opt.eigen2mat(opt.KG.inverse())*pG_br*opt.hb; 
    opt.PG_br=PG_br;

    cout<<"*********************************start behind**********************************"<<endl;
    CameraOptimization(opt,"behind");
    Mat GB_opt=opt.project_on_ground(imgb,opt.extrinsic_behind_opt,opt.intrinsic_behind,opt.distortion_params_behind,opt.KG,opt.brows,opt.bcols,opt.hb);
    GB_opt=opt.tail(GB_opt,"b");
    imwrite(file+"/GB_opt.png",GB_opt);   
    opt.imgb_bev_rgb=GB_opt;
    opt.imgb_bev=opt.gray_gamma(opt.imgb_bev_rgb);

    //save results
    opt.SaveOptResult("imgs/luminorsity_example/after_all_calib.png");


}