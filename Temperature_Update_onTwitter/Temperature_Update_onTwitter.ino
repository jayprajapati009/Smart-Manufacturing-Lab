// This code is to update the temperature data recieved from the DHT11 sensor to the Twitter Handle.

#include <SoftwareSerial.h>
#include <stdlib.h>
#include <String.h>

String reset  = "AT+RST";
String twitterAPIKey = "ENTER YOUR API KEY HERE";
String tweetURI = "/apps/thingtweet/1/statuses/update";
String thingSpeak = "api.thingspeak.com";
String urlEncoded = "application/x-www-form-urlencoded";
String cmd;

// Set the pin connected to the Data Pin of the DHT11 Sensor
int sensorPin = 0;

SoftwareSerial ser(2, 3); // RX, TX

void setup() {
  Serial.begin(9600);
  ser.begin(9600);
  ser.println(reset);
}

void loop() {
  // Reading the value from the DHT11 Temperature sensor
  int reading = analogRead(sensorPin);

  float voltage = reading * 5.0;
  voltage /= 1024.0;

  //Serial.print(voltage);
  //Serial.println(" volts");

  float temperatureC = (voltage - 0.5) * 100 ;
  Serial.print(temperatureC);
  Serial.println(" degrees C");

  if (temperatureC > 29) {

    //Creating a POST request header structure
    String payload = postRequest(tweetURI, thingSpeak, urlEncoded, tempAlertTweet(temperatureC));

    //TCP Connect
    tcp_connect(cmd);

    // Send DATA Length
    if (postStatus(cmd, payload)) {
      // Setting up a delay of 30 seconds between updates
      delay(30000);
    }
    else {
      ser.println("AT+CIPCLOSE");
      // alert user
      Serial.println("AT+CIPCLOSE");
    }
  }
  delay(5000);

}

//TCP connection
bool tcp_connect(String tcp_connect_cmd) {
  tcp_connect_cmd = "AT+CIPSTART=\"TCP\",\"";
  tcp_connect_cmd += "184.106.153.149"; // api.thingspeak.com
  tcp_connect_cmd += "\",80";
  ser.println(tcp_connect_cmd);
  Serial.println(tcp_connect_cmd);
  //check for connection error
  if (ser.find("Error") || ser.find("closed")) {
    Serial.println("AT+CIPSTART error");
    return;
  }
}

String postRequest(String url, String host, String content_type, String message) {
  String postString = "POST " + url + " HTTP/1.1\r\n" +
                      "Host: " + host + "\r\n" +
                      "Content-Type: " + content_type + "\r\n" +
                      "Connection: " + "close\r\n" +
                      "Content-Length: " + message.length() + "\r\n" +
                      "Cache-Control: " + "no-cache\r\n" +
                      + "\r\n" + message;
  return postString;
}

//tweet message + api key + temperature
String tempAlertTweet(float temp) {
  //tweet message
  String tempAlert = "api_key=" + twitterAPIKey +
                     "&status= Temperature = " +
                     temp + " (C) ";
  return tempAlert;
}

bool postStatus(String command, String message) {
  command = "AT+CIPSEND=";
  command += String(message.length());
  Serial.println(command);
  ser.println(command);
  if (ser.find(">")) {
    ser.print(message);
    Serial.print(message);
    return;
  }
}
