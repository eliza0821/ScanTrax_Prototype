/*Load Library 
Adafruit_PN532    -for NFC Breakout
Adafruit_SSD1306  -OLED Display
Adafruit_GFX      -OLED Graphics
Keypad            -keypad
SPI and Wire      -connection
*/

#include <SPI.h>
#include <Adafruit_PN532.h>
#include <Wire.h>
#include <Keypad.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>


//NFC set-up
#define PN532_SCK  (52)
#define PN532_MOSI (51)
#define PN532_SS   (53)
#define PN532_MISO (50)
Adafruit_PN532 nfc(SCK, MISO, MOSI, SS);

//OLED Display set-up
#define OLED_MOSI   9
#define OLED_CLK   10
#define OLED_DC    11
#define OLED_CS    12
#define OLED_RESET 13
Adafruit_SSD1306 lcd(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);


//Keypad set-up
const byte rows = 4;
const byte cols = 4;
int count=0;
char keys[rows][cols] = {
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}
};
byte rowPins[rows] = {49, 8, 7, 6}; //{2,3,4,5};
byte colPins[cols] = {5, 4, 3, 2}; //{6,7,8,49};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, rows, cols);


int active;
String msg;
String key_enter;

int green = 37;
int red =33;
char pin_number[4];


uint32_t cardidentifier = 0;
int employee_count = 4;            //no. of employees stored in the program 

//Array of employees details includes:CardId, FirstName,LastName,Password
char *employees[][4] = {
  {"3488988545","JEN","CABALES","9876"},
  {"3488815121","RICK","OLIVAREZ","2580"}
 // {"546851509","SAM","EMBATE","2580"},
  //{"1981821863","ELIZA","SANO","1245"}
};


void setup() {
  Serial.begin(9600);
  pinMode(PN532_SS, OUTPUT);      
  pinMode(green,OUTPUT);
  pinMode(red,OUTPUT);
  digitalWrite(red,LOW);
  digitalWrite(green,HIGH);  
  lcd.begin(SSD1306_SWITCHCAPVCC); //(SSD1306_SWITCHCAPVCC, 0x3D);      
  lcd.clearDisplay();
  digitalWrite(PN532_SS, HIGH);             //Turn 0ff RFID Reader
  welcomemsg();                             //Display welcome message to the User  
  begin_NFC();                              //PN532 starts read mifare card/tag

  active = 0;
}

void loop() {
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 }; // Buffer to store the returned UID
  uint8_t uidLength;                       // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  if (active == 0){
    digitalWrite(PN532_SS, LOW);
    delay(1000);
    digitalWrite(PN532_SS, LOW);
    active = 1;
  
  }
  if(digitalRead(PN532_SS) == LOW) {
    Serial.println("NFC Active");
    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
    
    String empid;
    String empfirstname;                      //Variable to store employee first name
    String emplastname;                       //store employee last name
    int pos;
    if (success) {
      // Found a card!
  
      Serial.print("Card detected: ");
      // turn the four byte UID of a mifare classic into a single variable #
      cardidentifier = uid[3];
      cardidentifier <<= 8; cardidentifier |= uid[2];
      cardidentifier <<= 8; cardidentifier |= uid[1];
      cardidentifier <<= 8; cardidentifier |= uid[0];
      
         
       
      empid = String(cardidentifier);              //Assign the card no. to the empid
      Serial.println(empid);
      
      if (check_empid(empid) != -1)
       {
         digitalWrite(red,LOW);
         digitalWrite(green,HIGH);  
  
        Serial.println("ID Found!");
        pos=check_empid(empid);
        empfirstname=String(employees[pos][1]);    //locate the employee first name from the array then assign to empfirstname
        emplastname=String(employees[pos][2]);      
        check_name(empfirstname,emplastname);
        disable_NFC();
        digitalWrite(PN532_SS, HIGH);
        delay(1000);
        digitalWrite(PN532_SS, HIGH);
      }else{
        digitalWrite(green,LOW);
        digitalWrite(red,HIGH);
        Serial.println("ID Not Found!");
        lcd.clearDisplay();
        lcd.setTextSize(2);
        lcd.setTextColor(WHITE);
        lcd.setCursor(18, 14);
        lcd.print("INVALID!");
        lcd.display();
        lcd.clearDisplay();
        delay(1000);
        welcomemsg();
        digitalWrite(PN532_SS, LOW);
        delay(2000);
        digitalWrite(PN532_SS, LOW);
      }
      
         
    }
   
  
}
   if(digitalRead(PN532_SS) == HIGH) {
     String pin;
     String empid;
     String empname1;
     int pos;
     
     empid = String(cardidentifier);
     pos = check_empid(empid);
     pin = String(employees[pos][3]);
    
     keys_pressed(pin);
     
   }
}

void begin_NFC() {

  digitalWrite(PN532_SS, LOW);
  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
      Serial.print("Didn't find PN53x board");
      while (1); // halt
  }
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX);
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC);
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
  nfc.SAMConfig();
  digitalWrite(PN532_SS, HIGH);

}

void disable_NFC() {

  digitalWrite(PN532_SS, HIGH);
  delay(1000);
  digitalWrite(PN532_SS, HIGH);

}

void enable_NFC() {

  digitalWrite(PN532_SS, LOW);
  delay(1000);
  digitalWrite(PN532_SS, LOW);

}




void keys_pressed(String pinnumber){

char key = keypad.getKey();
 
  if (key != NO_KEY)
 {
        lcd.print("*"); 
        Serial.print(key);
        lcd.display();
        key_enter = key_enter + String(key);
        delay(500);
        count++;
  }
    if (count==4)
    {
      Serial.print(key_enter);
      
 
      if (key_enter == pinnumber){
        lcd.clearDisplay();
        lcd.setTextSize(2);
        lcd.setTextColor(WHITE);
        lcd.setCursor(20, 24);
        lcd.print("GRANTED!");
        lcd.display();
        lcd.clearDisplay();
        delay(1000);
        welcomemsg();
        key_enter = "";
    }else{
        key_enter = "";
        digitalWrite(green,LOW);
        digitalWrite(red,HIGH);
        lcd.clearDisplay();
        lcd.setTextSize(2);
        lcd.setTextColor(WHITE);
        lcd.setCursor(20, 24);
        lcd.print("DENIED!");
        lcd.display();
        lcd.clearDisplay();
        delay(2000);
        welcomemsg();
        
      }

      enable_NFC();
      count=0;
      active = 0;
    }
  }


int check_empid(String empid){

    int l;

    String emp_id;
      for (l=0; l<employee_count; ++l)
    {
      
      emp_id = String(employees[l][0]);
 
       if(empid == emp_id) 
       {
          
          //Serial.println("ID Found!");
        return l;
        
        }
    }
  return -1;
}
void check_name(String ename, String lname){
    lcd.setTextSize(2);
    lcd.setTextColor(WHITE);
    lcd.setCursor(20, 24);
    lcd.print("Hi! "+ename);
    lcd.display();
    delay(1000);
    lcd.clearDisplay();
    lcd.setTextSize(2);
    lcd.setTextColor(WHITE);
    lcd.setCursor(20, 24);
    lcd.print("PIN:");
    lcd.display();

}

void welcomemsg(){
  digitalWrite(green,HIGH);
  digitalWrite(red,LOW);
  lcd.setTextSize(2);
  lcd.setTextColor(WHITE);
  lcd.setCursor(20, 24);
  lcd.print("WELCOME");
  
  lcd.display();
  lcd.clearDisplay();
  delay(100);
}


