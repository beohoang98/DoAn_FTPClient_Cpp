#include "stdafx.h"
#include "ProgressBar.h"

#include <iostream>
#include <time.h>
#include <iomanip>

using namespace std;

ProgressBar::ProgressBar()
{
	startTime = clock();
	pinTime = startTime;
}

void ProgressBar::setTotalSize(const UINT64 &totalSize)
{
	this->totalSize = totalSize;
}

void ProgressBar::setBarSize(const int size)
{
	this->barSize = size;
}


void ProgressBar::update(const UINT64 &curSize)
{
	this->countTime = clock() - this->startTime;
	this->curSize = curSize;

	if (this->countTime > 0)
		this->speed = (this->curSize * CLOCKS_PER_SEC / this->countTime);
	else
		this->speed = 0;
}

void ProgressBar::draw(ostream& os)
{
	UINT64 progressSize = curSize * barSize / totalSize;
	if (progressSize > barSize)
		progressSize = barSize;

	UINT64 esTime = (speed > 0) ? (totalSize - curSize) / speed : 0;
	int hours = esTime / 3600;
	int min = (esTime / 60) % 60;
	int sec = (esTime % 60);

	//progress bar
	os << "\r\t[";
	for (int i = 0; i < (UINT)progressSize; ++i)
		os << char(219);
	for (int i = 0; i < barSize - progressSize; ++i)
		os << '-';
	os << "] ";

	//show downloaded bytes
	os << (curSize >> 10) << "/"// curSize in KB
		<< (totalSize >> 10) << "KB "; // totalSize in KB

	//show speed
	os << setw(9) << (speed >> 10) << "KB/s ";

	//show esTime
	os << setfill('0')
		<< setw(2) << hours << "h"
		<< setw(2) << min << "m"
		<< setw(2) << sec << "s"
		<< setfill(' ');
}


void ProgressBar::updateAndDraw(ostream& os, const UINT64 &cursize)
{
	if (totalSize <= 0) return;
	if (clock() - pinTime > CLOCKS_PER_SEC)
	{
		pinTime = clock();
		this->update(cursize);
		this->draw(os);
	}
}



ProgressBar::~ProgressBar()
{
}
