package com.example.uottahackapp;

import androidx.appcompat.app.AppCompatActivity;
import androidx.fragment.app.FragmentActivity;

import android.graphics.Camera;
import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;

import com.google.android.gms.maps.CameraUpdateFactory;
import com.google.android.gms.maps.GoogleMap;
import com.google.android.gms.maps.OnMapReadyCallback;
import com.google.android.gms.maps.SupportMapFragment;
import com.google.android.gms.maps.model.LatLng;
import com.google.android.gms.maps.model.MarkerOptions;
import org.eclipse.paho.android.service.MqttAndroidClient;
import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.MqttCallbackExtended;
import org.eclipse.paho.client.mqttv3.MqttMessage;

import java.util.HashMap;
import java.util.Map;


//FIRST GET THE HELPER FILE
//THEN MAKE THE CONNECTION IN THIS FILE

public class MapsActivity extends AppCompatActivity implements OnMapReadyCallback {

    private GoogleMap mMap;
    MqttHelper mqttHelper;
    TextView dataReceived = null;
    Map<String,String> m;



    /**
     * Commented the next block of declarations for debugging purposes
     */
//    final String SOLACE_MQTT_HOST = "tcp://mr2hd0llj3vwp7.messaging.solace.cloud:1883";
//    final String SOLACE_CLIENT_PASSWORD = "86g9ulg16tconofedqq61h0i1p";
//
//    final int EXIT_TIME = 6;
//    final int KEEP_ALIVE = 60;
//
//    final boolean SOLACE_CLEAN = true;
//    final boolean SOLACE_RECONNECT = true;



    //
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_maps);
        // Obtain the SupportMapFragment and get notified when the map is ready to be used.
        SupportMapFragment mapFragment = (SupportMapFragment) getSupportFragmentManager()
                .findFragmentById(R.id.map);

        dataReceived = findViewById(R.id.textView);

        m = new HashMap<>();

        m.put("p","43.507,-80.4986");
        m.put("pr", "43.4986,-80.504");
        m.put("so", "43.4481,-80.51925");
        m.put("sh", "43.479476,-80.5159");
        m.put("i","43.4762,-80.5250");
        m.put("a", "43.4696,-80.523");



        mapFragment.getMapAsync(this);
        dataReceived =  findViewById(R.id.textView);
        mqttHelper = new MqttHelper(getApplicationContext());


        mqttHelper.setCallback(new MqttCallbackExtended() {
            @Override
            public void connectComplete(boolean reconnect, String serverURI) {

            }

            @Override
            public void connectionLost(Throwable cause) {

            }


            @Override
            public void messageArrived(String topic, MqttMessage message) throws Exception {

            String input = message.toString();
            String parsedInput =  m.get(input.toLowerCase());
            System.out.println("\n------------------------\nINSIDE MESSAGE ARRIVED\n----------------\n");

            double coordOne = Double.parseDouble(parsedInput.split(",")[0]);
            double coordTwo = Double.parseDouble(parsedInput.split(",")[1]);
            setMarker(mMap,coordOne,coordTwo);
            dataReceived.setText("The current coordinates are: "+ +coordOne + " " +coordTwo);


            }

            @Override
            public void deliveryComplete(IMqttDeliveryToken token) {

            }
        });




    }

    /**
     * Manipulates the map once available.
     * This callback is triggered when the map is ready to be used.
     * This is where we can add markers or lines, add listeners or move the camera. In this case,
     * we just add a marker near Sydney, Australia.
     * If Google Play services is not installed on the device, the user will be prompted to install
     * it inside the SupportMapFragment. This method will only be triggered once the user has
     * installed Google Play services and returned to the app.
     */
    @Override
    public void onMapReady(GoogleMap googleMap) {
        mMap = googleMap;

        // Add a marker in Sydney and move the camera
        LatLng marker = new LatLng(45, 90);
        mMap.addMarker(new MarkerOptions().position(marker).title("Marker in Sydney"));
        mMap.moveCamera(CameraUpdateFactory.newLatLng(marker));
    }

    public void setMarker(GoogleMap map,double lat, double lng) {
        mMap = map;

        LatLng marker = new LatLng(lat,lng);
        mMap.clear();
        mMap.addMarker(new MarkerOptions().position(marker).title("Updated marker"));
        mMap.moveCamera(CameraUpdateFactory.newLatLng(marker));
    }
}
