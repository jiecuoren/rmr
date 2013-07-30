import QtQuick 1.0
import MReader 1.0
import "../component"

Item{
    id: manager

    property int noAnimation: 0      //no animation effect

    property int fade: 1             //current view fade in, new view fade out

    property int slideLeft: 2        //current view and new view slide from right to left

    property int slideRight: 3       //current view and new view slide from left to right

    property int slideDown: 4        //current view's opacity reduced, new view slide from top to bottom

    property int slideUp: 5          //current view slides from bottom to top, new view's opacity change back to 1.0

    property Item currentView

    function activeView(viewId, animType)
    {
        if (isAnimationOngoing())
        {
            console.log("animation is ongoing, not allow to activate a new viwe");
            return;
        }

        var newView = getView(viewId);

        doAnimationEffect(currentView, newView, animType);
    }

    function getView(viewId)
    {
        for (var i = 0; i < viewsList.count; i++)
        {
            var item = viewsList.get(i)

            if (item.viewId == viewId)
            {
                 return item.model
            }
        }

        console.log("view does not exist, create it");
        var newView = application.createView(viewId);
        viewsList.append({"model": newView, "viewId": viewId})

        return newView;
    }

    function isAnimationOngoing()
    {
        //remember to add new animation.running!!!!
        return (fadeAnimation.running ||
                slideLeftAnimation.running ||
                slideRightAnimation.running ||
                slideDownAnimation.running ||
                slideUpAnimation.running);
    }

    function doAnimationEffect(oldView, newView, animType)
    {
        switch (animType)
        {
        case noAnimation:
            if (oldView != null)
            {
                console.log("oldView is not null");
                oldView.visible = false;
                if (oldView.handleDeactivation != undefined)
                    oldView.handleDeactivation();
            }

            newView.visible = true;
            if (newView.handleActivation != undefined)
                newView.handleActivation();
            currentView = newView;
            animationStoped();
            break;

        case fade:
            fadeAnimation.oldView = oldView;
            fadeAnimation.newView = newView;
            fadeAnimation.start();
            break;

        case slideLeft:
            slideLeftAnimation.oldView = oldView;
            slideLeftAnimation.newView = newView;
            slideLeftAnimation.start();
            break;

        case slideRight:
            slideRightAnimation.oldView = oldView;
            slideRightAnimation.newView = newView;
            slideRightAnimation.start();
            break;

        case slideDown:
            slideDownAnimation.oldView = oldView;
            slideDownAnimation.newView = newView;
            slideDownAnimation.start();
            break;

        case slideUp:
            slideUpAnimation.oldView = oldView;
            slideUpAnimation.newView = newView;
            slideUpAnimation.start();
            break;

        default:
            console.log("god, what animation type you have passed?");
            break;
        }
    }


    function currentViewID() {
        return currentView.viewId
    }

    function animationStoped() {
        console.debug("ViewManager animation stopped, currentViewId:", currentViewID());
    }


    ListModel{ id: viewsList  }

    SequentialAnimation {
        id: fadeAnimation
        running: false

        property Item oldView;
        property Item newView;

        //pre-animation process
        PropertyAction { target: fadeAnimation.newView; property: "opacity"; value: 0.3 }
        ScriptAction {
            script: {
                if (fadeAnimation.oldView.handleDeactivation != undefined)
                    fadeAnimation.oldView.handleDeactivation();
            }
        }

        //real animation
        ParallelAnimation {
            PropertyAnimation {target: fadeAnimation.oldView; property: "opacity";
                from: 1.0; to: 0.0; duration: 1500 }

            PropertyAnimation {target: fadeAnimation.newView; property: "opacity"; to: 1.0;
                duration: 1500 }
        }

        //post-animation process
        ScriptAction {
            script: {
                if (fadeAnimation.newView.handleActivation != undefined)
                    fadeAnimation.newView.handleActivation();
                currentView = fadeAnimation.newView;
                animationStoped();
            }
        }
    }

    SequentialAnimation {
        id: slideLeftAnimation
        running: false

        property Item oldView;
        property Item newView;

        //pre-animation process
        PropertyAction { target: slideLeftAnimation.newView; property: "x";
                         value: application.width }
        ScriptAction {
            script: {
                if (slideLeftAnimation.oldView.handleDeactivation != undefined)
                    slideLeftAnimation.oldView.handleDeactivation();
                console.debug("slideLeftAnimation started...: ", application.width);
            }
        }

        //real animation
        ParallelAnimation {
            PropertyAnimation {target: slideLeftAnimation.oldView; property: "x"; from: 0.0;
                               to: -application.width; duration: 1000;
                               easing.type: Easing.OutExpo }

            PropertyAnimation {target: slideLeftAnimation.newView; property: "x"; to: 0.0;
                               duration: 1000; easing.type: Easing.OutExpo }
        }

        //post-animation process
        ScriptAction {
            script: {
                if (slideLeftAnimation.newView.handleActivation != undefined)
                    slideLeftAnimation.newView.handleActivation();
                currentView = slideLeftAnimation.newView;
                console.debug("slideLeftAnimation ended...");
                animationStoped();
            }
        }
    }

    SequentialAnimation {
        id: slideRightAnimation
        running: false

        property Item oldView;
        property Item newView;

        //pre-animation process
        PropertyAction { target: slideRightAnimation.newView; property: "x";
                         value: -application.width }
        ScriptAction {
            script: {
                if (slideRightAnimation.oldView.handleDeactivation != undefined)
                    slideRightAnimation.oldView.handleDeactivation();
            }
        }

        //real animation
        ParallelAnimation {
            PropertyAnimation {target: slideRightAnimation.oldView; property: "x"; from: 0.0;
                to: application.width; duration: 1000; easing.type: Easing.OutExpo}

            PropertyAnimation {target: slideRightAnimation.newView; property: "x"; to: 0.0;
                               duration: 1000; easing.type: Easing.OutExpo }
        }

        //post-animation process
        ScriptAction {
            script: {
                if (slideRightAnimation.newView.handleActivation != undefined)
                    slideRightAnimation.newView.handleActivation();
                currentView = slideRightAnimation.newView;
                animationStoped();
            }
        }
    }

    SequentialAnimation {
        id: slideDownAnimation
        running: false

        property Item oldView;
        property Item newView;

        //pre-animation process
        PropertyAction {target: slideDownAnimation.newView; property: "y";
                        value: -application.height}
        PropertyAction {target: slideDownAnimation.oldView; property: "opacity";
                        value: 0.5}
        ScriptAction {
            script: {
                if (slideDownAnimation.oldView.handleDeactivation != undefined)
                    slideDownAnimation.oldView.handleDeactivation();
            }
        }

        //real animation
        PropertyAnimation {target: slideDownAnimation.newView; property: "y"; to: 0.0;
            duration: 1000; easing.type: Easing.OutExpo}

        //post-animation process
        ScriptAction {
            script: {
                if (slideDownAnimation.newView.handleActivation != undefined)
                    slideDownAnimation.newView.handleActivation();
                currentView = slideDownAnimation.newView;
                animationStoped();
            }
        }
    }

    SequentialAnimation {
        id: slideUpAnimation
        running: false

        property Item oldView;
        property Item newView;

        //pre-animation process
        ScriptAction {
            script: {
                if (slideUpAnimation.oldView.handleDeactivation != undefined)
                    slideUpAnimation.oldView.handleDeactivation();
            }
        }

        //real animation
        PropertyAnimation {target: slideUpAnimation.oldView; property: "y";
                           to: -application.height;  duration: 1000;
                           easing.type: Easing.InBack}

        //post-animation process
        ScriptAction {
            script: {
                slideUpAnimation.newView.opacity = 1.0
                if (slideUpAnimation.newView.handleActivation != undefined)
                    slideUpAnimation.newView.handleActivation();
                currentView = slideUpAnimation.newView;
                animationStoped();
            }
        }
    }
}
