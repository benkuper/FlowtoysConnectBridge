#define STREAM_MAX_COUNT 100
#define STREAM_MAX_PACKET_SIZE STREAM_MAX_COUNT*3

class StreamManager
{
public:
  StreamManager() : byteIndex(0),
                    hasOverflowed(false),
                    isInit(false)
                   {}
                   
  ~StreamManager() { stop(); }

  WiFiUDP udp;

  bool isInit;
  //tempo
  const int receiveRate = 100; //receive at 20fps max
  long lastReceiveTime;

  CRGB leds[STREAM_MAX_COUNT];
  uint8_t streamBuffer[STREAM_MAX_PACKET_SIZE];
  int byteIndex;
  bool hasOverflowed;
    
  
  void  init()
  {
    start();
    isInit = true;
  }

  bool update()
  {
      if(!isInit) return false;
      long curTime = millis();
      if (curTime > lastReceiveTime + (1000 / receiveRate))
      {
          lastReceiveTime = curTime;
          return receiveUDP();
      }
      return false;
  }

  bool receiveUDP()
  {
      while (int packetSize = udp.parsePacket())
      {
         //DBG("Receiving !");
          int numRead = udp.read(streamBuffer, STREAM_MAX_PACKET_SIZE);
         
          if(numRead == 0) return false;
          bool isFinal = streamBuffer[numRead - 1] == 255;
  
          if(isFinal)
          {
              if(hasOverflowed) //if had overflowed, discard current packet and reset for next one
              {
                  DBG("Discard overflowed packet, reset for next one");
                  byteIndex = 0;
                  hasOverflowed = false;
                  return false;
              }
              
              numRead--;
          } 
  
          if(byteIndex + numRead > STREAM_MAX_PACKET_SIZE)
          {
              DBG("Stream OVERFLOW, end index would reach " +String(byteIndex+numRead));
              hasOverflowed = true;
          }else
          {
              //DBG(" > Copying at "+String(byteIndex));
              memcpy((uint8_t *)leds + byteIndex, streamBuffer, numRead);
          }
          
          byteIndex += numRead;
          if (isFinal)
          {
              byteIndex = 0;
             // DBG("Received final");
              return true;
          }
      }
  
      return false;
  }

  void start()
  {
      udp.begin(8888);
      udp.flush();
  }
  
  void stop()
  {
      udp.flush();
      udp.stop();
  }
};
