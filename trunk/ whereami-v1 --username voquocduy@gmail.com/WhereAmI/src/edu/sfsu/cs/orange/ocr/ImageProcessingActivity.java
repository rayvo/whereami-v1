package edu.sfsu.cs.orange.ocr;

import java.util.List;

import com.googlecode.leptonica.android.ReadFile;

import android.app.Activity;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Point;
import android.graphics.drawable.BitmapDrawable;
import android.os.Bundle;
import android.util.Log;
import android.view.View;


public class ImageProcessingActivity extends Activity{
	private static final String TAG = ImageProcessingActivity.class.getSimpleName();
	
	private String fileNameOrg;
	private String fileNameHL;
	private Bitmap curBitmap = null;
	private static TouchView imgView ;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		setContentView(R.layout.image_processing);
		
		Intent intent = getIntent();
		fileNameOrg = intent.getStringExtra("FILE_NAME");
		Log.d(TAG, fileNameOrg);
		imgView = (TouchView) findViewById(R.id.imageView1);
		//Bitmap bitmap = BitmapFactory.decodeFile(file.getAbsolutePath(), bmOptions);
		
		imgView.setFileName(fileNameOrg); 
		Bitmap bitmap = BitmapFactory.decodeFile(fileNameOrg);
		
		//curBitmap = bitmap;
		String tmp = fileNameOrg.substring(0, fileNameOrg.indexOf(".jpg"));
		fileNameHL = tmp + "_hl.jpg";
		
		//saveImage(bitmap, fileNameOrg);
		
		imgView.setImageBitmap(bitmap);
				
        /*mRgba = Utils.loadResource(ImageProcessingActivity.this, R.drawable.your_image,Highgui.CV_LOAD_IMAGE_COLOR);
        Bitmap img = Bitmap.createBitmap(mRgba.cols(), mRgba.rows(),Bitmap.Config.ARGB_8888);
        Utils.matToBitmap(mRgba, img);*/
//        /imgView.setImageBitmap(img);
	}
	
	@Override
	protected void onDestroy() {
		// TODO Auto-generated method stub
		super.onDestroy();
		Intent returnIntent = new Intent();
		setResult(RESULT_OK,returnIntent);     
		finish();
	}

	
	public void doOCR(View view) {
        // call to the native method
		curBitmap = ((BitmapDrawable) imgView.getDrawable()).getBitmap();
		String folder = fileNameOrg.substring(0,fileNameOrg.indexOf(".jpg"));
			
		List<Point> points = imgView.getPoints();
		int size = points.size();
		int[] xPoints = new int[size];
		int[] yPoints = new int[size];
		int i = 0;
		for(Point p:points) {
			xPoints[i] = p.x;
			yPoints[i] = p.y;
			i++;
		}		
		boolean result = extractFrags(curBitmap, folder, fileNameOrg, fileNameHL, xPoints, yPoints);
		if (result) {
			String fragName = folder + "_crop";
			for (i = 1; i <= size; i++) {

				cleanImage(fragName + i + ".jpg", fragName + i + "_out_m0.jpg",
						"0");
				cleanImage(fragName + i + ".jpg", fragName + i + "_out_m1.jpg",
						"1");
			}
			Intent returnIntent = new Intent();
			returnIntent.putExtra("NUM_FRAG", size);
			returnIntent.putExtra("FRAG_NAME", fragName);

			setResult(RESULT_OK, returnIntent);
			finish();
		}
    }
	
	//
	// Native JNI 
	//
	private native boolean cleanImage(String input, String output, String mode);
	private native boolean getFragments(Bitmap bitmap, int x, int y, byte[] imgData, int[] outPixels);
	public native boolean extractFrags(Bitmap bmp, String folder, String fileName, String fileNameHL, int[] xPoints, int[] yPoints);
	
	public native boolean saveImage(Bitmap bitmap, String fileName);
	
	public native boolean brightness(Bitmap bmp, float brightness);
	static 
    {
        System.loadLibrary("ImageProcessing");
    }
       
    private Runnable DoImageProcessing = new Runnable() 
    {

		@Override
		public void run() {
			// TODO Auto-generated method stub
			
		}
        /*public void run() 
        {
        	Log.i("MyRealTimeImageProcessing", "DoImageProcessing():");
        	bProcessing = true;
        	
			HightLight(curBitmap, x, y, inputData, pixels);			
			bitmap.setPixels(pixels, 0, width, 0, 0, width, height);
			
			TouchView.this.setImageBitmap(curBitmap);	
        	
			
			bProcessing = false;
        }*/
    };  
    
 
	
	
}
