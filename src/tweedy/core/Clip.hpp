#ifndef __CLIP_HPP__
#define __CLIP_HPP__

#include <tweedy/core/Imedia.hpp>
#include <tweedy/core/PisteClip.hpp>
#include <tweedy/core/Id.hpp>

#include <boost/filesystem/path.hpp>

#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>

class Clip : public Imedia {
public:
    Clip(const boost::filesystem::path& url,const Id& idParent, const std::string& id)
        : Imedia(ImediaTypeClip),
        _id(idParent,id),
        _imgPath(url),
        _imgLength(0.0), 
        _imgwidth(0), 
        _imgHeight(0), 
        _imgreduction(0), 
        _timeIn(0), 
        _timeOut(0){ this->_name = url.filename();}
    
    
    Clip()
        : Imedia(ImediaTypeClip),
        _id(Id(""),""),
        _imgLength(0.0), 
        _imgwidth(0), 
        _imgHeight(0), 
        _imgreduction(0), 
        _timeIn(0), 
        _timeOut(0){}
   

    Clip& operator=(const Clip&);
        
    ~Clip(){}
    
    const boost::filesystem::path& imgPath() const { return _imgPath; }
    unsigned int timeIn() const { return _timeIn; }
    unsigned int timeOut() const { return _timeOut; }
    void setPosition(unsigned int ti, unsigned int to) { _timeIn = ti; _timeOut = to; }
    void setTimeIn(int value){ _timeIn += value; }
    void setTimeOut(int value){ _timeOut += value; }
    
    Id& getId(){ return _id; }
    
protected :
    boost::filesystem::path _imgPath;
    float _imgLength;
    unsigned int _imgwidth;
    unsigned int _imgHeight;
    unsigned int _imgreduction;
    unsigned int _timeIn;
    unsigned int _timeOut;
    
    Id _id;

    PisteClip * piste;
    
private:
    
    friend class boost::serialization::access;
    
    template<class Archive>
    void serialize(Archive &, const unsigned int);
    
    
};

#endif
