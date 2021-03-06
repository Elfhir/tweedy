#include <tweedy/core/Timeline.hpp>
#include <string>

#include <boost/lexical_cast.hpp>

Timeline::Timeline( const Id& idParent, const std::string& id )
: Imedia( ImediaTypeTimeline )
, _maxTime( 2 )
, _nbClip( 2 )
, _id( idParent, id )
{
    Clip blank( "img/none.jpg", getId(), "none" );
    blank.setPosition( 0, 1 );
    _mapClip[blank.getId().getIdStringForm()] = blank;


    Clip realTime( "img/flux.jpg", getId(), "flux" );
    realTime.setPosition( 1, 2 );
    _idRealTime = realTime.getId().getIdStringForm();
    _mapClip[_idRealTime] = realTime;
}

Timeline::Timeline( const Timeline& timeline )
: Imedia( timeline )
, _id( timeline._id )
{
	_maxTime = timeline._maxTime;
	_mapClip = timeline._mapClip;
}

Timeline::OMapClip Timeline::getOrderedClips()
{
	OMapClip orderedClips;

	BOOST_FOREACH( const UOMapClip::value_type& s, _mapClip )
	{
		orderedClips[s.second->timeIn()] = s.second;
	}
	return orderedClips;
}

const Timeline::OMapClip Timeline::getOrderedClips() const
{
	return const_cast<Timeline&> ( *this ).getOrderedClips();
}

const Id& Timeline::getId() const
{
	return _id;
}

void Timeline::insertClip( Clip& newClip, double currentTime )
{
        int duration = newClip.timeOut() - newClip.timeIn();

        if (currentTime < _maxTime && time > 0)
        {
            std::string clipName = findCurrentClip( currentTime );
            currentTime = _mapClip[clipName].timeIn();
        }

        //move back a row next clips
        BOOST_FOREACH( const UOMapClip::value_type& s, _mapClip )
	{
                if( s.second->timeIn() >= currentTime )
		{
                        s.second->increaseTimeIn( duration );
                        s.second->increaseTimeOut( duration );
		}
	}

        newClip.setPosition( currentTime, currentTime+duration );
	_mapClip[newClip.getId().getIdStringForm()] = newClip;

        _maxTime += duration;

	_signalChanged();
}

void Timeline::moveElement( std::string clipName, int newPosition )
{
	int difference = newPosition - _mapClip[clipName].timeIn();

	if( newPosition >= _maxTime || difference == 0 )
		return;

	int dureeClip = _mapClip[clipName].timeOut() - _mapClip[clipName].timeIn();

        std::string triggeredFilename = findCurrentClip( newPosition );

	int addedValueCurrent;

	if( difference > 0 )
	{
		addedValueCurrent = _mapClip[triggeredFilename].timeOut() - dureeClip;

		BOOST_FOREACH( const Timeline::UOMapClip::value_type& s, _mapClip )
		{
                    if( s.second->timeIn() > _mapClip[clipName].timeIn() && s.second->timeIn() <= newPosition )
                    {
                        s.second->increaseTimeIn( -dureeClip );
                        s.second->increaseTimeOut( -dureeClip );
                    }
		}
	}
	else
	{
		addedValueCurrent = _mapClip[triggeredFilename].timeIn();

		BOOST_FOREACH( const Timeline::UOMapClip::value_type& s, _mapClip )
		{
                    if( s.second->timeIn() >= addedValueCurrent && s.second->timeIn() < _mapClip[clipName].timeIn() )
                    {
                        s.second->increaseTimeIn( dureeClip );
                        s.second->increaseTimeOut( dureeClip );
                    }
		}
	}


	_mapClip[clipName].setPosition( addedValueCurrent, addedValueCurrent + dureeClip );

	_signalChanged();

}

void Timeline::addTimeToClip( const std::string& clipName, double decalage )
{
	if( decalage < 0 )
	{
		double dureeClip = _mapClip[clipName].timeOut() - _mapClip[clipName].timeIn();
		if( -decalage >= dureeClip )
			decalage = -dureeClip + 1;
	}

	_mapClip[clipName].increaseTimeOut( decalage );

	BOOST_FOREACH( const UOMapClip::value_type& s, _mapClip )
	{
		if( s.second->timeIn() > _mapClip[clipName].timeIn() )
		{
			s.second->increaseTimeIn( decalage );
			s.second->increaseTimeOut( decalage );
		}
	}

	_maxTime += decalage;

	_signalChanged();

}

std::string Timeline::findCurrentClip(int time) const
{
    if (time >= _maxTime || time < 0)
        return "img/none.jpg";

    OMapClip orderedClips = getOrderedClips();

    BOOST_FOREACH( const OMapClip::value_type& s, orderedClips )
    {
        if( s.first <= time && ( *s.second )->timeOut() > time )
            return ( *s.second )->getId().getIdStringForm();
    }
}

void Timeline::updateMaxTime()
{
	unsigned int max = 0;

	BOOST_FOREACH( const UOMapClip::value_type& s, _mapClip )
	{
		if( s.second->timeOut() > max )
			max = s.second->timeOut();
	}
	_maxTime = max;
}

void Timeline::deleteClip(const std::string& clipName)
{
    //get the clip duration
    Clip c = _mapClip[clipName];
    int duration = c.timeOut() - c.timeIn();

    //move back a row the other clips
    BOOST_FOREACH( const UOMapClip::value_type& s, _mapClip )
    {
        if( s.second->timeIn() > c.timeIn() )
        {
            s.second->increaseTimeIn( -duration );
            s.second->increaseTimeOut( -duration );
        }
    }

    //remove clip from the map
    UOMapClip::iterator it = _mapClip.find( clipName );
    BOOST_ASSERT( it != _mapClip.end() );
    _mapClip.erase( it );

    _maxTime -= duration;
    _signalChanged();
}

void Timeline::unselectAll()
{
        BOOST_FOREACH( const UOMapClip::value_type& s, _mapClip )
        {
                s.second->setSelected(false);
        }
}

void Timeline::selectClip(int timeIn)
{
   OMapClip orderedClips = getOrderedClips();
   if (orderedClips[timeIn]->getSelected())
        orderedClips[timeIn]->setSelected(false);
   else
       orderedClips[timeIn]->setSelected(true);
}

boost::signal0<void>& Timeline::getSignalChanged()
{
	return _signalChanged;
}
