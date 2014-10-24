package com.baidu.detector.view;

import java.util.ArrayList;


import android.annotation.SuppressLint;
import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.PointF;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.widget.ImageView;

public class CustomerImageView extends ImageView {
	enum LastDrawWho {INVALID, A, B}
	enum TouchedPoint {INVALID, A1, A2, B1, B2, LINE_A, LINE_B}
	
	private LastDrawWho lastDrawWho = LastDrawWho.INVALID;
	private Paint paint;
    private ArrayList<PointF> graphicsA = new ArrayList<PointF>();
    private ArrayList<PointF> graphicsB = new ArrayList<PointF>();
    private Paint  lPaint;
    private Path   mPathA;
    private Path   mPathB;
    private float mX, mY;
    private boolean isDrawLineEnabled = false;
    private boolean isDrawLineFinished = false;
    private boolean hasLinePositionChanged = false;
    private PointF[] adjustedPoints = null;
    private int radius = 8;
    private float mLastAX = -1, mLastAY = -1;
    private float mLastBX = -1, mLastBY = -1;

    public CustomerImageView(Context context) {
        super(context);
        init();
    }
    public CustomerImageView(Context context, AttributeSet attrs) {
        super(context, attrs);
        init();
    }
    public CustomerImageView(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
        init();
    }
    
    public void init()
    {
    	paint = new Paint(Paint.ANTI_ALIAS_FLAG);
        paint.setColor(Color.RED);
        paint.setStrokeJoin(Paint.Join.ROUND);
        paint.setStrokeCap(Paint.Cap.ROUND);
        paint.setStrokeWidth(65);
         
        lPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
        lPaint.setColor(Color.BLACK);
        lPaint.setStyle(Paint.Style.FILL_AND_STROKE);//实心
        lPaint.setStrokeJoin(Paint.Join.ROUND);
        lPaint.setStrokeCap(Paint.Cap.ROUND);
        lPaint.setStrokeWidth(5);
         
        mPathA = new Path();
        mPathB = new Path();
    }
    
    
    public void clearLineInfo()
    {
    	graphicsA.clear();
    	graphicsB.clear();
    	mPathA.reset();
    	mPathB.reset();
    	lastDrawWho = LastDrawWho.INVALID;
    	invalidate();
    }
    
    @SuppressLint("ClickableViewAccessibility")
	@Override
    public boolean onTouchEvent(MotionEvent event) {
    	if(true == isDrawLineFinished) {
    		float x = event.getX(); //getAdjustedPoints();invalidate();
	        float y = event.getY();
	        if(x<=0)
	        	x=0;
	        if(y<=0)
	        	y=0;
	        if(x>=getWidth())
	        	x=getWidth();
	        if(y>=getHeight())
	        	y=getHeight();
	        
	        TouchedPoint touchedPoint = locateTouchedEndPoint(x, y);
	        if(touchedPoint == TouchedPoint.INVALID) {
	        	return true;
	        }
	        
	        hasLinePositionChanged = true;
	        switch (event.getAction()) {
	        case MotionEvent.ACTION_DOWN:
	        	mLastAX = -1;
	        	mLastAY = -1;
	        	mLastBX = -1;
	        	mLastBY = -1;
	            invalidate();
	            break;
	        case MotionEvent.ACTION_MOVE:
	            touch_move(x, y, touchedPoint);
	            invalidate();
	            break;
	        case MotionEvent.ACTION_UP:
	        	mX = x;
	        	mY = y;
	        	mLastBX = -1;
				mLastBY = -1;
				mLastAX = -1;
				mLastAY = -1;
	            invalidate();
	            break;
	        }
    	}
    	else if(true == this.isDrawLineEnabled) {
	        float x = event.getX();
	        float y = event.getY();
	        
	        if(x<=0)
	        	x=0;
	        if(y<=0)
	        	y=0;
	        if(x>=getWidth())
	        	x=getWidth();
	        if(y>=getHeight())
	        	y=getHeight();
	        
	        switch (event.getAction()) {
	        case MotionEvent.ACTION_DOWN:
	            touch_start(x, y);
	            invalidate();
	            break;
	        case MotionEvent.ACTION_MOVE:
	            invalidate();
	            break;
	        case MotionEvent.ACTION_UP:
	        	mX = x;
	        	mY = y;
	            touch_up();
	            invalidate();
	            break;
	        }
    	}

        return true;
    }
     
    private void touch_start(float x, float y) {
    	if(this.lastDrawWho == LastDrawWho.INVALID) {
    		mPathA.reset();
    		mPathB.reset();
            graphicsA.clear();
            graphicsB.clear();
            mPathA.moveTo(x, y);
            graphicsA.add(new PointF(x, y));
    	}
    	else if(this.lastDrawWho == LastDrawWho.A) {
    		mPathB.reset();
    		graphicsB.clear();
    		mPathB.moveTo(x, y);    		
            graphicsB.add(new PointF(x, y));
    	}
    	else {
    		/*mPathA.reset();
    		graphicsA.clear();
            mPathA.moveTo(x, y);
            graphicsA.add(new PointF(x, y));*/
    		this.isDrawLineFinished = true;
    	}
        mX = x;
        mY = y;
    }
    private void touch_move(float x, float y, TouchedPoint touchedEndpoint) {
    	float deltaX = 0;
		float deltaY = 0;
    	switch (touchedEndpoint) {
		case A1:
			mPathA.reset();
			mPathA.moveTo(graphicsA.get(1).x, graphicsA.get(1).y);
			graphicsA.get(0).x = x;
			graphicsA.get(0).y = y;
			mPathA.lineTo(x, y);
			break;
		case A2:
			mPathA.reset();
			mPathA.moveTo(graphicsA.get(0).x, graphicsA.get(0).y);
			graphicsA.get(1).x = x;
			graphicsA.get(1).y = y;
			mPathA.lineTo(x, y);
			break;
		case B1:
			mPathB.reset();
			mPathB.moveTo(graphicsB.get(1).x, graphicsB.get(1).y);
			graphicsB.get(0).x = x;
			graphicsB.get(0).y = y;
			mPathB.lineTo(x, y);
			break;
		case B2:
			mPathB.reset();
			mPathB.moveTo(graphicsB.get(0).x, graphicsB.get(0).y);
			graphicsB.get(1).x = x;
			graphicsB.get(1).y = y;
			mPathB.lineTo(x, y);
			break;
		case LINE_A:
			if(mLastAX != -1) {
				deltaX = x - mLastAX;
				deltaY = y - mLastAY;
			}
			mLastBX = -1;
			mLastBY = -1;
			mLastAX = x;
			mLastAY = y;	
			mPathA.reset();
			graphicsA.get(0).x = graphicsA.get(0).x + deltaX;
			graphicsA.get(0).y = graphicsA.get(0).y + deltaY;
			graphicsA.get(1).x = graphicsA.get(1).x + deltaX;
			graphicsA.get(1).y = graphicsA.get(1).y + deltaY;
			mPathA.moveTo(graphicsA.get(0).x, graphicsA.get(0).y);
			mPathA.lineTo(graphicsA.get(1).x, graphicsA.get(1).y);
			adjustXY();
			return;
		case LINE_B:
			if(mLastBX != -1) {
				deltaX = x - mLastBX;
				deltaY = y - mLastBY;
			}
			mLastBX = x;
			mLastBY = y;
			mLastAX = -1;
			mLastAY = -1;
			mPathB.reset();
			graphicsB.get(0).x = graphicsB.get(0).x + deltaX;
			graphicsB.get(0).y = graphicsB.get(0).y + deltaY;
			graphicsB.get(1).x = graphicsB.get(1).x + deltaX;
			graphicsB.get(1).y = graphicsB.get(1).y + deltaY;
			mPathB.moveTo(graphicsB.get(0).x, graphicsB.get(0).y);
			mPathB.lineTo(graphicsB.get(1).x, graphicsB.get(1).y);
			adjustXY();
			return;
		default:
			break;
		}
    	mLastBX = -1;
		mLastBY = -1;
		mLastAX = -1;
		mLastAY = -1;
    }
    private void touch_up() {
    	if(this.lastDrawWho == LastDrawWho.A) {
    		mPathB.lineTo(mX, mY);
    		graphicsB.add(new PointF(mX, mY));
    		this.lastDrawWho = LastDrawWho.B;
        }
        else if(this.lastDrawWho == LastDrawWho.INVALID) {
        	mPathA.lineTo(mX, mY);
        	graphicsA.add(new PointF(mX, mY));
        	this.lastDrawWho = LastDrawWho.A;
        }
        else if(this.lastDrawWho == LastDrawWho.B) {
        	this.isDrawLineFinished = true;
        	hasLinePositionChanged = true;
        }
    }

    @Override
    protected void onDraw(Canvas canvas) {
     	canvas.drawColor(Color.TRANSPARENT);//清理
    	super.onDraw(canvas);
    	if(true == isDrawLineEnabled) {
	        lPaint.setColor(Color.BLUE);
	        canvas.drawPath(mPathA, lPaint);
	        lPaint.setColor(Color.BLUE);
	        canvas.drawPath(mPathB, lPaint);
	        if(graphicsA.size() == 2) {
	        	canvas.drawCircle(graphicsA.get(0).x, graphicsA.get(0).y, radius, lPaint);
	        	canvas.drawCircle(graphicsA.get(1).x, graphicsA.get(1).y, radius, lPaint);
	        	canvas.drawCircle((graphicsA.get(0).x + graphicsA.get(1).x)/2, 
	        						(graphicsA.get(0).y + graphicsA.get(1).y)/2, radius, lPaint);
	        }
	        if(graphicsB.size() == 2) {
	        	canvas.drawCircle(graphicsB.get(0).x, graphicsB.get(0).y, radius, lPaint);
	        	canvas.drawCircle(graphicsB.get(1).x, graphicsB.get(1).y, radius, lPaint);
	        	canvas.drawCircle((graphicsB.get(0).x + graphicsB.get(1).x)/2, 
						(graphicsB.get(0).y + graphicsB.get(1).y)/2, radius, lPaint);
	        }
    	}
    }
	public boolean isDrawLineEnabled() {
		return isDrawLineEnabled;
	}
	public void setDrawLineEnabled(boolean isDrawLineEnabled) {
		this.isDrawLineEnabled = isDrawLineEnabled;
	}
	public boolean isDrawLineFinished() {
		return isDrawLineFinished;
	}
	public void setDrawLineFinished(boolean isDrawLineFinished) {
		this.isDrawLineFinished = isDrawLineFinished;
	}

	private TouchedPoint locateTouchedEndPoint(float x, float y)
	{
		TouchedPoint touchedEndpoint = TouchedPoint.INVALID;
		float a1x = graphicsA.get(0).x;
		float a1y = graphicsA.get(0).y;
		float a2x = graphicsA.get(1).x;
		float a2y = graphicsA.get(1).y;
		float b1x = graphicsB.get(0).x;
		float b1y = graphicsB.get(0).y;
		float b2x = graphicsB.get(1).x;
		float b2y = graphicsB.get(1).y;
		float radiusSquare = radius*radius;
		/*float slopeA = Integer.MAX_VALUE, slopeB = Integer.MAX_VALUE;
		float interceptA = Integer.MAX_VALUE, interceptB = Integer.MAX_VALUE;
		
		if(a1x != a2x) {
			slopeA = (a1y-a2y)/(a1x-a2x);
			interceptA = a1y - slopeA*a1x;
		}
		if(b1x != b2x) {
			slopeB = (b1y-b2y)/(b1x-b2x);
			interceptB = b1y - slopeB*b1x;
		}*/
				
		if( Math.sqrt( (x-a1x)*(x-a1x) + (y-a1y)*(y-a1y) ) <= radiusSquare ) {
			touchedEndpoint = TouchedPoint.A1;
		}
		else if( Math.sqrt( (x-a2x)*(x-a2x) + (y-a2y)*(y-a2y) ) <= radiusSquare ) {
			touchedEndpoint = TouchedPoint.A2;
		}
		else if( Math.sqrt( (x-b1x)*(x-b1x) + (y-b1y)*(y-b1y) ) <= radiusSquare ) {
			touchedEndpoint = TouchedPoint.B1;
		}
		else if( Math.sqrt( (x-b2x)*(x-b2x) + (y-b2y)*(y-b2y) ) <= radiusSquare ) {
			touchedEndpoint = TouchedPoint.B2;
		}
		/*else if( ( Math.abs(x - a1x) <= radius && Math.abs(x -a2x) <= radius )
				|| ( Math.abs(y - a1y) <= radius && Math.abs(y - a2y) <= radius )
				|| ( Math.abs(y-slopeA*x-interceptA)/Math.sqrt(slopeA*slopeA+1) <= radius ) ) {
			touchedEndpoint = TouchedPoint.LINE_A;
		}
		else if( ( Math.abs(x - b1x) <= radius && Math.abs(x -b2x) <= radius )
				|| ( Math.abs(y - b1y) <= radius && Math.abs(y - b2y) <= radius )
				|| ( Math.abs(y-slopeB*x-interceptB)/Math.sqrt(slopeB*slopeB+1) <= radius ) ) {
			touchedEndpoint = TouchedPoint.LINE_B;
		}*/
		else if(  (x-(a1x+a2x)/2)*(x-(a1x+a2x)/2) + (y-(a1y+a2y)/2)*(y-(a1y+a2y)/2) <= 8*radiusSquare ) {
			touchedEndpoint = TouchedPoint.LINE_A;
		}
		else if( (x-(b1x+b2x)/2)*(x-(b1x+b2x)/2) + (y-(b1y+b2y)/2)*(y-(b1y+b2y)/2) <= 8*radiusSquare ) {
			touchedEndpoint = TouchedPoint.LINE_B;
		}
		/*else if(  Math.abs(x-(a1x+a2x)/2) + (y-(a1y+a2y)/2)*(y-(a1y+a2y)/2) <= 4*radiusSquare ) {
			touchedEndpoint = TouchedPoint.LINE_A;
		}
		else if( (x-(b1x+b2x)/2)*(x-(b1x+b2x)/2) + (y-(b1y+b2y)/2)*(y-(b1y+b2y)/2) <= 4*radiusSquare ) {
			touchedEndpoint = TouchedPoint.LINE_B;
		}*/
		return touchedEndpoint;
	}
	
	public PointF[] getAdjustedPoints(int width, int height)
	{
		if(!( (this.graphicsA.size() == 2) && (this.graphicsB.size() == 2) ) )
			return null;
		
		if(false == hasLinePositionChanged) {
			hasLinePositionChanged = false;
			return null;
		}
		
		ArrayList<PointF> graphicsAcopy = new ArrayList<PointF>();
		ArrayList<PointF> graphicsBcopy = new ArrayList<PointF>();
		graphicsAcopy.add(new PointF(graphicsA.get(0).x, graphicsA.get(0).y));
		graphicsAcopy.add(new PointF(graphicsA.get(1).x, graphicsA.get(1).y));
		graphicsBcopy.add(new PointF(graphicsB.get(0).x, graphicsB.get(0).y));
		graphicsBcopy.add(new PointF(graphicsB.get(1).x, graphicsB.get(1).y));
		
		float a1x = graphicsAcopy.get(0).x;
		float a1y = graphicsAcopy.get(0).y;
		float a2x = graphicsAcopy.get(1).x;
		float a2y = graphicsAcopy.get(1).y;
		float b1x = graphicsBcopy.get(0).x;
		float b1y = graphicsBcopy.get(0).y;
		float b2x = graphicsBcopy.get(1).x;
		float b2y = graphicsBcopy.get(1).y;
		float slopeA = Integer.MAX_VALUE, slopeB = Integer.MAX_VALUE;
		float interceptA = Integer.MAX_VALUE, interceptB = Integer.MAX_VALUE;
		
		if(a1x != a2x) {
			slopeA = (a1y-a2y)/(a1x-a2x);
			interceptA = a1y - slopeA*a1x;
		}
		if(b1x != b2x) {
			slopeB = (b1y-b2y)/(b1x-b2x);
			interceptB = b1y - slopeB*b1x;
		}
		
		float minY = a1y;
		float maxY = a1y;
		
		minY = min(min(min(a1y,a2y), b1y), b2y);
		maxY = max(max(max(a1y,a2y), b1y), b2y);
		
		if(a1y>a2y) {
			if(a1x==a2x) {
				graphicsAcopy.get(0).y = maxY;
				graphicsAcopy.get(1).y = minY;
			}
			else {
				graphicsAcopy.get(0).y = maxY;
				graphicsAcopy.get(1).y = minY;
				graphicsAcopy.get(0).x = (graphicsAcopy.get(0).y-interceptA) / slopeA;
				graphicsAcopy.get(1).x = (graphicsAcopy.get(1).y-interceptA) / slopeA;
			}
		}
		else {
			if(a1x==a2x) {
				graphicsAcopy.get(1).y = maxY;
				graphicsAcopy.get(0).y = minY;
			}
			else {
				graphicsAcopy.get(1).y = maxY;
				graphicsAcopy.get(0).y = minY;
				graphicsAcopy.get(0).x = (graphicsAcopy.get(0).y-interceptA) / slopeA;
				graphicsAcopy.get(1).x = (graphicsAcopy.get(1).y-interceptA) / slopeA;
			}
		}
			
		if(b1y>b2y) {
			if(b1x==b2x) {
				graphicsBcopy.get(0).y = maxY;
				graphicsBcopy.get(1).y = minY;
			}
			else {
				graphicsBcopy.get(0).y = maxY;
				graphicsBcopy.get(1).y = minY;
				graphicsBcopy.get(0).x = (graphicsBcopy.get(0).y-interceptB) / slopeB;
				graphicsBcopy.get(1).x = (graphicsBcopy.get(1).y-interceptB) / slopeB;
			}
		}
		else {
			if(a1x==a2x) {
				graphicsBcopy.get(1).y = maxY;
				graphicsBcopy.get(0).y = minY;
			}
			else {
				graphicsBcopy.get(1).y = maxY;
				graphicsBcopy.get(0).y = minY;
				graphicsBcopy.get(0).x = (graphicsBcopy.get(0).y-interceptB) / slopeB;
				graphicsBcopy.get(1).x = (graphicsBcopy.get(1).y-interceptB) / slopeB;
			}
		}
		
		adjustedPoints = new PointF[4];
		float scaleFactor = getDrawable().getIntrinsicHeight()/(getHeight()*1.0f);
		scaleFactor = getDrawable().getIntrinsicWidth()/(getWidth()*1.0f);
		if(getDrawable().getIntrinsicWidth()==-1 || getDrawable().getIntrinsicHeight() == 1) {
			scaleFactor = width/(getWidth()*1.0f);
			scaleFactor = height/(getHeight()*1.0f);
		}
		
		adjustedPoints[0] = new PointF();
		adjustedPoints[0].x = graphicsAcopy.get(0).x * scaleFactor;
		adjustedPoints[0].y = graphicsAcopy.get(0).y * scaleFactor;
		adjustedPoints[1] = new PointF();
		adjustedPoints[1].x = graphicsAcopy.get(1).x * scaleFactor;
		adjustedPoints[1].y = graphicsAcopy.get(1).y * scaleFactor;
		adjustedPoints[2] = new PointF();
		adjustedPoints[2].x = graphicsBcopy.get(0).x * scaleFactor;
		adjustedPoints[2].y = graphicsBcopy.get(0).y * scaleFactor;
		adjustedPoints[3] = new PointF();
		adjustedPoints[3].x = graphicsBcopy.get(1).x * scaleFactor;
		adjustedPoints[3].y = graphicsBcopy.get(1).y * scaleFactor;
		
		// 2014/8/4 由于四点要满足一定的关系，因此按要求将四点的坐标进行交换
		sortPoints(adjustedPoints, sortIndex.YCoordinate);
		if(adjustedPoints[0].x > adjustedPoints[1].x) {
			PointF tmPointF = adjustedPoints[0];
			adjustedPoints[0] = adjustedPoints[1];
			adjustedPoints[1] = tmPointF;
		}
		if(adjustedPoints[3].x > adjustedPoints[2].x) {
			PointF tmPointF = adjustedPoints[3];
			adjustedPoints[3] = adjustedPoints[2];
			adjustedPoints[2] = tmPointF;
		}
		hasLinePositionChanged = false;
		return adjustedPoints;
	}
	
	 public float max(float x, float y)
	 {
		 return x>y?x:y;
	 }
	 public float min(float x, float y)
	 {
		 return x<y?x:y;
	 }
	 
	 private void adjustXY()
	 {
		 if(graphicsA.get(0).x<=0)
			graphicsA.get(0).x=0;
		 if(graphicsA.get(1).x<=0)
			graphicsA.get(1).x=0;
		 if(graphicsA.get(0).y<=0)
			graphicsA.get(0).y=0;
		 if(graphicsA.get(1).y<=0)
			graphicsA.get(1).y=0;
		 
		if (graphicsA.get(0).x >= getWidth())
			graphicsA.get(0).x = getWidth();
		if (graphicsA.get(1).x >= getWidth())
			graphicsA.get(1).x = getWidth();
		if (graphicsA.get(0).y >= getHeight())
			graphicsA.get(0).y = getHeight();
		if (graphicsA.get(1).y >= getHeight())
			graphicsA.get(1).y = getHeight();

		if (graphicsB.get(0).x <= 0)
			graphicsB.get(0).x = 0;
		if (graphicsB.get(1).x <= 0)
			graphicsB.get(1).x = 0;
		if (graphicsB.get(0).y <= 0)
			graphicsB.get(0).y = 0;
		if (graphicsB.get(1).y <= 0)
			graphicsB.get(1).y = 0;
		
		if (graphicsB.get(0).x >= getWidth())
			graphicsB.get(0).x = getWidth();
		if (graphicsB.get(1).x >= getWidth())
			graphicsB.get(1).x = getWidth();
		if (graphicsB.get(0).y >= getHeight())
			graphicsB.get(0).y = getHeight();
		if (graphicsB.get(1).y >= getHeight())
			graphicsB.get(1).y = getHeight();
	 }
	 
	 enum sortIndex{XCoordinate,YCoordinate}
	 
	 private void sortPoints(PointF[] points, sortIndex index)
	 {
		 if(null==points) {
			 return;
		 }
		 
		 if(index == sortIndex.XCoordinate) {
			 for(int i=0; i<points.length-1; i++) {
				 for(int j=0; j<points.length-i-1; j++) {
					 if(points[j].x < points[j+1].x) {
						 PointF tmPointF = points[j];
						 points[j] = points[j+1];
						 points[j+1] = tmPointF;
					 }
				 }
			 }
		 }
		 else if(index == sortIndex.YCoordinate) {
			 for(int i=0; i<points.length-1; i++) {
				 for(int j=0; j<points.length-i-1; j++) {
					 if(points[j].y < points[j+1].y) {
						 PointF tmPointF = points[j];
						 points[j] = points[j+1];
						 points[j+1] = tmPointF;
					 }
				 }
			 }
		 }
	 }
	
	 public boolean isHasLinePositionChanged() {
		return hasLinePositionChanged;
	}
	public void setHasLinePositionChanged(boolean hasLinePositionChanged) {
		this.hasLinePositionChanged = hasLinePositionChanged;
	}
}
