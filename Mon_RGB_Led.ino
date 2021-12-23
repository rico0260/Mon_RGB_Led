/* 
 * Example sketch showing how to control an RGB Led Strip.
 * This example will remember the last rgb color set after power failure.
 * Auteur : Eric HANACEK
 * Modification : 04/04/2020 pour V2.3.2
 * 
  */

// Enable debug prints to serial monitor
//#define MY_DEBUG

// Enable and select radio type attached
#define MY_RADIO_RF24
//#define MY_RADIO_NRF5_ESB
//#define MY_RADIO_RFM69
//#define MY_RADIO_RFM95

//Options: RF24_PA_MIN, RF24_PA_LOW, (RF24_PA_HIGH), RF24_PA_MAX
//#define MY_RF24_PA_LEVEL RF24_PA_MAX

//uncomment this line to assign a static ID
//#define MY_NODE_ID 7 

//Define this to use the IRQ pin of the RF24 module
//#define MY_RF24_IRQ_PIN (2) 
//Define this to use the RF24 power pin (optional).
//#define MY_RF24_POWER_PIN (3)
//Channels: 1 to 126 - 76 = Channel 77
//MY_RF24_CHANNEL (76)

// Enable repeater functionality for this node
//#define MY_REPEATER_FEATURE

#include <MySensors.h>

#define SN "RGB Node"
#define SV "1.2"

#define GREEN 6 // Arduino PWM pin for Green
#define RED  3  // Arduino PWM pin for Red
#define BLUE 5  // Arduino PWM pin for Blue
#define CHILD_ID_RGB 0  // sensor number needed in the custom devices set up

int RGB_pins[3] = {RED,GREEN,BLUE};
long RGB_values[3] = {0,0,0};

MyMessage msgColor(CHILD_ID_RGB, V_RGB);

void before()
{
    // Optional method - for initialisations that needs to take place before MySensors transport has been setup (eg: SPI devices).
}

void setup()
{
    // Initialize library and add callback for incoming messages
    //begin(incomingMessage,  AUTO);//, true);
    
    //request( 0, V_RGB);
    
    // Test des LED
    /*for (int i = 0; i<3; i++) {
        pinMode(RGB_pins[i], OUTPUT);
        analogWrite(RGB_pins[i], 255);
        
        // Lecture des dernieres valeures dans eeprom 
        RGB_values[i] = loadState(RGB_pins[i]);
    }*/
    
    // Set the rgb pins in output mode
    for (int i = 0; i<3; i++) {
        pinMode(RGB_pins[i], OUTPUT);
        
        // Lecture des dernieres valeures dans eeprom 
        RGB_values[i] = loadState(RGB_pins[i]);
        
        //analogWrite(RGB_pins[i], loadState(RGB_pins[i]));
        analogWrite(RGB_pins[i], RGB_values[i]);
        
        // Afficher des infos de debugage
        if (i==0) {
          Serial.print("Rouge: " );
        }
        else if (i==1) {
          Serial.print("Vert : " );
        }
        else {
          Serial.print("Bleu : " );
        }
        //Serial.println(loadState(RGB_pins[i]));
        Serial.println(RGB_values[i]);

    }

    //if you want to convert it again from rgb to hex
    long rgb = 0;
    rgb = ((long)RGB_values[0] << 16) | ((long)RGB_values[1] << 8 ) | (long)RGB_values[2];
    Serial.print("This is the reconverted HEX value: #" );
    Serial.println(String(rgb, HEX));
    
    //Informe Jeedom de la couleur actuelle
    String RGB;
    String hRGB ="#";
    String valRGB = String(rgb, HEX);
    RGB = hRGB + valRGB;
    send(msgColor.set(&RGB[0]));
    /*long number = (long) strtol( &valRGB[0], NULL, 16);
    send(msgColor.set(number));*/
    
    //request( 0, V_RGB);

}

void presentation()
{
    // Register the sensor to gw
    present(CHILD_ID_RGB, S_RGB_LIGHT);
     
    // Send the sketch version information to the gateway and Controller
    sendSketchInfo(SN, SV);
 
}

void loop()
{
    // Alway process incoming messages whenever possible

}

void receive(const MyMessage &message) {
    
    /*Serial.print("Type message ");
    Serial.println(message.type);*/
    
    // We only expect one type of message from controller. But we better check anyway.
    if (message.type==V_RGB) {
	      // starting to process the hex code
        String hexstring = message.getString(); //here goes the hex color code coming from Jeedom through MySensors (like FF9A00)
        
        //String hexstring = "FF3Fa0"; //ATTENTION Jeedom retour "#FF3Fa0"
        Serial.print("hexstring " );
        Serial.println(hexstring);
        
        //unsigned long number = strtol( &hexstring[0], NULL, 16);
        // Ne pas convertir le premier caractere retourné par Jeedom
        long number = (long) strtol( &hexstring[1], NULL, 16);
        Serial.print("number " );
        Serial.println(number);
        
        //Serial.print("number en HEX " );
        //Serial.println(String(number, HEX));
        
        RGB_values[0] = number >> 16;
        RGB_values[1] = number >> 8 & 0xFF;
        RGB_values[2] = number & 0xFF;
        //RGB_values[0] = lowByte(number >> 16);
        //RGB_values[1] = lowByte(number >> 8);
        //RGB_values[2] = lowByte(number);
        
        for (int i = 0; i<3; i++) {
            analogWrite(RGB_pins[i],RGB_values[i]);
            
            // Enregistrer dans eeprom
            saveState(RGB_pins[i], RGB_values[i]);

        }
        
        // Write some debug info
        /*Serial.print("Rouge: " );
        Serial.println(RGB_values[0]);
        Serial.print("Vert : " );
        Serial.println(RGB_values[1]);
        Serial.print("Bleu : " );
        Serial.println(RGB_values[2]);*/

        //if you want to convert it again from rgb to hex
        long rgb = 0;
        rgb = ((long)RGB_values[0] << 16) | ((long)RGB_values[1] << 8 ) | (long)RGB_values[2];
	      /*Serial.print("This is the reconverted HEX value: #" );
        Serial.println(String(rgb, HEX));*/
        
        //Informe Jeedom de la couleur
        String RGB;
        String hRGB ="#";
        String valRGB = String(rgb, HEX);
        RGB = hRGB + valRGB;
        send(msgColor.set(&RGB[0]));
        Serial.print("Envoye a jeedom" );
        Serial.println(&RGB[0]);
        /*send(msgColor.set(number));
        Serial.print("Envoye a jeedom ");
        Serial.println(number);*/

    }
}
