package org.opencv.samples.facedetect;
import com.baidu.detector.jni.LaneDetectNativeInterface;
import com.baidu.detector.jni.bean.CubicPoly;
import com.baidu.detector.jni.bean.Spline;
import com.baidu.detector.view.CustomerImageView;
import com.baidu.trafficplatedetect.TrafficPlateDetect;
import com.baidu.trafficplaterecon.TrafficplateRecon;
import com.baidu.trafficsigndetect.TrafficsignDetect;
import com.baidu.trafficsignrecon.TrafficsignRecon;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;

import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.CameraBridgeViewBase;
import org.opencv.android.Utils;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewFrame;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.android.OpenCVLoader;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.MatOfRect;
import org.opencv.core.Rect;
import org.opencv.core.Point;
import org.opencv.core.Scalar;
import org.opencv.highgui.Highgui;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewListener2;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.graphics.Color;
import android.graphics.PointF;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.WindowManager;


enum DetectType { TRAFFIC_SIGN, PLATE, LANE}

@SuppressLint("HandlerLeak")
public class FdActivity extends Activity implements CvCameraViewListener2 {

    private static final String    TAG                 = "StMobileDemo::Activity";
    private static final Scalar    PLATE_RECT_COLOR     = new Scalar(255, 0, 0, 255);
    private static final Scalar    TRAFFICSIGN_RECT_COLOR     = new Scalar(255, 0, 255, 255);
    private static final Scalar    GREEN_COLOR     = new Scalar(0, 255, 0, 255);
    private static final Scalar    BLACK_COLOR     = new Scalar(0, 0, 0, 255);
    private static final Scalar    WHITE_COLOR     = new Scalar(255, 255, 255, 255);
    public static final int        JAVA_DETECTOR       = 0;
    public static final int        NATIVE_DETECTOR     = 1;
    
    DetectType enDetectType = DetectType.TRAFFIC_SIGN;

    private MenuItem               mItemTrafficSign;
    private MenuItem               mItemPlate;
    private MenuItem               mItemLane;

    private Mat                    mRgba;
    private Mat                    mGray;
    private File				   mPlateCascadeFile;
    private File				   mPlateOcrFile;
    private File				   mTrafficSignCascadeFile;
    private TrafficPlateDetect     mPlateDetector;
    private TrafficplateRecon	   mPlateRecon;
    private TrafficsignDetect      mSignDetector;
    private TrafficsignRecon 	   mSignRecon;
    //private DetectionBasedTracker  mTrafficSignDetector;


    private CameraBridgeViewBase  mOpenCvCameraView;
    private CustomerImageView mImgView;

    private BaseLoaderCallback  mLoaderCallback = new BaseLoaderCallback(this) {
        @Override
        public void onManagerConnected(int status) {
            switch (status) {
                case LoaderCallbackInterface.SUCCESS:
                {
                    Log.i(TAG, "OpenCV loaded successfully");

                    // Load native library after(!) OpenCV initialization
                    System.loadLibrary("detection_based_tracker");

                    // 加载交通标志分类器
                    try {
                        // load cascade file from application resources
                        InputStream is = getResources().openRawResource(R.raw.cascade17_ts19);
                        File cascadeDir = getDir("cascade", Context.MODE_PRIVATE);
                        mTrafficSignCascadeFile = new File(cascadeDir, "cascade17_ts19.xml");
                        if (mTrafficSignCascadeFile.exists())
                        {
                        	Log.i(TAG, "Loaded cascade classifier from " + mTrafficSignCascadeFile.getAbsolutePath());
                        }
                        FileOutputStream os = new FileOutputStream(mTrafficSignCascadeFile);

                        byte[] buffer = new byte[4096];
                        int bytesRead;
                        while ((bytesRead = is.read(buffer)) != -1) {
                            os.write(buffer, 0, bytesRead);
                        }
                        is.close();
                        os.close();

                        //mTrafficSignDetector = new DetectionBasedTracker(mTrafficSignCascadeFile.getAbsolutePath(), 0);
                        mSignDetector = new TrafficsignDetect(mTrafficSignCascadeFile.getAbsolutePath());

                        cascadeDir.delete();
                        
                        // 初始化交通标志识别
                        mSignRecon = new TrafficsignRecon();

                    } catch (IOException e) {
                        e.printStackTrace();
                        Log.e(TAG, "Failed to load cascade. Exception thrown: " + e);
                    }
                    
                    // 加载车牌分类器
                    try {
                        // load cascade file from application resources
                        InputStream is = getResources().openRawResource(R.raw.plate1);
                        File cascadeDir = getDir("cascade", Context.MODE_PRIVATE);
                        mPlateCascadeFile = new File(cascadeDir, "plate1.xml");
                        if (mPlateCascadeFile.exists())
                        {
                        	Log.i(TAG, "Loaded cascade classifier from " + mPlateCascadeFile.getAbsolutePath());
                        }
                        FileOutputStream os = new FileOutputStream(mPlateCascadeFile);

                        byte[] buffer = new byte[4096];
                        int bytesRead;
                        while ((bytesRead = is.read(buffer)) != -1) {
                            os.write(buffer, 0, bytesRead);
                        }
                        is.close();
                        os.close();
                        
                        //加载ocr分类器的路径
                        InputStream ocris = getResources().openRawResource(R.raw.lisenceplate_ocr);
                        File ocrDir = getDir("cascade", Context.MODE_PRIVATE);
                        mPlateOcrFile = new File(ocrDir, "lisenceplate_ocr.xml");
                        if (mPlateOcrFile.exists())
                        {
                        	Log.i(TAG, "Loaded ocr lisenceplate classifier from " + mPlateOcrFile.getAbsolutePath());
                        }
                        FileOutputStream ocros = new FileOutputStream(mPlateOcrFile);

                        byte[] ocrbuffer = new byte[4096];
                        while ((bytesRead = ocris.read(ocrbuffer)) != -1) {
                        	ocros.write(ocrbuffer, 0, bytesRead);
                        }
                        ocris.close();
                        ocros.close();

                        mPlateDetector = new TrafficPlateDetect(mPlateCascadeFile.getAbsolutePath(), mPlateOcrFile.getAbsolutePath());

                        cascadeDir.delete();
                        ocrDir.delete();
                        
                        // 车牌识别
                        mPlateRecon = new TrafficplateRecon();

                    } catch (IOException e) {
                        e.printStackTrace();
                        Log.e(TAG, "Failed to load cascade. Exception thrown: " + e);
                    }

                    mOpenCvCameraView.enableView();
                } break;
                default:
                {
                    super.onManagerConnected(status);
                } break;
            }
        }
    };

    public FdActivity() {
        Log.i(TAG, "Instantiated new " + this.getClass());
    }

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        Log.i(TAG, "called onCreate");
        super.onCreate(savedInstanceState);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setContentView(R.layout.face_detect_surface_view);

        mOpenCvCameraView = (CameraBridgeViewBase) findViewById(R.id.fd_activity_surface_view);
        mOpenCvCameraView.setCvCameraViewListener(this);
        mImgView = (CustomerImageView) findViewById(R.id.customer_img_View);
    }

    @Override
    public void onPause()
    {
        super.onPause();
        if (mOpenCvCameraView != null)
            mOpenCvCameraView.disableView();
    }

    @Override
    public void onResume()
    {
        super.onResume();
        OpenCVLoader.initAsync(OpenCVLoader.OPENCV_VERSION_2_4_9, this, mLoaderCallback);
    }

    public void onDestroy() {
        super.onDestroy();
        mOpenCvCameraView.disableView();
    }

    public void onCameraViewStarted(int width, int height) {
        mGray = new Mat();
        mRgba = new Mat();
    }

    public void onCameraViewStopped() {
        mGray.release();
        mRgba.release();
    }

    public Mat onCameraFrame(CvCameraViewFrame inputFrame) {

        mRgba = inputFrame.rgba();
        mGray = inputFrame.gray();
        
        Point start1 = new Point(0.0,mRgba.rows()*0.2);
        Point end1 = new Point(mRgba.cols()-1,mRgba.rows()*0.2);
        Core.line(mRgba, start1, end1, GREEN_COLOR);
        
        Point end2 = new Point(mRgba.cols()-1,mRgba.rows()*0.8);
        //Core.line(mRgba, start2, end2, GREEN_COLOR);
        Core.rectangle(mRgba, start1, end2, GREEN_COLOR, 3);
        
        org.opencv.core.Rect rect = new org.opencv.core.Rect();
        rect.x = (int)start1.x;
        rect.y = (int)start1.y;
        rect.width = (int)(end2.x - start1.x);
        rect.height = (int)(end2.y - start1.y);
        Mat	roiGray = new Mat(mGray, rect);

        double stime = System.currentTimeMillis();
        if(enDetectType == DetectType.LANE) {
        	Mat rgba = mRgba;
        	
        	ArrayList<Spline> splines = new ArrayList<Spline>();
    		ArrayList<Float> scores = new ArrayList<Float>();
    		ArrayList<CubicPoly> cubicPolies = new ArrayList<CubicPoly>();
    		
    		int width = rgba.cols();
			int height = rgba.rows();
			PointF[] adjustedPoints = mImgView.getAdjustedPoints(width, height);
    		if(! (adjustedPoints == null) ) {    			
    			LaneDetectNativeInterface.saveAdjustedPoints(adjustedPoints, width, height);
    		}
    		else {
    			
    		}
    		LaneDetectNativeInterface.getLane(rgba , splines, scores, cubicPolies);
    		paintLane(cubicPolies, rgba);
    		return rgba;
        }
        else if(enDetectType == DetectType.PLATE)
        {
        	MatOfRect plates = new MatOfRect();
            String strLisence = new String();
            
            if (mPlateDetector != null)
            {
            	mPlateDetector.detect(mRgba, plates, strLisence);
            	
                Rect[] plateArray = plates.toArray();
                for (int i = 0; i < plateArray.length; i++)
                {
                	Mat img = new Mat(mGray, plateArray[i]);
                	Highgui.imwrite("/storage/emulated/0/lisenceplate/a.png", img);
                	String strNo = mPlateRecon.detect(img);
                	Core.rectangle(mRgba, plateArray[i].tl(), plateArray[i].br(), PLATE_RECT_COLOR, 3);
                	
                	int dslength = strNo.length();
                	int dswidth = dslength * 20;
                	Core.rectangle(mRgba, plateArray[i].tl(), plateArray[i].br(), TRAFFICSIGN_RECT_COLOR, 3);
                	//Point pos = new Point(trafficsignArray[i].tl().x, trafficsignArray[i].br().y);
                	Core.rectangle(mRgba, new Point(plateArray[i].br().x,plateArray[i].br().y+5), new Point(plateArray[i].br().x+dswidth, plateArray[i].br().y-25), WHITE_COLOR, -1);
                	Core.putText(mRgba, strNo, plateArray[i].br(), 3, 1.0, BLACK_COLOR);
                }
            }
        }
        else if(enDetectType == DetectType.TRAFFIC_SIGN)
        {
            MatOfRect trafficsigns = new MatOfRect();
                      
            if (mSignDetector != null)
            {
            	mSignDetector.detect(roiGray, trafficsigns);
            }
            
            Rect[] trafficsignArray = trafficsigns.toArray();
            for (int i = 0; i < trafficsignArray.length; i++)
            {
            	trafficsignArray[i].y += start1.y;
            	
            	
            	Mat img = new Mat(mRgba, trafficsignArray[i]);
            	long signType = mSignRecon.detect(img);
            	String strType = new String();
            	if (signType == 0)
            	{
            		strType = "not recognized";
            	}
            	else if (signType == 1)
            	{
            		strType = "no_left";
            	}
            	else if (signType == 2)
            	{
            		strType = "no_right";
            	}
            	else if (signType == 3)
            	{
            		strType = "no_straight";
            	}
            	else if (signType == 4)
            	{
            		strType = "no_left_right";
            	}
            	else if (signType == 5)
            	{
            		strType = "no_u_turn";
            	}
            	else if (signType == 6)
            	{
            		strType = "no_entry_motor";
            	}
            	else if (signType == 7)
            	{
            		strType = "stop";
            	}
            	else if (signType == 8)
            	{
            		strType = "yield";
            	}
            	else if (signType == 9)
            	{
            		strType = "give_way";
            	}
            	else if (signType == 10)
            	{
            		strType = "no_entry";
            	}
            	else if (signType == 11)
            	{
            		strType = "no_entry_car";
            	}
            	else if (signType == 12)
            	{
            		strType = "no_entry_passanger_car";
            	}
            	else if (signType == 13)
            	{
            		strType = "no_left_straight";
            	}
            	else if (signType == 14)
            	{
            		strType = "no_right_straight";
            	}
            	else if (signType == 15)
            	{
            		strType = "no_overtaking";
            	}
            	else if (signType == 16)
            	{
            		strType = "speed_limit";
            	}
            	else if (signType == 17)
            	{
            		strType = "speed_limit_free";
            	}
            	
            	int dslength = strType.length();
            	int dswidth = dslength * 20;
            	Core.rectangle(mRgba, trafficsignArray[i].tl(), trafficsignArray[i].br(), TRAFFICSIGN_RECT_COLOR, 3);
            	//Point pos = new Point(trafficsignArray[i].tl().x, trafficsignArray[i].br().y);
            	Core.rectangle(mRgba, new Point(trafficsignArray[i].br().x,trafficsignArray[i].br().y+5), new Point(trafficsignArray[i].br().x+dswidth, trafficsignArray[i].br().y-25), WHITE_COLOR, -1);
            	Core.putText(mRgba, strType, trafficsignArray[i].br(), 3, 1.0, BLACK_COLOR);
            }
        }
        double TotalTime = System.currentTimeMillis() - stime;
        String strTime = Double.toString(TotalTime);
        strTime += "ms";
        Point org = new Point(20,50);
        Core.putText(mRgba, strTime, org, 3, 2, GREEN_COLOR);

        return mRgba;
    }

	@Override
    public boolean onCreateOptionsMenu(Menu menu) {
        Log.i(TAG, "called onCreateOptionsMenu");
        mItemTrafficSign = menu.add("交通标志检测");
        mItemPlate = menu.add("车牌检测");
        mItemLane = menu.add("车道线检测");
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        Log.i(TAG, "called onOptionsItemSelected; selected item: " + item);
        setTitle(item.getTitle());
        if (item == mItemTrafficSign) {
        	enDetectType = DetectType.TRAFFIC_SIGN;
        }
        else if (item == mItemPlate){
        	enDetectType = DetectType.PLATE;
        }
        else if (item == mItemLane){
        	enDetectType = DetectType.LANE;
        	mImgView.setDrawLineEnabled(true);
        	mImgView.clearLineInfo();
        	mImgView.setDrawLineFinished(false);
        	mImgView.setVisibility(View.VISIBLE);
        	
        	return true;
        }
        mImgView.setDrawLineEnabled(false);
        mImgView.setDrawLineFinished(false);
        /*if (mImgView != null) {
            BitmapDrawable bd = (BitmapDrawable) mImgView.getDrawable();
            if (bd != null) {
                Bitmap bitmap = bd.getBitmap();
                if (bitmap != null && !bitmap.isRecycled()) {
                    bitmap.recycle();
                    bitmap = null;
                }
            }
            bd = null;
        }*/
        mImgView.setVisibility(View.INVISIBLE);
        return true;
    }

    private Bitmap bmp;
    Handler mHandler = new Handler()  
	{  
    	public void handleMessage(Message msg)  
    	{  
    	    if(msg.what == 1)  
    	    {  
    	    	mImgView.setImageBitmap(bmp);
    	    }
    	}
	};
    public void paintLane(ArrayList<CubicPoly> cubicPolies, Mat rgba)
    {
    	bmp= Bitmap.createBitmap(rgba.cols(), rgba.rows(), Config.ARGB_8888);
		Utils.matToBitmap(rgba, bmp);
    	int numLanes = cubicPolies.size();
    	for(int i=0; i<numLanes; i++) {
    		int ymin = min(cubicPolies.get(i).getStartPoint().getY(), cubicPolies.get(i).getEndPoint().getY());
    		int ymax = max(cubicPolies.get(i).getStartPoint().getY(), cubicPolies.get(i).getEndPoint().getY());
    		for(int y=ymin; y<=ymax; y++) {
    			float p0 = cubicPolies.get(i).getP0();
    			float p1 = cubicPolies.get(i).getP1();
    			float p2 = cubicPolies.get(i).getP2();
    			float p3 = cubicPolies.get(i).getP3();
    		//	int x = (int) ( p0 + p1*y + p2*y*y + p3*y*y*y );
    			int x = (int) ( p0 + y*(p1 + y*(p2 + p3*y) ) );
    			for(int j=0; j<4; j++) {
    				bmp.setPixel(x-j>0?(x-j>=bmp.getWidth()?bmp.getWidth()-1:x-j):1, y>=bmp.getHeight()?bmp.getHeight()-1:(y<0?1:y), Color.RED);
    				bmp.setPixel(x+j<bmp.getWidth()?(x+j>0?x+j:1):bmp.getWidth()-1, y>=bmp.getHeight()?bmp.getHeight()-1:(y<0?1:y), Color.RED);
    			}
    		}
    	}
    	
    	mHandler.sendEmptyMessage(1);  
    }
    
    public int max(int x, int y)
    {
    	return x>y?x:y;
    }
    public int min(int x, int y)
    {
    	return x<y?x:y;
    }
}
