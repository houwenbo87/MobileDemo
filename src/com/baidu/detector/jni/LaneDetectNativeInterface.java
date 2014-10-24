package com.baidu.detector.jni;

import java.util.ArrayList;

import org.opencv.core.Mat;

import android.graphics.PointF;

import com.baidu.detector.jni.bean.CubicPoly;
import com.baidu.detector.jni.bean.Spline;

public class LaneDetectNativeInterface 
{
	public static void getLane(Mat imgGray, ArrayList<Spline> splines, ArrayList<Float> spline_scores, ArrayList<CubicPoly> cubicPolies)
	{
		nativeGetLane(imgGray.getNativeObjAddr(), splines, spline_scores, cubicPolies);
	}
	private native static void nativeGetLane( long imgGray, 
												ArrayList<Spline> splines,  
													ArrayList<Float> spline_scores,
														ArrayList<CubicPoly> cubicPolies);

	public static void saveAdjustedPoints(PointF[] adjustedPoints, int cols, int rows)
	{
		float x1;
		float y1;
		float x2;
		float y2;
		float x3;
		float y3;
	    float x4; 
	    float y4;
	    
		if(!( (adjustedPoints == null) || (adjustedPoints.length != 4) ) ) {
			x1 = adjustedPoints[0].x;
			y1 = adjustedPoints[0].y;
			x2 = adjustedPoints[1].x;
			y2 = adjustedPoints[1].y;
			x3 = adjustedPoints[2].x;
			y3 = adjustedPoints[2].y;
		    x4 = adjustedPoints[3].x;
		    y4 = adjustedPoints[3].y;
			
			nativeSaveAdjustedPoints(x1, y1,
					 x2, y2,
					  x3, y3,
					   x4, y4,
					    cols, rows);
		}
	}

	private native static void nativeSaveAdjustedPoints(float x1, float y1,
											 float x2, float y2,
											  float x3, float y3,
											   float x4, float y4,
											    int cols, int rows);
}
