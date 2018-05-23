#pragma once

using namespace std;
class ProgressBar
{
public:
	ProgressBar();

	void setTotalSize(const UINT64 &totalSize);
	void setBarSize(const int size);

	void update(const UINT64 &curSize);
	void draw(ostream& os);

	void updateAndDraw(ostream& os, const UINT64 &cursize);

	~ProgressBar();

private:
	clock_t startTime;
	clock_t pinTime;
	clock_t countTime;
	int barSize = 10; //default

	UINT64 speed = 0;
	UINT64 totalSize = 0;
	UINT64 curSize = 0;
};

