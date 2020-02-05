package com.example.arduinoweb;

import android.app.Activity;
import android.os.Build;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.Window;
import android.webkit.WebChromeClient;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.webkit.WebViewClient;

public class GreekNewsActivity extends Activity {
	
	WebView webview;
    
    public void onCreate(Bundle savedInstanceState) {
    	super.onCreate(savedInstanceState);
        this.getWindow().requestFeature(Window.FEATURE_PROGRESS);
        //setContentView(R.layout.main);    
        getWindow().setFeatureInt(	Window.FEATURE_PROGRESS, Window.PROGRESS_VISIBILITY_ON); 
        
        webview = (WebView) findViewById(R.id.web_engine); 
        webview.setWebViewClient(new HelloWebViewClient());
        webview.getSettings().setJavaScriptEnabled(true); 
        //webview.getSettings().setPluginsEnabled(true);  //depriciated
        if (Integer.parseInt(Build.VERSION.SDK) >= Build.VERSION_CODES.FROYO)
        	webview.getSettings().setPluginState(WebSettings.PluginState.ON); 
        webview.getSettings().setAllowFileAccess(true);        
        webview.getSettings().setBuiltInZoomControls(true); //multitouch an ipostirizetai        
        webview.setScrollBarStyle(WebView.SCROLLBARS_OUTSIDE_OVERLAY);
        webview.loadUrl("http://192.168.10.250"); //o server tou Arduino
        
        //webview.loadUrl("http://google.com"); //gia debug   
      
       final Activity MyActivity = this;
	   webview.setWebChromeClient(new WebChromeClient() {
       public void onProgressChanged(WebView view, int progress)   
       {
       	//Na svisw tin mpara afou fortwsei to url
       	MyActivity.setTitle("Φόρτωση...");
       	MyActivity.setProgress(progress * 100); //svisimo

       	// Kai na emfanisw to app name
           if(progress == 100)
           	MyActivity.setTitle(R.string.app_name);
         }
       });
    }
    
  //Na krataw ta clicks within the app
    private class HelloWebViewClient extends WebViewClient {
        @Override
        public boolean shouldOverrideUrlLoading(WebView view, String url) {        	 
                return false;            
        }    
    }
    
    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        if ((keyCode == KeyEvent.KEYCODE_BACK) && webview.canGoBack()) {
            webview.goBack();
            return true;
        }
        return super.onKeyDown(keyCode, event);
    }    
    
    

}
