#pragma once

int extractFeatureDemo1();

//绘制特征图和关键帧图的特征点，并拼接两张图（物理）
int extractFeatureDemo2();
//绘制特征图和关键帧图的特征点，并拼接两张图（化学）
int extractFeatureDemo3();


int bfMatch();

int flannMatch();

//FLANN匹配（物理）,连接特征点
int flannMatch_p();

//FLANN匹配（化学）,连接特征点
int flannMatch_c();


int objectSearch();

int orb();

void matchDemo();

int mergeImage_c();

void compressFile();

void compressFile2();

int IpcPreview();




