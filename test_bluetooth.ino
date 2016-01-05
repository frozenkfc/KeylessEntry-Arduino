String input;
String Mypass[10] = {"aaa","bbb","ccc","ddd","","","","","",""};
const String password = "1234";


const int START = 0;
const int AUTHEN = 1;
const int ADMIN = 2; 
const int UNLOCK = 3;
unsigned long startTime;
unsigned long stopTime;
int count = 0;
int state = START;

void setup() {
  pinMode(8,INPUT_PULLUP);
  pinMode(13,OUTPUT);
  digitalWrite(13,LOW);
  Serial.begin(9600);
  Serial1.begin(9600);
  attachInterrupt(digitalPinToInterrupt(2), backToStart, FALLING);
}
/////////////////////////////////////////////////////////
void start(){
  if(Serial1.available()>0){
      input = Serial1.readString();//read text or  get response from anddroid
    }
  if(input.equals("AuthenticationRequest")){
      state = AUTHEN;
      Serial1.println("AuthenticationStartEND");
      Serial.println("AuthenticationStartEND");
    } 
  if(input.equals("UnlockRequest")){
    Serial1.println("UnlockStartEND");
    Serial.println("Unlock Started");
    state = UNLOCK;
    
  }
}

void backToStart(){

    if(state == START )
      return;

    if(count < 5)
      return;
      
    if(count == 5)
      startTime = millis();
    if(count == 6){
      stopTime = millis();
      //interrupt
      if(stopTime - stopTime < 500){
        String s = String(state);
        Serial.println("Interrupted state: " + s);
        state = START;
      }
      count = 0;
    }
    count++;
}
/////////////////////////////////////////////////////////
void unlock(){
  if(Serial1.available()>0){ //check every state ,It helpful for swap state in switch case
      input = Serial1.readString();
    }

    bool isFinalPassword = false; //
    if(input.equals("")) //check,If empty response from anddroid
      return;

     if(input.lastIndexOf("UnlockFailed") != -1){ //android code have text "UnlockFailed" in the last key
        input.replace("UnlockFailed","");
        isFinalPassword = true;
     }
      Serial.println(input);
      bool unlocked = false;
      for(int i = 0;i<10;i++){
        if(input.equals(Mypass[i])){
          unlocked = true;
          break;
        }
      }
      if(unlocked){
          digitalWrite(13,HIGH);
          Serial1.println("WelcomeEND");
          Serial.println("Welcome");
          state = START;
      }
      else{
          if(isFinalPassword){  //check, Is final keys ??
              state = START;
              Serial1.println("ByeEND");
              Serial.println("BYE");
              Serial.println(state);
              return;
          }
          Serial1.println("TryAgainEND");
          Serial.println("TryAgain");
      }
}
/////////////////////////////////////////////////////////
void authen(){
  if(Serial1.available()>0){
      input = Serial1.readString();
  }

  if(input.equals(""))
      return;
  
  if(input.equals(password)){
    state = ADMIN;
    Serial.println(state);
    Serial1.println(state);
    Serial.println("ValidEND");
    Serial1.println("ValidEND");
    return;
   }    
   else{ 
    Serial.println("InvalidEND");
    Serial1.println("InvalidEND");
    state = AUTHEN;
   }
}
/////////////////////////////////////////////////////////
void admin(){
  if(Serial1.available()>0){
      input = Serial1.readString();
  }

  if(input.equals(""))
      return;
      
  if(input.equals("ShowKey")){
    String keys = "KeysAre:";
      for(int i = 0; i<10; i++){
        if(!Mypass[i].equals("")){
           keys += Mypass[i] + ",";
         }
      }
      keys = keys.substring(0,keys.length()-1);//Delete the last ,
      keys += "END";//plus End for sent respone with android
    Serial.println(keys);
    Serial1.println(keys);
    
  }
  if(input.equals("SignOutRequest")){
      state = START;
      Serial1.print("SignOutEND");
      Serial.print("Sign Out END");
  }
  if(input.lastIndexOf("Add:") != -1){
    input.replace("Add:", "");
    bool isFull = true;
    for(int j=0;j<10;j++){
                    if(Mypass[j].equals("")){
                        Mypass[j]=input;
                        isFull = false;
                        break;
                      }
                  }
    if(isFull){
      Serial1.println("AddFailedEND");
      return;
    }

    String keys = "";
    for(int i = 0; i<10; i++){
      if(!Mypass[i].equals("")){
         keys += Mypass[i] + ",";
       }
    }
    keys = keys.substring(0,keys.length()-1);
    keys += "END";
      
    Serial.println("AddSuccess->" + keys);
    Serial1.println("AddSuccess->" + keys);
    
  }
  if(input.lastIndexOf("Remove:") != -1){
    input.replace("Remove:", "");
    removes(input,',');

    String keys = "";
    for(int i = 0; i<10; i++){
      if(!Mypass[i].equals("")){
         keys += Mypass[i] + ",";
       }
    }
    keys = keys.substring(0,keys.length()-1);
    keys += "END";
    
    Serial.println("RemoveSuccess->" + keys);
    Serial1.println("RemoveSuccess->" + keys);
    for(int f = 0;f < 10;f++){
        Serial.println(Mypass[f]);
      }
  }
  if(input.equals("Logout")){
    Serial.println("LogoutEnd");
    Serial1.println("LogoutEnd");
    
  }
}
/////////////////////////////////////////////////////////
void removes(String input,char delim){ //remove target (char) from input
  input += delim;
  Serial.println("Input: " + input);
  String target = "";
  int i,j;
      for(i = 0;i<=input.length();i++){
          if(input.charAt(i)!=delim){
                target += input.charAt(i);
            }
          else{
            Serial.println("target:"+target);
                for(j=0;j<10;j++){
                    if(Mypass[j].equals(target)){
                        Mypass[j]="";
                      }
                  }
                  target = "";
            }
        }
  }

/////////////////////////////////////////////////////////
void loop() {
  if(digitalRead(8)==0){
    digitalWrite(13,LOW);
  }
  switch(state){
    case START: 
      start();
      break; 
    
    case AUTHEN: 
      authen();
      break; 
    
    case ADMIN: 
      admin();
      break; 
    
    
    case UNLOCK: 
      unlock();
      break; 
    
    default: break;
  }

  input = "";
  delay(1);
}
