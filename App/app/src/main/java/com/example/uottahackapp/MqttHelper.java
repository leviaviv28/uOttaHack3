package com.example.uottahackapp;

import android.content.Context;
import android.util.Log;

import org.eclipse.paho.android.service.MqttAndroidClient;
import org.eclipse.paho.client.mqttv3.*;
import org.eclipse.paho.client.mqttv3.IMqttActionListener;
import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.IMqttToken;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttCallbackExtended;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;

import java.nio.charset.Charset;
import java.nio.charset.StandardCharsets;

public class MqttHelper {
    public MqttAndroidClient mqttAndroidClient;
//
//    final String serverUri = "tcp://mr2hd0llj3vwjx.messaging.solace.cloud:1883";
//
//    final String clientId = MqttClient.generateClientId();
//    final String fetchTrackingListen = "fetch_tracking/listen";
//    final String trackingUpdate = "update_tracking/#";
//
//    final String username = "solace-cloud-client";
//    final String password = "vvngpe3cn2ss72a7425hgf1l7f";

    final String serverUri = "tcp://mr2hd0llj3vwp7.messaging.solace.cloud:1883";

    final String clientId = MqttClient.generateClientId();
    final String topic = "sensor/+";

    final String username = "solace-cloud-client";
    final String password = "86g9ulg16tconofedqq61h0i1p";


    public MqttHelper(Context context) {

        mqttAndroidClient = new MqttAndroidClient(context,serverUri,clientId);
        mqttAndroidClient.setCallback(new MqttCallbackExtended() {

            @Override
            public void connectComplete(boolean b, String s) {
                Log.w("mqtt",s);
            }

            @Override
            public void connectionLost(Throwable cause) {

            }

            @Override
            public void messageArrived(String topic, MqttMessage message) throws Exception {

                Log.w("mqtt", message.toString());
            }

            @Override
            public void deliveryComplete(IMqttDeliveryToken token) {
                Log.w("mqtt",token.toString());
            }
        });


        connect();

//        MqttMessage mqttMessage;
//        String msg = "Hello world";
//        mqttMessage = new MqttMessage();
//
//
//        mqttMessage.setPayload(msg.getBytes(StandardCharsets.UTF_8));
//        try {
//            mqttAndroidClient.publish(fetchTrackingListen,mqttMessage);
//        } catch (MqttException e) {
//            e.printStackTrace();
//        }

    }

    public void setCallback(MqttCallbackExtended callback) {
        mqttAndroidClient.setCallback(callback);
    }


    private void connect()  {
        MqttConnectOptions mqttConnectOptions = new MqttConnectOptions();
        mqttConnectOptions.isAutomaticReconnect();
        mqttConnectOptions.isCleanSession();
        mqttConnectOptions.setUserName(username);
        mqttConnectOptions.setPassword(password.toCharArray());

        try {
            mqttAndroidClient.connect(mqttConnectOptions, null, new IMqttActionListener() {
                @Override
                public void onSuccess(IMqttToken asyncActionToken) {
                    DisconnectedBufferOptions disconnectedBufferOptions = new DisconnectedBufferOptions();

                    disconnectedBufferOptions.setBufferEnabled(true);
                    disconnectedBufferOptions.setBufferSize(100);
                    disconnectedBufferOptions.setPersistBuffer(false);
                    disconnectedBufferOptions.setDeleteOldestMessages(false);

                    mqttAndroidClient.setBufferOpts(disconnectedBufferOptions);
                    subscribeToTopic();
                }

                @Override
                public void onFailure(IMqttToken asyncActionToken, Throwable exception) {
                    Log.w("Mqtt", "Failed to connect: " + serverUri + exception.toString());
                }
            });
        } catch (MqttException ex) {
            ex.printStackTrace();
        }

//        try {
//            mqttAndroidClient.connect(mqttConnectOptions, null, new IMqttActionListener() {
//                @Override
//                public void onSuccess(IMqttToken asyncActionToken) {
//
//                }
//
//                @Override
//                public void onFailure(IMqttToken asyncActionToken, Throwable exception) {
//
//                }
//            });
//        } catch (Exception ex){
//
//        }

    }

    private void subscribeToTopic() {
        try {
            mqttAndroidClient.subscribe(topic, 0, null, new IMqttActionListener() {
                @Override
                public void onSuccess(IMqttToken asyncActionToken) {
                    Log.w("Mqtt", "subscribed");
                }

                @Override
                public void onFailure(IMqttToken asyncActionToken, Throwable exception) {
                    Log.w("Mqtt", "Subscribed fail");
                }

            });

//            mqttAndroidClient.subscribe("update_tracking/#", 0, null, new IMqttActionListener() {
//                @Override
//                public void onSuccess(IMqttToken asyncActionToken) {
//                    Log.w("Mqtt", "subscribed");
//                }
//
//                @Override
//                public void onFailure(IMqttToken asyncActionToken, Throwable exception) {
//                    Log.w("Mqtt", "Subscribed fail");
//                }
//
//            });

        } catch (MqttException ex) {
            System.err.println("Error while subscribing");
            ex.printStackTrace();
        }
    }

}
