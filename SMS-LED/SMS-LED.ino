/*************************************************************************/
// Please note: 
//              If SIM800C receive "on" by SMS, the LED(pin13) will turn on.
//              And the "off", the LED(pin13)  will turn off.
//              Open your serial monitor,select 19200 baud.
//              When you send "Q",the GSM signal quality will be return.
//              If you send "T",SIM800 will send "A test message!" by SMS.
//              If you send "S",SIM800 will show all message!
//              If you send "D",SIM800 will delete all message!
//              If you send "R",the Serial moniter will get the  current time from SIM80x!
//For Mega2560: 
//              You need to USE PIN 10(RX) and PIN 11(TX) as SoftwareSerial.
//For UNO R3: 
//              You need to USE PIN 7(RX) and PIN 8(TX) as SoftwareSerial.
//
/*************************************************************************/

#include <SoftwareSerial.h>

SoftwareSerial mySerial(10,11); //
#define DEBUG true

String msg = String("");
int SmsContentFlag = 0;
int ledPin = 13;

//String SigQ[20];
char SigQ[20];
int value = 576;

void setup()
{
//  pinMode(ledPin22, OUTPUT);

  pinMode( ledPin, OUTPUT ); 
  digitalWrite( ledPin, LOW ); 
     
  mySerial.begin(19200);    // the GPRS baud rate  
  Serial.begin(19200);    // the GPRS baud rate 
  sendData("AT+CMGF=1",2000,DEBUG);//Because we want to send the SMS in text mode
  delay(100);
  sendData("AT+CMGS=\"+8**********8\"",2000,DEBUG);//send sms message, be careful need to add a country code before the cellphone number
  delay(100);
  sendData("Power on test message!",2000,DEBUG);//the content of the message
  delay(100);
  mySerial.println((char)26);//the ASCII code of the ctrl+z is 26
  delay(100);
}


void loop()
{
  if (Serial.available())     // This statement is never TRUE when receiving SMS
  switch(Serial.read())
  {
    case 'Q':
    GetSignalQuality();
    break;
    case 'T':
    SendTextMessage();
    break;
    case 'S':
    ShowSMS();
    break;
    case 'D':
    DeleteSMS();
    break;
    case 'R':
    ReadTime();
    break;
  }
  else if(mySerial.available())
      {
          char SerialInByte;
          SerialInByte = (unsigned char)mySerial.read();
          Serial.print( SerialInByte );
          if( SerialInByte==13||SerialInByte==14)
          {
            ProcessGprsMsg();
           }
           if( SerialInByte == 10 ){
              // EN: Skip Line feed
           }
           else {
             // EN: store the current character in the message string buffer
             msg += String(SerialInByte);
           }
       }   

}
///GetSignalQuality()
///get the signal quality of GSM model.

void GetSignalQuality(){
  String response = "";
  long int time = millis();    
    Serial.println("Getting the sinal quality...");
    mySerial.println("AT+CSQ"); 
    delay(5);   
    while( (time+1000) > millis()){
      while(mySerial.available()){       
        response += char(mySerial.read());
      }  
    }    
     Serial.print(response); 
     Serial.println("Tips:+CSQ: XX,QQ : It means the Signal Quality poorly when the XX is less than 16!");
}


///SendTextMessage()
///this function is to send a sms message
void SendTextMessage()
{
  //message 1
  sendData("AT+CMGF=1",2000,DEBUG);//Because we want to send the SMS in text mode
  delay(100);
  sendData("AT+CMGS=\"+8**************8\"",2000,DEBUG);//send sms message, be careful need to add a country code before the cellphone number
  delay(100);
  sendData("A test message1!",2000,DEBUG);//the content of the message
  delay(100);
  mySerial.println((char)26);//the ASCII code of the ctrl+z is 26
  delay(5000);

  //message 2
  sendData("AT+CMGS=\"+8**************8\"",2000,DEBUG);//send sms message, be careful need to add a country code before the cellphone number
  delay(100);
  sendData("A test message2!",2000,DEBUG);//the content of the message
  delay(100);
  mySerial.println((char)26);//the ASCII code of the ctrl+z is 26
  delay(5000);

  //message 3
  sendData("AT+CMGS=\"+8**************8\"",2000,DEBUG);//send sms message, be careful need to add a country code before the cellphone number
  delay(100);
  sendData("A test message3!",2000,DEBUG);//the content of the message
  delay(100);
  mySerial.println((char)26);//the ASCII code of the ctrl+z is 26
  delay(5000);
  
}

// EN: Make action based on the content of the SMS. 
//     Notice than SMS content is the result of the processing of several GPRS shield messages.

void ProcessSms( String sms )
{
  Serial.print( "ProcessSms for [" );
  Serial.print( sms );
  Serial.println( "]" );
 
  if( sms.indexOf("on") >= 0 ){
    digitalWrite( ledPin, HIGH );
    Serial.println( "LED IS ON" );
    return;
  }
  if( sms.indexOf("off") >= 0 ){
    digitalWrite( ledPin, LOW );
    Serial.println( "LED IS OFF" );
    return;
  }
  if( sms.indexOf("Q") >= 0 ){
    GetSignalQuality();
    return;
  }
  if( sms.indexOf("T") >= 0 ){
    SendTextMessage();
    return;
  }
  if( sms.indexOf("S") >= 0 ){
   ShowSMS();
    return;
  }
  if( sms.indexOf("D") >= 0 ){
   DeleteSMS();
    return;
  }
  if( sms.indexOf("R") >= 0 ){
    ReadTime();
    return;
  }
}

void GprsReadSmsStore( String SmsStorePos ){
  // Serial.print( "GprsReadSmsStore for storePos " );
  // Serial.println( SmsStorePos ); 
  mySerial.print( "AT+CMGR=" );
  mySerial.println( SmsStorePos );
}
 
// EN: Clear the GPRS shield message buffer

void ClearGprsMsg(){
  msg = "";
}
 
// EN: interpret the GPRS shield message and act appropiately

void ProcessGprsMsg() 
{
    Serial.println("");
    Serial.print( "GPRS Message: [" );
    Serial.print( msg );
    Serial.println( "]" );
    if( msg.indexOf( "Call Ready" ) >= 0 )
   {
       Serial.println( "*** GPRS Shield registered on Mobile Network ***" );
       mySerial.println( "AT+CMGF=1" );
   }
 
  // EN: unsolicited message received when getting a SMS message
  // FR: Message non sollicité quand un SMS arrive
  if( msg.indexOf( "+CMTI" ) >= 0 )
  {
     Serial.println( "*** SMS Received ***" );
     // EN: Look for the coma in the full message (+CMTI: "SM",6)
     //     In the sample, the SMS is stored at position 6
     int iPos = msg.indexOf( "," );
     String SmsStorePos = msg.substring( iPos+1 );
     Serial.print( "SMS stored at " );
     Serial.println( SmsStorePos );
 
     // EN: Ask to read the SMS store
     GprsReadSmsStore( SmsStorePos );
  }
 
  // EN: SMS store readed through UART (result of GprsReadSmsStore request)  
  if( msg.indexOf( "+CMGR:" ) >= 0 )
  {
    // EN: Next message will contains the BODY of SMS
    SmsContentFlag = 1;
    // EN: Following lines are essentiel to not clear the flag!
    ClearGprsMsg();
    return;
  }
 
  // EN: +CMGR message just before indicate that the following GRPS Shield message 
  //     (this message) will contains the SMS body

  if( SmsContentFlag == 1 )
  {
    Serial.println( "*** SMS MESSAGE CONTENT ***" );
    Serial.println( msg );
    Serial.println( "*** END OF SMS MESSAGE ***" );
    ProcessSms( msg );
  }
 
  ClearGprsMsg();
  // EN: Always clear the flag
  SmsContentFlag = 0; 
}

void ShowSMS(){
  sendData("AT+CMGF=1",2000,DEBUG);
  Serial.println("*** Show all SMS message ***");
  sendData("AT+CMGL=\"ALL\"",5000,DEBUG);
  Serial.println("***         END          ***");
}

void DeleteSMS(){
  sendData("AT+CMGF=1",2000,DEBUG);
  Serial.println("*** Delete all SMS message ***");
  sendData("AT+CMGD=1,4",5000,DEBUG);
  Serial.println("***         END            ***");
}

void ReadTime(){
  Serial.println("*** Read time for the SIM80x message ***");
  sendData("AT+CCLK?",3000,DEBUG);
  Serial.println("***         END            ***");
}

void sendData(String command, const int timeout, boolean debug)
{
    String response = "";    
    mySerial.println(command); 
    long int time = millis();
    while( (time+timeout) > millis())
    {
      while(mySerial.available())
      {       
        char c = mySerial.read(); 
        response+=c;
      }  
    }    
    if(debug)
    {
      Serial.print(response);
    }    
    return response;
}
