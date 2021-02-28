
class CSignal {

  public:
    CSignal(int pin);
    ~CSignal();

    void  Read();
	void  SyncRisingTimeWith(CSignal Sig);
    float ReadDistance();
    float Distance;
    unsigned long start_time;
    unsigned long falling_time;
    unsigned long rising_time;

  private:
    void Sample();
    int Pin;
    bool resulOk;
    float pulseHTime;
    float pulseLTime;

    int last_state;
    int state;

};


CSignal::~CSignal() {
}
CSignal::CSignal(int pin)
{
  Pin = pin;
  start_time = micros();
  last_state = digitalRead(pin);
  if (last_state == HIGH)
  {
    rising_time = micros();
  }
  resulOk = false;
  pulseHTime = 0;
  pulseLTime = 0;
  falling_time = 0;
  rising_time = 0;
  Distance = 0;

}

void CSignal::Sample() {
  state = digitalRead(Pin);

  if (last_state == LOW && state == HIGH) {
    rising_time = micros();
    pulseLTime =  rising_time - falling_time;
  }

  if (last_state == HIGH && state == LOW) {
    falling_time = micros();
    pulseHTime = falling_time - rising_time;
  }

  last_state = state;
}

void  CSignal::Read()
{
  this->Sample();
  if (pulseHTime != 0)
    resulOk = true;
}

float CSignal::ReadDistance() {
  if (resulOk && pulseHTime < 30000) { //500 CM
    Distance = pulseHTime * 0.034 / 2 ;
  } else {
    Distance = 0;
  }
  return Distance;
}


void  CSignal::SyncRisingTimeWith(CSignal Sig){
	
	rising_time = Sig.rising_time;
	if(falling_time > rising_time){
		pulseHTime = falling_time - rising_time;
	}else{
		resulOk = false;
	}
}
