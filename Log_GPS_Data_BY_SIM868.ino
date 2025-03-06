uint8_t NewDataInt1;
uint8_t NewDataInt3;
//uint8_t rx_buff[20];
char rx_buff1[100];
char rx[100];
uint8_t rx_buff3[200];
uint8_t rx_buff_count1 = 0;
uint8_t size_buff1 = 0;
uint8_t rx_buff_count3 = 0;
uint8_t size_buff3 = 0;
uint8_t NewDataLineCount1 = 0;
uint8_t NewDataLineCount3 = 0;
uint8_t flag = 0;
uint8_t flag2 = 0;
//char str_command[80];
//char str_location[80];

#define GPS_MODE 0
#define SMS_MODE 1


struct Data_structure {
  struct {
    char  GNSS_run_status;
    char  Fix_status;
    char  UTC_date_Time[18];
    char   Latitude[10];
    char   Longitude[11];
    char   MSL_Altitude[8];
    char Speed_Over_Ground[6];
    char Course_Over_Ground[6];
    char Fix_Mode;
    char HDOP[4];
    char PDOP[4];
    char VDOP[4];
    char GPS_Satellites_in_View[2];
    char GNSS_Satellites_Used[2];
    char GLONASS_Satellites_in_View[2];
    char C_N0_max[2];
    char HPA[6];
    char VPA[6];
  } GPS_data;
  struct {
    char number_of_massage;
    char phone_number[15];
    char *massage;
  } SMS_data;
};


struct Data_structure module_data;
//struct Data_structure module_data_sms;
void GPS_set_data(struct Data_structure *Data, char loc_temp[], uint8_t count) {
  switch (count)
  {
    case 0:
      Data->GPS_data.GNSS_run_status = loc_temp[0];
      break;

    case 1:
      Data->GPS_data.Fix_status = loc_temp[0];
      break;

    case 2:
      strcpy(Data->GPS_data.UTC_date_Time, loc_temp);
      break;

    case 3:
      strcpy(Data->GPS_data.Latitude, loc_temp);
      break;

    case 4:
      strcpy(Data->GPS_data.Longitude, loc_temp);
      break;

    case 5:
      strcpy(Data->GPS_data.MSL_Altitude, loc_temp);
      break;

    case 6:
      strcpy(Data->GPS_data.Speed_Over_Ground, loc_temp);
      break;

    case 7:
      strcpy(Data->GPS_data.Course_Over_Ground, loc_temp);
      break;

    case 8:
      Data->GPS_data.Fix_Mode = loc_temp[0];
      break;

    case 9:

      break;

    case 10:
      strcpy(Data->GPS_data.HDOP, loc_temp);
      break;

    case 11:
      strcpy(Data->GPS_data.PDOP, loc_temp);
      break;

    case 12:
      strcpy(Data->GPS_data.VDOP, loc_temp);
      break;

    case 13:

      break;

    case 14:
      strcpy(Data->GPS_data.GPS_Satellites_in_View, loc_temp);
      break;

    case 15:
      strcpy(Data->GPS_data.GNSS_Satellites_Used, loc_temp);
      break;

    case 16:
      strcpy(Data->GPS_data.GLONASS_Satellites_in_View, loc_temp);
      break;

    case 17:

      break;

    case 18:
      strcpy(Data->GPS_data.C_N0_max, loc_temp);
      break;

    case 19:
      strcpy(Data->GPS_data.HPA, loc_temp);
      break;

    case 20:
      strcpy(Data->GPS_data.VPA, loc_temp);
      break;

    // operator doesn't match any case constant +, -, *, /
    default:
      printf("Error! operator is not correct");
  }
}

void SMS_set_data(struct Data_structure *Data, char sms_temp[], uint8_t count) {
  if (count == 0) {
    //    Serial.println(sms_temp);
    for (int i; i < strlen(sms_temp); i++) {
      Data->SMS_data.phone_number[i] = sms_temp[i];
    }
    //    Serial.println(Data->SMS_data.phone_number);
    //    strcpy(Data->SMS_data.phone_number, sms_temp);
  }
}


void conv_uart_to_struct(struct Data_structure * Data, char data_in[100], uint8_t mode) {
  char data_temp[100];
  char src[1];
  for (int j = 0; j < strlen(data_temp); j++) data_temp[j] = 0;
  uint8_t count = 0;
  for (int i = 0; i < strlen(data_in); i++) {
    if (data_in[i] == 44) {
      if (mode == 0) {
        GPS_set_data(Data, data_temp, count);
      }
      if (mode == 1) {
        SMS_set_data(Data, data_temp, count);
      }
      for (int j = 0; j < strlen(data_temp); j++) data_temp[j] = 0;
      count++;
    } else {
      src[0] = data_in[i];
      strncat(data_temp, src, 1);
    }
  }
}

void check_GPS_data() {
  char *ret;
  uint8_t loc = 0;
  char loc_info[100];
  ret = strstr(rx_buff1, "+CGNSINF: ");
  if (ret != NULL) {
    loc = ret - rx_buff1 + sizeof("+CGNSINF: ") - 1;
    memcpy(loc_info, rx_buff1 + loc, strlen(rx_buff1));
    conv_uart_to_struct(&module_data, loc_info, GPS_MODE);
  }
}

void check_SMS_Receive() {
  char *ret;
  char *massage;
  uint8_t sms = 0;
  char sms_info[100];
  ret = strstr(rx_buff1, "+CMT: ");
  massage = strstr(rx_buff1, "RGPS");
  if (ret != NULL) {
    sms = ret - rx_buff1 + sizeof("+CMT: ") - 1;
    memcpy(sms_info, rx_buff1 + sms, strlen(rx_buff1));
    //      Serial.println(loc_info);
    conv_uart_to_struct(&module_data, sms_info, SMS_MODE);
  }


  if (massage != NULL) {
    flag2 = 1;
  }
}

void setup()
{
  cli();//stop interrupts

  //set timer4 interrupt at 1Hz
  TCCR4A = 0;// set entire TCCR1A register to 0
  TCCR4B = 0;// same for TCCR1B
  TCNT4  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR4A = 65535 / 1; // = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR4B |= (1 << WGM12);
  // Set CS12 and CS10 bits for 1024 prescaler
  TCCR4B |= (1 << CS12) | (1 << CS10);
  // enable timer compare interrupt
  TIMSK4 |= (1 << OCIE4A);

  sei();//allow interrupts
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  Serial1.begin(9600);
  //  Serial2.begin(9600);
  //  while (!Serial) {
  //    ; // wait for serial port to connect. Needed for Native USB only
  //  }

  Serial1.write("AT+CGPSPWR=1\n\r");
  delay(1000);
  Serial1.write("AT+CMGF=1\n\r");
  delay(1000);
  Serial1.write("AT+CNMI=2,2,0,0,0\n\r");
  delay(1000);
}

ISR(TIMER4_COMPA_vect) { //timer1 interrupt 1Hz toggles pin 13 (LED)
  //generates pulse wave of frequency 1Hz/2 = 0.5kHz (takes two cycles for full wave- toggle high then toggle low)
  Serial1.write("AT+CGNSINF\n\r");
}
//int count = 0;
void loop() // run over and over
{
  if (Serial.available()) {
    //    Serial1.write(Serial.read());
    Serial1.write(Serial.read());
  }

  if (Serial1.available()) {
    //    Serial.write(Serial1.read());
    NewDataInt1 = Serial1.read();
    rx_buff1[rx_buff_count1++] = NewDataInt1;
    rx_buff1[rx_buff_count1] = 0;
    if (((NewDataInt1 == '\n')) && (rx_buff_count1 > 0))
    {
      NewDataLineCount1 = rx_buff_count1;
      rx_buff_count1 = 0;
      flag = 1;
//      Serial.print(rx_buff1);
    }
  }

  if (flag == 1) {
    check_GPS_data();
    check_SMS_Receive();
    flag = 0;
  }
  if (flag2 == 1) {
    char str_command[80];
    char str_location[80];
    sprintf(str_command, "AT+CMGS=%s\r", module_data.SMS_data.phone_number);
    sprintf(str_location, "https://www.google.com/maps/@%s,%s,16z", module_data.GPS_data.Latitude, module_data.GPS_data.Longitude);
    //    sprintf(str_location, "https://www.google.com/maps/@%s,%s,16z", "36.6746013", "48.4624045");
    delay(500);
    Serial1.print(str_command);  //Your phone number don't forget to include your country code, example +212123456789"
    delay(500);
    Serial1.print(str_location);       //This is the text to send to the phone number, don't make it too long or you have to modify the SoftwareSerial buffer
    delay(500);
    Serial1.print((char)26);// (required according to the datasheet)
    delay(500);
    Serial1.println();
    delay(500);
    flag2 = 0;
  }
}
