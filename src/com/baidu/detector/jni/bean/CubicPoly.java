package com.baidu.detector.jni.bean;

public class CubicPoly {
	float p0, p1, p2, p3;
	Point startPoint, endPoint;
	public float getP0() {
		return p0;
	}
	public void setP0(float po) {
		this.p0 = po;
	}
	public float getP1() {
		return p1;
	}
	public void setP1(float p1) {
		this.p1 = p1;
	}
	public float getP2() {
		return p2;
	}
	public void setP2(float p2) {
		this.p2 = p2;
	}
	public float getP3() {
		return p3;
	}
	public void setP3(float p3) {
		this.p3 = p3;
	}
	public Point getStartPoint() {
		return startPoint;
	}
	public void setStartPoint(Point startPoint) {
		this.startPoint = startPoint;
	}
	public Point getEndPoint() {
		return endPoint;
	}
	public void setEndPoint(Point endPoint) {
		this.endPoint = endPoint;
	}
	
}
