/***************************************************
Copyright (c) 2020 Luis Llamas
(www.luisllamas.es)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program.  If not, see <http://www.gnu.org/licenses
****************************************************/

#include "Face.h"
#include "Common.h"

Face::Face(ICanvas* canvas, uint16_t eyeSize, uint16_t screenWidth, uint16_t screenHeight, uint16_t backgroundColor , uint16_t eyeColor)
	: _canvas(canvas), LeftEye(*this), RightEye(*this), Blink(*this), Look(*this), Behavior(*this), Expression(*this)
{
	Width = canvas->width();
	Height = canvas->height();
	EyeSize = eyeSize;

	CenterX = Width / 2;
	CenterY = Height / 2;

	LeftEye.IsMirrored = true;
	_x = (screenWidth - Width) / 2;
	_y = (screenHeight - Height) / 2;
	canvas->setBackgroundColor(backgroundColor);
	canvas->setForegroundColor(eyeColor);
	Behavior.Timer.Start();
	Behavior.GoToEmotion(eEmotions::Normal);
}

void Face::LookFront()
{
	Look.LookAt(0.0, 0.0);
}

void Face::LookRight()
{
	Look.LookAt(-1.0, 0.0);
}

void Face::LookLeft()
{
	Look.LookAt(1.0, 0.0);
}

void Face::LookTop()
{
	Look.LookAt(0.0, 1.0);
}

void Face::LookBottom()
{
	Look.LookAt(0.0, -1.0);
}

void Face::Wait(unsigned long milliseconds)
{
	unsigned long start;
	start = millis();
	while (millis() - start < milliseconds)
	{
		Draw();
	}
}

void Face::DoBlink()
{
	Blink.Blink();
}

void Face::Update()
{
	if (RandomBehavior)
		Behavior.Update();
	if (RandomLook)
		Look.Update();
	if (RandomBlink)
		Blink.Update();
	Draw();
}

void Face::Draw()
{
	_canvas->clear(0);
	// Draw left eye
	LeftEye.CenterX = CenterX - EyeSize / 2 - EyeInterDistance;
	LeftEye.CenterY = CenterY;
	LeftEye.Draw(_canvas);
	// Draw right eye
	RightEye.CenterX = CenterX + EyeSize / 2 + EyeInterDistance;
	RightEye.CenterY = CenterY;
	RightEye.Draw(_canvas);
	_canvas->push(_x, _y);
}
void Face::SetPos(int x, int y) 
{
	_x = x;
	_y = y;
}
