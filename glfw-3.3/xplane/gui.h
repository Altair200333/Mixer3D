#pragma once

class GUI
{
public:
	virtual ~GUI() = default;
	GUI() = default;
	virtual void onStart(Window* window) = 0;
	virtual void draw() = 0;
};