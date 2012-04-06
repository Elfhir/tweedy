import QtQuick 1.1

Item {
        id: tw_clipContainer
        width: (object.timeOut - object.timeIn + object.blankDuration) * _tw_timelineData.timelineScale
        height: tw_track.height

        scale: tw_clipHandle.pressed ? 1.03 : 1.0


        property int clipRightPressed : 0
        property int clipLeftPressed : 0
        property bool clipSelected : false

        Rectangle {
                id: tw_blankClip

                width: object.blankDuration * _tw_timelineData.timelineScale // * 0.5
                height: tw_track.height

                y: 0
                z: -1

                border.color: (doubleClickedBlank != object.timeIn)?"black":"white"
                border.width: 2
                radius: 10
                color:(timeBlankSelected != object.timeIn)?"black":"red"

                // zone de deplacement
                MouseArea {
                        id: tw_blankClipHandle

                        anchors.fill: tw_blankClip
                        hoverEnabled: true
                        //drag.minimumX: 0
                        //drag.maximumX: _tw_timelineData.maxTime * _tw_timelineData.timelineScale - _tw_timelineData.timelineScale
                        //drag.target: tw_blankClip
                        //drag.axis: "XAxis"


                        onPressed: {
                            clipSelected = false
                            timeBlankSelected = object.timeIn
                            timeInClipSelected = -1
                            //parent.color = 'red'
                        }

                        onDoubleClicked:{
                            timeInDoubleClickedClip = -1
                            doubleClickedBlank = object.timeIn
                        }


                }

                // zone gauche pour l'agrandissement du clip
                MouseArea {
                        id: tw_blankClipLeftHandle

                        width: tw_handleWidth
                        height: tw_clip.height
                        hoverEnabled: true

                        onEntered: {
                                _tw_timelineData.displayCursor("scale");
                                console.log("qml tw_blankClipLeftHandle onEntered.")
                        }
                        onExited: {
                                _tw_timelineData.displayCursor("none");
                                console.log("qml tw_blankClipLeftHandle onExited.")
                        }
                }

                // zone droite pour l'agrandissement du clip
                MouseArea {
                        id: tw_blankClipRightHandle

                        width: tw_handleWidth
                        height: tw_clip.height
                        anchors.right: parent.right
                        //x: tw_clip.width - width
                        hoverEnabled: true

                        onEntered: {
                                _tw_timelineData.displayCursor("scale");
                                console.log("qml tw_blankClipRightHandle onEntered.")
                        }
                        onExited: {
                                _tw_timelineData.displayCursor("none");
                                console.log("qml tw_blankClipRightHandle onExited.")
                        }
                }
        }//end blank clip

    Rectangle {
            id: tw_clip

            //anchors.fill: parent
            width: (object.timeOut - object.timeIn) * _tw_timelineData.timelineScale // * 0.5
            height: tw_track.height

            x: object.blankDuration * _tw_timelineData.timelineScale
            y: 0
            z: -1

            border.color: (timeInDoubleClickedClip != object.timeIn)?"black":"white"
            border.width: 2
            radius: 10
            color:(timeInClipSelected != object.timeIn)?"#e28a26":"red"

            // image du Clip
            Image {
                    id:image
                    anchors.fill: parent
                    anchors.margins: 5
                    width: parent.width
                    height: parent.height
                    fillMode: Image.PreserveAspectFit
                    smooth: true
                    source:object.imgPath
                    asynchronous: true
                 }

            // zone de deplacement
            MouseArea {
                    id: tw_clipHandle

                    anchors.fill: tw_clip
                    hoverEnabled: true
                    //drag.minimumX: 0
                    //drag.maximumX: _tw_timelineData.maxTime * _tw_timelineData.timelineScale - _tw_timelineData.timelineScale
                    drag.target: tw_clip
                    drag.axis: "XAxis"
                    acceptedButtons: Qt.LeftButton | Qt.RightButton

                    onEntered: {
                        if( timeInClipSelected != object.timeIn  )
                            parent.color= '#FDAE37'
                    }

                    onClicked: {
                        /*_tw_timelineData.displayCurrentClip(tw_timeCursor.x / _tw_timelineData.timelineScale)

                        timeInClipSelected = tw_timeCursor.x / _tw_timelineData.timelineScale

                        console.log( " timeInClipSelected" + timeInClipSelected)

                        parent.color = 'red'
                        console.log("qml tw_clipHandle onPressed.")
                        timeInClipSelected = object.timeIn
                        parent.z = 9999;*/

                    }


                    onExited:  {
                        parent.z = -1;
                        if( timeInClipSelected != object.timeIn  )
                            parent.color = '#e28a26'
                    }

                    onPressed: {
                        timeBlankSelected = -1

                        clipSelected = true
                        console.log("qml tw_clipHandle onPressed.")
                        timeInClipSelected = object.timeIn

                        parent.color = 'red'
                    }

                    onDoubleClicked:{
                        doubleClickedBlank = -1
                        timeInDoubleClickedClip = object.timeIn
                        _tw_timelineData.displayClip(timeInDoubleClickedClip)
                    }

                    onPositionChanged: {
                        if (clipSelected)
                        {
                            var markerPosition = -1

                            if(parent.x / _tw_timelineData.timelineScale >= 1)
                                markerPosition = _tw_timelineData.getMarkerPosition(timeInClipSelected + parent.x / _tw_timelineData.timelineScale - object.blankDuration, true)
                            if(parent.x / _tw_timelineData.timelineScale < 0)
                                markerPosition = _tw_timelineData.getMarkerPosition(timeInClipSelected + parent.x / _tw_timelineData.timelineScale - object.blankDuration, false)


                            if (markerPosition != -1)
                            {
                                tw_marker.visible = true
                                tw_marker.x = markerPosition * _tw_timelineData.timelineScale - tw_marker.width / 2
                            }
                        }
                    }

                    onReleased: {
                        tw_marker.visible = false

                        if (clipSelected)
                        {
                            print ("timeInClipSelected, timeInClipSelected + parent.x / _tw_timelineData.timelineScale",timeInClipSelected, timeInClipSelected + parent.x / _tw_timelineData.timelineScale)
                            var selectedClip = -1
                                selectedClip = _tw_timelineData.translate(timeInClipSelected, timeInClipSelected + parent.x/_tw_timelineData.timelineScale - object.blankDuration)
                            if (selectedClip != -1)
                                timeInClipSelected = selectedClip
                        }

                    }
            }

            // zone gauche pour l'agrandissement du clip
            MouseArea {
                    id: tw_clipLeftHandle

                    width: tw_handleWidth
                    height: tw_clip.height
                    hoverEnabled: true

                    onEntered: {
                            _tw_timelineData.displayCursor("scale");

                    }
                    onExited: {
                            _tw_timelineData.displayCursor("none");

                    }
                    onPressed: {
                        console.log("qml tw_clipRightHandle onPressed.")
                    }
            }

            // zone droite pour l'agrandissement du clip
            MouseArea {
                    id: tw_clipRightHandle
                    width: 10
                    height: tw_clip.height
                    anchors.right: parent.right
                    x: tw_clip.width - width
                    hoverEnabled: true
                    drag.target: tw_clip
                    drag.axis: "XAxis"

                    onEntered: {
                        _tw_timelineData.displayCursor("scale");
                    }
                    onExited: {
                            _tw_timelineData.displayCursor("none");
                    }
                    onPressed: {
                        clipRightPressed = 1
                        console.log("mouse X clip Released"+mouseX);
                    }
                    onReleased:{
                        clipRightPressed = 0

                        console.log("mouse X clip Released"+mouseX);

                        if( tw_clip.width + mouseX * _tw_timelineData.timelineScale > tw_clip.width )
                        {
                            tw_clip.width = tw_clip.width + mouseX * _tw_timelineData.timelineScale
                        }

                    }
            }
    }

}
