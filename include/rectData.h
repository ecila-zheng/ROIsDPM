#ifndef RECTDATA_H
#define RECTDATA_H

#include <fstream>
#include <list>
#include <opencv2/opencv.hpp>

struct FrameRect
{
	FrameRect(const unsigned int &numOfFrame)
	{
		_numOfFrame = numOfFrame;
	}

	FrameRect(const unsigned int &numOfFrame, const std::list<CvRect*> &rects)
	{
		_numOfFrame = numOfFrame;
		_rects = rects;
	}

	FrameRect(const FrameRect &fr)
	{
		_numOfFrame = fr._numOfFrame;
		_rects = fr._rects;
	}

	~FrameRect()
	{
		
	}

	FrameRect& operator=(const FrameRect& fr)
	{
		if (this != &fr)
		{
			_numOfFrame = fr._numOfFrame;
			_rects = fr._rects;
		}
		return *this;
	}

	void clear()
	{
		for each (CvRect* ptr in _rects)
			delete ptr;
		_rects.clear();
	}

	void addRect(CvRect *rect)
	{
		_rects.push_back(rect);
	}

	bool deleteRects(const std::vector<CvRect*> &rects)
	{
		for each (CvRect* deleted in rects)
			for (std::list<CvRect*>::iterator it = _rects.begin(); it != _rects.end();)
			{
				if (deleted == *it)
					it = _rects.erase(it);
				else ++it;
			}
	
		return _rects.empty();
	}

	void addRects(const std::vector<CvRect*> &rects)
	{
		for each (CvRect* r in rects)
			_rects.push_back(r);
	}

	unsigned int _numOfFrame;
	std::list<CvRect*> _rects;
};

struct RectData
{
	RectData(const unsigned int frameCount, const cv::Size &vidSize)
	{
		_frameCount = frameCount;
		_vidSize = vidSize;
	}

	~RectData()
	{
		for each (FrameRect fr in _allRects)
			fr.clear();
		_allRects.clear();
	}

	void addRects(const unsigned int &numOfFrame, const std::vector<CvRect*> &rects)
	{
		std::list<FrameRect>::iterator tmpFR = getFrameRectIt(numOfFrame);
		if (_allRects.empty())
		{
			_allRects.push_back(FrameRect(numOfFrame));
			tmpFR = --_allRects.end();
		}
		else if (tmpFR == _allRects.end())
		{
			for (std::list<FrameRect>::iterator it = _allRects.begin();
				it != _allRects.end(); it++)
				if (it->_numOfFrame > numOfFrame)
				{
					_allRects.insert(it, FrameRect(numOfFrame));
					tmpFR = --it;
					break;
				}
			if (tmpFR == _allRects.end())
			{
				_allRects.push_back(FrameRect(numOfFrame));
				tmpFR = --_allRects.end();
			}
		}
		tmpFR->addRects(rects);
	}

	void deleteFrame(const unsigned int numOfFrame)
	{
		_allRects.erase(getFrameRectIt(numOfFrame));
	}

	bool deleteRect(const unsigned int numOfFrame, const std::vector<CvRect*> &rects)
	{
		std::list<FrameRect>::iterator tmpIt = getFrameRectIt(numOfFrame);

		bool isEmpty = tmpIt->deleteRects(rects);

		if (isEmpty) _allRects.erase(tmpIt);

		return isEmpty;
	}

	std::list<FrameRect>::iterator getFrameRectIt(const unsigned int numOfFrame)
	{
		if (!_allRects.empty())
			for (std::list<FrameRect>::iterator it = _allRects.begin();
				it != _allRects.end(); it++)
				if (it->_numOfFrame == numOfFrame)
					return it;
		return _allRects.end();
	}

	FrameRect* getFrameRectPtr(const unsigned int numOfFrame)
	{
		std::list<FrameRect>::iterator tempIt = getFrameRectIt(numOfFrame);
		if (tempIt != _allRects.end())
			return &*tempIt;
		else return NULL;
	}
	
	unsigned int _frameCount;
	cv::Size _vidSize;
	std::list<FrameRect> _allRects;
};

inline bool readFile(const std::string fileName, RectData* &dataPtr)
{
	std::ifstream fin(fileName, std::ifstream::binary);
	if (!fin) return false;

	cv::Size tmpSize;
	unsigned int tmpCount;
	size_t tmpRectsSize;

	fin.read((char*)&tmpSize, sizeof(cv::Size));
	fin.read((char*)&tmpCount, sizeof(unsigned int));
	fin.read((char*)&tmpRectsSize, sizeof(size_t));

	dataPtr = new RectData(tmpCount, tmpSize);

	for (size_t i = 0; i < tmpRectsSize; i++)
	{
		unsigned int tmpNum;
		fin.read((char*)&tmpNum, sizeof(unsigned int));
		dataPtr->_allRects.push_back(FrameRect(tmpNum));
		std::list<FrameRect>::iterator tmpFR = --dataPtr->_allRects.end();
		size_t tmpListSize;
		fin.read((char*)&tmpListSize, sizeof(size_t));
		for (size_t j = 0; j < tmpListSize; j++)
		{
			CvRect tmpRect;
			fin.read((char*)&tmpRect, sizeof(CvRect));
			tmpFR->_rects.push_back(new CvRect(tmpRect));
		}
	}
	fin.close();
	return true;
}

inline bool writeFile(const std::string fileName, RectData* dataPtr)
{
	std::ofstream fout(fileName, std::ofstream::binary);
	if (!fout) return false;

	cv::Size tmpSize = dataPtr->_vidSize;
	unsigned int tmpCount = dataPtr->_frameCount;
	size_t tmpRectsSize = dataPtr->_allRects.size();

	fout.write((const char*)(&tmpSize), sizeof(cv::Size));
	fout.write((const char*)(&tmpCount), sizeof(unsigned int));
	fout.write((const char*)(&tmpRectsSize), sizeof(size_t));

	for each (FrameRect frame in dataPtr->_allRects)
	{
		unsigned int tmpNum = frame._numOfFrame;
		fout.write((const char*)(&tmpNum), sizeof(unsigned int));
		size_t tmpListSize = frame._rects.size();
		fout.write((const char*)(&tmpListSize), sizeof(size_t));
		for each (CvRect* ptr in frame._rects)
		{
			CvRect tmpRect = *ptr;
			fout.write((const char*)&tmpRect, sizeof(CvRect));
		}
	}
	fout.close();
	return true;
}

#endif // RECTDATA_H