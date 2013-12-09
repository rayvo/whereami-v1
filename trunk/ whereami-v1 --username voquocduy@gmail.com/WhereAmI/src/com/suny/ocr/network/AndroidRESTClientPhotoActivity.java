package com.suny.ocr.network;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

import org.apache.http.HttpResponse;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.params.BasicHttpParams;
import org.apache.http.params.HttpConnectionParams;
import org.apache.http.params.HttpParams;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;

import com.suny.ocr.R;


public class AndroidRESTClientPhotoActivity extends Activity {

	private static final String SERVICE_URL = "http://223.194.199.18:80/whereami-server/webapi/location";
	 
    private static final String TAG = "AndroidRESTClientActivity";
    
     
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.ocr_result);
        
        Intent intent = getIntent();
        String lat = intent.getStringExtra("LAT");
        String lon = intent.getStringExtra("LON");
        String query = intent.getStringExtra("QUERY");
        
        
        query="SUNY Korea";
        
       /* lat = "37.592075";
		lon = "126.683438";
		query = "신토오리" + "&" + "커피베이";*/
		
		getLocation(lat, lon, query);
    }
    
    public void doOCR(View view) {
    	
    }

    public void getLocation(String lat, String lon, String query) {
    	query = query.replace(" ", "%20");
    	String url = SERVICE_URL + "/" + lat + "/" + lon + "/" + query;    	   	
    	String rawResult = "";
        try {
            HttpParams httpParameters = new BasicHttpParams();
            int timeoutConnection = 5000;
            HttpConnectionParams.setConnectionTimeout(httpParameters, timeoutConnection);
            int timeoutSocket = 5000;
            HttpConnectionParams.setSoTimeout(httpParameters, timeoutSocket);
            DefaultHttpClient httpClient = new DefaultHttpClient(httpParameters);
            HttpGet getRequest = new HttpGet(url);
            getRequest.addHeader("accept", "text/plain");
            
            HttpResponse response = httpClient.execute(getRequest);
            
            rawResult = getResult(response).toString();
            httpClient.getConnectionManager().shutdown();
            
        } catch (Exception e) {
            System.out.println(e.getMessage());
        } 
        String[] result = rawResult.split(",");
        int flag = (int) Integer.parseInt(result[0]);
        if (flag == 1) {
        	Intent returnIntent = new Intent();        	
    		returnIntent.putExtra("LAT", result[1]);
    		returnIntent.putExtra("LON", result[2]);
    		setResult(RESULT_OK, returnIntent);
        } else {
        	Intent returnIntent = new Intent();
    		setResult(RESULT_CANCELED, returnIntent);
        }
        finish();         
	}
    
    private StringBuilder getResult(HttpResponse response) throws IllegalStateException, IOException {
        StringBuilder result = new StringBuilder();
        BufferedReader br = new BufferedReader(new InputStreamReader((response.getEntity().getContent())), 1024);
        String output;
        while ((output = br.readLine()) != null) 
            result.append(output);

        return result;      
  }    
}
