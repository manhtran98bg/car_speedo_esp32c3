/***************************************************
Copyright (c) 2020 Luis Llamas
(www.luisllamas.es)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program.  If not, see <http://www.gnu.org/licenses
****************************************************/
#include "Arduino.h"
#include "Face.h"
#include "FaceBehavior.h"
#include "FaceEmotions.hpp"
static const eEmotions moods[] = {	
  	eEmotions::Normal,
	eEmotions::Angry,
	eEmotions::Glee,
	eEmotions::Happy,
	eEmotions::Sad,
	eEmotions::Worried,
	eEmotions::Focused,
	eEmotions::Annoyed, //Khó chịu
	eEmotions::Surprised,
	eEmotions::Skeptic, //Người hoài nghi
	eEmotions::Frustrated, //Bực bội
	eEmotions::Unimpressed, //Không ấn tượng
	eEmotions::Sleepy, //Buồn ngủ
	eEmotions::Suspicious, //Khả nghi
	eEmotions::Squint, //Nheo mắt
	eEmotions::Furious, //Giận dữ
	eEmotions::Scared, //Sợ hãi
	eEmotions::Awe, //Kinh ngạc
}; 
FaceBehavior::FaceBehavior(Face& face) : _face(face), Timer(6000) {
	Timer.Start();
	UseAllEmotion();
}

void FaceBehavior::UseAllEmotion() {
	for (int i = 0; i < eEmotions::EMOTIONS_COUNT ; i ++) {
		Emotions[i] = moods[i];
	}
}

// Use roulette wheel to select a new emotion, based on assigned weights
eEmotions FaceBehavior::GetRandomEmotion() {
  	int index = esp_random() % eEmotions::EMOTIONS_COUNT;
	Serial.printf("Go to Emotion %d \n", index);
	return Emotions[index];
}

void FaceBehavior::Update() {
	Timer.Update();

	if (Timer.IsExpired()) {
		Timer.Reset();
		eEmotions newEmotion = GetRandomEmotion();
		if (CurrentEmotion != newEmotion) {
			GoToEmotion(newEmotion);
		}
	}
}

void FaceBehavior::GoToEmotion(eEmotions emotion) {
  // Set the currentEmotion to the desired emotion
	CurrentEmotion = emotion;

  // Call the appropriate expression transition function 
	switch (CurrentEmotion) {
    case eEmotions::Normal: _face.Expression.GoTo_Normal(); break;
    case eEmotions::Angry: _face.Expression.GoTo_Angry(); break;
    case eEmotions::Glee: _face.Expression.GoTo_Glee(); break;
    case eEmotions::Happy: _face.Expression.GoTo_Happy(); break;
    case eEmotions::Sad: _face.Expression.GoTo_Sad(); break;
    case eEmotions::Worried: _face.Expression.GoTo_Worried(); break;
    case eEmotions::Focused: _face.Expression.GoTo_Focused(); break;
    case eEmotions::Annoyed: _face.Expression.GoTo_Annoyed(); break;
    case eEmotions::Surprised: _face.Expression.GoTo_Surprised(); break;
    case eEmotions::Skeptic: _face.Expression.GoTo_Skeptic(); break;
    case eEmotions::Frustrated: _face.Expression.GoTo_Frustrated(); break;
    case eEmotions::Unimpressed: _face.Expression.GoTo_Unimpressed(); break;
    case eEmotions::Sleepy: _face.Expression.GoTo_Sleepy(); break;
    case eEmotions::Suspicious: _face.Expression.GoTo_Suspicious(); break;
    case eEmotions::Squint: _face.Expression.GoTo_Squint(); break;
    case eEmotions::Furious: _face.Expression.GoTo_Furious(); break;
    case eEmotions::Scared: _face.Expression.GoTo_Scared(); break;
    case eEmotions::Awe: _face.Expression.GoTo_Awe(); break;
    default: break;
	}
}