package com.suny.ocr.network;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.HttpURLConnection;
import java.net.URL;
import java.net.URLConnection;
import java.util.List;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.os.AsyncTask;
import android.os.Bundle;
import android.telephony.NeighboringCellInfo;
import android.telephony.TelephonyManager;
import android.telephony.cdma.CdmaCellLocation;
import android.telephony.gsm.GsmCellLocation;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.TextView;

import com.suny.ocr.R;

public class ServiceProviderActivity extends Activity {

	
	private static final String LOG_TAG = "WHEREAMI-MainActivity";
	private static final String GOOGLE_URL = "http://www.google.com/glm/mmap";
	
	public static double lat = 0;
	public static double lon = 0;

	int cid = 0;
	int lac = 0;
	int mcc = 450; //Korea
	int mnc = 5;   //SK Telecom

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		//setContentView(R.layout.activity_main);
		
		TelephonyManager tm = (TelephonyManager) getSystemService(Context.TELEPHONY_SERVICE);

		GsmCellLocation cell = null;

		// Get the phone type
		String strphoneType = "";

		int phoneType = tm.getPhoneType();

		switch (phoneType) {
		case (TelephonyManager.PHONE_TYPE_CDMA):
			strphoneType = "CDMA";
			lat = ((CdmaCellLocation) tm.getCellLocation())
					.getBaseStationLatitude();
			lon = ((CdmaCellLocation) tm.getCellLocation())
					.getBaseStationLongitude();
			break;
		case (TelephonyManager.PHONE_TYPE_GSM):
			strphoneType = "GSM";
			break;
		case (TelephonyManager.PHONE_TYPE_NONE):
			strphoneType = "NONE";
			break;
		}
		
		if (strphoneType.equals("GSM")) {
			cell = (GsmCellLocation) tm.getCellLocation();
			cid = cell.getCid();
			lac = cell.getLac();

			String networkOperator = tm.getNetworkOperator();
			String str_mcc = "";
			String str_mnc = "";
			
			if (networkOperator != null) {
				str_mcc = networkOperator.substring(0, 3);
				str_mnc = networkOperator.substring(3);
				Log.d(LOG_TAG, "mcc: " + str_mcc);
				Log.d(LOG_TAG, "mnc: " + str_mnc);
			}

			String str_cid = String.valueOf(cid);
			String str_lac = String.valueOf(lac);
			// String str_mcc = String.valueOf(cell.get);

			/*
			 * str_cid = "29021"; str_lac = "328";
			 */

			new NetworkTask().execute(new String[] { str_cid, str_lac, str_mcc, str_mnc });

		} 

	}

	private void WriteData(OutputStream out, int cid, int lac, int mcc, int mnc)
			throws IOException {

		DataOutputStream dataOutputStream = new DataOutputStream(out);
		dataOutputStream.writeShort(21);
		dataOutputStream.writeLong(0);
		dataOutputStream.writeUTF("en");
		dataOutputStream.writeUTF("Android");
		dataOutputStream.writeUTF("1.0");
		dataOutputStream.writeUTF("Web");
		dataOutputStream.writeByte(27);
		dataOutputStream.writeInt(0);
		dataOutputStream.writeInt(0);
		// GSM uses 4 digits while UTMS used 6 digits (HEX)
		if (cid > 65536) {
			dataOutputStream.writeInt(5);
		} else {
			dataOutputStream.writeInt(3);
		}
		dataOutputStream.writeUTF("");

		dataOutputStream.writeInt(cid);
		dataOutputStream.writeInt(lac);

		dataOutputStream.writeInt(mnc);
		dataOutputStream.writeInt(mcc);
		dataOutputStream.writeInt(0);
		dataOutputStream.writeInt(0);
		dataOutputStream.flush();
	}

	// AsyncTask to call web service
	private class NetworkTask extends AsyncTask<String, Integer, String> {
		@Override
		protected String doInBackground(String... params) {			
			// Create a connection to some 'hidden' Google-API
			try {
				URL url = new URL(GOOGLE_URL);
				URLConnection conn = url.openConnection();
				HttpURLConnection httpConn = (HttpURLConnection) conn;
				httpConn.setRequestMethod("POST");
				httpConn.setDoOutput(true);
				httpConn.setDoInput(true);
				httpConn.connect();

				cid = Integer.parseInt(params[0]);
				lac = Integer.parseInt(params[1]);
				mcc = Integer.parseInt(params[2]);
				mnc = Integer.parseInt(params[3]);
				
				Log.d(LOG_TAG, "cid = " + cid + ", lac = " + lac);

				OutputStream outputStream = httpConn.getOutputStream();
				
				WriteData(outputStream, cid, lac, mcc, mnc);

				InputStream inputStream = httpConn.getInputStream();
				DataInputStream dataInputStream = new DataInputStream(
						inputStream);

				// ---interpret the response obtained---
				dataInputStream.readShort();
				dataInputStream.readByte();
				int code = dataInputStream.readInt();

				Log.d(LOG_TAG, "***code:" + code);
				if (code == 0) {
					Log.d(LOG_TAG, "***subGPS true");
					lat = (double) dataInputStream.readInt() / 1000000D;
					lon = (double) dataInputStream.readInt() / 1000000D;

					Log.d(LOG_TAG, "***lat:" + lat + "  log" + lon);

					dataInputStream.readInt();
					dataInputStream.readInt();
					dataInputStream.readUTF();

					runOnUiThread(new Runnable() {
						@Override
						public void run() {
							Intent returnIntent = new Intent();
							returnIntent.putExtra("LAT", String.valueOf(lat));
							returnIntent.putExtra("LON", String.valueOf(lon));
							setResult(RESULT_OK, returnIntent);
						}
					});

				} else {
					Log.d(LOG_TAG, "***subGPS false");
					Intent returnIntent = new Intent();
					setResult(RESULT_CANCELED, returnIntent);
				}
				finish();
			} catch (Exception e) {
				e.printStackTrace();
			}
			return null;
		}
	}

}
