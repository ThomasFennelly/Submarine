const byte NUM_CHARS = 255;
char receivedChars[NUM_CHARS];

boolean newData = false;

int startOfChunk = -1;
int endOfChunk = -1;

double input[4];

void setup() {
  Serial.begin(115200);
  Serial.println("ready, eddy");
}

void loop() {
  receive();
  showNewData();
}

//receives the input from serial.
void receive() {
  static boolean recvInProgress = false;
  static byte ndx = 0;
  char endMarker = '\n';
  char rc; //read character
 
  while (Serial.available() > 0 && newData == false) {
    rc = Serial.read();
    if (rc != endMarker) {
      receivedChars[ndx] = rc;
        ndx++;
        if (ndx >= NUM_CHARS) {
          ndx = NUM_CHARS - 1;
        }
    }
    else {
      receivedChars[ndx] = '\0'; // terminate the string
      ndx = 0;
      newData = true;
    }
  }
}

//finds a chunk in the input string that is surrounded by ':'s, and gives the beginning and ending indices to the appropriately names global variables
//which consecutive chunk is returned is given by the variable 'chunk'
void findChunk(int chunk) {
  char marker = ':';
  int index = 0;
  startOfChunk = -1;
  endOfChunk = -1;
  for (int i = 0; i < NUM_CHARS; ++i) {
    if (receivedChars[i] == marker) {
      index++;
      if (index == chunk) {
        startOfChunk = i+1;
      }
      else if (index == chunk + 1) {
        endOfChunk = i-1;
        return;
      }
    }
  }
  if (endOfChunk == -1) {
    for (int j = startOfChunk; receivedChars[j] != '\0' ; ++j) {
        endOfChunk = j;
    }
  }
  return;
}

void showNewData() {
  if (newData == true) {
    Serial.print("Input: ");
    Serial.println(receivedChars);
    Serial.print("    ");
    //takes the input string and splits it up into double in the array called input
    int j = 0;
    int index = 0;
    for (int k = 1; k <= 4; ++k) {
      char temp[NUM_CHARS];
      findChunk(k);
      j = 0;
      for (int i = startOfChunk; i <= endOfChunk; i++) {
        temp[j] = receivedChars[i];
        ++j;
      }
      
      input[index] = atoi(temp);
      ++index;
      Serial.print("var ");
      Serial.print(k);
      Serial.print(": ");
      Serial.print(input[index-1]);
      Serial.print(", ");
      
      for (int a = 0; a < endOfChunk; ++a) {
        temp[a] = '\0';
      }
    }
    
    //checks the first character then the second character to find what it is actually doing
    switch (receivedChars[0]) {
      //absolute movement
      case 'A':
        if (receivedChars[1] == '0') {
          Serial.println("absolute movement, Z motor");
          
          //'a' represents degree of rotation in relative coordinates
          if (input[0] == -2) {
            Serial.println("Target location infinite");
          }
          else if (input[0] == -1) {
            if (input[2] == -1 || input[2] == 1) {
              Serial.println("a cannot equal -1 when c is -1 or 1.");
            }
            else {
              Serial.println("Target rotation point unchanged");
            }
          }
          else if (input[0] == 0) {
            Serial.println("Motor stops");
          }
          else if (input[0] > 0) {
            Serial.println("The motor location zeroes and moves to a");
          }
          else {
            Serial.println("ERR: invalid degree of rotation");
          }
        }
        else {
          Serial.println("ERR: invalid number proceeding A");
        }

        //'b' represents RPM
        if (input[1] == -1) {
          Serial.println("Current speed of motor is unchanged");
        }
        else if (input[1] == 0) {
          Serial.println("Motor stops");
        }
        else if (input[1] > 0) {
          Serial.println("the motor changes to the new RPM given as b");
        }
        else {
          Serial.println("ERR: invalid RPM");
        }

        //'c' represents direction of rotation
        if (input[0] != -2) {
          if (input[2] == -1) {
            Serial.println("Motor moves backwards");
          }
          else if (input[2] == 0) {
            Serial.println("Motor stops");
          }
          else if (input[2] == 1) {
           Serial.println("Motor moves forwards");
          }
          else {
            Serial.println("ERR: invalid direction of rotation");
          }
        }
        else {
          Serial.println("c is ignored when a == -2");        
        }
        break;
      
      //relative movement
      case 'B':
        if (receivedChars[1] == '0') {
          Serial.println("relative movement, X motor");
          //'d' represents degree of rotation in absolute coordinates
          if (input[1] == 0) {
            if (input[0] == -1) {
              Serial.println("Target rotation point unchanged");
            }
            else if (input[0] >= -180 && input[0] <= 180) {
              Serial.println("Motor moves to new target location d");
            }
            else {
              Serial.println("ERR: invalid number of degrees.");
            }
          }
          else {
            Serial.println("d is ignored when e is 0");
          }
          
          //'e' represents RPM
          if (input[1] == -1) {
            Serial.println("the current speed of the motor is unchanged");
          }
          else if (input[1] == 0) {
            Serial.println("Motor stops");
          }
          else if (input[1] > 0) {
            Serial.println("Motor changes rpm to e");
          }
          else {
            Serial.println("ERR: invalid RPM");
          }
          
          
        }
        else if (receivedChars[1] == '1') {
          Serial.println("relative movement, Y motor");
          
        }
        else { 
          Serial.println("ERR: invalid number proceeding B");
        }
        break;
      default:
        Serial.println("ERR: invalid command");
        break;
    }
    newData = false;
  }
}
