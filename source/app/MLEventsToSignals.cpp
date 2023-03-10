// MadronaLib: a C++ framework for DSP applications.
// Copyright (c) 2013 Madrona Labs LLC. http://www.madronalabs.com
// Distributed under the MIT license: http://madrona-labs.mit-license.org/

#include "MLEventsToSignals.h"

namespace ml {

// EventsToSignals::Voice
//

void EventsToSignals::Voice::setSampleRate(float sr)
{
  _sampleRate = sr;
  
  // separate glide time for note pitch
  pitchGlide.setGlideTimeInSamples(sr*kPitchGlideTimeSeconds);
  
  pitchBendGlide.setGlideTimeInSamples(sr*kGlideTimeSeconds);
  modGlide.setGlideTimeInSamples(sr*kGlideTimeSeconds);
  xGlide.setGlideTimeInSamples(sr*kGlideTimeSeconds);
  yGlide.setGlideTimeInSamples(sr*kGlideTimeSeconds);
  zGlide.setGlideTimeInSamples(sr*kGlideTimeSeconds);
}

// done when DSP is reset.
void EventsToSignals::Voice::reset()
{
  state = kOff;
  nextFrameToProcess = 0;
  ageInSamples = 0;

  currentVelocity = 0;
  currentPitch = 0;
  currentPitchBend = 0;
  currentMod = 0;
  currentX = 0;
  currentY = 0;
  currentZ = 0;

  creatorID = 0;
  
  pitchBendGlide.setValue(0.f);
  modGlide.setValue(0.f);
  xGlide.setValue(0.f);
  yGlide.setValue(0.f);
  zGlide.setValue(0.f);
}

void EventsToSignals::Voice::beginProcess()
{
  nextFrameToProcess = 0;
}

float getAgeInSeconds(uint32_t age, float sr)
{
  double dAge(age);
  double dSr(sr);
  double dSeconds = dAge / dSr;
  float fSeconds(dSeconds);
  return fSeconds;
}

void EventsToSignals::Voice::writeNoteEvent(const Event& e, const Scale& scale)
{
  auto time = e.time;
  
  switch(e.type)
  {
    case Event::kNoteOn:
    {
      //     std::cout << "write note: " << pitchj
      state = kOn;
      creatorID = e.creatorID;
      ageInSamples = 0;
      size_t destTime = e.time;
      destTime = clamp(destTime, 0UL, (size_t)kFloatsPerDSPVector);
      
      // write current pitch and velocity up to note start
      for(size_t t = nextFrameToProcess; t < destTime; ++t)
      {
        outputs.row(kVelocity)[t] = currentVelocity;

        // TODO sample accurate pitch glide
        outputs.row(kPitch)[t] = currentPitch;
        
        outputs.row(kElapsedTime)[t] = getAgeInSeconds(ageInSamples++, _sampleRate);
      }
      
      // set new values
      currentPitch = scale.noteToLogPitch(e.value1);
      currentVelocity = e.value2; // TODO union
      nextFrameToProcess = destTime;
      
      break;
    }
    case Event::kNoteRetrig:
    {
      state = kOn;
      creatorID = e.creatorID;
      size_t destTime = e.time;
      destTime = clamp(destTime, 0UL, (size_t)kFloatsPerDSPVector);
      
      // if the retrigger falls on frame 0, make room for retrigger
      if(destTime == 0)
      {
        destTime++;
      }
      
      // write current pitch and velocity up to retrigger
      for(size_t t = nextFrameToProcess; t < destTime - 1; ++t)
      {
        outputs.row(kVelocity)[t] = currentVelocity;
        
        // TODO sample accurate glide
        outputs.row(kPitch)[t] = currentPitch;
        
        outputs.row(kElapsedTime)[t] = getAgeInSeconds(ageInSamples++, _sampleRate);
      }
      
      // write retrigger frame
      outputs.row(kVelocity)[destTime - 1] = 0;
      outputs.row(kPitch)[destTime - 1] = currentPitch;
      
      // set new values
      currentPitch = scale.noteToLogPitch(e.value1);
      currentVelocity = e.value2; // TODO union
      nextFrameToProcess = destTime;
      ageInSamples = 0;

      break;
    }
     
    case Event::kNoteSustain:
      state = kSustain;
      break;
      
    case Event::kNoteOff:
    {
      state = kOff;
      creatorID = 0;
      
      size_t destTime = e.time;
      destTime = clamp(destTime, 0UL, (size_t)kFloatsPerDSPVector);
      
      // write current values up to change TODO DRY
      for(size_t t = nextFrameToProcess; t < destTime; ++t)
      {
        outputs.row(kVelocity)[t] = currentVelocity;
        
        // TODO sample accurate glide
        outputs.row(kPitch)[t] = currentPitch;
        
        outputs.row(kElapsedTime)[t] = getAgeInSeconds(ageInSamples++, _sampleRate);
      }
      
      // set new values
      currentVelocity = 0.;
      nextFrameToProcess = destTime;
      ageInSamples = 0;
      break;
    }
    default:
      state = kOff;
      ageInSamples = 0;
      break;
  }
}

void EventsToSignals::Voice::endProcess()
{
  for(size_t t = nextFrameToProcess; t < kFloatsPerDSPVector; ++t)
  {
    // write velocity to end of buffer.
    outputs.row(kVelocity)[t] = currentVelocity;
    
    // write pitch to end of buffer.
    // TODO sample accurate glide
    outputs.row(kPitch)[t] = currentPitch;
    
    // keep increasing age
    outputs.row(kElapsedTime)[t] = getAgeInSeconds(ageInSamples++, _sampleRate);
  }
  
  // process glides, accurate to the DSP vector
  outputs.row(kPitchBend) = pitchBendGlide(currentPitchBend);
  outputs.row(kMod) = modGlide(currentMod);
  outputs.row(kX) = xGlide(currentX);
  outputs.row(kY) = yGlide(currentY);
  outputs.row(kZ) = zGlide(currentZ);
}

#pragma mark -
//
// EventsToSignals
//

EventsToSignals::EventsToSignals(int sr) : _eventQueue(kMaxEventsPerVector)
{
  _sampleRate = sr;
  
  voices.resize(kMaxVoices);
  
  for(int i=0; i<kMaxVoices; ++i)
  {
    voices[i].setSampleRate(sr);
    voices[i].reset();
    voices[i].outputs.row(kVoice) = DSPVector(i);
  }
}

EventsToSignals::~EventsToSignals()
{
}

size_t EventsToSignals::setPolyphony(int n)
{
  _polyphony = std::min(n, kMaxVoices);
  return _polyphony;
}

size_t EventsToSignals::getPolyphony()
{
  return _polyphony;
}

void EventsToSignals::reset()
{
  _eventQueue.clear();
  
  for(auto& v : voices)
  {
    v.reset();
  }
  
  _voiceRotateOffset = 0;
}

void EventsToSignals::addEvent(const Event& e)
{
  _eventQueue.push(e);
}

void EventsToSignals::process()
{
  for(auto& v : voices)
  {
    v.beginProcess();
  }
  while(Event e = _eventQueue.pop())
  {
    processEvent(e);
  }
  for(auto& v : voices)
  {
    v.endProcess();
  }
}

// process one incoming event by making the appropriate changes in state and change lists.
void EventsToSignals::processEvent(const Event &eventParam)
{
  Event event = eventParam;
  
  switch(event.type)
  {
    case Event::kNoteOn:
      processNoteOnEvent(event);
      break;
    case Event::kNoteOff:
      processNoteOffEvent(event);
      break;
    case Event::kController:
      processControllerEvent(event);
      break;
    case Event::kPitchWheel:
      processPitchWheelEvent(event);
      break;
    case Event::kNotePressure:
      processNotePressureEvent(event);
      break;
    case Event::kSustainPedal:
      processSustainEvent(event);
      break;
    case Event::kNull:
    default:
      break;
  }
}

void EventsToSignals::processNoteOnEvent(const Event& e)
{
  auto v = findFreeVoice(0, _polyphony);
  if(v >= 0)
  {
    _voiceRotateOffset++;
    voices[v].writeNoteEvent(e, _scale);
  }
  else
  {
    v = findVoiceToSteal(e);
    
    // steal it with retrigger
    // TODO: this may make some clicks when the previous notes
    // are cut off. add more graceful stealing
    Event f = e;
    f.type = Event::kNoteRetrig;
    voices[v].writeNoteEvent(f, _scale);
  }
}

void EventsToSignals::processNoteOffEvent(const Event& e)
{
  // send either off or sustain event to voices matching creator
  Event::Type newEventType = _sustainPedalActive ? Event::kNoteSustain : Event::kNoteOff;
  
  for(int v = 0; v < _polyphony; ++v)
  {
    Voice& voice = voices[v];
    if((voice.creatorID == e.creatorID) && (voice.state == Voice::kOn))
    {
      Event eventToSend = e;
      eventToSend.type = newEventType;
      voice.writeNoteEvent(eventToSend, _scale);
    }
  }
}

void EventsToSignals::processNotePressureEvent(const Event& event)
{
  for (int v=0; v<_polyphony; ++v)
  {
    voices[v].currentZ = event.value1;
  }
}

void EventsToSignals::processPitchWheelEvent(const Event& event)
{
  for (int v=0; v<_polyphony; ++v)
  {
    // write pitch bend
    voices[v].currentPitchBend = event.value1;
  }
}

// this handles all controller numbers
void EventsToSignals::processControllerEvent(const Event& event)
{
  float val = event.value1;
  int ctrl = event.value2;
  
  if(ctrl == 120)
  {
    if(val == 0)
    {
      // all sound off
      reset();
    }
  }
  else if(ctrl == 123)
  {
    if(val == 0)
    {
      // all notes off
      for(int v=0; v<_polyphony; ++v)
      {
        Voice& voice = voices[v];
        if(voice.state != Voice::kOff)
        {
          Event eventToSend = event;
          eventToSend.type = Event::kNoteOff;
          voice.writeNoteEvent(eventToSend, _scale);
        }
      }
    }
  }
  else
  {
    // modulate all voices.
    for (int v=0; v<_polyphony; ++v)
    {
      if(ctrl == 1)
      {
        voices[v].currentMod = val;
      }
      if(ctrl == 73)
      {
        voices[v].currentX = val;
      }
      else if(ctrl == 74)
      {
        voices[v].currentY = val;
      }
    }
  }
}

void EventsToSignals::processSustainEvent(const Event& event)
{
  _sustainPedalActive = (event.value1 > 0.5f) ? 1 : 0;
  if(!_sustainPedalActive)
  {
    // clear any sustaining voices
    for(int i=0; i<_polyphony; ++i)
    {
      Voice& v = voices[i];
      if(v.state == Voice::kSustain)
      {
        Event newEvent;
        newEvent.type = Event::kNoteOff;
        v.writeNoteEvent(newEvent, _scale);
      }
    }
  }
}


#pragma mark -

// return index of free voice or -1 for none.
// increments mVoiceRotateOffset.
//
int EventsToSignals::findFreeVoice(size_t start, size_t len)
{
  int r = -1;
  for (auto v = start; v < start + len; ++v)
  {
    auto vr = v;

    // rotate voices
    vr = (vr + _voiceRotateOffset) % len;

    if (voices[vr].state == Voice::kOff)
    {
      r = static_cast<int>(vr);
      break;
    }
  }
  return r;
}

int EventsToSignals::findVoiceToSteal(Event e)
{
  // just steal the voice with the nearest note.
  return findNearestVoice(e.creatorID);
}

// return the index of the voice with the note nearest to the note n.
// Must always return a valid voice index.
int EventsToSignals::findNearestVoice(int note)
{
  int r = 0;
  int minDist = 128;
  
  for (int v=0; v<_polyphony; ++v)
  {
    int vNote = voices[v].creatorID;
    int noteDist = std::abs(note - vNote);
    if (noteDist < minDist)
    {
      minDist = noteDist;
      r = v;
    }
  }
  return r;
}


void EventsToSignals::dumpVoices()
{
  std::cout << "voices:\n";
  for (int i=0; i<_polyphony; ++i)
  {
    std::cout << "    " << i << ": ";
    
    Voice& voice = voices[i];
    std::cout << "[i: " << voice.creatorID << "]";
    
    switch(voice.state)
    {
      case Voice::kOff:
        std::cout  << "off";
        break;
      case Voice::kOn:
        std::cout  << " on";
        break;
      case Voice::kSustain:
        std::cout  << "sus";
        break;
      default:
        std::cout  << " ? ";
        break;
    }
    std::cout  << "\n";
  }
}

}
 
