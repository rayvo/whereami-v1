#include <jni.h>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>

#include <android/log.h>
#include <android/bitmap.h>

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

using namespace std;
using namespace cv;

Mat * mCanny = NULL;
#define DEBUG_TAG "NDK_AndroidNDK1SampleActivity"
#define  LOG_TAG    "ImageProcessing"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

IplImage* img = NULL;

static int rgb_clamp(int value) {
	if (value > 255) {
		return 255;
	}
	if (value < 0) {
		return 0;
	}
	return value;
}

static void brightness(AndroidBitmapInfo* info, void* pixels,
		float brightnessValue) {
	int xx, yy, red, green, blue;
	uint32_t* line;

	for (yy = 0; yy < info->height; yy++) {
		line = (uint32_t*) pixels;
		for (xx = 0; xx < info->width; xx++) {

			//extract the RGB values from the pixel
			red = (int) ((line[xx] & 0x00FF0000) >> 16);
			green = (int) ((line[xx] & 0x0000FF00) >> 8);
			blue = (int) (line[xx] & 0x00000FF);

			//manipulate each value
			red = rgb_clamp((int) (red * brightnessValue));
			green = rgb_clamp((int) (green * brightnessValue));
			blue = rgb_clamp((int) (blue * brightnessValue));

			// set the new pixel back in
			line[xx] = ((red << 16) & 0x00FF0000) | ((green << 8) & 0x0000FF00)
					| (blue & 0x000000FF);
		}

		pixels = (char*) pixels + info->stride;
	}
}

void drawCircle1(IplImage* img, int x, int y) {
	//cvRectangle( img, cvPoint(rect.x, rect.y), cvPoint(box.x+box.width,box.y+box.height), cvScalar(0xff,0x00,0x00),CV_FILLED );
	cvCircle(img, cvPoint(x, y), 10, cvScalar(0xff, 0x00, 0x00), CV_FILLED);
}

void drawCircle(AndroidBitmapInfo* info, unsigned char* pixels, jint x,
		jint y) {

	IplImage *image;

	IplImage *rgb565 = cvCreateImage(cvSize(info->width, info->height), //size
	IPL_DEPTH_8U, //depth
			2); //channels

}

extern "C"

jboolean Java_my_project_MyRealTimeImageProcessing_TouchView_brightness(
		JNIEnv * env, jobject obj, jobject bitmap, jfloat brightnessValue) {
	AndroidBitmapInfo info;
	int ret;
	void* pixels;
	unsigned char *rgb565Data;
	cv::Mat* mat;

	if ((ret = AndroidBitmap_getInfo(env, bitmap, &info)) < 0) {
		LOGE("AndroidBitmap_getInfo() failed ! error=%d", ret);
		return false;
	}
	if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
		LOGE("Bitmap format is not RGBA_8888 !");
		return false;
	}

	IplImage *rgb565 = cvCreateImage(cvSize(info.width, info.height), //size
	IPL_DEPTH_8U, //depth
			2);

	if ((ret = AndroidBitmap_lockPixels(env, bitmap, &pixels)) < 0) {
		LOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
	}

	//memcpy(pixels, mat->data, info.height * info.width * 4);

	AndroidBitmap_lockPixels(env, bitmap, (void**) &rgb565Data);
	//memcpy((void*)rgb565->imageData,rgb565Data,rgb565->imageSize);

	brightness(&info, pixels, brightnessValue);

	AndroidBitmap_unlockPixels(env, bitmap);
	return true;
}

extern "C" jboolean Java_edu_sfsu_cs_orange_ocr_TouchView_HightLight(
		JNIEnv * env, jobject obj, jobject bitmap, jint x, jint y,
		jbyteArray imgData, jintArray outPixels) {
	__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "NDK: HightLight");

	AndroidBitmapInfo info;
	int ret;
	void* pixels;

	if ((ret = AndroidBitmap_getInfo(env, bitmap, &info)) < 0) {
		LOGE("AndroidBitmap_getInfo() failed ! error=%d", ret);
		return false;
	}
	__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "1");
	__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG,
			"h-w [%i]x[%i] \t[%i], [%i]", info.height, info.width, x, y);
	__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "2");

	if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
		LOGE("Bitmap format is not RGBA_8888 !");
		return false;
	}

	if ((ret = AndroidBitmap_lockPixels(env, bitmap, &pixels)) < 0) {
		LOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
	}

	/*	jbyte * pImageData = env->GetByteArrayElements(imgData, 0); */
	Mat myuv(info.height, info.width, CV_8UC4, (unsigned char *) pixels);
	img = new IplImage(myuv);

	/*IplImage* img = cvCreateImage(cvSize(info.width,info.height), IPL_DEPTH_8U, 4);
	 img->imageData =(char *) pixels;*/

	cvCircle(img, cvPoint(x, y), 20, cvScalar(255, 0, 0), CV_FILLED);

	AndroidBitmap_unlockPixels(env, bitmap);
	return true;
}

Mat src;
Mat srcHL;
Mat crop;

int cnt=1;
int ct=0;
int max_x, max_y, min_x, min_y;

void flood_crop(int x,int y)
{
	Vec3b intensity = src.at<Vec3b>(x, y);


	if(intensity.val[0] == 255 && intensity.val[1]==0 && intensity.val[2]==0)
	{
		intensity.val[0] = 255;
		intensity.val[1] = 255;
		intensity.val[2] = 255;
		src.at<Vec3b>(x, y) = intensity;


		if(x>max_x)
		{
			max_x=x;
		}
		if(y>max_y)
		{
			max_y=y;
		}
		if(x<min_x)
		{
			min_x=x;
		}
		if(y<min_y)
		{
			min_y=y;
		}

		if(x>0)
		{
			flood_crop(x-1, y);
		}
		if(y>0)
		{
			flood_crop(x, y-1);
		}
		if(x<src.rows-1)
		{
			flood_crop(x+1, y);
		}
		if(y<src.cols-1)
		{
			flood_crop(x, y+1);
		}
	}
	return;
}

extern "C" jboolean Java_edu_sfsu_cs_orange_ocr_ImageProcessingActivity_extractFrags(
		JNIEnv * env, jobject obj, jobject bitmap, jstring _folder, jstring _fileName,
		jstring _fileNameHL, jintArray xPoints, jintArray yPoints) {

	const char *folder = (*env).GetStringUTFChars(_folder, NULL);
	const char *fileName = (*env).GetStringUTFChars(_fileName, NULL);
	const char *fileNameHL = (*env).GetStringUTFChars(_fileNameHL, NULL);

	__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG,
				"extractFrags: Folder = %s", folder);
	__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG,
			"extractFrags: Original file = %s", fileName);
	__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG,
			"extractFrags: Highlighted file = %s", fileNameHL);

	//Save Highlighted Image
	AndroidBitmapInfo info;
	int ret;
	void* pixels;

	if ((ret = AndroidBitmap_getInfo(env, bitmap, &info)) < 0) {
		LOGE("AndroidBitmap_getInfo() failed ! error=%d", ret);
		return false;
	}

	if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
		LOGE("Bitmap format is not RGBA_8888 !");
		return false;
	}

	if ((ret = AndroidBitmap_lockPixels(env, bitmap, &pixels)) < 0) {
		LOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
	}

	Mat myuv(info.height, info.width, CV_8UC4, (unsigned char *) pixels);
	img = new IplImage(myuv);

	cvSaveImage(fileNameHL, img);
	LOGI("Highlighted Image saved successfully !");
	AndroidBitmap_unlockPixels(env, bitmap);

	// Load source image and convert it to gray
	LOGI("Load images!");
	src = imread(fileName);
	srcHL = imread(fileNameHL);

	int i;
	jsize len = (*env).GetArrayLength(xPoints);
	jint *x = (*env).GetIntArrayElements(xPoints, 0);
	jint *y = (*env).GetIntArrayElements(yPoints, 0);

	LOGI("Len = %i", len);

	for (i = 0; i < len; i++) {
		LOGI("x[%i] = %i",i, x[i]);
		LOGI("y[%i] = %i",i, y[i]);
	}

	for (i = 0; i < len; i++) {
		max_x = x[i];
		max_y = y[i];
		min_x = x[i];
		min_y = y[i];

		flood_crop(x[i], y[i]);
		cout << min_x << ", " << min_y << "  " << max_x << ", " << max_y;

		if (max_x + 3 < src.rows) {
			max_x = max_x + 3;
		}

		if (max_y + 3 < src.cols) {
			max_y = max_y + 3;
		}
		if (min_x - 3 < 0) {
			min_x = min_x - 3;
		}
		if (min_y - 3 < 0) {
			min_y = min_y - 3;
		}

		Mat temp = Mat::zeros(max_x - min_x + 1, max_y - min_y + 1, CV_8UC3);
		int width = temp.cols;
		int height = temp.rows;

		LOGI("width = %i",width);
		LOGI("height = %i",height);

		for (int r = 0; r < height; r++) {
			for (int c = 0; c < width; c++) {
				temp.at<Vec3b>(r, c) = srcHL.at < Vec3b > (r + min_x, c + min_y);
			}
		}

		//namedWindow( "Display window", CV_WINDOW_AUTOSIZE );// Create a window for display.
		//imshow( "Display window", temp );

		string nm = "crop";
		string str;
		ostringstream tmp;
		tmp << cnt;
		str = tmp.str();
		nm += str;

		//string loc = "/home/darius/WhereAmI_Code/hghImg/crops/";

		string loc = folder;
		loc = "/mnt/sdcard/";
		loc += nm;
		loc += ".jpg";
		//LOGI(loc);
		const char *cstr = loc.c_str();
		cout << cstr << endl;
		imwrite(cstr, temp);
		cnt++;
	}
	LOGI("Finish!");
	(*env).ReleaseStringUTFChars(_fileName, fileName);
	(*env).ReleaseStringUTFChars(_fileNameHL, fileNameHL);
	return true;
}
extern "C" jboolean Java_edu_sfsu_cs_orange_ocr_ImageProcessingActivity_saveImage(
		JNIEnv * env, jobject obj, jobject bitmap, jstring _fileName) {

	const char *fileName = (*env).GetStringUTFChars(_fileName, NULL);

	__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "saveImage: %s",
			fileName);

	AndroidBitmapInfo info;
	int ret;
	void* pixels;

	if ((ret = AndroidBitmap_getInfo(env, bitmap, &info)) < 0) {
		LOGE("AndroidBitmap_getInfo() failed ! error=%d", ret);
		return false;
	}
	__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "1");

	if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
		LOGE("Bitmap format is not RGBA_8888 !");
		return false;
	}

	if ((ret = AndroidBitmap_lockPixels(env, bitmap, &pixels)) < 0) {
		LOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
	}

	/*	jbyte * pImageData = env->GetByteArrayElements(imgData, 0); */
	Mat myuv(info.height, info.width, CV_8UC4, (unsigned char *) pixels);
	img = new IplImage(myuv);

	/*IplImage* img = cvCreateImage(cvSize(info.width,info.height), IPL_DEPTH_8U, 4);
	 img->imageData =(char *) pixels;*/

	cvSaveImage(fileName, img);

	AndroidBitmap_unlockPixels(env, bitmap);
	(*env).ReleaseStringUTFChars(_fileName, fileName);
	return true;

}
extern "C" jboolean Java_my_project_MyRealTimeImageProcessing_TouchView_HightLight1(
		JNIEnv* env, jobject object, jint height, jint width, jint x, jint y,
		jbyteArray imgData, jintArray outPixels) {
	__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG,
			"NDK: HightLight h-w [%i]x[%i] \t[%i], [%i]", height, width, x, y);
	jbyte * pImageData = env->GetByteArrayElements(imgData, 0);
	jint * poutPixels = env->GetIntArrayElements(outPixels, 0);

	if (img == NULL) {
		Mat myuv(height, width, CV_8UC1, (unsigned char *) pImageData);
		img = new IplImage(myuv);
	}

	cvCircle(img, cvPoint(x, y), 10, cvScalar(0xff, 0x00, 0x00), CV_FILLED);

	Mat mResult(height, width, CV_8UC4, (unsigned char *) poutPixels);
	IplImage ResultImg = mResult;
	mResult = img;

	env->ReleaseByteArrayElements(imgData, pImageData, 0);
	env->ReleaseIntArrayElements(outPixels, poutPixels, 0);
	return true;
}

jboolean Java_my_project_MyRealTimeImageProcessing_CameraPreview_ImageProcessing(
		JNIEnv* env, jobject thiz, jint width, jint height,
		jbyteArray NV21FrameData, jintArray outPixels) {
	__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "NDK:Ray Vo");
	jbyte * pNV21FrameData = env->GetByteArrayElements(NV21FrameData, 0);
	jint * poutPixels = env->GetIntArrayElements(outPixels, 0);

	if (mCanny == NULL) {
		mCanny = new Mat(height, width, CV_8UC1);
	}

	Mat mGray(height, width, CV_8UC1, (unsigned char *) pNV21FrameData);
	Mat mResult(height, width, CV_8UC4, (unsigned char *) poutPixels);
	IplImage srcImg = mGray;
	IplImage CannyImg = *mCanny;
	IplImage ResultImg = mResult;

	cvCanny(&srcImg, &CannyImg, 80, 100, 3);
	cvCvtColor(&CannyImg, &ResultImg, CV_GRAY2BGRA);

	env->ReleaseByteArrayElements(NV21FrameData, pNV21FrameData, 0);
	env->ReleaseIntArrayElements(outPixels, poutPixels, 0);
	return true;
}

