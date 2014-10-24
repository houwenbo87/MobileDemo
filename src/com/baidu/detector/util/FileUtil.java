package com.baidu.detector.util;

import java.io.File;

import android.os.Environment;

public class FileUtil {
	public static String getSDCardPath()
	{
		File sdDir = null;
		boolean sdCardExist = Environment.getExternalStorageState()
		           .equals(android.os.Environment.MEDIA_MOUNTED); //判断sd卡是否存在
		if (sdCardExist)
		{
			sdDir = Environment.getExternalStorageDirectory();//获取跟目录
		}
		else {
			return null;
		}
		return sdDir.toString();
	}
}
