#ifndef PROJET_HPP
#define PROJET_HPP

#include <tweedy/core/Singleton.hpp>
#include "tweedy/core/Gphoto/Gphoto.hpp"
#include <tweedy/core/Timeline.hpp>
#include <tweedy/core/ChutierMediaExt.hpp>
#include <tweedy/core/Imedia.hpp>
#include <tweedy/core/CommandManager.hpp>
#include <tweedy/core/Timeline.hpp>
#include <tweedy/core/Id.hpp>


#include<iostream>
#include <boost/assign/ptr_map_inserter.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/ptr_container/ptr_unordered_map.hpp>


class Projet : public Singleton<Projet>
{
  friend class Singleton<Projet>;

private:
  Projet ()
      : _value (0),_id("projet"),_timeline(getId(),"timeline1") {}
  ~Projet () { }

public:
  void setValue (int val) { _value = val; }
  int getValue () { return _value; }
  void setGphotoInstance () { Gphoto::getInstance ();};
  ChutierMediaExt& getChutierMediaExt();
  ChutierMediaExt& getChutierPictures();

  void setValueCameraIsInit (bool val);

  //static boost::ptr_vector<Imedia> getListMedia(){ return listMedia; }
  static boost::ptr_unordered_map<std::string, Imedia> getMapMedia() {}
  Imedia getImedia(int idMedia) { }
  boost::filesystem::path getNameImedia (Imedia & imedia);
  void addImedia(Imedia & media);
  void supprImedia(int idMedia);
  void printAllMedia();
  //void makeChutier();

  //function for gPhotoInstance
  int tryToConnectCamera ();
  //bool getValueCameraIsInit();
  void setFolderToSavePictures ();
  boost::filesystem::path captureToFile();


  Gphoto& gPhotoInstance() { return Gphoto::getInstance(); }
  
  CommandManager& getCommandManager();
  Timeline& getTimeline();
  
  Id& getId();

private:
  //Gphoto * gPhotoInstance;
  ChutierMediaExt _chutierMediaExt;
  ChutierMediaExt _chutierPictures;
  boost::filesystem::path projectFolder;
  int _value;

  //static boost::ptr_vector<Imedia> listMedia;
  boost::ptr_unordered_map<std::string, Imedia> _mapMedia;

  CommandManager _cmdManager;
  Timeline _timeline;
  
  Id _id;

};
#endif  // PROJET_HPP
