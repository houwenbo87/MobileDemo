package com.baidu.detector.jni.bean;

public class Spline {
	private int degree;
	private CvPoint2D32f[] points = new CvPoint2D32f[4];
	private int color;
	float score;
	
	
	public int getDegree() {
		return degree;
	}
	public void setDegree(int degree) {
		this.degree = degree;
	}
	public CvPoint2D32f[] getPoints() {
		return points;
	}
	public void setPoints(CvPoint2D32f[] points) {
		this.points = points;
	}
	public int getColor() {
		return color;
	}
	public void setColor(int color) {
		this.color = color;
	}
	public float getScore() {
		return score;
	}
	public void setScore(float score) {
		this.score = score;
	}
	
	
}
