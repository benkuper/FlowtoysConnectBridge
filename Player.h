#pragma once


class Player
{
  public:
    
    struct PlayerCue
    {
      float time;
      uint8_t group;
      uint8_t page;
      uint8_t mode;
      uint8_t lfo[4];
      uint8_t intensity;
      uint8_t h;
      uint8_t s;
      uint8_t v;
      uint8_t speed;
      uint8_t density;
      uint8_t palette;
    };

    
    bool isPlaying;
    bool isPaused;

    bool loopAtFinish;
    
    long timeAtPlay; //ms
    long totalTime;
    long currentTime;
    long lastTime;
    
    int currentCue;
    int numCues;
    
    void init()
    {
      loopAtFinish = false;
      
      currentTime = 0;
      currentCue = -1;
      
      DBG("Player init.");
    }

    void update()
    {
      lastTime = millis();

      if (!isPlaying) return;
      if (isPlaying) currentTime += millis() - lastTime;

      if(currentTime >= totalTime)
      {
        DBG("Finished !");
        if(loopAtFinish) 
        {
          DBG("Loop.");
          currentTime = 0;
        }else
        {
          DBG("Stop.");
          stop();
        }
      }
    }

    void play(String filename)
    {
      File file = FileManager::openFile(filename);
      DBG("File opened : "+String(file.name())+", size : "+String(file.size())+" bytes");
      
      /*
      totalTime = doc["totalTime"];
      numCues = doc["cues"].size();
      
      for(int i=0;i<numCues;i++)
      {
        cues[i].time = doc["cues"][i]["time"];
        
      }

      DBG("File total time : "+String(totalTime));
      */

      
      currentTime = 0;
      currentCue = -1;
      isPlaying = true;
      isPaused = false;
    }

    void stop()
    {
      isPlaying = false;
      isPaused = false;
      currentTime = 0;
    }

    void pause()
    {
      if (!isPlaying) return;
      isPaused = true;
    }

    void seek(float toTime)
    {
      currentTime = toTime * 1000;
    }

    void resume()
    {
      isPlaying = true;
      isPaused = false;
    }

};
