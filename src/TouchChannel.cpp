#include "TouchChannel.h"


void TouchChannel::init() {
  
  touch->init();
  
  if (!touch->isConnected()) {
    this->updateLeds(0xFF);
    return;
  }
  touch->calibrate();
  touch->clearInterupt();


  io->init();
  io->setDirection(MCP23017_PORTA, 0x00);           // set all of the PORTA pins to output
  io->setDirection(MCP23017_PORTB, 0b00001111);     // set PORTB pins 0-3 as input, 4-7 as output
  io->setPullUp(MCP23017_PORTB, 0b00001111);        // activate PORTB pin pull-ups for toggle switches
  io->setInputPolarity(MCP23017_PORTB, 0b00000000); // invert PORTB pins input polarity for toggle switches
  io->setInterupt(MCP23017_PORTB, 0b00001111);

  currSwitchStates = readSwitchStates();
  prevSwitchStates = currSwitchStates;

  for (int i = 0; i < 8; i++) {
    this->updateLeds(leds[i]);
    wait_ms(50);
  }
  this->updateLeds(0x00);
  this->setOctaveLed();

  dac->referenceMode(dacChannel, MCP4922::REF_UNBUFFERED);
  dac->gainMode(dacChannel, MCP4922::GAIN_1X);
  dac->powerMode(dacChannel, MCP4922::POWER_NORMAL);
}

// HANDLE ALL INTERUPT FLAGS
void TouchChannel::poll() {
  if (touchDetected) {
    handleTouch();
    touchDetected = false;
  }

  if (switchHasChanged) {
    wait_us(5); // debounce
    currSwitchStates = readSwitchStates();
    int modeSwitchState = currSwitchStates & 0b00000011;   // set first 6 bits to zero
    int octaveSwitchState = currSwitchStates & 0b00001100; // set first 4 bits, and last 2 bits to zero
    
    if (modeSwitchState != (prevSwitchStates & 0b00000011) ) {
      handleModeSwitch(modeSwitchState);
    }

    if (octaveSwitchState != (prevSwitchStates & 0b00001100) ) {
      handleOctaveSwitch(octaveSwitchState);
    }
    
    prevSwitchStates = currSwitchStates;
    switchHasChanged = false;
  }

  if (degrees->hasChanged[channel]) {
    handleDegreeChange();
  }

  if (mode == LOOPER && hasEventInQueue() && ETL ) {
    handleQueuedEvent(metronome->position);
  }
}

void TouchChannel::handleTouch() {
  touched = touch->touched();
  if (touched != prevTouched) {
    for (int i=0; i<8; i++) {
      // if it *is* touched and *wasnt* touched before, alert!
      if (touch->getBitStatus(touched, i) && !touch->getBitStatus(prevTouched, i)) {

        switch (mode) {
          case MONOPHONIC:
            triggerNote(i, currOctave, ON);
            break;
          case QUANTIZER:
            break;
          case LOOPER:
            ETL = false; // deactivate event triggering loop
            createEvent(metronome->position, i);
            writeLed(i, HIGH);
            break;
        }
      }
      // if it *was* touched and now *isnt*, alert!
      if (!touch->getBitStatus(touched, i) && touch->getBitStatus(prevTouched, i)) {
        
        switch (mode) {
          case MONOPHONIC:
            triggerNote(i, currOctave, OFF);
            break;
          case QUANTIZER:
            break;
          case LOOPER:
            addEvent(metronome->position);
            writeLed(i, LOW);
            ETL = true; // activate event triggering loop
            break;
        }

      }
    }
    // reg.writeByte(touched); // toggle channel LEDs
    prevTouched = touched;
  }
}


/**
 * mode switch states determined by the last 2 bits of io's port B
**/
void TouchChannel::handleModeSwitch(int state) {
  
  switch (state) {
    case 0b00000011:
      ETL = false;
      mode = MONOPHONIC;
      if (currNoteState == ON) {
        triggerNote(prevNoteIndex, currOctave, ON);
      }
      break;
    case 0b00000010:
      ETL = false;
      mode = QUANTIZER;
      triggerNote(prevNoteIndex, currOctave, OFF);
      break;
    case 0b00000001:
      ETL = true;
      mode = LOOPER;
      triggerNote(prevNoteIndex, currOctave, OFF);
      break;
  }
}


/**
 * octave switch states determined by bits 5 and 6 of io's port B
**/
void TouchChannel::handleOctaveSwitch(int state) {
  // update the octave value
  switch (state) {
    case OCTAVE_UP:
      if (currOctave < 3) { currOctave += 1; }
      break;
    case OCTAVE_DOWN:
      if (currOctave > 0) { currOctave -= 1; }
      break;
  }
  setOctaveLed();

  if (state == OCTAVE_UP || state == OCTAVE_DOWN) {  // only want this to happen once
    switch (mode) {
      case MONOPHONIC:
        if (currNoteState == ON) {
          triggerNote(prevNoteIndex, prevOctave, OFF);
          wait_us(5);
          triggerNote(prevNoteIndex, currOctave, ON);
        }
        break;
      case QUANTIZER:
        break;
      case LOOPER:
        break;
    }
  }
  prevOctave = currOctave;
}

void TouchChannel::handleDegreeChange() {
  switch (mode) {
    case MONOPHONIC:
        if (currNoteState == ON) {
          triggerNote(prevNoteIndex, currOctave, OFF);
          wait_us(5);
          triggerNote(prevNoteIndex, currOctave, ON);
        }
      break;
    case QUANTIZER:
      break;
    case LOOPER:
      break;
  }
  degrees->hasChanged[channel] = false;
}

int TouchChannel::readSwitchStates() {
  return io->digitalRead(MCP23017_PORTB) & 0xF;
}

void TouchChannel::writeLed(int index, int state) {
  switch (state) {
    case HIGH:
      ledStates |= 1 << index;
      io->digitalWrite(MCP23017_PORTA, ledStates);
      break;
    case LOW:
      ledStates &= ~(1 << index);
      io->digitalWrite(MCP23017_PORTA, ledStates);
      break;
  }
  
}

void TouchChannel::updateLeds(uint8_t touched) {
  io->digitalWrite(MCP23017_PORTA, touched);
}

void TouchChannel::setOctaveLed() {
  int state = 1 << (currOctave + 4);
  io->digitalWrite(MCP23017_PORTB, state);
}

void TouchChannel::createEvent(int position, int noteIndex) {
  newEvent = new EventNode;
  newEvent->index = noteIndex;
  newEvent->startPos = position;
  newEvent->triggered = false;
  newEvent->next = NULL;
}

void TouchChannel::addEvent(int position) {
  newEvent->endPos = position;
  if (head == NULL) { // initialize the list
    head = newEvent;
    queued = head;
  }
  // algorithm
  else {
    // iterate over all existing events in list, placing the new event in the list relative to those events
    EventNode *iteration = head;
    EventNode *prev = head;

    while(iteration != NULL) {
      
      // OCCURS BEFORE ITERATION->START
      if (newEvent->startPos < iteration->startPos) {
        
        // does the new event end before the iteration starts?
        if (newEvent->endPos <= iteration->startPos) {
          
          // If iteration is head, reset head to new event
          if (iteration == head) {
            newEvent->next = iteration;
            head = newEvent;
            break;
          }
          
          // place inbetween the previous iteration and current iteration
          else {
            prev->next = newEvent;
            newEvent->next = iteration;
            break;
          }
        }
        
        // newEvent overlaps the current iteration start? delete this iteration and set iteration to iteration->next
        else {
          if (iteration == head) { // if iteration is head, set head to the newEvent
            head = newEvent;
          } else {
            prev->next = newEvent; // right here
          }
          
          EventNode *temp = iteration;
          iteration = iteration->next;    // repoint to next iteration in loop
          delete temp;                    // delete current iteration
          queued = iteration ? iteration : head;
          continue;                       // continue the while loop for further placement of newEvent
        }
      }
      
      // OCCURS AFTER ITERATION->START
      else {
        
        // if the newEvent begins after the end of current iteration, move on to next iteration
        if (newEvent->startPos > iteration->endPos) {
          if (iteration->next) {
            prev = iteration;
            iteration = iteration->next;
            continue;
          }
          iteration->next = newEvent;
          break;
        }
        
        // if newEvent starts before iteration ends, update end position of current iteration and place after
        else {
          iteration->endPos = newEvent->startPos - 2;
          prev = iteration;
          iteration = iteration->next;
          continue;
        }
      }
    } // END OF WHILE LOOP
  }
  newEvent = NULL;

}


bool TouchChannel::hasEventInQueue() {
  if (queued) {
    return true;
  } else {
    return false;
  }
}

void TouchChannel::handleQueuedEvent(int position) {
  if (queued->triggered == false ) {
    if (position == queued->startPos) {
      triggerNote(queued->index, currOctave, ON);
      queued->triggered = true;
    }
  }
  else if (position == queued->endPos) {
    triggerNote(queued->index, currOctave, OFF);
    queued->triggered = false;
    if (queued->next != NULL) {
      queued = queued->next;
    } else {
      queued = head;
    }
  }
}

void TouchChannel::triggerNote(int index, int octave, NoteState state) {
  switch (state) {
    case ON:
      currNoteIndex = index;
      currNoteState = ON;
      gateOut.write(HIGH);
      writeLed(index, HIGH);
      dac->write_u16(dacChannel, calculateDACNoteValue(index, octave));
      midi->sendNoteOn(channel, calculateMIDINoteValue(index, octave), 100);
      break;
    case OFF:
      currNoteState = OFF;
      gateOut.write(LOW);
      writeLed(index, LOW);
      midi->sendNoteOff(channel, calculateMIDINoteValue(index, octave), 100);
      break;
  }
  prevNoteIndex = index;
}

int TouchChannel::calculateDACNoteValue(int index, int octave) {
  return DAC_NOTE_MAP[index][degrees->switchStates[index]] + DAC_OCTAVE_MAP[octave];
}

int TouchChannel::calculateMIDINoteValue(int index, int octave) {
  return MIDI_NOTE_MAP[index][degrees->switchStates[index]] + MIDI_OCTAVE_MAP[octave];
}


// ensuing, succeeding