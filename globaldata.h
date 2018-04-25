#ifndef GLOBALDATA_H
#define GLOBALDATA_H

/*
 *Global Data
*/
#include <pthread.h>
#include <string>
#include <QObject>
#include <stdio.h>
#include <queue>
#include <mutex>

using namespace std;

template<typename T>
class ImageBuffQueue {
public:
    ImageBuffQueue()
    {
        while(!_queue.empty())
        {
            _queue.pop();
        }
    }
    ~ImageBuffQueue(){}

    void push(T data)
    {
        mutex.lock();
        _queue.push(data);
        mutex.unlock();
    }

    T front()
    {
        T ret = NULL;
        mutex.lock();
        if(_queue.size() > 0)
            ret = _queue.front();
        mutex.unlock();
        return ret;
    }

    void pop()
    {
        mutex.lock();
        _queue.pop();
        mutex.unlock();
    }

    ssize_t size()
    {
        ssize_t size = -1;
        mutex.lock();
        size = _queue.size();
        mutex.unlock();
        return size;
    }

private:
    std::mutex mutex;
    std::queue<T> _queue;
};

struct Frame{
    Frame(const void* src,uint32_t bufferSize, int widht, int height, double timestamp, uint64_t blockID):pData(nullptr)
    {
        if(!pData)
        {
            pData = new uint8_t[bufferSize];
            //memset(pData, 0, bufferSize);
        }
        memcpy(pData,src,bufferSize);
        nWidth = widht;
        nHeigth = height;
        dtimeStamp = timestamp;
        nBlockID = blockID;
    }
    ~Frame()
    {
        if(pData)
        {
            delete [] pData;
            pData = nullptr;
        }
    }

    uint64_t blockID()
    {
        return nBlockID;
    }

    double timeStamp()
    {
        return dtimeStamp;
    }

    int imageWidth()
    {
        return nWidth;
    }

    int imageHeight()
    {
        return nHeigth;
    }

    uint8_t* bufPtr()
    {
        return pData;
    }
private:
    uint8_t* pData;
    int nWidth;
    int nHeigth;
    double dtimeStamp;
    uint64_t nBlockID;
};

class GlobalData //: public QObject
{
private:
    GlobalData();
    ~GlobalData();

    static GlobalData* m_pInstance;
//private slots://TODO
 //   void returnValue(char *pData);
public:
    static GlobalData* GetInstance()
    {
        if(m_pInstance == nullptr)
        {
            m_pInstance = new GlobalData();
        }
        return m_pInstance;
    }
    typedef ImageBuffQueue<Frame*> _imageListType;

    _imageListType _leftImageQue;
    _imageListType _rightImageQue;

    static void* doWorkThread(void* param)
    {
        GlobalData* _this = (GlobalData*)param;
        _this->run();
    }
    void run();
private:

};

#endif // GLOBALDATA_H
